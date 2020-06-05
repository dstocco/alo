#include <iostream>
#include <sstream>
#include <cstdint>
#include <unordered_map>

#include "BoostReader.h"
#include "ColumnData.h"
#include "DataformatConverter.h"
#include "RawData.h"

#include "DetectorsRaw/RawFileReader.h"
#include "DataFormatsMID/ColumnData.h"
#include "DataFormatsMID/ROFRecord.h"
#include "MIDRaw/DecodedDataAggregator.h"
#include "MIDRaw/Decoder.h"

#include "boost/program_options.hpp"

namespace po = boost::program_options;

template <typename DATA>
void sortData(std::vector<DATA>& data)
{
  std::sort(data.begin(), data.end(), [](const DATA& d1, const DATA& d2) {
    if (d1.deId == d2.deId) {
      return d1.columnId < d2.columnId;
    }
    return d1.deId < d2.deId;
  });
}

unsigned long getUniqueId(uint8_t deId, uint8_t columnId)
{
  return columnId | ((deId % 9) << 4) | (deId / 36) << 8;
}

std::vector<alo::mid::ColumnData> getTriggered(const std::vector<alo::mid::ColumnData>& aloData)
{
  std::unordered_map<unsigned long, alo::mid::ColumnData> masks;
  for (auto& col : aloData) {
    auto& mask = masks[getUniqueId(col.deId, col.columnId)];
    if (col.patterns[4] == 0) {
      continue;
    }
    for (int ipat = 0; ipat < 4; ++ipat) {
      if (col.patterns[ipat] != 0) {
        mask.patterns[4] = 0xFFFF;
        mask.patterns[ipat] = 0xFFFF;
      }
    }
  }

  std::vector<alo::mid::ColumnData> triggered;
  for (auto& col : aloData) {
    auto& mask = masks[getUniqueId(col.deId, col.columnId)];
    alo::mid::ColumnData trig = col;
    bool reject = true;
    for (int ipat = 0; ipat < 5; ++ipat) {
      trig.patterns[ipat] &= mask.patterns[ipat];
      if (trig.patterns[ipat] != 0) {
        reject = false;
      }
    }
    if (!reject) {
      triggered.emplace_back(trig);
    }
  }
  return triggered;
}

void printEvent(const o2::mid::ROFRecord& rof, const std::vector<o2::mid::ColumnData>& data, const std::vector<alo::mid::ColumnData>& aloData, bool isSubRange = false)
{
  std::cout << rof.interactionRecord << "  alo: " << aloData.size() << "  o2: " << rof.nEntries << std::endl;
  std::cout << "alo:" << std::endl;
  for (auto& col : aloData) {
    std::cout << "  " << col << std::endl;
  }
  std::cout << "o2:" << std::endl;
  if (isSubRange) {
    for (auto& col : data) {
      std::cout << "  " << col << std::endl;
    }
  } else {
    for (auto colIt = data.begin() + rof.firstEntry; colIt != data.begin() + rof.firstEntry + rof.nEntries; ++colIt) {
      std::cout << "  " << *colIt << std::endl;
    }
  }
}

bool checkEvent(const o2::mid::ROFRecord& rof, const std::vector<o2::mid::ColumnData>& data, const std::vector<alo::mid::ColumnData>& aloData)
{
  auto triggered = getTriggered(aloData);
  std::vector<o2::mid::ColumnData> o2Data(data.begin() + rof.firstEntry, data.begin() + rof.firstEntry + rof.nEntries);
  sortData(triggered);
  sortData(o2Data);

  if (triggered.size() != o2Data.size()) {
    std::cout << "Wrong size" << std::endl;
    printEvent(rof, o2Data, triggered, true);
    return false;
  }

  auto aloIt = triggered.begin();
  for (auto o2It = o2Data.begin(); o2It != o2Data.end(); ++o2It) {
    bool isSame = false;
    if (o2It->deId == aloIt->deId && o2It->columnId == aloIt->columnId) {
      for (int ipat = 0; ipat < 5; ++ipat) {
        if (o2It->patterns[ipat] != aloIt->patterns[ipat]) {
          isSame = false;
          break;
        }
      }
      isSame = true;
    }
    if (!isSame) {
      std::cout << "Difference: " << std::endl;
      std::cout << " alo: " << *aloIt << std::endl;
      std::cout << "  o2: " << *o2It << std::endl;
      return false;
    }
    ++aloIt;
  }
  return true;
}

bool findSame(const o2::mid::ColumnData& col, const std::vector<alo::mid::ColumnData>& aloData)
{
  for (auto& aloCol : aloData) {
    if (col.deId == aloCol.deId && col.columnId == aloCol.columnId) {
      bool isSame = true;
      for (int ipat = 0; ipat < 5; ++ipat) {
        if (col.patterns[ipat] != aloCol.patterns[ipat]) {
          isSame = false;
          break;
        }
      }
      if (isSame) {
        return true;
      }
    }
  }
  return false;
}

int main(int argc, char* argv[])
{
  std::string aloFilename, o2Filename, eventCorrFilename;

  // clang-format off
  po::options_description generic("Generic options");
  generic.add_options()
          ("help", "produce help message")
          ("alo-file", po::value<std::string>(&aloFilename),"Raw alo file")
          ("o2-file", po::value<std::string>(&o2Filename),"Raw o2 file")
          ("event-correspondence", po::value<std::string>(&eventCorrFilename),"Event correspondence filename");

  // clang-format on

  po::options_description cmdline;
  cmdline.add(generic);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).options(cmdline).run(), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << generic << std::endl;
    return 2;
  }
  if (vm.count("alo-file") == 0 || vm.count("o2-file") == 0) {
    std::cout << "Please specify alo-file and o2-file" << std::endl;
    return 1;
  }

  std::unordered_map<uint64_t, uint64_t> eventCorrespondence;
  std::ifstream eventCorrFile(eventCorrFilename.c_str());
  if (eventCorrFile.is_open()) {
    std::string line;
    while (std::getline(eventCorrFile, line)) {
      auto idx = line.find(" ");
      uint64_t orb1 = std::stoul(line.substr(0, idx).c_str());
      uint64_t orb2 = std::stoul(line.substr(idx + 1).c_str());
      eventCorrespondence[orb1] = orb2;
    }
  }

  alo::mid::BoostReader reader(aloFilename.data());
  alo::mid::RawData aloRawData;
  std::unordered_map<uint64_t, alo::mid::RawData> aloRawMap;
  while (reader.read(aloRawData)) {
    auto ir = alo::mid::bcOrbitPeriodToLong(aloRawData.bunchCrossingID, aloRawData.orbitID, aloRawData.periodID);
    aloRawMap[ir] = aloRawData;
  }

  o2::raw::RawFileReader rawReader;
  rawReader.addFile(o2Filename.c_str());
  rawReader.init();
  std::vector<char> buffer;
  for (size_t itf = 0; itf < rawReader.getNTimeFrames(); ++itf) {
    rawReader.setNextTFToRead(itf);
    for (size_t ilink = 0; ilink < rawReader.getNLinks(); ++ilink) {
      auto& link = rawReader.getLink(ilink);
      auto tfsz = link.getNextTFSize();
      if (!tfsz) {
        continue;
      }
      std::vector<char> linkBuffer(tfsz);
      link.readNextTF(linkBuffer.data());
      buffer.insert(buffer.end(), linkBuffer.begin(), linkBuffer.end());
    }
  }

  o2::mid::Decoder<o2::mid::GBTUserLogicDecoder> decoder;
  gsl::span<const uint8_t> data(reinterpret_cast<uint8_t*>(buffer.data()), buffer.size());
  decoder.process(data);

  o2::mid::DecodedDataAggregator aggregator;
  aggregator.process(decoder.getData(), decoder.getROFRecords());

  unsigned long nErrors = 0;

  for (auto& rof : aggregator.getROFRecords()) {
    uint64_t ir = rof.interactionRecord.toLong();
    auto item = eventCorrespondence.find(ir);
    if (item == eventCorrespondence.end()) {
      std::cout << "Cannot find " << rof.interactionRecord << std::endl;
    } else {
      auto& aloData = aloRawMap[item->second];
      if (!checkEvent(rof, aggregator.getData(), aloData.columnData)) {
        ++nErrors;
      }
    }
  }

  return 0;
}

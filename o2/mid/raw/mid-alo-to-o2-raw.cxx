#include <iostream>
#include <sstream>
#include <cstdint>
#include <map>

#include "ColumnData.h"
#include "RawData.h"
#include "BoostReader.h"
#include "DataformatConverter.h"
#include "DataFormatsMID/ROFRecord.h"
#include "MIDRaw/Encoder.h"

#include "boost/program_options.hpp"

namespace po = boost::program_options;

int main(int argc, char* argv[])
{
  std::string inFilename, outputDir;

  // clang-format off
  po::options_description generic("Generic options");
  generic.add_options()
          ("help", "produce help message")
          ("output-dir", po::value<std::string>(&outputDir)->default_value("."),"output directory");

  po::options_description hidden("hidden options");
  hidden.add_options()
          ("input", po::value<std::vector<std::string>>(),"intermidiate raw data files");
  // clang-format on

  po::positional_options_description pos;
  pos.add("input", -1);

  po::options_description cmdline;
  cmdline.add(generic).add(hidden);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).options(cmdline).positional(pos).run(), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << generic << std::endl;
    return 2;
  }
  if (vm.count("input") == 0) {
    std::cout << "no input file specified" << std::endl;
    return 1;
  }

  std::vector<std::string> inputfiles{
    vm["input"].as<std::vector<std::string>>()
  };

  int ievt = -1;
  // This syntax should be able to run the command in parallel
  std::for_each(
    // std::execution::par_unseq,
    inputfiles.begin(),
    inputfiles.end(),
    [&outputDir, &ievt](auto&& input) {
      alo::mid::BoostReader reader(input.data());
      alo::mid::RawData aloRawData;
      std::map<uint64_t, alo::mid::RawData> aloRawMap;
      // Run 2 data might not be time ordered.
      // However, we do need increasing orbit and bc for Run3.
      // So we need to read all data and order them before encoding them
      while (reader.read(aloRawData)) {
        auto ir = alo::mid::bcOrbitPeriodToLong(aloRawData.bunchCrossingID, aloRawData.orbitID, aloRawData.periodID);
        aloRawMap[ir] = aloRawData;
      }

      std::string basename = input;
      std::string fdir = "./";
      auto pos = input.find_last_of("/");
      if (pos != std::string::npos) {
        basename.erase(0, pos + 1);
        fdir = input;
        fdir.erase(pos);
      }
      pos = basename.find_last_of(".");
      if (pos != std::string::npos) {
        basename.erase(pos);
      }
      basename += "_o2.dat";
      pos = basename.find("_alo");
      if (pos != std::string::npos) {
        basename.replace(pos, 4, "");
      }
      if (outputDir.empty()) {
        outputDir = fdir;
      }
      if (outputDir.back() != '/') {
        outputDir += "/";
      }
      std::string outFilename = outputDir + basename;
      std::ofstream outFile(outFilename.c_str(), std::ios::binary);
      o2::mid::Encoder encoder;
      // encoder.setSkipEmptyTFs();
      uint32_t lastOrbit = 0, currentOrbit = 0;
      std::map<uint32_t, uint64_t> orbitCorrespondence;
      for (auto& itRaw : aloRawMap) {
        ++ievt;

        o2::InteractionRecord ir(itRaw.second.bunchCrossingID, currentOrbit);
        encoder.process(alo::mid::getColumnData(itRaw.second), ir, o2::mid::EventType::Standard);
        auto convertedOrbit = alo::mid::bcOrbitPeriodToLong(0, itRaw.second.orbitID, itRaw.second.periodID);
        if (convertedOrbit != lastOrbit) {
          orbitCorrespondence[currentOrbit] = convertedOrbit;
          ++currentOrbit;
          lastOrbit = itRaw.second.orbitID;
          outFile.write(reinterpret_cast<const char*>(encoder.getBuffer().data()), encoder.getBufferSize());
          encoder.clear();
        }
      } // loop on events
      if (encoder.getBuffer().size() > 0) {
        outFile.write(reinterpret_cast<const char*>(encoder.getBuffer().data()), encoder.getBufferSize());
        encoder.clear();
      }

      outFilename.erase(outFilename.length() - 4);
      outFilename += "_orbit_correspondence.txt";
      std::ofstream outFileCorr(outFilename.c_str());
      for (auto& item : orbitCorrespondence) {
        outFileCorr << item.first << " " << item.second << std::endl;
      }
      outFileCorr.close();
    });

  if (ievt < 0) {
    std::cerr << "Error: no event found" << std::endl;
    return 1;
  }

  std::cout << "Number of events processed: " << ievt << std::endl;
  return 0;
}

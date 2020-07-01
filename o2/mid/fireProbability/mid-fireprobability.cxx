#include <iostream>
#include <sstream>

#include "DataFormatsMID/Cluster2D.h"
#include "MIDBase/GeometryTransformer.h"
#include "MIDClustering/PreClusterizer.h"
#include "MIDTestingSimTools/HitFinder.h"

#include "FireProbability.h"
#include "ColumnData.h"
#include "RawData.h"
#include "Event.h"
#include "BoostReader.h"
#include "DataformatConverter.h"

#include "boost/program_options.hpp"

namespace po = boost::program_options;

int main(int argc, char* argv[])
{
  std::string inFilenameRaw, inFilenameESD, outFilename;

  // clang-format off
  po::options_description generic("Generic options");
  generic.add_options()
          ("help", "produce help message")
          ("alo-esd-file",po::value<std::string>(&inFilenameESD),"input alo ESDs")
          ("alo-raw-file",po::value<std::string>(&inFilenameRaw),"input alo RAW")
          ("output", po::value<std::string>(&outFilename)->default_value("MIDFireProbability.root"),"basename of output file with cluster size");
  // clang-format on

  po::options_description cmdline;
  cmdline.add(generic);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).options(cmdline).run(), vm);
  po::notify(vm);

  if (vm.count("help") || vm.count("alo-esd-file") == 0 || vm.count("alo-raw-file") == 0) {
    std::cout << "Usage: " << argv[0] << " [options]\n";
    std::cout << generic << std::endl;
    return 2;
  }

  std::unordered_map<uint64_t, std::vector<alo::mid::Track>> tracks;
  alo::mid::Event esdEvent;
  alo::mid::BoostReader esdReader(inFilenameESD.data());
  while (esdReader.read(esdEvent)) {
    auto ir = alo::mid::bcOrbitPeriodToLong(esdEvent.bunchCrossingID, esdEvent.orbitID, esdEvent.periodID);
    tracks[ir] = esdEvent.tracks;
  }

  o2::mid::FireProbability fireProb;
  o2::mid::PreClusterizer preClusterizer;
  o2::mid::HitFinder hitFinder(o2::mid::createDefaultTransformer());
  std::unordered_map<uint8_t, std::vector<o2::mid::Cluster2D>> hits;
  int ievt = 0;

  alo::mid::BoostReader rawReader(inFilenameRaw.data());
  alo::mid::RawData rawData;
  while (rawReader.read(rawData)) {
    auto ir = alo::mid::bcOrbitPeriodToLong(rawData.bunchCrossingID, rawData.orbitID, rawData.periodID);
    auto item = tracks.find(ir);
    if (item == tracks.end()) {
      std::cout << "Warning: cannot find bc: " << rawData.bunchCrossingID << " orbit: " << rawData.orbitID << " period: " << rawData.periodID << std::endl;
      continue;
    }
    ++ievt;
    preClusterizer.process(alo::mid::getColumnData(rawData));
    auto preClusters = preClusterizer.getPreClusters();
    for (auto& track : item->second) {
      auto o2Track = alo::mid::convertTrack(track);
      for (int ich = 0; ich < 4; ++ich) {
        auto clusters = hitFinder.getLocalPositions(o2Track, ich);
        for (auto& cl : clusters) {
          hits[cl.deId].emplace_back(cl);
        }
      }
    }
    fireProb.process(preClusters, hits);
    hits.clear();
  } // loop on events

  if (ievt == 0) {
    std::cerr << "Error: no event found" << std::endl;
    return 1;
  }

  fireProb.saveResults(outFilename.data());
  std::cout << "Number of analyzed events: " << ievt << std::endl;

  return 0;
}

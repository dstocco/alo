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
  std::string inFilename, outFilename, ocdb;

  // clang-format off
  po::options_description generic("Generic options");
  generic.add_options()
          ("help", "produce help message")
          ("output", po::value<std::string>(&outFilename)->default_value("MIDFireProbability.root"),"basename of output file with cluster size");

  po::options_description hidden("hidden options");
  hidden.add_options()
          ("inputRaw", po::value<std::string>(),"converted raw filename")
          ("inputEsd", po::value<std::string>(),"converted esd filename");
  // clang-format on

  po::positional_options_description pos;
  pos.add("inputRaw", 1);
  pos.add("inputEsd", 1);

  po::options_description cmdline;
  cmdline.add(generic).add(hidden);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).options(cmdline).positional(pos).run(), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << generic << std::endl;
    return 2;
  }
  if (vm.count("inputRaw") == 0) {
    std::cout << "no converted raw input file specified" << std::endl;
    return 1;
  }
  if (vm.count("inputEsd") == 0) {
    std::cout << "no converted ESD input file specified" << std::endl;
    return 1;
  }

  std::string inputRaw = vm["inputRaw"].as<std::string>();
  std::string inputEsd = vm["inputEsd"].as<std::string>();

  o2::mid::FireProbability fireProb;

  o2::mid::PreClusterizer preClusterizer;
  int ievt = -1;

  o2::mid::HitFinder hitFinder(o2::mid::createDefaultTransformer());
  std::array<std::vector<o2::mid::Cluster2D>, 72> hits;
  for (auto& hitVec : hits) {
    hitVec.reserve(10);
  }

  // This syntax should be able to run the command in parallel
  alo::mid::BoostReader rawReader(inputRaw.data());
  alo::mid::RawData rawData;
  alo::mid::Event esdEvent;
  alo::mid::BoostReader esdReader(inputEsd.data());
  while (rawReader.read(rawData)) {
    esdReader.read(esdEvent);
    if (rawData.bunchCrossingID != esdEvent.bunchCrossingID || rawData.orbitID != esdEvent.orbitID || rawData.periodID != esdEvent.periodID) {
      return 2;
    }
    ++ievt;
    preClusterizer.process(alo::mid::getColumnData(rawData));
    auto preClusters = preClusterizer.getPreClusters();
    for (auto& track : esdEvent.tracks) {
      auto o2Track = alo::mid::convertTrack(track);
      for (int ich = 0; ich < 4; ++ich) {
        auto clusters = hitFinder.getLocalPositions(o2Track, ich);
        for (auto& cl : clusters) {
          hits[cl.deId].emplace_back(cl);
        }
      }
    }
    fireProb.process(preClusters, hits);
    for (auto& hitVec : hits) {
      hitVec.clear();
    }
  } // loop on events

  if (ievt < 0) {
    std::cerr << "Error: no event found" << std::endl;
    return 1;
  }

  fireProb.saveResults(outFilename.data());
  std::cout << "Number of analyzed events: " << ievt << std::endl;

  return 0;
}

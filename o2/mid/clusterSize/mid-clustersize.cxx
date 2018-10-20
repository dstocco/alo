#include <iostream>
#include <sstream>
// #include <execution>

#include "MIDClustering/PreClusterizer.h"

#include "ClusterSize.h"
#include "ColumnData.h"
#include "RawData.h"
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
          ("output", po::value<std::string>(&outFilename)->default_value("MIDClusterSize.root"),"basename of output file with cluster size");

  po::options_description hidden("hidden options");
  hidden.add_options()
          ("input", po::value<std::vector<std::string>>(),"converted raw filename");
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

  o2::mid::ClusterSize clusterSize;

  o2::mid::PreClusterizer preClusterizer;
  int ievt = -1;

  // This syntax should be able to run the command in parallel
  std::for_each(
    // std::execution::par_unseq,
    inputfiles.begin(),
    inputfiles.end(),
    [&preClusterizer, &clusterSize, &ievt](auto&& input) {
      // for (auto input : inputfiles) {

      // fair::Logger::SetConsoleSeverity(fair::Severity::debug);

      alo::mid::BoostReader reader(input.data());
      alo::mid::RawData aloRawData;
      while (reader.read(aloRawData)) {
        ++ievt;
        preClusterizer.process(alo::mid::getColumnData(aloRawData));
        auto preClusters = preClusterizer.getPreClusters();
        clusterSize.process(preClusters);
      } // loop on events
    });

  if (ievt < 0) {
    std::cerr << "Error: no event found" << std::endl;
    return 1;
  }

  clusterSize.saveResults(outFilename.data());
  std::cout << "Number of analyzed events: " << ievt << std::endl;

  return 0;
}

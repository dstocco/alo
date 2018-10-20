#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <array>

#include "MIDSimulation/ChamberHV.h"
#include "MIDSimulation/ChamberResponseParams.h"
#include "MIDSimulation/ChamberResponse.h"

#include "ClusterSizeAnalysis.h"

#include "boost/program_options.hpp"
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/array.hpp>

namespace po = boost::program_options;

o2::mid::ChamberHV getHV(const char* ocdb)
{
  o2::mid::ChamberHV hv;
  std::ifstream inFile(ocdb);
  std::array<double, 72> hvValues;
  boost::archive::binary_iarchive ia(inFile);
  ia >> hvValues;
  for (int ide = 0; ide < 72; ++ide) {
    hv.setHV(ide, hvValues[ide]);
    std::cout << "HV: " << ide << "  " << hvValues[ide] << std::endl; // TODO: REMOVE
  }
  return hv;
}

int main(int argc, char* argv[])
{
  std::string inFilename, outFilename, ocdb;

  // clang-format off
  po::options_description generic("Generic options");
  generic.add_options()
          ("help", "produce help message")
          ("ocdb", po::value<std::string>(&ocdb)->default_value("ocdb.dat"),"ocdb")
          ("output", po::value<std::string>(&outFilename)->default_value("MIDClusterSizeFit.root"),"basename of output file with tuned parameters");

  po::options_description hidden("hidden options");
  hidden.add_options()
          ("input", po::value<std::string>(),"clustersize output");
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

  std::string inputfile{vm["input"].as<std::string>()};

  o2::mid::ChamberResponse resp(o2::mid::createDefaultChamberResponseParams(), getHV(ocdb.data()));
  o2::mid::ClusterSizeAnalysis clusterSizeAnalysis(inputfile.data());
  clusterSizeAnalysis.process(resp);
  clusterSizeAnalysis.saveResults(outFilename.data());
  
  return 0;
}

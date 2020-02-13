#include "RawToMID.h"
#include "FileHandler.h"
#include "boost/program_options.hpp"
#include <iostream>
#include <vector>

namespace po = boost::program_options;

int main(int argc, char** argv)
{
  std::string outFilename, ocdb, mappingOcdb;
  int run = 0;

  // clang-format off
  po::options_description generic("Generic options");
  generic.add_options()
          ("help", "produce help message")
          ("output", po::value<std::string>(&outFilename)->default_value("mid_raw_alo.dat"),"basename of output raw file")
          ("ocdb", po::value<std::string>(&ocdb)->default_value("local://$ALIROOT_OCDB_ROOT/OCDB"),"ocdb")
          ("mappingocdb", po::value<std::string>(&mappingOcdb)->default_value(""),"OCDB for mapping (use local to save time))")
          ("run", po::value<int>(&run)->default_value(0),"run number")
          ("splitPerDE", "create one file per detection element");

  po::options_description hidden("hidden options");
  hidden.add_options()
          ("input", po::value<std::vector<std::string>>(),"raw file list");
          po::positional_options_description pos;
  // clang-format on

  pos.add("input", -1);

  po::options_description cmdline;
  cmdline.add(generic).add(hidden);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).options(cmdline).positional(pos).run(), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << "Usage: " << argv[0] << " [options] <raw_file> [<raw_file 2> ...]\n";
    std::cout << generic << std::endl;
    return 2;
  }
  if (vm.count("input") == 0) {
    std::cout << "no input file specified" << std::endl;
    return 1;
  }

  bool splitPerDE = (vm.count("splitPerDE") > 0);

  std::vector<std::string> inputfiles{
    vm["input"].as<std::vector<std::string>>()
  };

  setupCDB(run, ocdb.c_str(), mappingOcdb.c_str());

  alo::mid::FileHandler fileHandler(outFilename.c_str());

  for (auto input : inputfiles) {
    std::cout << "Converting file " << input << "\n";
    rawToMID(input.c_str(), fileHandler, splitPerDE);
  }

  return 0;
}

#include "ESDToMID.h"

#include "boost/program_options.hpp"
#include <iostream>
#include <vector>
#include "FileHandler.h"

namespace po = boost::program_options;

int main(int argc, char** argv)
{
  std::string outESD, ocdb;
  int run = 0;

  // clang-format off
  po::options_description generic("Generic options");
  generic.add_options()
          ("help", "produce help message")
          ("output", po::value<std::string>(&outESD)->default_value("mid_esd_alo.dat"),"Output MID alo ESD event")
          ("ocdb", po::value<std::string>(&ocdb)->default_value("local://$ALIROOT_OCDB_ROOT/OCDB"),"ocdb")
          ("run", po::value<int>(&run)->default_value(0),"run number");

  po::options_description hidden("hidden options");
  hidden.add_options()
          ("input", po::value<std::vector<std::string>>(),"path to simulation");
          po::positional_options_description pos;
  // clang-format on

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
  if (!setupCDB(run, ocdb.c_str())) {

    return 2;
  }

  alo::mid::FileHandler esdHandler(outESD.c_str());

  for (auto input : inputfiles) {
    esdToMID(input.c_str(), esdHandler);
  }

  return 0;
}

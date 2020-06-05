#include "HvToMID.h"
#include "boost/program_options.hpp"
#include <iostream>
#include <sstream>
#include "AliCDBManager.h"
#include "AliMpCDB.h"

namespace po = boost::program_options;

int main(int argc, char** argv)
{
  po::variables_map vm;
  po::options_description generic("Generic options");
  std::string outputDir = ".";
  std::string ocdb = "raw://";
  int runNumber = 0;

  // clang-format off
  generic.add_options()
          ("help", "produce help message")
          ("output-dir", po::value<std::string>(&outputDir),"output directory")
          ("ocdb", po::value<std::string>(&ocdb),"input ocdb");


  po::options_description hidden("hidden options");
  hidden.add_options()
          ("runNumber", po::value<int>(&runNumber),"run number");
  // clang-format on

  po::options_description cmdline;
  cmdline.add(generic).add(hidden);

  po::positional_options_description pos;
  pos.add("runNumber", -1);

  po::store(po::command_line_parser(argc, argv).options(cmdline).positional(pos).run(), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << generic << std::endl;
    return 2;
  }
  if (vm.count("runNumber") == 0) {
    std::cout << "no run number specified" << std::endl;
    return 1;
  }

  runNumber = vm["runNumber"].as<int>();

  AliCDBManager::Instance()->SetDefaultStorage(ocdb.c_str());
  AliCDBManager::Instance()->SetRun(runNumber);

  AliMpCDB::LoadDDLStore();

  std::stringstream ss;
  ss << outputDir << "/" << "ocdb_hv_" << runNumber << ".dat";

  hvToMID(ss.str().data());

  return 0;
}

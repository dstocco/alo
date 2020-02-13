#include "boost/program_options.hpp"
#include <iostream>
#include <vector>
#include "FileHandler.h"
#include "BoostReader.h"
#include "Event.h"

namespace po = boost::program_options;

std::string copyLocally(const char* filename){
  std::string sFilename(filename);
  if (sFilename.find("alien://") == std::string::npos) {
    return sFilename;
  }
  auto idx = sFilename.find_last_of("/");
  std::string baseName = sFilename.substr(idx+1);
  baseName.insert(0,"tmp_");
  std::stringstream command;
  // We cannot do this with TFileCp since we have an archive of binary files
  // and TFile does not know how to treat this.
  command << "alien_cp " << sFilename << " file:" << baseName;
  system(command.str().c_str());
  return baseName;
}

void cleanLocalFiles(const char* filename) {
  std::string sFilename(filename);
  if ( sFilename.find("tmp_") == 0 ) {
    std::remove(filename);
  }
}

int main(int argc, char** argv)
{
  std::string outFilename;

  // clang-format off
  po::options_description generic("Generic options");
  generic.add_options()
          ("help", "produce help message")
          ("output", po::value<std::string>(&outFilename)->default_value("mid_esd_alo_merged.dat"),"merged esd filename");

  po::options_description hidden("hidden options");
  hidden.add_options()
          ("input", po::value<std::vector<std::string>>(),"input directory list");
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

  std::vector<std::string> inputFiles{
    vm["input"].as<std::vector<std::string>>()
  };

  alo::mid::FileHandler outHandler(outFilename.c_str());
  alo::mid::Event data;

  for (auto input : inputFiles) {
    std::string inFilename = copyLocally(input.c_str());
    alo::mid::BoostReader reader(inFilename.c_str());
    while(reader.read(data)) {
      outHandler.getArchive() << data;
    }
    cleanLocalFiles(inFilename.c_str());
  }

  return 0;
}

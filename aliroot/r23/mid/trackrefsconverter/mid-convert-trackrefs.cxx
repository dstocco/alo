#include "TrackRefsToMID.h"
#include "boost/program_options.hpp"
#include <iostream>
#include <vector>

namespace po = boost::program_options;

int main(int argc, char **argv)
{
    po::variables_map vm;
    po::options_description generic("Generic options");
    std::string outFilename = "trackRefs_mid.dat";

    // clang-format off
  generic.add_options()
          ("help", "produce help message")
          ("output", po::value<std::string>(&outFilename),"basename of output track refs file");

  po::options_description hidden("hidden options");
  hidden.add_options()
          ("input", po::value<std::vector<std::string>>(),"path to simulation");
    // clang-format on

    po::options_description cmdline;
    cmdline.add(generic).add(hidden);

    po::positional_options_description pos;
    pos.add("input", -1);

    po::store(
        po::command_line_parser(argc, argv).options(cmdline).positional(pos).run(),
        vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << "Usage: " << argv[0] << " [options] <path_to_simulation> [<path_to_simulation 2> ...]\n";
        std::cout << generic << std::endl;
        return 2;
    }
    if (vm.count("input") == 0)
    {
        std::cout << "no input file specified" << std::endl;
        return 1;
    }

    std::vector<std::string> inputfiles{
        vm["input"].as<std::vector<std::string>>()};

    std::ofstream outFile(outFilename.c_str(), std::ios::binary);

    for (auto input : inputfiles)
    {
        std::cout << "Converting file " << input << "\n";
        trackRefsToMID(input.c_str(), outFile);
    }
    outFile.close();

    return 0;
}

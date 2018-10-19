#include "TrackRefsToMID.h"
#include "boost/program_options.hpp"
#include <iostream>
#include <vector>

// #include <boost/archive/binary_iarchive.hpp> // REMEMBER TO CUT
// #include "TrackRefs.h"                       // REMEMBER TO CUT

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

    // // REMEMBER TO CUT
    // std::ifstream inFile(outFilename.c_str(), std::ios::binary);
    // std::streampos archiveOffset = inFile.tellg();
    // std::streampos streamEnd = inFile.seekg(0, std::ios_base::end).tellg();
    // inFile.seekg(archiveOffset);
    // boost::archive::binary_iarchive ia(inFile);
    // alo::mid::TrackRefs tr;
    // while (inFile.tellg() < streamEnd)
    // {
    //     ia >> tr;
    //     std::cout << tr.mGenerated.mMomentum[0] << std::endl;
    // }

    return 0;
}

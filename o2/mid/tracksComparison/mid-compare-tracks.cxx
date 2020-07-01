#include <iostream>
#include <vector>
#include <map>
// ROOT
#include "TFile.h"
#include "TTree.h"
// O2
#include "CommonDataFormat/InteractionRecord.h"
#include "DataFormatsMID/ROFRecord.h"
#include "DataFormatsMID/Track.h"

// alo
#include "BoostReader.h"
#include "CompareTracks.h"
#include "Event.h"
#include "Track.h"
#include "DataformatConverter.h"

#include "boost/program_options.hpp"

namespace po = boost::program_options;
typedef std::chrono::high_resolution_clock myclock;

int main(int argc, char* argv[])
{
  std::string inFilenameAlo = "alo_esd_event.dat";
  std::string inFilenameO2 = "mid-tracks.root";
  std::string eventCorrFilename = "event-correspondence.txt";
  std::string outFilename = "MIDCompareTracks.root";
  float maxChi2MatchTrackTrig = 16;
  bool verbose = false;

  po::options_description generic("Generic options");

  // clang-format off
  generic.add_options()
          ("help", "produce help message")
          ("alo-esd-event-filename",po::value<std::string>(&inFilenameAlo),"input alo ESDs")
          ("o2-tracks-filename",po::value<std::string>(&inFilenameO2),"input o2 tracks")
          ("event-correspondence-filename",po::value<std::string>(&eventCorrFilename),"event correspondence")
          ("max-chi2-matching", po::value<float>(&maxChi2MatchTrackTrig),"Maximum chi2 for tracking/trigger matching")
          ("output", po::value<std::string>(&outFilename),"basename of output file")
          ("verbose", po::value<bool>(&verbose)->implicit_value(true),"print details on errors");

  // po::options_description hidden("hidden options");
  // hidden.add_options()
  //         ("input", po::value<std::vector<std::string>>(),"converted track refs filename");
  // clang-format on

  po::options_description cmdline;
  cmdline.add(generic); //.add(hidden);

  // po::positional_options_description pos;
  // pos.add("input", -1);

  po::variables_map vm;
  // po::store(po::command_line_parser(argc, argv).options(cmdline).positional(pos).run(), vm);
  po::store(po::command_line_parser(argc, argv).options(cmdline).run(), vm);
  po::notify(vm);

  if (vm.count("help") || vm.count("alo-esd-event-filename") == 0 || vm.count("o2-tracks-filename") == 0) {
    std::cout << "Usage: " << argv[0] << " [options]\n";
    std::cout << generic << std::endl;
    return 2;
  }

  std::map<uint64_t, uint64_t> eventCorrespondence;
  std::ifstream eventCorrFile(eventCorrFilename.c_str());
  if (eventCorrFile.is_open()) {
    std::string line;
    while (std::getline(eventCorrFile, line)) {
      auto idx = line.find(" ");
      uint64_t orb1 = std::stoul(line.substr(0, idx).c_str());
      uint64_t orb2 = std::stoul(line.substr(idx + 1).c_str());
      eventCorrespondence[orb1] = orb2;
    }
  }

  std::map<uint64_t, std::vector<alo::mid::Track>> aloTracks;
  alo::mid::BoostReader reader(inFilenameAlo.c_str());
  alo::mid::Event esdEvent;
  while (reader.read(esdEvent)) {
    auto ir = alo::mid::bcOrbitPeriodToLong(esdEvent.bunchCrossingID, esdEvent.orbitID, esdEvent.periodID);
    aloTracks[ir] = esdEvent.tracks;
  }

  TFile* file = TFile::Open(inFilenameO2.c_str());
  TTree* tree = static_cast<TTree*>(file->Get("midtracks"));
  std::vector<o2::mid::ROFRecord>* rofRecords = nullptr;
  tree->SetBranchAddress("MIDTrackROF", &rofRecords);
  auto tracksBranch = tree->GetBranch("MIDTrack");
  std::vector<o2::mid::Track>* tracks = nullptr;
  tracksBranch->SetAddress(&tracks);
  std::map<uint64_t, std::vector<o2::mid::Track>> o2Tracks;
  for (auto ievt = 0; ievt < tree->GetEntries(); ++ievt) {
    tree->GetEvent(ievt);
    for (auto& rof : *rofRecords) {
      auto ts = rof.interactionRecord.toLong();
      auto eventFound = eventCorrespondence.find(ts);
      if (eventFound != eventCorrespondence.end()) {
        ts = eventFound->second;
      }
      auto& trVec = o2Tracks[ts];
      for (auto trIt = tracks->begin() + rof.firstEntry; trIt != tracks->begin() + rof.firstEntry + rof.nEntries; ++trIt) {
        trVec.emplace_back(*trIt);
      }
    }
  }

  o2::mid::CompareTracks compareTracks;
  for (auto& evtIt : aloTracks) {
    auto o2It = o2Tracks.find(evtIt.first);
    if (o2It != o2Tracks.end()) {
      compareTracks.process(evtIt.second, o2It->second, maxChi2MatchTrackTrig, verbose);
    }
  }
  compareTracks.saveResults(outFilename.c_str());

  return 0;
}

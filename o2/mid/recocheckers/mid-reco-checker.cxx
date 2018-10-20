#include <iostream>
#include <sstream>
#include <bitset>
#include <chrono>
#include "Framework/Logger.h"
#include "SimulationDataFormat/MCTruthContainer.h"
#include "DataFormatsMID/Cluster2D.h"
#include "DataFormatsMID/ColumnData.h"
#include "DataFormatsMID/ROFRecord.h"
#include "DataFormatsMID/Track.h"
#include "MIDBase/GeometryTransformer.h"
#include "MIDClustering/PreClusterizer.h"
#include "MIDClustering/Clusterizer.h"
#include "MIDSimulation/ColumnDataMC.h"
#include "MIDSimulation/Digitizer.h"
#include "MIDSimulation/DigitsMerger.h"
#include "MIDSimulation/Hit.h"
#include "MIDSimulation/MCLabel.h"
#include "MIDTracking/Tracker.h"
#include "CheckReco.h"
#include "BoostReader.h"
#include "TrackRefs.h"
#include "DataformatConverter.h"
#include "boost/program_options.hpp"

namespace po = boost::program_options;
typedef std::chrono::high_resolution_clock myclock;

int main(int argc, char* argv[])
{
  po::variables_map vm;
  po::options_description generic("Generic options");
  std::string inFilename = "trackRefs_mid.dat";
  std::string outFilename = "MIDCheckReco.root";
  int nTracksPerEvent = 1;

  // clang-format off
  generic.add_options()
          ("help", "produce help message")
          ("output", po::value<std::string>(&outFilename),"basename of output track refs file")
          ("nTracksPerEvent", po::value<int>(&nTracksPerEvent),"number of tracks per event");

  po::options_description hidden("hidden options");
  hidden.add_options()
          ("input", po::value<std::vector<std::string>>(),"converted track refs filename");
  // clang-format on

  po::options_description cmdline;
  cmdline.add(generic).add(hidden);

  po::positional_options_description pos;
  pos.add("input", -1);

  po::store(
    po::command_line_parser(argc, argv).options(cmdline).positional(pos).run(),
    vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << "Usage: " << argv[0] << " [options] <input_trackRef_filename> [<input_trackRef_filename 2> ...]\n";
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

  o2::mid::CheckReco checkReco;
  o2::mid::Digitizer digitizer(o2::mid::createDefaultDigitizer());
  o2::mid::DigitsMerger digitsMerger;
  o2::mid::GeometryTransformer geoTrans = o2::mid::createDefaultTransformer();
  o2::mid::PreClusterizer preClusterizer;
  o2::mid::Clusterizer clusterizer;
  o2::mid::Tracker tracker(geoTrans);
  std::vector<o2::mid::ColumnDataMC> digitStoreMC;
  o2::dataformats::MCTruthContainer<o2::mid::MCLabel> digitLabelsMC;
  std::vector<o2::mid::ROFRecord> rofRecords;
  std::vector<o2::mid::Hit> hits;
  std::vector<alo::mid::TrackRefs> trackRefsVector;

  int ievt = -1;

  auto start = myclock::now();

  for (auto input : inputfiles) {

    // fair::Logger::SetConsoleSeverity(fair::Severity::debug);

    alo::mid::BoostReader reader(input.data());
    while (reader.read(trackRefsVector, nTracksPerEvent)) {
      ++ievt;

      std::stringstream debugStr;
      debugStr << "Event " << ievt << "\n";
      bool isOk = true;
      for (auto& trackRefs : trackRefsVector) {
        // bool isOk = true;
        debugStr << "  New trackRefs:\n";
        std::vector<o2::mid::Hit> convHits = alo::mid::convertHits(trackRefs.mHits);
        std::copy(convHits.begin(), convHits.end(), std::back_inserter(hits));
        digitizer.process(convHits, digitStoreMC, digitLabelsMC);
        rofRecords.emplace_back(1, o2::mid::EventType::Standard, 0, digitStoreMC.size() - 1);
        // digitsMerger.process(digitStoreMC, digitLabelsMC, rofRecords); // TODO: UNCOMMENT
        debugStr << "nDigits: " << digitsMerger.getColumnData().size() << "\n";
        for (auto& hit : convHits) {
          //   auto localEntrance = geoTrans.globalToLocal(
          //     hit.GetDetectorID(), hit.entrancePoint().x(),
          //     hit.entrancePoint().y(), hit.entrancePoint().z());
          //   auto localExit =
          //     geoTrans.globalToLocal(hit.GetDetectorID(), hit.exitPoint().x(),
          //                            hit.exitPoint().y(), hit.exitPoint().z());
          //   localHits.emplace_back(hit.GetTrackID(), hit.GetDetectorID(),
          //                          localEntrance, localExit);
          debugStr << hit << "\n";
          //   digitizer.hitToDigits(localHits.back(),
          //                         localHits.back().GetDetectorID(), digitStore);
        } // loop on hits
      }   // loop on trackRefsVector
      preClusterizer.process(digitsMerger.getColumnData());
      auto preClusters = preClusterizer.getPreClusters();
      clusterizer.process(preClusters);
      auto recoClusters = clusterizer.getClusters();
      isOk &= checkReco.checkClusters(hits, recoClusters, geoTrans);
      if (!isOk) {
        for (auto& digit : digitsMerger.getColumnData()) {
          std::cerr << digit << std::endl;
        }
      }
      tracker.process(recoClusters);
      for (auto& trackRefs : trackRefsVector) {
        if (std::abs(trackRefs.mParticlePDG) != 13) {
          continue;
        }
        o2::mid::Track genTrack = alo::mid::convertTrack(trackRefs.mGenerated);
        o2::mid::Track firstChTrack = alo::mid::convertTrack(trackRefs.mFirstChamber);
        std::vector<o2::mid::Hit> convHits = alo::mid::convertHits(trackRefs.mHits);
        isOk &= checkReco.checkTrack(genTrack, firstChTrack,
                                     convHits, tracker.getTracks());
      }
      hits.clear();
      if (!isOk) {
        std::cerr << debugStr.str() << std::endl;
      }
      trackRefsVector.clear();
    } // loop on events
  }   // loop on files
  auto end = myclock::now();
  auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
  std::cout << "Reconstruction took: " << duration << " / " << ievt << " = " << duration/ievt << " ns/events" << std::endl;
  checkReco.saveResults(outFilename.data());
  return 0;
}

#include "AliCDBManager.h"
#include "DataFormatsMID/Cluster2D.h"
#include "DataFormatsMID/ColumnData.h"
#include "DataFormatsMID/Track.h"
#include "MIDBase/Constants.h"
#include "MIDBase/GeometryTransformer.h"
#include "MIDBase/LegacyUtility.h"
#include "MIDClustering/Clusterizer.h"
#include "MIDSimulation/Digitizer.h"
#include "MIDSimulation/Hit.h"
#include "MIDTracking/Tracker.h"
#include "PerformanceMC.h"
#include "TrackRefReader.h"
#include <iostream>
#include <sstream>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "Requires the full galice.root name as argument" << std::endl;
    return 1;
  }

  unsigned long int nevents = 999999999;
  if (argc >= 3) {
    nevents = std::atol(argv[2]);
  }

  AliCDBManager::Instance()->SetDefaultStorage(
      "local://$ALIROOT_OCDB_ROOT/OCDB");
  AliCDBManager::Instance()->SetRun(0);

  o2::mid::PerformanceMC performanceMC;
  o2::mid::Digitizer digitizer;
  o2::mid::GeometryTransformer geoTrans;
  o2::mid::Clusterizer clusterizer;
  o2::mid::Tracker tracker;
  std::vector<o2::mid::ColumnData> digitStore;
  std::vector<o2::mid::Hit> localHits;

  TrackRefReader trRead(argv[1]);
  std::cout << "Number of events: " << trRead.numberOfEvents() << std::endl;
  if (trRead.numberOfEvents() < nevents)
    nevents = trRead.numberOfEvents();
  int progress = nevents / 10;
  for (int ievt = 0; ievt < nevents; ++ievt) {
    if (ievt % progress == 0) {
      std::cout << "Analizying event " << ievt << " of " << nevents
                << std::endl;
    }
    auto tracks = trRead.readEvent(ievt);
    for (auto &track : tracks) {
      bool isOk = true;
      std::stringstream debugStr;
      debugStr << "Event " << ievt << "\n";
      debugStr << "  New track:\n";
      for (auto &hit : track.hits) {
        auto localEntrance = geoTrans.globalToLocal(
            hit.GetDetectorID(), hit.entrancePoint().x(),
            hit.entrancePoint().y(), hit.entrancePoint().z());
        auto localExit =
            geoTrans.globalToLocal(hit.GetDetectorID(), hit.exitPoint().x(),
                                   hit.exitPoint().y(), hit.exitPoint().z());
        localHits.emplace_back(hit.GetTrackID(), hit.GetDetectorID(),
                               localEntrance, localExit);
        debugStr << hit << " => " << localHits.back() << "\n";
        digitizer.hitToDigits(localHits.back(),
                              localHits.back().GetDetectorID(), digitStore);
      } // loop on hits
      clusterizer.process(digitStore);
      std::vector<o2::mid::Cluster2D> recoClusters(
          clusterizer.getClusters().begin(),
          clusterizer.getClusters().begin() + clusterizer.getNClusters());
      isOk &= performanceMC.checkClusters(localHits, recoClusters);
      if (!isOk) {
        for (auto &digit : digitStore) {
          std::cerr << "deId: " << (int)digit.deId
                    << "  col: " << (int)digit.columnId;
          for (auto &pattern : digit.patterns) {
            std::cerr << "  " << pattern;
          }
          std::cerr << std::endl;
        }
      }
      tracker.process(recoClusters);
      std::vector<o2::mid::Track> recoTracks(tracker.getTracks().begin(),
                                             tracker.getTracks().begin() +
                                                 tracker.getNTracks());
      isOk &= performanceMC.checkTrack(track.generated, track.atFirstChamber,
                                       track.hits, recoTracks);

      digitStore.clear();
      localHits.clear();
      if (!isOk) {
        std::cerr << debugStr.str() << std::endl;
      }
    } // loop on tracks
  }   // loop on events
  performanceMC.saveResults("MIDPerformanceMC.root");
  return 0;
}

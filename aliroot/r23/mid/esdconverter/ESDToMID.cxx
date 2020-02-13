// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#include "ESDToMID.h"

#include <iostream>
#include <vector>
#include <map>
#include <bitset>

#include "Event.h"
#include "FileHandler.h"

#include "TFile.h"
#include "TTree.h"
#include "TGeoGlobalMagField.h"

// AliRoot
#include "AliCDBManager.h"
#include "AliGeomManager.h"
#include "AliESDEvent.h"
#include "AliESDMuonTrack.h"
#include "AliMUONCDB.h"
#include "AliMUONRecoParam.h"
#include "AliMUONTrack.h"
#include "AliMUONTrackParam.h"
#include "AliMUONESDInterface.h"
#include "AliMUONTrackExtrap.h"
#include "AliMUONConstants.h"

#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>

void convertEvent(alo::mid::Event& data, AliESDEvent& esdEvent)
{
  AliMUONTrack track;

  data.bunchCrossingID = esdEvent.GetBunchCrossNumber();
  data.orbitID = esdEvent.GetOrbitNumber();
  data.periodID = esdEvent.GetPeriodNumber();
  data.nTrackletsSPD = esdEvent.GetPrimaryVertexSPD()->GetNContributors();
  data.tracks.clear();

  const Int_t kFirstTrigCh = AliMUONConstants::NTrackingCh();
  const Float_t kZFilterOut = AliMUONConstants::MuonFilterZEnd();
  const Float_t kFilterThickness = kZFilterOut - AliMUONConstants::MuonFilterZBeg(); // cm

  auto ntracks = esdEvent.GetNumberOfMuonTracks();
  for (auto itrack = 0; itrack < ntracks; ++itrack) {
    auto esdTrack = esdEvent.GetMuonTrack(itrack);
    if (esdTrack->ContainTrackerData()) {
      AliMUONESDInterface::ESDToMUON(*esdTrack, track);
      AliMUONTrackParam trackParam(*static_cast<AliMUONTrackParam*>(track.GetTrackParamAtCluster()->Last()));

      // Extrap to muon filter end
      AliMUONTrackExtrap::ExtrapToZCov(&trackParam, kZFilterOut);

      // Add MCS effects
      AliMUONTrackExtrap::AddMCSEffect(&trackParam, kFilterThickness, AliMUONConstants::MuonFilterX0());

      AliMUONTrackExtrap::ExtrapToZCov(&trackParam, AliMUONConstants::DefaultChamberZ(kFirstTrigCh));

      data.tracks.push_back({});

      data.tracks.back().mMomentum = { static_cast<float>(esdTrack->Px()), static_cast<float>(esdTrack->Py()), static_cast<float>(esdTrack->Pz()) };

      data.tracks.back().mPosition = { static_cast<float>(trackParam.GetNonBendingCoor()), static_cast<float>(trackParam.GetBendingCoor()), static_cast<float>(trackParam.GetZ()) };
      data.tracks.back().mDirection = { static_cast<float>(trackParam.GetNonBendingSlope()), static_cast<float>(trackParam.GetBendingSlope()), 1. };
      const auto& cov = trackParam.GetCovariances();
      data.tracks.back().mCovarianceParameters = { static_cast<float>(cov(0, 0)), static_cast<float>(cov(2, 2)), static_cast<float>(cov(1, 1)), static_cast<float>(cov(3, 3)), static_cast<float>(cov(1, 0)), static_cast<float>(cov(3, 2)) };
      data.tracks.back().mIsMatched = esdTrack->ContainTriggerData();
    }
  } // loop on tracks
}

void esdToMID(const char* esdFilename, alo::mid::FileHandler& outTracksHandler)
{
  /// Converts the Run2 esd tracks to the Run3 format
  std::unique_ptr<TFile> esdFile{ TFile::Open(esdFilename) };

  if (!esdFile || !esdFile->IsOpen()) {
    Error("tracksToMID", "opening ESD file %s failed", esdFilename);
    return;
  }

  TTree* tree = static_cast<TTree*>(esdFile->Get("esdTree"));
  if (!tree) {
    Error("tracksToMID", "no ESD tree found");
    return;
  }

  AliESDEvent esdEvent;
  esdEvent.ReadFromTree(tree);

  auto nevents = tree->GetEntries();

  std::cout << nevents << " entries to process\n";

  tree->SetBranchStatus("*", false);
  tree->SetBranchStatus("AliESDRun*", true);
  tree->SetBranchStatus("AliESDHeader*", true);
  tree->SetBranchStatus("Muon*", true);

  alo::mid::Event data;

  for (auto ievent = 0; ievent < nevents; ievent++) {

    if (tree->GetEvent(ievent) <= 0) {
      Error("tracksToMID", "no ESD object found for event %d", ievent);
      return;
    }

    convertEvent(data, esdEvent);
    outTracksHandler.getArchive() << data;
  } // loop on events
}

bool setupCDB(int run, const char* ocdb)
{
  /// Setup the OCDB
  AliCDBManager::Instance()->SetDefaultStorage(ocdb);
  AliCDBManager::Instance()->SetRun(run);

  // load magnetic field for track extrapolation
  if (!TGeoGlobalMagField::Instance()->GetField()) {
    if (!AliMUONCDB::LoadField())
      return false;
  }

  // // load geometry for track extrapolation to vertex and for checking hits are under pads in reconstructible tracks
  // if (!AliGeomManager::GetGeometry()) {
  //   AliGeomManager::LoadGeometry();
  //   if (!AliGeomManager::GetGeometry())
  //     return false;
  //   if (!AliGeomManager::ApplyAlignObjsFromCDB("MUON"))
  //     return false;
  // }

  AliMUONRecoParam* recoParam = AliMUONCDB::LoadRecoParam();
  if (!recoParam)
    return false;
  if (!AliMUONESDInterface::GetTracker())
    AliMUONESDInterface::ResetTracker(recoParam);

  return true;
}
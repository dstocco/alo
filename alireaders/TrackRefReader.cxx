// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   alireaders/TrackRefReader.cxx
/// \brief  Implementation of a AliRoot TrackRefs reader for MID testing
/// \author Diego Stocco <Diego.Stocco at cern.ch>
/// \date   06 July 2018

#include "TrackRefReader.h"

#include <iostream>
#include <memory>

// O2
#include "MIDBase/LegacyUtility.h"

// ROOT
#include "TClonesArray.h"
#include "TFile.h"
#include "TTree.h"

// AliRoot
#include "AliMCEvent.h"
#include "AliMUONConstants.h"
#include "AliTrackReference.h"

//______________________________________________________________________________
TrackRefReader::TrackRefReader(const char *pathToSim) : mMCHandler() {
  /// Default constructor
  mMCHandler.SetInputPath(pathToSim);
  mMCHandler.InitIO("");
}

//______________________________________________________________________________
std::vector<TrackRefReader::TrackHits> TrackRefReader::readEvent(int ievent) {
  /// Read file
  /// @param ievent Event number

  std::vector<TrackRefReader::TrackHits> tracks;

  if (!mMCHandler.BeginEvent(ievent)) {
    std::cerr << "Failed to read MC objects for event " << ievent << std::endl;
    return tracks;
  }

  TParticle *particle;
  TClonesArray *trackRefs;
  int nTracks = mMCHandler.MCEvent()->GetNumberOfTracks();

  TrackRefReader::TrackHits track;
  Point3D<float> points[2];

  for (int itrack = 0; itrack < nTracks; ++itrack) {
    mMCHandler.GetParticleAndTR(itrack, particle, trackRefs);
    AliTrackReference *atr = nullptr;
    int prevDeId = -1;
    TIter next(trackRefs);
    while ((atr = static_cast<AliTrackReference *>(next()))) {
      if (atr->DetectorId() != AliTrackReference::kMUON) {
        continue;
      }
      int detElemId = atr->UserId();
      int chamberId = detElemId / 100 - 1;
      if (chamberId < AliMUONConstants::NTrackingCh() ||
          chamberId >= AliMUONConstants::NCh()) {
        continue;
      }
      int deId = o2::mid::LegacyUtility::convertFromLegacyDeId(atr->UserId());
      int ipt = (deId == prevDeId) ? 1 : 0;
      points[ipt].SetXYZ(atr->X(), atr->Y(), atr->Z());

      if (ipt == 1) {
        track.hits.emplace_back(itrack, deId, points[0], points[1]);
      }
      if (prevDeId < 0) {
        // This means that it is the first hit
        track.atFirstChamber.setDirection(atr->Px() / atr->Pz(),
                                          atr->Py() / atr->Pz(), 1.);
        track.atFirstChamber.setPosition(atr->X(), atr->Y(), atr->Z());
      }
      prevDeId = deId;
    } // loop on hits

    if (!track.hits.empty()) {
      track.generated.setDirection(particle->Px(), particle->Py(),
                                   particle->Pz());
      tracks.emplace_back(track);
      track.hits.clear();
    }
  } // loop on tracks
  mMCHandler.FinishEvent();
  return tracks;
}

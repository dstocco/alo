// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   alireaders/TrackRefsToMID.cxx
/// \brief  Implementation of a AliRoot TrackRefs reader for MID testing
/// \author Diego Stocco <Diego.Stocco at cern.ch>
/// \date   06 July 2018

#include "TrackRefsToMID.h"

#include <iostream>
#include <array>

// ROOT
#include "TClonesArray.h"
#include "TFile.h"
#include "TTree.h"

// AliRoot
#include "AliMCEventHandler.h"
#include "AliMCEvent.h"
#include "AliMUONConstants.h"
#include "AliTrackReference.h"

#include "TrackRefs.h"
#include "ConvertDE.h"

#include <boost/archive/binary_oarchive.hpp>

//________________________________________________________________
void trackRefsToMID(const char *pathToSim, const char *outFilename)
{
  std::ofstream outFile(outFilename, std::ios::binary);
  trackRefsToMID(pathToSim, outFile);
  outFile.close();
}
//________________________________________________________________
void trackRefsToMID(const char *pathToSim, std::ofstream &outFile)
{
  AliMCEventHandler mcHandler;
  mcHandler.SetInputPath(pathToSim);
  mcHandler.InitIO("");

  alo::mid::TrackRefs outTR;
  alo::mid::Hit hit;

  TParticle *particle;
  TClonesArray *trackRefs;
  AliTrackReference *atr = nullptr;
  
  boost::archive::binary_oarchive oa(outFile);

  unsigned long int nEntries = mcHandler.GetTree()->GetEntries();
  for (unsigned long ientry = 0; ientry < nEntries; ++ientry)
  {
    if (!mcHandler.BeginEvent(ientry))
    {
      std::cerr << "Failed to read MC objects for event " << ientry << std::endl;
      return;
    }
    int nTracks = mcHandler.MCEvent()->GetNumberOfTracks();

    for (int itrack = 0; itrack < nTracks; ++itrack)
    {
      mcHandler.GetParticleAndTR(itrack, particle, trackRefs);
      int prevDeId = -1;
      TIter next(trackRefs);
      while ((atr = static_cast<AliTrackReference *>(next())))
      {
        if (atr->DetectorId() != AliTrackReference::kMUON)
        {
          continue;
        }
        int detElemId = atr->UserId();
        int chamberId = detElemId / 100 - 1;
        if (chamberId < AliMUONConstants::NTrackingCh() ||
            chamberId >= AliMUONConstants::NCh())
        {
          continue;
        }
        int deId = alo::mid::ConvertDE::convertFromLegacyDeId(atr->UserId());
        if (deId == prevDeId)
        {
          hit.exitPoint = {atr->X(), atr->Y(), atr->Z()};
          hit.trackId = itrack;
          hit.deId = deId;
          outTR.mHits.emplace_back(hit);
        }
        else
        {
          hit.entryPoint = {atr->X(), atr->Y(), atr->Z()};
        }

        if (prevDeId < 0)
        {
          // This means that it is the first hit
          outTR.mFirstChamber.mMomentum = {atr->Px() / atr->Pz(),
                                           atr->Py() / atr->Pz(), 1.};
          outTR.mFirstChamber.mPosition = {atr->X(), atr->Y(), atr->Z()};
        }
        prevDeId = deId;
      } // loop on hits

      if (!outTR.mHits.empty())
      {
        outTR.mGenerated.mMomentum = {particle->Px(), particle->Py(),
                                      particle->Pz()};
        outTR.mGenerated.mPosition = {particle->Vx(), particle->Vy(),
                                      particle->Vz()};
        oa << outTR;
        outTR.mHits.clear();
      }
    } // loop on tracks
    mcHandler.FinishEvent();
  }
}

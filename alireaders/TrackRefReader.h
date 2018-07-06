// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   alireaders/TrackRefReader.h
/// \brief  TrackRef reader for MID testing
/// \author Diego Stocco <Diego.Stocco at cern.ch>
/// \date   06 July 2018

#ifndef O2_MID_TRACKREFREADER_H
#define O2_MID_TRACKREFREADER_H

#include "AliMCEventHandler.h"
#include "DataFormatsMID/Track.h"
#include "MIDSimulation/Hit.h"
#include <vector>

/// Class to generate tracks for MID
class TrackRefReader {
public:
  TrackRefReader(const char *pathToSim);
  virtual ~TrackRefReader() = default;

  TrackRefReader(const TrackRefReader &) = delete;
  TrackRefReader &operator=(const TrackRefReader &) = delete;
  TrackRefReader(TrackRefReader &&) = delete;
  TrackRefReader &operator=(TrackRefReader &&) = delete;

  struct TrackHits {
    o2::mid::Track generated;       /// Generated track
    o2::mid::Track atFirstChamber;  /// Momentum at first MID chamber
    std::vector<o2::mid::Hit> hits; /// Associated hits
  };

  std::vector<TrackHits> readEvent(int event);
  /// Number of events
  int numberOfEvents() const { return mMCHandler.GetTree()->GetEntries(); }

private:
  AliMCEventHandler mMCHandler; ///< MC handler
};

#endif /* O2_MID_TRACKREFREADER_H */

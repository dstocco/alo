// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   TrackRefs.h
/// \brief  Track refs for MID
/// \author Diego Stocco <Diego.Stocco at cern.ch>
/// \date   12 October 2018

#ifndef O2_MID_TRACKREFS_H
#define O2_MID_TRACKREFS_H

#include <vector>

#include "Track.h"
#include "Hit.h"

#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>

namespace alo
{
namespace mid
{
struct TrackRefs
{
  Track mGenerated;       /// Generated track
  Track mFirstChamber;  /// Momentum at first MID chamber
  std::vector<Hit> mHits; /// Associated hits

  friend class boost::serialization::access;

  /// Serializes the struct
  template <class Archive>
  void serialize(Archive &ar, const unsigned int version)
  {
    ar &mGenerated;
    ar &mFirstChamber;
    ar &mHits;
  }
};

//______________________________________________________________________________
std::ostream& operator<<(std::ostream& stream, const TrackRefs& trackRefs)
{
  /// Overload ostream operator
  stream << "Generated: " << trackRefs.mGenerated;
  stream << "\nAt first chamber: " << trackRefs.mFirstChamber;
  stream << "\nHits:";
  for ( auto& hit : trackRefs.mHits ) {
    stream << "\n" << hit;
  }
  return stream;
}

} // namespace mid
} // namespace alo
#endif /* O2_MID_TRACKREFS_H */
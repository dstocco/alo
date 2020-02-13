// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#ifndef ALO_MID_TRACKREFS_H
#define ALO_MID_TRACKREFS_H

#include <vector>

#include "TrackMC.h"
#include "Hit.h"

#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>

namespace alo
{
namespace mid
{
struct TrackRefs {
  int mParticlePDG;       /// Particle ID
  TrackMC mGenerated;     /// Generated track
  TrackMC mFirstChamber;  /// Momentum at first MID chamber
  std::vector<Hit> mHits; /// Associated hits

  /// Serializes the struct
  template <class Archive>
  void serialize(Archive& ar, const unsigned int version)
  {
    ar& mParticlePDG;
    ar& mGenerated;
    ar& mFirstChamber;
    ar& mHits;
  }
};

std::ostream& operator<<(std::ostream& stream, const TrackRefs& trackRefs)
{
  /// Overload ostream operator
  stream << "Particle PDG: " << trackRefs.mParticlePDG;
  stream << "\nGenerated: " << trackRefs.mGenerated;
  stream << "\nAt first chamber: " << trackRefs.mFirstChamber;
  stream << "\nHits:";
  for (auto& hit : trackRefs.mHits) {
    stream << "\n"
           << hit;
  }
  return stream;
}
} // namespace mid
} // namespace alo

namespace boost
{
namespace serialization
{
template <class Archive>
void serialize(Archive& ar, alo::mid::TrackRefs& tr, const unsigned int version)
{
  /// Non-intrusive boost serialization
  ar& tr.mParticlePDG& tr.mGenerated& tr.mFirstChamber& tr.mHits;
}
} // namespace serialization
} // namespace boost
#endif
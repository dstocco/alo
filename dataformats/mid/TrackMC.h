// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#ifndef ALO_MID_TRACKMC_H
#define ALO_MID_TRACKMC_H

#include <iostream>
#include <array>

#include <boost/serialization/access.hpp>
#include <boost/serialization/array.hpp>

namespace alo
{
namespace mid
{
struct TrackMC {
  std::array<double, 4> mMomentum; /// Track momentum
  std::array<double, 3> mPosition; /// Track position
};

//______________________________________________________________________________
inline std::ostream& operator<<(std::ostream& stream, const TrackMC& track)
{
  /// Overload ostream operator
  stream << "Momentum (" << track.mMomentum[0] << ", " << track.mMomentum[1] << ", " << track.mMomentum[2] << ")";
  stream << "  position (" << track.mPosition[0] << ", " << track.mPosition[1] << ", " << track.mPosition[2] << ")";
  return stream;
}
} // namespace mid
} // namespace alo

namespace boost
{
namespace serialization
{
template <class Archive>
void serialize(Archive& ar, alo::mid::TrackMC& track, const unsigned int version)
{
  /// Non-intrusive boost serialization
  ar& track.mMomentum& track.mPosition;
}
} // namespace serialization
} // namespace boost
#endif /* ALO_MID_TRACKMC_H */
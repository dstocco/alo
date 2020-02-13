// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#ifndef ALO_MID_TRACK_H
#define ALO_MID_TRACK_H

#include <iostream>
#include <array>

#include <boost/serialization/access.hpp>
#include <boost/serialization/array.hpp>

namespace alo
{
namespace mid
{
struct Track {
  std::array<float, 3> mMomentum{};             ///< Momentum at vertex
  std::array<float, 3> mPosition{};             ///< Position at first trigger chamber
  std::array<float, 3> mDirection{};            ///< Direction at first trigger chamber
  std::array<float, 6> mCovarianceParameters{}; ///< Covariance parameters at first trigger chamber
  bool mIsMatched = false;                      ///< Tracker track matches trigger
};

inline std::ostream& operator<<(std::ostream& stream, const Track& track)
{
  /// Overload ostream operator
  stream << "Momentum at vertex: (" << track.mMomentum[0] << ", " << track.mMomentum[1] << ", " << track.mMomentum[2] << ")";
  stream << "  Parameters at first trigger chambers: position: (" << track.mPosition[0] << ", " << track.mPosition[1] << ", " << track.mPosition[2] << ")";
  stream << " Direction: (" << track.mDirection[0] << ", " << track.mDirection[1] << ", " << track.mDirection[2] << ")";
  stream << " Covariance (X, Y, SlopeX, SlopeY, X-SlopeX, Y-SlopeY): (";
  for (int ival = 0; ival < 6; ++ival) {
    stream << track.mCovarianceParameters[ival];
    stream << ((ival == 5) ? ")" : ", ");
  }
  stream << " MatchTrigger: " << track.mIsMatched;
  return stream;
}
} // namespace mid
} // namespace alo

namespace boost
{
namespace serialization
{
template <class Archive>
void serialize(Archive& ar, alo::mid::Track& track, const unsigned int version)
{
  /// Non-intrusive boost serialization
  ar& track.mMomentum& track.mPosition& track.mDirection& track.mCovarianceParameters& track.mIsMatched;
}
} // namespace serialization
} // namespace boost
#endif /* ALO_MID_TRACK_H */
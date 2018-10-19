// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#ifndef O2_MID_HIT_H
#define O2_MID_HIT_H

#include <array>

#include <boost/serialization/access.hpp>
#include <boost/serialization/array.hpp>

namespace alo
{
namespace mid
{
struct Hit
{
    int trackId;                      /// Track ID
    int deId;                         /// Detector ID
    std::array<double, 3> entryPoint; /// Entry point
    std::array<double, 3> exitPoint;  /// Exit point

    friend class boost::serialization::access;

    /// Serializes the struct
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar &trackId;
        ar &deId;
        ar &entryPoint;
        ar &exitPoint;
    }
};
} // namespace mid
} // namespace alo
#endif /* O2_MID_HIT_H */
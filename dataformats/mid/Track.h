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

#include <array>

#include <boost/serialization/access.hpp>
#include <boost/serialization/array.hpp>

namespace alo
{
namespace mid
{
struct Track
{
    std::array<double, 4> mMomentum; /// Track momentum
    std::array<double, 4> mPosition; /// Track position

    friend class boost::serialization::access;

    /// Serializes the struct
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar &mMomentum;
        ar &mPosition;
    }
};
} // namespace mid
} // namespace alo
#endif /* ALO_MID_TRACK_H */
// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#ifndef ALO_MID_COLUMNDATA_H
#define ALO_MID_COLUMNDATA_H

#include <iostream>
#include <array>
#include <bitset>
#include <boost/serialization/access.hpp>
#include <boost/serialization/array.hpp>

namespace alo
{
namespace mid
{
struct ColumnData {
  uint8_t deId;                     ///< Index of the detection element
  uint8_t columnId;                 ///< Column in DE
  std::array<uint16_t, 5> patterns; ///< patterns
};

inline std::ostream& operator<<(std::ostream& os, const ColumnData& col)
{
  /// Overload ostream operator
  os << "deId: " << static_cast<int>(col.deId) << "  col: " << static_cast<int>(col.columnId);
  os << "  NBP: " << std::bitset<16>(col.patterns[4]);
  os << "  BP: ";
  for (int iline = 0; iline < 4; ++iline) {
    os << " " << std::bitset<16>(col.patterns[iline]);
  }
  return os;
}

} // namespace mid
} // namespace alo

namespace boost
{
namespace serialization
{
template <class Archive>
void serialize(Archive& ar, alo::mid::ColumnData& data, const unsigned int version)
{
  /// Non-intrusive boost serialization
  ar& data.deId& data.columnId& data.patterns;
}
} // namespace serialization
} // namespace boost
#endif
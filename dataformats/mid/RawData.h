// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#ifndef ALO_MID_RAWDATA_H
#define ALO_MID_RAWDATA_H

#include <iostream>
#include <vector>
#include "ColumnData.h"
#include <boost/serialization/access.hpp>
#include <boost/serialization/array.hpp>

namespace alo
{
namespace mid
{
struct RawData {
  uint16_t bunchCrossingID;           // Bunch crossing ID
  uint32_t orbitID;                   // Orbit ID
  uint32_t periodID;                  // Period ID
  std::vector<ColumnData> columnData; // ColumnData
};

inline std::ostream& operator<<(std::ostream& os, const RawData& data)
{
  /// Overload ostream operator
  os << "period: " << data.periodID << "  orbit: " << data.orbitID << "  BC: " << data.bunchCrossingID;
  for (auto& col : data.columnData) {
    os << "  data: " << col;
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
void serialize(Archive& ar, alo::mid::RawData& data, const unsigned int version)
{
  /// Non-intrusive boost serialization
  ar& data.bunchCrossingID& data.orbitID& data.periodID& data.columnData;
}
} // namespace serialization
} // namespace boost
#endif
// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   DataformatConverter.h
/// \brief  Free functions to convert alo dataformats to o2
/// \author Diego Stocco <Diego.Stocco at cern.ch>
/// \date   08 August 2019

#ifndef O2_MID_DATAFORMATCONVERTER_H
#define O2_MID_DATAFORMATCONVERTER_H

#include <vector>
#include <cstdint>

#include "ColumnData.h"
#include "Hit.h"
#include "RawData.h"
#include "Track.h"
#include "TrackMC.h"

#include "DataFormatsMID/ColumnData.h"
#include "DataFormatsMID/Track.h"
#include "MIDSimulation/Hit.h"

namespace alo
{
namespace mid
{
std::vector<o2::mid::Hit> convertHits(const std::vector<Hit> aloHits);
o2::mid::Track convertTrack(const TrackMC& aloTrack);
o2::mid::Track convertTrack(const Track& aloTrack);
o2::mid::ColumnData convertColumnData(const ColumnData& aloCol);
std::vector<o2::mid::ColumnData> getColumnData(const RawData& aloRawData);
inline uint64_t bcOrbitPeriodToLong(uint16_t bcId, uint32_t orbitId, uint32_t periodId) { return static_cast<uint64_t>(periodId) << 36 | static_cast<uint64_t>(orbitId) << 12 | static_cast<uint64_t>(bcId); }
} // namespace mid
} // namespace alo

#endif /* O2_MID_DATAFORMATCONVERTER_H */

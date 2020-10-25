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

#include "DataformatConverter.h"

namespace alo
{
namespace mid
{
std::vector<o2::mid::Hit> convertHits(const std::vector<Hit> aloHits)
{
  std::vector<o2::mid::Hit> hits;
  for (auto& aloHit : aloHits) {
    hits.emplace_back(aloHit.trackId, aloHit.deId, o2::math_utils::Point3D<float>(aloHit.entryPoint[0], aloHit.entryPoint[1], aloHit.entryPoint[2]), o2::math_utils::Point3D<float>(aloHit.exitPoint[0], aloHit.exitPoint[1], aloHit.exitPoint[2]));
  }
  return hits;
}

o2::mid::Track convertTrack(const TrackMC& aloTrack)
{
  o2::mid::Track track;
  track.setPosition(aloTrack.mPosition[0], aloTrack.mPosition[1], aloTrack.mPosition[2]);
  track.setDirection(aloTrack.mMomentum[0], aloTrack.mMomentum[1], aloTrack.mMomentum[2]);
  return track;
}

o2::mid::Track convertTrack(const Track& aloTrack)
{
  o2::mid::Track track;
  track.setPosition(aloTrack.mPosition[0], aloTrack.mPosition[1], aloTrack.mPosition[2]);
  track.setDirection(aloTrack.mDirection[0], aloTrack.mDirection[1], aloTrack.mDirection[2]);
  track.setCovarianceParameters(aloTrack.mCovarianceParameters);
  return track;
}

o2::mid::ColumnData convertColumnData(const ColumnData& aloCol)
{
  return o2::mid::ColumnData{ aloCol.deId, aloCol.columnId, aloCol.patterns };
}

std::vector<o2::mid::ColumnData> getColumnData(const RawData& aloRawData)
{
  std::vector<o2::mid::ColumnData> columnData;
  for (auto& aloCol : aloRawData.columnData) {
    columnData.emplace_back(convertColumnData(aloCol));
  }
  return columnData;
}

} // namespace mid
} // namespace alo

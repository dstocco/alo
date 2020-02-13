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

#ifndef ALO_MID_DATAFORMATCONVERTER_H
#define ALO_MID_DATAFORMATCONVERTER_H

#include <vector>
#include <map>

#include "ColumnData.h"

#include "AliMUONVDigitStore.h"

namespace alo
{
namespace mid
{
    int convertFromLegacyDeId(int detElemId);
    void decodeDigit(UInt_t uniqueId, int& detElemId, int& boardId, int& strip, int& cathode);
    void boardToPattern(int boardId, int detElemId, int cathode, int& deId, int& column, int& line);
    std::map<int, std::vector<alo::mid::ColumnData>> digitsToColumnData(const AliMUONVDigitStore& digitStore);
} // namespace mid
} // namespace alo

#endif /* ALO_MID_DATAFORMATCONVERTER_H */

// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   alireaders/DigitMaker.h
/// \brief  Raw reader for MID testing
/// \author Diego Stocco <Diego.Stocco at cern.ch>
/// \date   06 September 2018

#ifndef O2_MID_DIGITMAKER_H
#define O2_MID_DIGITMAKER_H

#include <vector>
#include "DataFormatsMID/ColumnData.h"
#include "AliMUONDigitStoreV2R.h"
#include "AliMUONTriggerStoreV1.h"
#include "AliMUONDigitMaker.h"
class AliRawReader;


/// Class to generate digits for MID
class DigitMaker {
public:
  DigitMaker();
  virtual ~DigitMaker() = default;

  DigitMaker(const DigitMaker &) = delete;
  DigitMaker &operator=(const DigitMaker &) = delete;
  DigitMaker(DigitMaker &&) = delete;
  DigitMaker &operator=(DigitMaker &&) = delete;

  std::vector<o2::mid::ColumnData> process(AliRawReader& rawReader);

private:
  std::vector<uint32_t> digitsToIds() const;

  AliMUONDigitMaker mDigitMaker; ///< Digit maker
  AliMUONDigitStoreV2R mDigitStore; ///< Digit store
  AliMUONTriggerStoreV1 mTriggerStore; ///< Trigger store
};

#endif /* O2_MID_DIGITMAKER_H */

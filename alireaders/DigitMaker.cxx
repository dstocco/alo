// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   alireaders/Raw.cxx
/// \brief  Implementation of a AliRoot TrackRefs reader for MID testing
/// \author Diego Stocco <Diego.Stocco at cern.ch>
/// \date   06 September 2018

#include "DigitMaker.h"

#include <iostream>

// O2
#include "MIDBase/LegacyUtility.h"

// AliRoot
#include "AliRawReader.h"
#include "AliMUONVDigit.h"

//______________________________________________________________________________
DigitMaker::DigitMaker() : mDigitMaker(), mDigitStore(), mTriggerStore()
{
  /// Default constructor
  mDigitMaker.SetMakeTriggerDigits(kTRUE);
  mDigitMaker.SetMakeTrackerDigits(kFALSE);
}

//______________________________________________________________________________
std::vector<o2::mid::ColumnData> DigitMaker::process(AliRawReader& rawReader)
{
  /// Process raw event and provide vector of column data
  /// @param rawReader Raw reader

  mDigitMaker.Raw2Digits(&rawReader, &mDigitStore, &mTriggerStore);
  return o2::mid::LegacyUtility::digitsToPattern(digitsToIds());
}

//______________________________________________________________________________
std::vector<uint32_t> DigitMaker::digitsToIds() const
{
  std::vector<uint32_t> ids;
  TIter next(mDigitStore.CreateTriggerIterator());
  AliMUONVDigit* mdig;
  while ((mdig = static_cast<AliMUONVDigit*>(next()))) {
    ids.push_back(static_cast<uint32_t>(mdig->GetUniqueID()));
  }
  return ids;
}
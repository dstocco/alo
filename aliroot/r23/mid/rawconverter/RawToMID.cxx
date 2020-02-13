// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#include "RawToMID.h"

#include <iostream>
#include <vector>
#include <map>
#include <bitset>

#include "ColumnData.h"
#include "FileHandler.h"
#include "RawData.h"
#include "DataformatConverter.h"

// AliRoot
#include "AliCDBManager.h"
#include "AliMpCDB.h"
#include "AliMpSegmentation.h"
#include "AliRawReader.h"
#include "AliMUONCDB.h"
#include "AliMUONDigitMaker.h"
#include "AliMUONDigitStoreV2R.h"

#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>

//______________________________________________________________________________
void rawToMID(const char* inputName, alo::mid::FileHandler& fileHandler, bool splitPerDE)
{
  /// Converts the Run2 raw data to the Run3 format
  /// The algorithm creates one file per detection element
  AliRawReader* rawReader = AliRawReader::Create(inputName);
  rawReader->Select("MUONTRG", 0, 1);

  AliMUONDigitMaker digitMaker;
  digitMaker.SetMakeTrackerDigits(false);
  digitMaker.SetMakeTriggerDigits(true);

  AliMUONDigitStoreV2R digitStore;

  alo::mid::RawData data;

  while (rawReader->NextEvent()) {
    digitMaker.Raw2Digits(rawReader, &digitStore);
    auto columnsMap = alo::mid::digitsToColumnData(digitStore);
    if ( columnsMap.empty() ) {
      continue;
    }
    data.bunchCrossingID = rawReader->GetBCID();
    data.orbitID = rawReader->GetOrbitID();
    data.periodID = rawReader->GetPeriod();
    data.columnData.clear();
    for ( auto& entry : columnsMap ) {
      for ( auto& col : entry.second ) {
        data.columnData.push_back(col);
      }
      if ( splitPerDE ) {
        fileHandler.getArchive(entry.first) << data;
        data.columnData.clear();
      }
    }
    if ( ! splitPerDE ) {
      fileHandler.getArchive() << data;
    }
  }
}

bool setupCDB(int run, const char* ocdb, const char* mappingOCDB)
{
  /// Setup the OCDB
  AliCDBManager::Instance()->SetDefaultStorage(ocdb);
  AliCDBManager::Instance()->SetRun(run);
  if (mappingOCDB) {
    AliCDBManager::Instance()->SetSpecificStorage("MUON/Calib/MappingData", mappingOCDB); // Save time
  }

  // load mapping
  if (!AliMpSegmentation::Instance(kFALSE)) {
    if (!AliMUONCDB::LoadMapping(kTRUE))
      return false;
  }

  AliMpCDB::LoadDDLStore();

  return true;
}
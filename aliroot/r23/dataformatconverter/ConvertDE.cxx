// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#include "ConvertDE.h"

namespace alo
{
namespace mid
{

//______________________________________________________________________________
int ConvertDE::convertFromLegacyDeId(int detElemId)
{
  /// Converts the detection element ID (Run2 format)
  /// into the new ID (Run3 format)
  int ich = (detElemId / 100 - 11);
  int irpc = (detElemId % 100 + 4) % 18;
  if (irpc >= 9)
  {
    irpc = 17 - irpc;
    ich += 4;
  }
  return ich * 9 + irpc;
}

} // namespace mid
} // namespace alo

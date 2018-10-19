// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#ifndef O2_MID_CONVERTDE_H
#define O2_MID_CONVERTDE_H

namespace alo
{
namespace mid
{
/// Set of utilities allowing to pass from the old Run2 formats to the new one
/// and viceversa.
/// These utilities are meant for testing, allowing one to compare
/// with the AliRoot input/output on existing data.
/// It is of course doomed to disappear with time.
struct ConvertDE
{
  static int convertFromLegacyDeId(int detElemId);
};
} // namespace mid
} // namespace alo

#endif /* O2_MID_CONVERTDE_H */

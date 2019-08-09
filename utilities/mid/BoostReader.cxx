// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#include "BoostReader.h"

namespace alo
{
namespace mid
{

//______________________________________________________________________________
BoostReader::BoostReader(const char* inFilename)
  : mInFile(inFilename, std::ios::binary), mInArchive(mInFile), mStreamEnd()
{
  /// Default constructor
  std::streampos archiveOffset = mInFile.tellg();
  mStreamEnd = mInFile.seekg(0, std::ios_base::end).tellg();
  mInFile.seekg(archiveOffset);
}

//______________________________________________________________________________
bool BoostReader::eof()
{
  return (mInFile.tellg() >= mStreamEnd);
}
} // namespace mid
} // namespace alo

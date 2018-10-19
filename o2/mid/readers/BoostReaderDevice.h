// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#ifndef ALO_MID_BOOSTREADERDEVICE_H
#define ALO_MID_BOOSTREADERDEVICE_H

#include <string>

#include "BoostReader.h"
#include "Framework/Output.h"
#include <Framework/InitContext.h>
#include <Framework/ProcessingContext.h>
#include <Framework/Task.h>
#include "Framework/OutputSpec.h"

namespace alo
{
namespace mid
{
class BoostReaderDevice
{
  public:
    BoostReaderDevice(const char *inputFilename, const char *outputBinding);

    void init(o2::framework::InitContext &ic);
    void run(o2::framework::ProcessingContext &pc);

  private:
    BoostReader mBoostReader;
    std::string mInputBinding;
    std::string mOutputBinding;
};
} // namespace mid
} // namespace alo

#endif
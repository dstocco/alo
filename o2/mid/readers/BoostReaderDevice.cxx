// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.


#include "BoostReaderDevice.h"
#include "Framework/DPLBoostSerializer.h"

namespace alo
{
namespace mid
{
BoostReaderDevice::BoostReaderDevice(const char* inputFilename, const char* outputBinding) : mBoostReader(inputFilename), mInputBinding(inputBinding), mOutputBinding(outputBinding){};

void BoostReaderDevice::init(o2::framework::InitContext &ic)
{
    LOG(INFO) << "Inputfile file" << inputFilename;
}

void BoostReaderDevice::run(o2::framework::ProcessingContext &pc)
{
    mClusterizer.process(patterns);

    LOG(INFO) << "Generated " << mClusterizer.getNClusters() << " Clusters.";

    if (mClusterizer.getNClusters() > 0)
    {
        o2::framework::DPLBoostSerialize(pc, mOutputBinding, mClusterizer.getClusters(), mClusterizer.getNClusters());
        LOG(INFO) << "Sent " << mClusterizer.getNClusters() << " Clusters.";
    }
}
} // namespace mid
} // namespace alo
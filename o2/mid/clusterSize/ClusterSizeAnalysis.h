// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   ClusterSizeAnalysis.h
/// \brief  Task to tune cluster size for MID
/// \author Diego Stocco <Diego.Stocco at cern.ch>
/// \date   30 July 2019

#ifndef O2_MID_CLUSTERSIZEANALYSIS_H
#define O2_MID_CLUSTERSIZEANALYSIS_H

#include <vector>

#include "MIDSimulation/ChamberResponse.h"

#include "TGraphErrors.h"

namespace o2
{
namespace mid
{

/// Class to compute the ClusterSizeAnalysis for MID
class ClusterSizeAnalysis
{
 public:
  ClusterSizeAnalysis(const char* inFilename);
  virtual ~ClusterSizeAnalysis() = default;

  bool process(const ChamberResponse& response);
  bool saveResults(const char* filename) const;

 private:
  bool readClusterSize(const char* filename);
  
  std::vector<TGraphErrors> mClusterSize; // Cluster size
};

} // namespace mid
} // namespace o2

#endif /* O2_MID_CLUSTERSIZEANALYSIS_H */

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
/// \brief  Task to assess the cluster size for MID
/// \author Diego Stocco <Diego.Stocco at cern.ch>
/// \date   06 September 2018

#ifndef O2_MID_CLUSTERSIZEANALYSIS_H
#define O2_MID_CLUSTERSIZEANALYSIS_H

#include <vector>
#include <array>

#include "MIDClustering/PreClusters.h"
#include "MIDSimulation/ChamberResponse.h"

#include "TH2.h"

namespace o2
{
namespace mid
{

/// Class to compute the ClusterSizeAnalysis for MID
class ClusterSizeAnalysis
{
 public:
  ClusterSizeAnalysis();
  virtual ~ClusterSizeAnalysis() = default;

  bool process(const std::vector<PreClusters>& preClusters);
  bool saveResults(const char* filename) const;

  bool tuneParameters(const char* inFilename, const char* outFilename, ChamberResponse& response) const;

 private:
  void initHistos();
  TH2F getFiredProbability(const TH2F& clusterSize) const;

  std::vector<TH2F> mClusterSize; // Cluster size
};

} // namespace mid
} // namespace o2

#endif /* O2_MID_CLUSTERSIZEANALYSIS_H */

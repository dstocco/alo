// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   ClusterSize.h
/// \brief  Task to produce the cluster size for MID
/// \author Diego Stocco <Diego.Stocco at cern.ch>
/// \date   30 July 2019

#ifndef O2_MID_CLUSTERSIZE_H
#define O2_MID_CLUSTERSIZE_H

#include <vector>

#include "MIDClustering/PreCluster.h"
#include "MIDClustering/PreClusterHelper.h"

#include "TH2.h"

namespace o2
{
namespace mid
{

/// Class to compute the ClusterSize for MID
class ClusterSize
{
 public:
  ClusterSize();
  virtual ~ClusterSize() = default;

  bool process(const std::vector<PreCluster>& preClusters);
  bool saveResults(const char* filename) const;

 private:
  void initHistos();
  std::vector<PreCluster> selectPreClusters(const std::vector<PreCluster>& preClusters) const;
  bool areContiguous(std::vector<PreCluster>& preClustersBP) const;

  std::vector<TH2F> mClusterSize;     // Cluster size
  PreClusterHelper mPreClusterHelper; // PreClusters helper
};

} // namespace mid
} // namespace o2

#endif /* O2_MID_CLUSTERSIZE_H */

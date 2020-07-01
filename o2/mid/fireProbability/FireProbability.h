// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   FireProbability.h
/// \brief  Task to produce the fire probability for MID
/// \author Diego Stocco <Diego.Stocco at cern.ch>
/// \date   30 July 2019

#ifndef O2_MID_FIREPROBABILITY_H
#define O2_MID_FIREPROBABILITY_H

#include <vector>
#include <unordered_map>

#include "DataFormatsMID/Cluster2D.h"
#include "MIDClustering/PreCluster.h"
#include "MIDClustering/PreClusterHelper.h"

#include "TH2.h"

namespace o2
{
namespace mid
{

/// Class to compute the FireProbability for MID
class FireProbability
{
 public:
  FireProbability();
  virtual ~FireProbability() = default;

  bool process(const std::vector<PreCluster>& preClusters, const std::unordered_map<uint8_t, std::vector<Cluster2D>>& hits);
  bool saveResults(const char* filename) const;

 private:
  void initHistos();

  std::vector<TH2F> mFireProbability; // Fire probability
  PreClusterHelper mPreClusterHelper; // PreClusters helper
};

} // namespace mid
} // namespace o2

#endif /* O2_MID_FIREPROBABILITY_H */

// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   PerformanceMC.h
/// \brief  Task to assess the MC peroformance for MID
/// \author Diego Stocco <Diego.Stocco at cern.ch>
/// \date   17 July 2018

#ifndef O2_MID_PERFORMANCEMC_H
#define O2_MID_PERFORMANCEMC_H

#include "DataFormatsMID/Cluster2D.h"
#include "DataFormatsMID/Track.h"
#include "MIDSimulation/Hit.h"
#include "TH1.h"
#include "TH2.h"
#include <vector>

namespace o2
{
namespace mid
{

/// Class to compute the MC performance for MID
class PerformanceMC
{
 public:
  PerformanceMC();
  virtual ~PerformanceMC() = default;

  PerformanceMC(const PerformanceMC&) = delete;
  PerformanceMC& operator=(const PerformanceMC&) = delete;
  PerformanceMC(PerformanceMC&&) = delete;
  PerformanceMC& operator=(PerformanceMC&&) = delete;

  bool checkClusters(const std::vector<Hit>& trackHits,
                     const std::vector<Cluster2D>& recoClusters,
                     float chi2Cut = 16.);
  bool checkTrack(const o2::mid::Track& generated,
                  const o2::mid::Track& atFirstChamber,
                  const std::vector<Hit>& hits,
                  const std::vector<Track>& recoTracks, float chi2Cut = 16.);
  bool saveResults(const char* filename) const;

 private:
  bool checkParamFromHits(double pTot, const std::vector<Hit>& trackHits);
  bool isReconstructible(const std::vector<Hit>& trackHits) const;
  void initHistos();

  std::vector<TH2F> mClusterResolution;  // Cluster resolution
  std::vector<TH1F> mClusterCounters;    // Cluster counters
  std::vector<TH2F> mTrackResolution;    // Track resolution
  std::vector<TH2F> mTrackPull;          // Track resolution over uncertainty
  std::vector<TH2F> mTrackCounters;      // Track counters
  std::vector<TH2F> mTrackParamFromHits; // Track parameters from hits
};

} // namespace mid
} // namespace o2

#endif /* O2_MID_PERFORMANCEMC_H */

// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   CompareTracks.h
/// \brief  Task to assess the MC peroformance for MID
/// \author Diego Stocco <Diego.Stocco at cern.ch>
/// \date   17 July 2018

#ifndef O2_MID_COMPARETRACKS_H
#define O2_MID_COMPARETRACKS_H

#include "DataFormatsMID/Track.h"
#include "Track.h"
#include "TH1.h"
#include "TH2.h"
#include <vector>

namespace o2
{
namespace mid
{

/// Class to compute the MC performance for MID
class CompareTracks
{
 public:
  CompareTracks();

  void process(const std::vector<alo::mid::Track>& aloTracks,
               const std::vector<Track>& o2Tracks,
               float chi2Cut = 16., bool verbose = false);
  bool saveResults(const char* filename) const;

  enum class HistoType { matchMatched,
                         matchTrackerOnly,
                         noMatchMatched,
                         noMatchTrackerOnly };

 private:
  void initHistos();

  std::vector<TH2F> mHistos; /// Output histograms
};

} // namespace mid
} // namespace o2

#endif /* O2_MID_COMPARETRACKS_H */

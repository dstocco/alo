// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   CompareTracks.cxx
/// \brief  Implementation of the task to assess the MC peroformance for MID
/// \author Diego Stocco <Diego.Stocco at cern.ch>
/// \date   17 July 2018

#include "CompareTracks.h"

#include "DataformatConverter.h"
#include "TFile.h"
#include "TSystem.h"
#include <sstream>
#include <string>

namespace o2
{
namespace mid
{

CompareTracks::CompareTracks() : mHistos()
{
  /// Default constructor
  initHistos();
}

void CompareTracks::initHistos()
{
  /// Initializes histograms

  std::vector<Double_t> pBinning;
  Double_t mom = 0.;
  for (; mom < 20.; mom += 1.) {
    pBinning.push_back(mom);
  }
  for (; mom < 60.; mom += 5.) {
    pBinning.push_back(mom);
  }
  for (; mom < 200.; mom += 10.) {
    pBinning.push_back(mom);
  }
  for (; mom <= 1000.; mom += 20.) {
    pBinning.push_back(mom);
  }

  int nEtaBins = 20;
  double minEta = -4.3, maxEta = -2.3;

  const size_t nHistos = static_cast<size_t>(HistoType::noMatchTrackerOnly) + 1;

  std::array<std::string, nHistos> hTypeName = { "match_matched", "match_trackerOnly", "noMatch_matched", "noMatch_trackerOnly" };

  for (auto& name : hTypeName) {
    mHistos.emplace_back(name.data(), name.data(),
                         pBinning.size() - 1, pBinning.data(),
                         nEtaBins, minEta, maxEta);

    mHistos.back().GetXaxis()->SetTitle("p (GeV/c)");
    mHistos.back().GetYaxis()->SetTitle("#eta");
    mHistos.back().SetDirectory(0);
  }
}

void CompareTracks::process(const std::vector<alo::mid::Track>& aloTracks,
                            const std::vector<Track>& o2Tracks,
                            float chi2Cut, bool verbose)
{
  /// Checks the track compatibility
  for (auto& aloTr : aloTracks) {
    double pTot = std::sqrt(aloTr.mMomentum[0] * aloTr.mMomentum[0] + aloTr.mMomentum[1] * aloTr.mMomentum[1] + aloTr.mMomentum[2] * aloTr.mMomentum[2]);
    double eta = 0.5 * std::log((pTot + aloTr.mMomentum[2]) / (pTot - aloTr.mMomentum[2]));
    auto track = alo::mid::convertTrack(aloTr);
    int idx = -1;
    bool foundCompatible = false;
    for (auto& o2Tr : o2Tracks) {
      if (track.isCompatible(o2Tr, chi2Cut)) {
        foundCompatible = true;
        break;
      }
    }

    if (foundCompatible) {
      if (aloTr.mIsMatched) {
        idx = static_cast<int>(HistoType::matchMatched);
      } else {
        idx = static_cast<int>(HistoType::matchTrackerOnly);
      }
    } else {
      if (aloTr.mIsMatched) {
        idx = static_cast<int>(HistoType::noMatchMatched);
      } else {
        idx = static_cast<int>(HistoType::noMatchTrackerOnly);
      }
    }

    if (verbose) {
      if (foundCompatible != aloTr.mIsMatched) {
        std::cout << "\nALO: " << track << std::endl;
        for (auto& o2Tr : o2Tracks) {
          std::cout << "O2: " << o2Tr << std::endl;
        }
      }
    }

    mHistos[idx].Fill(pTot, eta);
  }
}

//______________________________________________________________________________
bool CompareTracks::saveResults(const char* filename) const
{
  /// Save results on root file
  TFile* file = TFile::Open(gSystem->ExpandPathName(filename), "RECREATE");
  if (!file) {
    return false;
  }

  for (auto& histo : mHistos) {
    histo.Write();
  }

  delete file;
  return true;
}

} // namespace mid
} // namespace o2

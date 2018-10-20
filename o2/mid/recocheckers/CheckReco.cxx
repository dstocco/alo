// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   CheckReco.cxx
/// \brief  Implementation of the task to assess the MC peroformance for MID
/// \author Diego Stocco <Diego.Stocco at cern.ch>
/// \date   17 July 2018

#include "CheckReco.h"

#include "MIDBase/DetectorParameters.h"
#include "MIDBase/GeometryTransformer.h"
#include "TFile.h"
#include "TSystem.h"
#include <sstream>
#include <string>

namespace o2
{
namespace mid
{

//______________________________________________________________________________
CheckReco::CheckReco()
  : mClusterResolution(), mClusterCounters(), mTrackResolution(), mTrackPull(), mTrackCounters(), mTrackParamFromHits()
{
  /// Default constructor
  initHistos();
}

//______________________________________________________________________________
void CheckReco::initHistos()
{

  std::string varName[] = { "x", "y", "slope_x", "slope_y" };
  std::string varUnits[] = { "(cm)", "(cm)", "(rad)", "(rad)" };

  double minDeltaX = -10, maxDeltaX = 10;
  int nXbins = 100;

  double minDeId = -0.5, maxDeId = 72 - 0.5;
  int nDeIdBins = 72;
  std::string deIdName = "deId";

  for (int ihisto = 0; ihisto < 2; ++ihisto) {
    std::stringstream name, title, axisTitle;
    name << "cluster_residual_" << varName[ihisto];
    title << "Cluster residual in " << varName[ihisto];
    mClusterResolution.emplace_back(name.str().data(), title.str().data(),
                                    nDeIdBins, minDeId, maxDeId, nXbins,
                                    minDeltaX, maxDeltaX);
    axisTitle << varName[ihisto] << "_reco - " << varName[ihisto]
              << "_gen " << varUnits[ihisto];
    mClusterResolution.back().GetXaxis()->SetTitle(deIdName.data());
    mClusterResolution.back().GetYaxis()->SetTitle(axisTitle.str().data());
    mClusterResolution.back().SetDirectory(0);
  }

  mClusterCounters.emplace_back("cluster_generated", "Generated hits",
                                nDeIdBins, minDeId, maxDeId);
  mClusterCounters.emplace_back("cluster_reconstructed", "Reconstructed hits",
                                nDeIdBins, minDeId, maxDeId);
  for (auto& histo : mClusterCounters) {
    histo.GetXaxis()->SetTitle(deIdName.data());
    histo.SetDirectory(0);
  }

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

  std::string pAxisTitle = "p (GeV/c)";

  double minDeltaSlope = -0.2, maxDeltaSlope = 0.2;
  int nSlopeBins = 80;

  for (int ihisto = 0; ihisto < 4; ++ihisto) {
    std::stringstream name, title, axisTitle;
    name << "track_residual_" << varName[ihisto];
    title << "Track residual in " << varName[ihisto];
    int nBins = (ihisto < 2) ? nXbins : nSlopeBins;
    double min = (ihisto < 2) ? minDeltaX : minDeltaSlope;
    double max = (ihisto < 2) ? maxDeltaX : maxDeltaSlope;
    mTrackResolution.emplace_back(name.str().data(), title.str().data(), pBinning.size() - 1, pBinning.data(), nBins,
                                  min, max);
    axisTitle << varName[ihisto] << "_reco - " << varName[ihisto] << "_gen " << varUnits[ihisto];
    mTrackResolution.back().GetXaxis()->SetTitle(pAxisTitle.data());
    mTrackResolution.back().GetYaxis()->SetTitle(axisTitle.str().data());
    mTrackResolution.back().SetDirectory(0);
  }

  for (int ihisto = 0; ihisto < 4; ++ihisto) {
    std::stringstream name, title, axisTitle;
    name << "track_pull_" << varName[ihisto];
    title << "Track pull in " << varName[ihisto];
    mTrackPull.emplace_back(name.str().data(), title.str().data(), pBinning.size() - 1, pBinning.data(), 120,
                            -6., 6.);
    axisTitle << "(" << varName[ihisto] << "_reco - " << varName[ihisto] << "_gen) / (sigma)";
    mTrackPull.back().GetXaxis()->SetTitle(pAxisTitle.data());
    mTrackPull.back().GetYaxis()->SetTitle(axisTitle.str().data());
    mTrackPull.back().SetDirectory(0);
  }

  int nEtaBins = 20;
  double minEta = -4.3, maxEta = -2.3;

  mTrackCounters.emplace_back("track_generated", "Generated tracks",
                              pBinning.size() - 1, pBinning.data(), nEtaBins,
                              minEta, maxEta);
  mTrackCounters.emplace_back("track_reconstructed", "Reconstructed tracks",
                              pBinning.size() - 1, pBinning.data(), nEtaBins,
                              minEta, maxEta);

  for (auto& histo : mTrackCounters) {
    histo.GetXaxis()->SetTitle(pAxisTitle.data());
    histo.GetYaxis()->SetTitle("#eta");
    histo.SetDirectory(0);
  }

  std::string varName2[] = { "impact_x", "impact_y", "slope_x", "slope_y" };
  for (int ihisto = 0; ihisto < 4; ++ihisto) {
    std::stringstream name, title, axisTitle;
    name << "track_fromHits_" << varName2[ihisto];
    title << "Track from hits " << varName2[ihisto];
    int nBins = (ihisto < 2) ? 500 : 200;
    double min = (ihisto < 2) ? -250 : -0.4;
    double max = (ihisto < 2) ? 250 : 0.4;
    mTrackParamFromHits.emplace_back(name.str().data(), title.str().data(),
                                     pBinning.size() - 1, pBinning.data(),
                                     nBins, min, max);
    axisTitle << varName2[ihisto];
    axisTitle << ((ihisto < 2) ? " (cm)" : " (rad)");
    mTrackParamFromHits.back().GetXaxis()->SetTitle(pAxisTitle.data());
    mTrackParamFromHits.back().GetYaxis()->SetTitle(axisTitle.str().data());
    mTrackParamFromHits.back().SetDirectory(0);
  }
}

//______________________________________________________________________________
bool CheckReco::checkClusters(const std::vector<Hit>& trackHits,
                              const std::vector<Cluster2D>& recoClusters,
                              const GeometryTransformer& transformer,
                              float chi2Cut)
{
  /// Checks the cluster resolution
  for (auto& hit : trackHits) {
    int nMatched = 0;
    int deId = hit.GetDetectorID();
    for (auto& cluster : recoClusters) {
      if (deId == (int)cluster.deId) {
        auto localEntrance = transformer.globalToLocal(hit.GetDetectorID(), hit.entrancePoint());
        auto localExit = transformer.globalToLocal(hit.GetDetectorID(), hit.exitPoint());
        double deltaX = cluster.xCoor - 0.5 * (localEntrance.x() + localExit.x());
        double deltaY = cluster.yCoor - 0.5 * (localEntrance.y() + localExit.y());
        if (deltaX * deltaX / cluster.sigmaX2 < chi2Cut &&
            deltaY * deltaY / cluster.sigmaY2 < chi2Cut) {
          mClusterResolution[0].Fill(deId, deltaX);
          mClusterResolution[1].Fill(deId, deltaY);
          ++nMatched;
        }
      }
    }
    mClusterCounters[0].Fill(deId);
    if (nMatched > 0) {
      mClusterCounters[1].Fill(deId);
    } else {
      std::cerr << "CheckReco::checkClusters:\n"
                << hit << " => Clusters:\n";
      for (auto& cluster : recoClusters) {
        std::cerr << "   " << (int)cluster.deId << "  (" << cluster.xCoor
                  << ", " << cluster.yCoor << ") (" << cluster.sigmaX2 << ", "
                  << cluster.sigmaY2 << ")\n";
      }
      return false;
    }
  }
  return true;
}

//______________________________________________________________________________
bool CheckReco::isReconstructible(const std::vector<Hit>& trackHits) const
{
  /// Checks if the track is reconstructible
  int firedChambers = 0;
  for (auto& hit : trackHits) {
    firedChambers |= (1 << o2::mid::detparams::getChamber(hit.GetDetectorID()));
  }
  int nFired = 0;
  for (int ich = 0; ich < 4; ++ich) {
    if (firedChambers & (1 << ich)) {
      ++nFired;
    }
  }

  return (nFired >= 3);
}

//______________________________________________________________________________
bool CheckReco::checkParamFromHits(double pTot,
                                   const std::vector<Hit>& trackHits)
{
  /// Fills the histograms with the slope and impact parameters
  /// obtained from the hits on the first and last chamber.
  /// This is useful to define the seed for the tracking.
  Point3D<float> first(trackHits[0].entrancePoint());
  Point3D<float> last(trackHits.back().exitPoint());
  double dZ = last.z() - first.z();
  double firstPos[2] = { first.x(), first.y() };
  double lastPos[2] = { last.x(), last.y() };
  for (int ipos = 0; ipos < 2; ++ipos) {
    double slope = (lastPos[ipos] - firstPos[ipos]) / dZ;
    mTrackParamFromHits[ipos + 2].Fill(pTot, slope);
    double impactParam = firstPos[ipos] - first.z() * slope;
    mTrackParamFromHits[ipos].Fill(pTot, impactParam);
  }

  return true;
}

//______________________________________________________________________________
bool CheckReco::checkTrack(const o2::mid::Track& generated,
                           const o2::mid::Track& atFirstChamber,
                           const std::vector<Hit>& hits,
                           const std::vector<Track>& recoTracks,
                           float chi2Cut)
{
  /// Checks the track resolution
  if (hits.empty()) {
    return true;
  }

  bool isRecon = isReconstructible(hits);
  if (!isRecon) {
    return true;
  }

  double px = generated.getDirectionX();
  double py = generated.getDirectionY();
  double pz = generated.getDirectionZ();
  double pTot = std::sqrt(px * px + py * py + pz * pz);
  // double pt = std::sqrt(px * px + py * py);
  double eta = 0.5 * std::log((pTot + pz) / (pTot - pz));

  int nMatches = 0;
  double delta[4], sigma2[4];
  for (auto& track : recoTracks) {
    if (track.isCompatible(atFirstChamber, chi2Cut)) {
      delta[0] = track.getPositionX() -
                 atFirstChamber.getPositionX();
      sigma2[0] = track.getCovarianceParameter(Track::CovarianceParamIndex::VarX);
      delta[1] = track.getPositionY() -
                 atFirstChamber.getPositionY();
      sigma2[1] = track.getCovarianceParameter(Track::CovarianceParamIndex::VarY);
      delta[2] = track.getDirectionX() -
                 atFirstChamber.getDirectionX();
      sigma2[2] = track.getCovarianceParameter(Track::CovarianceParamIndex::VarSlopeX);
      delta[3] = track.getDirectionY() -
                 atFirstChamber.getDirectionY();
      sigma2[3] = track.getCovarianceParameter(Track::CovarianceParamIndex::VarSlopeY);
      for (int ivar = 0; ivar < 4; ++ivar) {
        mTrackResolution[ivar].Fill(pTot, delta[ivar]);
        mTrackPull[ivar].Fill(pTot, delta[ivar] / std::sqrt(sigma2[ivar]));
      }
      ++nMatches;
    }
  }

  checkParamFromHits(pTot, hits);

  mTrackCounters[0].Fill(pTot, eta);
  if (nMatches > 0) {
    mTrackCounters[1].Fill(pTot, eta);
  } else {
    std::cerr << "Generated momentum: " << pTot << "  " << atFirstChamber << " => Tracks:\n";
    for (auto& track : recoTracks) {
      std::cerr << "  " << track << "\n";
    }
    return false;
  }

  return true;
}

//______________________________________________________________________________
bool CheckReco::saveResults(const char* filename) const
{
  /// Save results on root file
  TFile* file = TFile::Open(gSystem->ExpandPathName(filename), "RECREATE");
  if (!file) {
    return false;
  }

  for (auto& histo : mClusterResolution) {
    histo.Write();
  }

  for (auto& histo : mClusterCounters) {
    histo.Write();
  }

  for (auto& histo : mTrackResolution) {
    histo.Write();
  }

  for (auto& histo : mTrackPull) {
    histo.Write();
  }

  for (auto& histo : mTrackCounters) {
    histo.Write();
  }

  for (auto& histo : mTrackParamFromHits) {
    histo.Write();
  }

  delete file;
  return true;
}

} // namespace mid
} // namespace o2

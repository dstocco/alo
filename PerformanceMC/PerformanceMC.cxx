// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   PerformanceMC.cxx
/// \brief  Implementation of the task to assess the MC peroformance for MID
/// \author Diego Stocco <Diego.Stocco at cern.ch>
/// \date   17 July 2018

#include "PerformanceMC.h"

#include "MIDBase/Constants.h"
#include "TFile.h"
#include "TSystem.h"
#include <sstream>
#include <string>

namespace o2 {
namespace mid {

//______________________________________________________________________________
PerformanceMC::PerformanceMC()
    : mClusterResolution(), mClusterCounters(), mTrackResolution(),
      mTrackCounters(), mTrackParamFromHits() {
  /// Default constructor
  initHistos();
}

//______________________________________________________________________________
void PerformanceMC::initHistos() {

  std::string varName[] = {"x", "y", "slope_x", "slope_y"};

  double minDeltaX = -10, maxDeltaX = 10;
  int nXbins = 100;

  double minDeId = -0.5, maxDeId = 72 - 0.5;
  int nDeIdBins = 72;

  for (int ihisto = 0; ihisto < 2; ++ihisto) {
    std::stringstream name, title, axisTitle;
    name << "cluster_residual_" << varName[ihisto];
    title << "Cluster residual in " << varName[ihisto];
    mClusterResolution.emplace_back(name.str().data(), title.str().data(),
                                    nDeIdBins, minDeId, maxDeId, nXbins,
                                    minDeltaX, maxDeltaX);
    axisTitle << varName[ihisto] << "_reco - " << varName[ihisto]
              << "_gen (cm)";
    mClusterResolution.back().GetXaxis()->SetTitle("deId");
    mClusterResolution.back().GetYaxis()->SetTitle(axisTitle.str().data());
    mClusterResolution.back().SetDirectory(0);
  }

  mClusterCounters.emplace_back("cluster_generated", "Generated hits",
                                nDeIdBins, minDeId, maxDeId);
  mClusterCounters.emplace_back("cluster_reconstructed", "Reconstructed hits",
                                nDeIdBins, minDeId, maxDeId);
  for (auto &histo : mClusterCounters) {
    histo.GetXaxis()->SetTitle("deId");
    histo.SetDirectory(0);
  }

  for (int ihisto = 0; ihisto < 4; ++ihisto) {
    std::stringstream name, title, axisTitle;
    name << "track_residual_" << varName[ihisto];
    title << "Track residual in " << varName[ihisto];
    int nBins = (ihisto < 2) ? nXbins : 80;
    double min = (ihisto < 2) ? minDeltaX : -0.2;
    double max = (ihisto < 2) ? maxDeltaX : 0.2;
    mTrackResolution.emplace_back(name.str().data(), title.str().data(), nBins,
                                  min, max);
    axisTitle << varName[ihisto] << "_reco - " << varName[ihisto] << "_gen ";
    axisTitle << ((ihisto < 2) ? "(cm)" : "(rad)");
    mTrackResolution.back().GetXaxis()->SetTitle(axisTitle.str().data());
    mTrackResolution.back().SetDirectory(0);
  }

  std::vector<Double_t> ptBinning;
  for (Double_t pt = 0.; pt < 2.; pt += 0.1) {
    ptBinning.push_back(pt);
  }
  for (Double_t pt = 2.; pt < 6.; pt += 0.5) {
    ptBinning.push_back(pt);
  }
  for (Double_t pt = 6.; pt < 20.; pt += 1.) {
    ptBinning.push_back(pt);
  }
  for (Double_t pt = 20.; pt < 100.; pt += 2.) {
    ptBinning.push_back(pt);
  }

  int nEtaBins = 20;
  double minEta = -4.3, maxEta = -2.3;

  mTrackCounters.emplace_back("track_generated", "Generated tracks",
                              ptBinning.size() - 1, ptBinning.data(), nEtaBins,
                              minEta, maxEta);
  mTrackCounters.emplace_back("track_reconstructed", "Reconstructed tracks",
                              ptBinning.size() - 1, ptBinning.data(), nEtaBins,
                              minEta, maxEta);

  for (auto &histo : mTrackCounters) {
    histo.GetXaxis()->SetTitle("p_{T} (GeV/c)");
    histo.GetYaxis()->SetTitle("#eta");
    histo.SetDirectory(0);
  }

  for (int ihisto = 0; ihisto < 4; ++ihisto) {
    std::stringstream name, title, axisTitle;
    name << "track_fromHits_" << varName[ihisto];
    title << "Track from hits " << varName[ihisto];
    int nBins = (ihisto < 2) ? 500 : 200;
    double min = (ihisto < 2) ? -250 : -0.4;
    double max = (ihisto < 2) ? 250 : 0.4;
    mTrackParamFromHits.emplace_back(name.str().data(), title.str().data(),
                                     ptBinning.size() - 1, ptBinning.data(),
                                     nBins, min, max);
    axisTitle << varName[ihisto];
    axisTitle << ((ihisto < 2) ? " (cm)" : " (rad)");
    mTrackParamFromHits.back().GetXaxis()->SetTitle("p_{T} (GeV/c)");
    mTrackParamFromHits.back().GetYaxis()->SetTitle(axisTitle.str().data());
    mTrackParamFromHits.back().SetDirectory(0);
  }
} // namespace mid

//______________________________________________________________________________
bool PerformanceMC::checkClusters(const std::vector<Hit> &trackHits,
                                  const std::vector<Cluster2D> &recoClusters,
                                  float chi2Cut) {
  /// Checks the cluster resolution
  for (auto &hit : trackHits) {
    int nMatched = 0;
    int deId = hit.GetDetectorID();
    for (auto &cluster : recoClusters) {
      if (deId == (int)cluster.deId) {
        double deltaX = cluster.xCoor - hit.entrancePoint().x();
        mClusterResolution[0].Fill(deId, deltaX);
        double deltaY = cluster.yCoor - hit.entrancePoint().y();
        mClusterResolution[1].Fill(deId, deltaY);
        if (deltaX * deltaX / cluster.sigmaX2 < chi2Cut &&
            deltaY * deltaY / cluster.sigmaY2 < chi2Cut)
          ++nMatched;
      }
    }
    mClusterCounters[0].Fill(deId);
    if (nMatched > 0) {
      mClusterCounters[1].Fill(deId);
    } else {
      std::cerr << hit << " => Clusters:\n";
      for (auto &cluster : recoClusters) {
        std::cerr << "   " << (int)cluster.deId << "  (" << cluster.xCoor
                  << ", " << cluster.yCoor << ") (" << cluster.sigmaX2 << ", "
                  << cluster.sigmaY2 << ")";
      }
      std::cerr << std::endl;
      return false;
    }
  }
  return true;
} // namespace mid

//______________________________________________________________________________
bool PerformanceMC::isReconstructible(const std::vector<Hit> &trackHits) const {
  /// Checks if the track is reconstructible
  int firedChambers = 0;
  for (auto &hit : trackHits) {
    firedChambers |= (1 << o2::mid::Constants::getChamber(hit.GetDetectorID()));
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
bool PerformanceMC::checkParamFromHits(double pt,
                                       const std::vector<Hit> &trackHits) {
  // Returns the slope from the first and last hit
  Point3D<float> first(trackHits[0].entrancePoint());
  Point3D<float> last(trackHits.back().exitPoint());
  double dZ = last.z() - first.z();
  double firstPos[2] = {first.x(), first.y()};
  double lastPos[2] = {last.x(), last.y()};
  for (int ipos = 0; ipos < 2; ++ipos) {
    double slope = (lastPos[ipos] - firstPos[ipos]) / dZ;
    mTrackParamFromHits[ipos + 2].Fill(pt, slope);
    double impactParam = firstPos[ipos] - first.z() * slope;
    mTrackParamFromHits[ipos].Fill(pt, impactParam);
  }

  return true;
}

//______________________________________________________________________________
bool PerformanceMC::checkTrack(const o2::mid::Track &generated,
                               const o2::mid::Track &atFirstChamber,
                               const std::vector<Hit> &hits,
                               const std::vector<Track> &recoTracks,
                               float chi2Cut) {
  /// Checks the track resolution
  if (hits.empty()) {
    return true;
  }

  bool isRecon = isReconstructible(hits);
  if (!isRecon) {
    return true;
  }

  double px = generated.getDirection().x();
  double py = generated.getDirection().y();
  double pz = generated.getDirection().z();
  double pTot = std::sqrt(px * px + py * py + pz * pz);
  double pt = std::sqrt(px * px + py * py);
  double eta = 0.5 * std::log((pTot + pz) / (pTot - pz));

  int nMatches = 0;
  for (auto &track : recoTracks) {
    if (track.isCompatible(atFirstChamber, chi2Cut)) {
      mTrackResolution[0].Fill(track.getPosition().x() -
                               atFirstChamber.getPosition().x());
      mTrackResolution[1].Fill(track.getPosition().y() -
                               atFirstChamber.getPosition().y());
      mTrackResolution[2].Fill(track.getDirection().x() -
                               atFirstChamber.getDirection().x());
      mTrackResolution[3].Fill(track.getDirection().y() -
                               atFirstChamber.getDirection().y());
      ++nMatches;
    }
  }

  checkParamFromHits(pt, hits);

  mTrackCounters[0].Fill(pt, eta);
  if (nMatches > 0) {
    mTrackCounters[1].Fill(pt, eta);
  } else {
    std::cerr << atFirstChamber << " => Tracks:\n";
    for (auto &track : recoTracks) {
      std::cerr << "  " << track;
    }
    std::cerr << std::endl;
    return false;
  }

  return true;
}

//______________________________________________________________________________
bool PerformanceMC::saveResults(const char *filename) const {
  /// Save results on root file
  TFile *file = TFile::Open(gSystem->ExpandPathName(filename), "RECREATE");
  if (!file) {
    return false;
  }

  for (auto &histo : mClusterResolution) {
    histo.Write();
  }

  for (auto &histo : mClusterCounters) {
    histo.Write();
  }

  for (auto &histo : mTrackResolution) {
    histo.Write();
  }

  for (auto &histo : mTrackCounters) {
    histo.Write();
  }

  for (auto &histo : mTrackParamFromHits) {
    histo.Write();
  }

  delete file;
  return true;
}

} // namespace mid
} // namespace o2

// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   FireProbability.cxx
/// \brief  Implementation of the task to produce the fire probability for MID
/// \author Diego Stocco <Diego.Stocco at cern.ch>
/// \date   30 July 2019

#include "FireProbability.h"

#include <sstream>
#include <string>
#include <algorithm>
#include "TFile.h"
#include "TSystem.h"
#include "TF1.h"
#include "MIDBase/DetectorParameters.h"

namespace o2
{
namespace mid
{

FireProbability::FireProbability()
  : mFireProbability(), mPreClusterHelper()
{
  /// Default constructor
  initHistos();
}

void FireProbability::initHistos()
{
  /// Initializes histograms

  for (int ide = 0; ide < detparams::NDetectionElements; ++ide) {
    std::stringstream name, title;
    name << "cluster_size_deId_" << ide;
    title << "Cluster size for DE " << detparams::getDEName(ide).c_str();
    mFireProbability.emplace_back(name.str().data(), title.str().data(), 8, -1.5, 6.5, 560, 0., 280.);
    mFireProbability.back().GetXaxis()->SetTitle("Column (first is NBP)");
    mFireProbability.back().GetYaxis()->SetTitle("Cluster size (cm)");
    mFireProbability.back().SetDirectory(0);
  }
}

bool FireProbability::process(const std::vector<PreCluster>& preClusters, const std::unordered_map<uint8_t, std::vector<Cluster2D>>& hits)
{
  /// Checks the cluster resolution
  double coor = 0., min = 0., max = 0., bin = 0.;
  for (auto& pc : preClusters) {
    auto item = hits.find(pc.deId);
    if (item == hits.end() || item->second.size() > 1) {
      continue;
    }
    MpArea area = mPreClusterHelper.getArea(pc);
    if (pc.cathode == 0) {
      bin = static_cast<double>(pc.firstColumn);
      coor = item->second.back().yCoor;
      min = area.getYmin();
      max = area.getYmax();
    } else {
      bin = -1.;
      coor = item->second.back().xCoor;
      min = area.getXmin();
      max = area.getXmax();
    }
    double d1 = max - coor;
    double d2 = coor - min;
    if (d1 < 0. || d2 < 0.) {
      continue;
    }
    double delta = (d1 > d2) ? d1 : d2;
    mFireProbability[pc.deId].Fill(bin, delta);
  }

  return true;
}

bool FireProbability::saveResults(const char* filename) const
{
  /// Save results on root file
  std::unique_ptr<TFile> file{ TFile::Open(gSystem->ExpandPathName(filename), "RECREATE") };
  if (!file) {
    return false;
  }

  for (auto& histo : mFireProbability) {
    histo.Write();
  }

  return true;
}

} // namespace mid
} // namespace o2

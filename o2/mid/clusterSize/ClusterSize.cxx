// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   ClusterSize.cxx
/// \brief  Implementation of the task to produce the cluster size for MID
/// \author Diego Stocco <Diego.Stocco at cern.ch>
/// \date   30 July 2019

#include "ClusterSize.h"

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

//______________________________________________________________________________
ClusterSize::ClusterSize()
  : mClusterSize(), mPreClusterHelper()
{
  /// Default constructor
  initHistos();
}

//______________________________________________________________________________
void ClusterSize::initHistos()
{
  /// Initializes histograms

  for (int ide = 0; ide < detparams::NDetectionElements; ++ide) {
    std::stringstream name, title;
    name << "cluster_size_deId_" << ide;
    title << "Cluster size for DE " << detparams::getDEName(ide).c_str();
    mClusterSize.emplace_back(name.str().data(), title.str().data(), 8, -1.5, 6.5, 560, 0., 280.);
    mClusterSize.back().GetXaxis()->SetTitle("Column (first is NBP)");
    mClusterSize.back().GetYaxis()->SetTitle("Cluster size (cm)");
    mClusterSize.back().SetDirectory(0);
  }
}

//______________________________________________________________________________
bool ClusterSize::process(const std::vector<PreCluster>& preClusters)
{
  /// Checks the cluster resolution
  std::vector<PreCluster> selected = selectPreClusters(preClusters);
  for (auto& pc : selected) {
    MpArea area = mPreClusterHelper.getArea(pc);
    double size = (pc.cathode == 0) ? area.getYmax() - area.getYmin() : area.getXmax() - area.getXmin();
    double bin = (pc.cathode == 0) ? static_cast<double>(pc.firstColumn) : -1.;
    mClusterSize[pc.deId].Fill(bin, size);
  }

  return true;
}

//______________________________________________________________________________
bool ClusterSize::areContiguous(std::vector<PreCluster>& preClustersBP) const
{
  /// Checks if the pre-clusters in the bending plane are all contiguous
  if (preClustersBP.empty()) {
    return true;
  }
  std::sort(preClustersBP.begin(), preClustersBP.end(), [](const PreCluster& pc1, const PreCluster& pc2) -> bool { return pc1.firstColumn < pc2.firstColumn; });
  auto it = preClustersBP.begin();
  auto col = it->firstColumn;
  MpArea area = mPreClusterHelper.getArea(*it);
  ++it;
  for (; it != preClustersBP.end(); ++it) {
    if (it->firstColumn - col != 1) {
      return false;
    }
    MpArea currArea = mPreClusterHelper.getArea(*it);
    if (currArea.getYmin() > area.getYmax() || currArea.getYmax() < area.getYmin()) {
      return false;
    }
    col = it->firstColumn;
    area = currArea;
  }
  return true;
}

//______________________________________________________________________________
std::vector<PreCluster> ClusterSize::selectPreClusters(const std::vector<PreCluster>& preClusters) const
{
  /// Selects the preclusters to be kept
  /// We reject the detection elements with many PCs.
  std::vector<PreCluster> pcNBP, pcBP, selected;
  for (int ide = 0; ide < detparams::NDetectionElements; ++ide) {
    for (auto& pc : preClusters) {
      if (static_cast<int>(pc.deId) == ide) {
        if (pc.cathode == 0) {
          pcBP.emplace_back(pc);
        } else {
          pcNBP.emplace_back(pc);
        }
      }
    }
    if (pcNBP.size() <= 1 && areContiguous(pcBP)) {
      selected.insert(selected.end(), pcBP.begin(), pcBP.end());
      selected.insert(selected.end(), pcNBP.begin(), pcNBP.end());
    }
    pcBP.clear();
    pcNBP.clear();
  }
  return selected;
}

//______________________________________________________________________________
bool ClusterSize::saveResults(const char* filename) const
{
  /// Save results on root file
  std::unique_ptr<TFile> file{ TFile::Open(gSystem->ExpandPathName(filename), "RECREATE") };
  if (!file) {
    return false;
  }

  for (auto& histo : mClusterSize) {
    histo.Write();
  }

  return true;
}

} // namespace mid
} // namespace o2

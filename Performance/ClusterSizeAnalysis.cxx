// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   ClusterSizeAnalysis.cxx
/// \brief  Implementation of the task to assess the MC peroformance for MID
/// \author Diego Stocco <Diego.Stocco at cern.ch>
/// \date   06 September 2018

#include "ClusterSizeAnalysis.h"

#include "MIDBase/Constants.h"
#include "MIDSimulation/ChamberResponseParams.h"
#include "TFile.h"
#include "TSystem.h"
#include "TF1.h"
#include <sstream>
#include <string>

namespace o2
{
namespace mid
{

//______________________________________________________________________________
ClusterSizeAnalysis::ClusterSizeAnalysis()
  : mClusterSize()
{
  /// Default constructor
  initHistos();
}

//______________________________________________________________________________
void ClusterSizeAnalysis::initHistos()
{
  /// Initializes histograms

  for (int ide = 0; ide < Constants::sNDetectionElements; ++ide) {
    std::stringstream name, title;
    name << "cluster_size_deId_" << ide;
    title << "Cluster size for DE " << Constants::getDEName(ide).c_str();
    mClusterSize.emplace_back(name.str().data(), title.str().data(), 8, -1.5, 6.5, 560, 0., 280.);
    mClusterSize.back().GetXaxis()->SetTitle("Column (first is NBP)");
    mClusterSize.back().GetYaxis()->SetTitle("Cluster size (cm)");
    mClusterSize.back().SetDirectory(0);
  }

  // std::string varName[] = { "x", "y" };

  // double minDeId = -0.5, maxDeId = Constants::sNDetectionElements - 0.5;
  // int nDeIdBins = Constants::sNDetectionElements;
  // std::string deIdName = "deId";

  // for (int ihisto = 0; ihisto < 2; ++ihisto) {
  //   for (int ide = 0; ide < Constants::sNDetectionElements; ++ide) {
  //     std::stringstream name, title, axisTitle;
  //     name << "cluster_size_" << varName[ihisto];
  //     title << "Cluster size in " << varName[ihisto];
  //     double maxSize = (ihisto == 0) ? 280. : 75;
  //     int nXbins = (ihisto == 0) ? 280. : 150;
  //     mClusterSize.emplace_back(name.str().data(), title.str().data(),
  //                               nDeIdBins, minDeId, maxDeId, nXbins,
  //                               0., maxSize);
  //     axisTitle << "Cluster size " << varName[ihisto] << " (cm)";
  //     mClusterSize.back().GetXaxis()->SetTitle(deIdName.data());
  //     mClusterSize.back().GetYaxis()->SetTitle(axisTitle.str().data());
  //     mClusterSize.back().SetDirectory(0);
  //   }
  // }
}

//______________________________________________________________________________
bool ClusterSizeAnalysis::process(const std::vector<PreClusters>& preClusters)
{
  /// Checks the cluster resolution
  for (auto& pcs : preClusters) {
    int deId = pcs.getDEId();
    for (int ipc = 0; ipc < pcs.getNPreClustersNBP(); ++ipc) {
      const auto& pc = pcs.getPreClusterNBP(ipc);
      double size = pc.area[pc.lastColumn].getXmax() - pc.area[pc.firstColumn].getXmin();
      mClusterSize[deId].Fill(-1., size);
    }
    for (int icol = 0; icol < 7; ++icol) {
      for (int ipc = 0; ipc < pcs.getNPreClustersBP(icol); ++ipc) {
        const auto& pc = pcs.getPreClusterBP(icol, ipc);
        double size = pc.area.getYmax() - pc.area.getYmin();
        mClusterSize[deId].Fill(icol, size);
      }
    }
  }
  return true;
}

//______________________________________________________________________________
TH2F ClusterSizeAnalysis::getFiredProbability(const TH2F& clusterSize) const
{
  /// From the cluster size, gets the probability of firing a strip
  /// This assumes that the fired strips are symmetric around the impact point.

  std::string name = clusterSize.GetName();
  name.replace(0, 12, "fired_probability");
  std::string title = clusterSize.GetTitle();
  title.replace(0, 12, "Fired probability");

  const TAxis* xAxis = clusterSize.GetXaxis();
  const TAxis* yAxis = clusterSize.GetYaxis();

  TH2F histo(name.data(), title.data(), xAxis->GetNbins(), xAxis->GetXmin(), xAxis->GetXmax(),
             yAxis->GetNbins(), 0., yAxis->GetXmax() / 2.);
  histo.GetXaxis()->SetTitle(xAxis->GetTitle());
  histo.GetYaxis()->SetTitle("Distance (cm)");
  histo.SetDirectory(0);
  for (int xbin = 1; xbin <= xAxis->GetNbins(); ++xbin) {
    for (int ybin = 1; ybin <= yAxis->GetNbins(); ++ybin) {
      double dist = yAxis->GetBinUpEdge(ybin) / 2.;
      double weight = clusterSize.GetBinContent(xbin, ybin);
      if (weight == 0.) {
        continue;
      }
      for (int ibin = 1; ibin <= histo.GetYaxis()->GetNbins(); ++ibin) {
        double pos = 0.999 * histo.GetYaxis()->GetBinUpEdge(ibin);
        if (pos > dist) {
          break;
        }
        histo.Fill(xAxis->GetBinCenter(xbin), pos, weight);
      }
    }
  }

  return histo;
}

//______________________________________________________________________________
bool ClusterSizeAnalysis::tuneParameters(const char* inFilename, const char* outFilename, ChamberResponse& response) const
{
  /// Tunes parameters
  std::unique_ptr<TFile> file(TFile::Open(inFilename));
  if (!file) {
    std::cerr << "Error: cannot open " << inFilename << std::endl;
    return false;
  }

  TF1 func("fitFunc", &response, &ChamberResponse::firedProbabilityFunction, 0., 200., 8, "ChamberResponse", "firedProbabilityFunction");
  func.FixParameter(2, 0.);
  func.FixParameter(4, response.getResponseParams().getParametersA()[0]);
  func.FixParameter(5, response.getResponseParams().getParametersA()[1]);
  func.FixParameter(6, response.getResponseParams().getParametersC()[0]);
  func.FixParameter(7, response.getResponseParams().getParametersC()[1]);

  func.SetParNames("Cath", "deId", "Theta", "B", "A0", "A1", "C0", "C1");

  std::vector<std::unique_ptr<TH1D>> fitHistos;

  ChamberResponseParams tunedParams(response.getResponseParams());

  for (int ide = 0; ide < Constants::sNDetectionElements; ++ide) {
    std::stringstream histoName;
    histoName << "fired_probability_deId_" << ide;
    TH2* histo2D = static_cast<TH2*>(file->Get(histoName.str().data()));
    if (!histo2D) {
      std::cerr << "Warning: cannot find " << histoName.str() << std::endl;
      return false;
    }
    for (int icath = 0; icath < 2; ++icath) {
      func.FixParameter(0, (double)icath);
      func.FixParameter(1, (double)ide);
      func.SetParameter(3, response.getResponseParams().getParB(icath, ide));
      int minBin = (icath == 0) ? 2 : 1;
      int maxBin = (icath == 0) ? 8 : 1;
      fitHistos.emplace_back(histo2D->ProjectionY(Form("%s_cath_%i", histo2D->GetName(), icath), minBin, maxBin));
      fitHistos.back()->SetDirectory(0);
      if ( fitHistos.back()->GetSumw2N() == 0 ) {
        fitHistos.back()->Sumw2();
      }
      double scale = fitHistos.back()->GetBinContent(1);
      if (scale > 0.) {
        fitHistos.back()->Scale(1. / scale);
      }
      std::stringstream ss;
      ss << "deId " << ide << "  cath " << icath << " " << response.getResponseParams().getParB(icath, ide);
      fitHistos.back()->Fit(&func, "QN0");
      fitHistos.back()->GetListOfFunctions()->Add(func.Clone());
      ss << " => " << func.GetParameter(3) << " +- " << func.GetParError(3);
      std::cout << ss.str() << std::endl;
      tunedParams.setParB(icath, ide, func.GetParameter(3));
    }
  }

  file.reset(TFile::Open(outFilename, "RECREATE"));
  for (auto& histo : fitHistos) {
    histo->Write();
  }

  return true;
}

//______________________________________________________________________________
bool ClusterSizeAnalysis::saveResults(const char* filename) const
{
  /// Save results on root file
  std::unique_ptr<TFile> file(TFile::Open(gSystem->ExpandPathName(filename), "RECREATE"));
  if (!file) {
    return false;
  }

  std::vector<TH2F> firedProb;
  for (auto& histo : mClusterSize) {
    firedProb.emplace_back(getFiredProbability(histo));
    histo.Write();
  }

  for (auto& histo : firedProb) {
    histo.Write();
  }

  return true;
}

} // namespace mid
} // namespace o2

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

#include <sstream>
#include "TFile.h"
#include "TSystem.h"
#include "TF1.h"
#include "TH2.h"
#include "MIDBase/DetectorParameters.h"
#include "MIDSimulation/ChamberResponseParams.h"

namespace o2
{
namespace mid
{

//______________________________________________________________________________
ClusterSizeAnalysis::ClusterSizeAnalysis(const char* filename)
  : mClusterSize()
{
  /// Default constructor
  readClusterSize(filename);
}

//______________________________________________________________________________
bool ClusterSizeAnalysis::readClusterSize(const char* filename)
{
  /// Initializes histograms

  std::unique_ptr<TFile> file{TFile::Open(filename)};

  if (!file) {
    std::cerr << "Cannot open " << filename << std::endl;
    return false;
  }

  for (int ide = 0; ide < detparams::NDetectionElements; ++ide) {
    std::stringstream name;
    name << "cluster_size_deId_" << ide;
    TH2* histo2D = static_cast<TH2*>(file->Get(name.str().c_str()));
    if (!histo2D) {
      std::cerr << "Cannot find histogram: " << name.str() << std::endl;
      continue;
    }

    for (int icath = 0; icath < 2; ++icath) {
      // The info on the non-bending plane is in the first bin
      // For the bending plane we have one cluster size per column.
      // However the fire probability should depend only on the RPC.
      // So we sum the cluster size of all columns
      int minBin = (icath == 0) ? 2 : 1;
      int maxBin = (icath == 0) ? 8 : 1;
      TH1* histo = histo2D->ProjectionY("auxProj", minBin, maxBin);
      if (histo->GetSumw2N() == 0) {
        histo->Sumw2();
      }
      int nbins = histo->GetXaxis()->GetNbins();

      // The probability to fire a strip should be 1 if the impact point is in the strip
      // (i.e. in the first bin)
      double scale = histo->Integral(1, nbins);
      if (scale > 0.) {
        histo->Scale(1. / scale);
      }
      int ipt = 0;
      double err;
      mClusterSize.push_back({});
      std::stringstream grName;
      grName << "fired_probability_deId_" << ide << "_cath_" << icath;
      mClusterSize.back().SetName(grName.str().c_str());
      // The cluster size is stored in a discrete way in the histogram.
      // So if we have a CS of 2cm and 4cm, we only fill the corresponding bins.
      // However, if the size of the CS is 4 cm, it means that all strips at a distance of up to 4cm are fired as well.
      // So we must integrate the entries of all bins above the one we are looking at.
      // Unfortunately, we cannot directly pass from the cluster size to the fired probability
      // since we do not now the position of the hit in the cluster.
      // We assume that the impact point is in the middle of the cluster:
      // this explains why the xmin and xmax are divided by 2.
      double xmin = 0;
      for (int ibin = 1; ibin <= nbins; ++ibin) {
        if ( histo->GetBinContent(ibin) > 0. ) {
          double content = histo->IntegralAndError(ibin, nbins, err);
          double xmax = histo->GetXaxis()->GetBinUpEdge(ibin)/2.;
          // mClusterSize.back().SetPoint(ipt, 0.5 * (xmax + xmin), content);
          // mClusterSize.back().SetPointError(ipt, 0.5 * (xmax - xmin), err);
          mClusterSize.back().SetPoint(ipt, xmin, content);
          mClusterSize.back().SetPointError(ipt, 0., err);
          xmin = xmax;
          ++ipt;
        }
      }
      delete histo;
    }
  }

  return true;
}

//______________________________________________________________________________
bool ClusterSizeAnalysis::process(const ChamberResponse& response)
{
  /// Checks the cluster resolution
  
  ChamberResponseParams tunedParams(response.getResponseParams());

  ChamberResponse resp(response);
  TF1 func("fitFunc", &resp, &ChamberResponse::firedProbabilityFunction, 0., 150., 8, "ChamberResponse", "firedProbabilityFunction");
  func.FixParameter(2, 0.);
  func.FixParameter(4, response.getResponseParams().getParametersA()[0]);
  func.FixParameter(5, response.getResponseParams().getParametersA()[1]);
  func.FixParameter(6, response.getResponseParams().getParametersC()[0]);
  func.FixParameter(7, response.getResponseParams().getParametersC()[1]);

  func.SetParNames("Cath", "deId", "Theta", "B", "A0", "A1", "C0", "C1");
  
  for (int ide = 0; ide < detparams::NDetectionElements; ++ide) {
    for (int icath = 0; icath < 2; ++icath) {
      func.FixParameter(0, (double)icath);
      func.FixParameter(1, (double)ide);
      func.SetParameter(3, response.getResponseParams().getParB(icath, ide));
      std::stringstream ss;
      ss << "deId " << ide << "  cath " << icath << " " << response.getResponseParams().getParB(icath, ide);
      int idx = 2 * ide + icath;
      mClusterSize[idx].Fit(&func, "QN0");
      mClusterSize[idx].GetListOfFunctions()->Add(func.Clone());
      ss << " => " << func.GetParameter(3) << " +- " << func.GetParError(3);
      ss << "  chi2/NDF: " << func.GetChisquare() << "/" << func.GetNDF() << " = " << func.GetChisquare() / func.GetNDF();
      std::cout << ss.str() << std::endl;
      tunedParams.setParB(icath, ide, func.GetParameter(3));
    }
  }

  return true;
}

//______________________________________________________________________________
bool ClusterSizeAnalysis::saveResults(const char* filename) const
{
  /// Save results in root file
  std::unique_ptr<TFile> file{TFile::Open(gSystem->ExpandPathName(filename), "RECREATE")};
  if (!file) {
    return false;
  }

  for (auto& gr : mClusterSize) {
    gr.Write();
  }

  return true;
}

} // namespace mid
} // namespace o2

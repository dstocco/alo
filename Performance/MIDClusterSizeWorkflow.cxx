#include <iostream>
#include <fstream>
#include <sstream>

#include "DigitMaker.h"
#include "ClusterSizeAnalysis.h"

#include "MIDClustering/PreClusterizer.h"

#include "MIDSimulation/ChamberHV.h"
#include "MIDSimulation/ChamberResponseParams.h"
#include "MIDSimulation/ChamberResponse.h"

#include "AliCDBManager.h"
#include "AliRawReader.h"

#include <bitset>

// #include <fairlogger/Logger.h>

int main(int argc, char* argv[])
{
  if (argc < 2) {
    std::cout << "Requires the raw file name as argument" << std::endl;
    return 1;
  }

  // fair::Logger::SetConsoleSeverity(fair::Severity::debug);

  unsigned long int nevents = 999999999;
  if (argc >= 3) {
    nevents = std::atol(argv[2]);
  }

  o2::mid::ClusterSizeAnalysis clusterSizeAnalysis;

  std::string outFilename = "MIDClusterSize.root";
  // Check if output file is already there
  std::ifstream inFile(outFilename.data());
  if (inFile.good()) {
    std::cout << "Output file already exists: will only run the terminate" << std::endl;
    inFile.close();
    o2::mid::ChamberResponseParams params;
    o2::mid::ChamberHV hv;
    o2::mid::ChamberResponse resp(params, hv);

    clusterSizeAnalysis.tuneParameters(outFilename.data(), "MIDClusterSizeFit.root", resp);
  } else {
    AliCDBManager::Instance()->SetDefaultStorage(
      "local://$ALIROOT_OCDB_ROOT/OCDB");
    AliCDBManager::Instance()->SetRun(0);

    AliRawReader* rawReader = AliRawReader::Create(argv[1]);
    if (!rawReader) {
      std::cerr << "Fatal: cannot open " << argv[1] << "\n";
      return 1;
    }

    rawReader->Select("MUONTRG", 0, 1);

    if (nevents > rawReader->GetNumberOfEvents()) {
      nevents = rawReader->GetNumberOfEvents();
    }

    o2::mid::PreClusterizer preClusterizer;
    DigitMaker digitMaker;

    unsigned long int ievt = 0;
    int progress = nevents / 10;
    while (rawReader->NextEvent() && ievt < nevents) {
      if (ievt % progress == 0) {
        std::cout << "Analyzing event " << ievt << " / " << nevents << " = " << 100 * ((double)ievt) / ((double)nevents) << "%" << std::endl;
      }
      auto digitStore = digitMaker.process(*rawReader);
      preClusterizer.process(digitStore);
      std::vector<o2::mid::PreClusters> preClusters(preClusterizer.getPreClusters().begin(), preClusterizer.getPreClusters().begin() + preClusterizer.getNPreClusters());
      clusterSizeAnalysis.process(preClusters);
      ++ievt;
    } // loop on events

    if (ievt == 0) {
      std::cerr << "Error: no event found" << std::endl;
      return 1;
    }

    clusterSizeAnalysis.saveResults(outFilename.data());
  }
  return 0;
}

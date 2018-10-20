find_package(FairLogger REQUIRED)

add_executable(mid-reco-check mid-reco-checker.cxx CheckReco.cxx)
target_link_libraries(mid-reco-check PUBLIC MIDDataFormat MIDDataFormatConverter MIDUtility Boost::program_options ROOT::Hist O2::DataFormatsMID O2::MIDBase O2::MIDSimulation O2::MIDClustering O2::MIDTracking FairLogger::FairLogger)
target_include_directories(mid-reco-check PUBLIC ${MS_GSL_INCLUDE_DIR})
install(TARGETS mid-reco-check RUNTIME DESTINATION bin)
add_executable(mid-fireprobability mid-fireprobability.cxx FireProbability.cxx)
target_link_libraries(mid-fireprobability PUBLIC Boost::program_options Boost::serialization ROOT::Hist O2::DataFormatsMID O2::MIDBase O2::MIDTestingSimTools O2::MIDClustering MIDDataFormat MIDDataFormatConverter MIDUtility)
target_include_directories(mid-fireprobability PUBLIC ${MS_GSL_INCLUDE_DIR})
install(TARGETS mid-fireprobability RUNTIME DESTINATION bin)

add_executable(mid-fireprobability-analysis mid-fireprobability-analysis.cxx FireProbabilityAnalysis.cxx)
target_link_libraries(mid-fireprobability-analysis PUBLIC Boost::program_options Boost::serialization ROOT::Hist O2::MIDBase O2::MIDSimulation)
install(TARGETS mid-fireprobability-analysis RUNTIME DESTINATION bin)
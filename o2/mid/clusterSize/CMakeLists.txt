add_executable(mid-clustersize mid-clustersize.cxx ClusterSize.cxx)
target_link_libraries(mid-clustersize PUBLIC Boost::program_options Boost::serialization ROOT::Hist O2::DataFormatsMID O2::MIDBase O2::MIDClustering MIDDataFormat MIDDataFormatConverter MIDUtility)
target_include_directories(mid-clustersize PUBLIC ${MS_GSL_INCLUDE_DIR})
install(TARGETS mid-clustersize RUNTIME DESTINATION bin)

add_executable(mid-clustersize-analysis mid-clustersize-analysis.cxx ClusterSizeAnalysis.cxx)
target_link_libraries(mid-clustersize-analysis PUBLIC Boost::program_options Boost::serialization ROOT::Hist O2::MIDBase O2::MIDSimulation)
# target_include_directories(mid-clustersize-analysis PUBLIC ${MS_GSL_INCLUDE_DIR})
install(TARGETS mid-clustersize-analysis RUNTIME DESTINATION bin)
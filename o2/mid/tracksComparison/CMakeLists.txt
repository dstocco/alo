add_executable(mid-compare-tracks mid-compare-tracks.cxx CompareTracks.cxx)
target_link_libraries(mid-compare-tracks PUBLIC MIDDataFormat MIDDataFormatConverter MIDUtility Boost::program_options ROOT::Hist O2::DataFormatsMID O2::MIDBase)
install(TARGETS mid-compare-tracks RUNTIME DESTINATION bin)
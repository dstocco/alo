add_executable(mid-alo-to-o2-raw mid-alo-to-o2-raw.cxx)
target_link_libraries(mid-alo-to-o2-raw PUBLIC Boost::program_options Boost::serialization O2::DataFormatsMID O2::MIDRaw MIDDataFormat MIDDataFormatConverter MIDUtility)
install(TARGETS mid-alo-to-o2-raw RUNTIME DESTINATION bin)

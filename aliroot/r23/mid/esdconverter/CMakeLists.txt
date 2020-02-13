add_executable(mid-ali-to-alo-esd mid-ali-to-alo-esd.cxx ESDToMID.cxx)
target_link_libraries(
  mid-ali-to-alo-esd
  PUBLIC Boost::program_options
         Boost::serialization
         AliRoot::MUONbase
         AliRoot::MUONrec
         AliRoot::MUONevaluation
         MIDDataFormat
         MIDUtility)
install(TARGETS mid-ali-to-alo-esd RUNTIME DESTINATION bin)

add_executable(mid-merge-alo-esd mid-merge-alo-esd.cxx)
target_link_libraries(
  mid-merge-alo-esd PUBLIC Boost::program_options Boost::serialization
                           MIDDataFormat MIDUtility)
install(TARGETS mid-merge-alo-esd RUNTIME DESTINATION bin)

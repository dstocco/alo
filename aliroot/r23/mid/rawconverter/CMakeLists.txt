add_executable(mid-ali-to-alo-raw mid-ali-to-alo-raw.cxx RawToMID.cxx)
target_link_libraries(
  mid-ali-to-alo-raw
  PUBLIC Boost::program_options
         Boost::serialization
         AliRoot::MUONbase
         AliRoot::MUONrec
         MIDDataFormat
         MIDUtility
         MIDConverterUtility)
install(TARGETS mid-ali-to-alo-raw RUNTIME DESTINATION bin)

add_executable(mid-merge-alo-raw mid-merge-alo-raw.cxx)
target_link_libraries(
  mid-merge-alo-raw PUBLIC Boost::program_options Boost::serialization
                           MIDDataFormat MIDUtility)
install(TARGETS mid-merge-alo-raw RUNTIME DESTINATION bin)

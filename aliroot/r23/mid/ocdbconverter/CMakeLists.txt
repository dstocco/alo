add_executable(mid-ali-to-alo-ocdb mid-ali-to-alo-ocdb.cxx HvToMID.cxx)

target_link_libraries(
  mid-ali-to-alo-ocdb PUBLIC Boost::program_options Boost::serialization
                          AliRoot::MUONsim AliRoot::MUONbase ROOT::Core)
install(TARGETS mid-ali-to-alo-ocdb RUNTIME DESTINATION bin)

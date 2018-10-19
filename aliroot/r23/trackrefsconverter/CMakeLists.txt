add_executable(mid-convert-trackrefs
        mid-convert-trackrefs.cxx 
        TrackRefsToMID.cxx
        )

target_link_libraries(mid-convert-trackrefs PUBLIC Boost::program_options Boost::serialization AliRoot::MUONsim AliRoot::MUONbase MIDDataFormat MIDUtility)
install(TARGETS mid-convert-trackrefs RUNTIME DESTINATION bin)

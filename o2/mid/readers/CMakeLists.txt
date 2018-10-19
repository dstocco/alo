add_library(MIDReaders SHARED BoostReader.cxx)
target_link_libraries(MIDReaders PUBLIC Boost::serialization)
set_target_properties(MIDReaders PROPERTIES PUBLIC_HEADER "BoostReader.h")
install(TARGETS MIDReaders LIBRARY DESTINATION lib PUBLIC_HEADER DESTINATION include/mid)
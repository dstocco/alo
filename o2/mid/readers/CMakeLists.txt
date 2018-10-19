add_library(MIDReaders SHARED BoostReader.cxx BoostReaderDevice.cxx)
target_link_libraries(MIDReaders PUBLIC Boost::serialization o2::Framework)
set_target_properties(MIDReaders PROPERTIES PUBLIC_HEADER "BoostReader.h;BoostReaderDevice.h")
install(TARGETS MIDReaders LIBRARY DESTINATION lib PUBLIC_HEADER DESTINATION include/mid)
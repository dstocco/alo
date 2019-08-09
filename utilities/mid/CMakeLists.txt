add_library(MIDUtility SHARED FileHandler.cxx FileHandler.h BoostReader.cxx BoostReader.h)
target_include_directories(MIDUtility PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(MIDUtility PUBLIC Boost::serialization)
install(TARGETS MIDUtility LIBRARY DESTINATION lib)
add_library(MIDDataFormat INTERFACE)
target_link_libraries(MIDDataFormat INTERFACE Boost::serialization)
target_include_directories(MIDDataFormat INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include FILES_MATCHING PATTERN "*.h")

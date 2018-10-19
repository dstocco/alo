add_library(MIDUtility SHARED ConvertDE.cxx)
target_include_directories(MIDUtility INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(MIDUtility PROPERTIES PUBLIC_HEADER "ConvertDE.h")
install(TARGETS MIDUtility LIBRARY DESTINATION lib PUBLIC_HEADER DESTINATION include/mid)
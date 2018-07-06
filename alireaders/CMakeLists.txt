set(SOURCES
        TrackRefReader.cxx
        TrackRefReader.h
)

add_library(alireaders SHARED ${SOURCES})
target_link_libraries(alireaders PUBLIC AliRoot::MUONsim o2::MIDBase)
target_include_directories(alireaders PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

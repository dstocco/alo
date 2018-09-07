set(SOURCES
        DigitMaker.cxx
        TrackRefReader.cxx
)

add_library(alireaders SHARED ${SOURCES})
target_link_libraries(alireaders PUBLIC AliRoot::MUONsim AliRoot::MUONbase o2::DataFormatsMID o2::MIDBase o2::MIDSimulation)
target_include_directories(alireaders PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# FIXME: the library depends on o2::MIDSimulation becaus Hit.h is defined there
# Consider moving to DataFormatsMID?
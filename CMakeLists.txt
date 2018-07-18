cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

### CMP0025   Compiler id for Apple Clang is now AppleClang.
### CMP0042   MACOSX_RPATH is enabled by default.

FOREACH (p
        CMP0025 # CMake 3.0
        CMP0042 # CMake 3.0
        CMP0028 # CMake 3.0
        CMP0063 # honor visibility settings also for static libs
        CMP0057 # for doxygen
        )
    IF (POLICY ${p})
        cmake_policy(SET ${p} NEW)
    ENDIF ()
endforeach ()

project(alo VERSION 0.9.0 LANGUAGES CXX C)

set(CMAKE_MODULE_PATH "${alo_SOURCE_DIR}/cmake")

find_package(ROOT)
find_package(AliRoot)
find_package(O2 REQUIRED)

include(CMakeALICE)

set(CMAKE_CXX_STANDARD 14)

find_package(Boost COMPONENTS unit_test_framework REQUIRED)
include(CTest)

find_package(RapidJSON)
if (RAPIDJSON_FOUND)
    add_subdirectory(jsonmap)
else ()
    message(WARNING "RapidJSON package not found : will not build jsonmap directory")
endif ()

if (ROOT_FOUND AND AliRoot_FOUND)
    add_subdirectory(dhlt)
    add_subdirectory(r23)
    add_subdirectory(binmap)
    add_subdirectory(vsaliroot)
    add_subdirectory(alireaders)
    add_subdirectory(PerformanceMC)
endif ()

if (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION} GREATER 3.9.6)
    # doxygen_add_docs command was added in CMake 3.9.6
    find_package(Doxygen OPTIONAL_COMPONENT dot)
    if (DOXYGEN_FOUND)
        set(DOXYGEN_JAVADOC_AUTOBRIEF YES)
        set(DOXYGEN_OPTIMIZE_OUTPUT_FOR_C YES)
        set(DOXYGEN_WARN_IF_UNDOCUMENTED NO)

        doxygen_add_docs(doc)
    endif ()
endif ()

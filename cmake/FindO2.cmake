# **************************************************************************
# * Copyright(c) 1998-2015, ALICE Experiment at CERN, All rights reserved. *
# *                                                                        *
# * Author: The ALICE Off-line Project.                                    *
# * Contributors are mentioned in the code where appropriate.              *
# *                                                                        *
# * Permission to use, copy, modify and distribute this software and its   *
# * documentation strictly for non-commercial purposes is hereby granted   *
# * without fee, provided that the above copyright notice appears in all   *
# * copies and that both the copyright notice and this permission notice   *
# * appear in the supporting documentation. The authors make no claims     *
# * about the suitability of this software for any purpose. It is          *
# * provided "as is" without express or implied warranty.                  *
# **************************************************************************

set(O2_FOUND FALSE)

function(depends_on_o2 target)
    # target depends on all argsn
    get_target_property(a o2::${target} INTERFACE_LINK_LIBRARIES)
    if (a)
        set(tmp ${a})
    endif ()
    foreach (t ${ARGN})
        list(APPEND tmp o2::${t})
    endforeach ()
    set_target_properties(o2::${target} PROPERTIES INTERFACE_LINK_LIBRARIES "${tmp}")
endfunction()

function(depends_on_boost target)
    # target depends on all argsn
    get_target_property(a o2::${target} INTERFACE_LINK_LIBRARIES)
    if (a)
        set(tmp ${a})
    endif ()
    foreach (t ${ARGN})
        list(APPEND tmp Boost::${t})
    endforeach ()
    set_target_properties(o2::${target} PROPERTIES INTERFACE_LINK_LIBRARIES "${tmp}")
endfunction()

if (NOT O2)
    if ($ENV{O2_ROOT})
        set(O2 ${O2_ROOT})
    endif ()
endif ()

if (O2)
    # Check if O2 is really installed there
    if (EXISTS ${O2}/bin/mch-mapping-svg-segmentation3 AND EXISTS ${O2}/lib AND EXISTS ${O2}/include)

        # foreach (t MCHMappingImpl3 MCHMappingInterface MCHMappingSegContour MCHMappingSegContour3 MCHContour MIDBase)
        foreach (t MCHMappingImpl3 MCHMappingInterface MCHMappingSegContour MCHMappingSegContour3 MCHContour DataFormatsMID MIDBase MIDSimulation MIDClustering MIDTracking)
            if (NOT TARGET o2::${t})
                if (EXISTS ${O2}/lib/lib${t}.so OR EXISTS ${O2}/lib/lib${t}.dylib)
                    add_library(o2::${t} SHARED IMPORTED)
                else ()
                    add_library(o2::${t} INTERFACE IMPORTED)
                endif ()
                if (EXISTS ${O2}/include/${t})
                    set_target_properties(o2::${t} PROPERTIES
                            INTERFACE_INCLUDE_DIRECTORIES ${O2}/include)
                endif ()
                if (EXISTS ${O2}/lib/lib${t}.so)
                    set_target_properties(o2::${t} PROPERTIES
                            IMPORTED_LOCATION ${O2}/lib/lib${t}.so)
                elseif (EXISTS ${O2}/lib/lib${t}.dylib)
                    set_target_properties(o2::${t} PROPERTIES
                            IMPORTED_LOCATION ${O2}/lib/lib${t}.dylib)
                endif ()
            endif ()
        endforeach ()

        # the following is of course not a very clever way / labor intensive of doing this, but it kind of works...
        # the correct alternative would be the change O2 to properly export its targets, but
        # that alone would require some more work...

        # we specify the first level dependencies of each library

        depends_on_o2(MCHMappingSegContour3 MCHMappingImpl3)
        depends_on_boost(MCHMappingInterface boost)
        depends_on_o2(MCHMappingImpl3 MCHMappingInterface)
        depends_on_o2(MCHMappingSegContour MCHMappingInterface MCHContour)
        depends_on_boost(MIDBase boost)

        set(O2_FOUND TRUE)
        message(STATUS "O2 ... - found ${O2} - and target dependencies defined")

    else ()

        message(STATUS "O2 ... - not found")

    endif ()
endif (O2)

if (NOT O2_FOUND)
    if (O2_FIND_REQUIRED)
        message(FATAL_ERROR "Please point to the O2 Core installation using -DO2=<O2_CORE_INSTALL_DIR> ")
    endif (O2_FIND_REQUIRED)
endif (NOT O2_FOUND)

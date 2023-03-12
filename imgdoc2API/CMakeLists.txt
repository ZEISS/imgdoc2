# SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
#
# SPDX-License-Identifier: MIT

add_library(imgdoc2API  SHARED
                imgdoc2API.h
                imgdoc2API.cpp 
                "importexport.h" 
                "errorcodes.h" 
                "tilecoordinateinterop.h" 
                "logicalpositioninfointerop.h" 
                "utilities.h"
                "utilities.cpp" 
                "queryresultinterop.h" 
                "dimcoordinatequeryclauseinterop.h" 
                "tilebaseinfointerop.h" 
                "statisticsinterop.h"
                "rectangledoubleinterop.h" "tileinfoqueryclauseinterop.h" "tileblobinfointerop.h" "minmaxfortiledimensioninterop.h")

target_link_libraries(imgdoc2API PRIVATE libimgdoc2)

target_compile_definitions(imgdoc2API PUBLIC LIBIMGDOC2_EXPORTS)

add_custom_command(TARGET imgdoc2API POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
    $<TARGET_FILE:imgdoc2API>
    "${CMAKE_SOURCE_DIR}/dotnet/native_dlls"
)
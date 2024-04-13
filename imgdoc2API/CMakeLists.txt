# SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
#
# SPDX-License-Identifier: MIT

add_library(imgdoc2API  SHARED
                "imgdoc2API.h"
                "imgdoc2API.cpp"
                "importexport.h" 
                "errorcodes.h" 
                "tilecoordinateinterop.h" 
                "logicalpositioninfointerop.h" 
                "utilities.h"
                "utilities.cpp" 
                "queryresultinterop.h" 
                "dimcoordinatequeryclauseinterop.h" 
                "tilebaseinfointerop.h" 
                "imgdoc2statisticsinterop.h"
                "rectangledoubleinterop.h" 
                "tileinfoqueryclauseinterop.h" 
                "tileblobinfointerop.h" 
                "minmaxfortiledimensioninterop.h" 
                "tilecountperlayerinterop.h" 
                "logicalpositioninfo3dinterop.h" 
                "brickbaseinfointerop.h" 
                "brickblobinfointerop.h" 
                "cuboiddoubleinterop.h" 
                "planenormalanddistanceinterop.h" 
                "imgdoc2apistatistics.h" 
                "sharedptrwrapper.h" 
                "versioninfointerop.h" 
                "allocationobject.h" 
                "codecsAPI.h" 
                "codecsAPI.cpp"
                "bitmapinfointerop.h"
                "decodedimageresultinterop.h" 
                "imgdoc2APIsupport.h" 
                "imgdoc2APIsupport.cpp")

target_link_libraries(imgdoc2API PRIVATE libimgdoc2)

# Link against the GSL interface library (i.e. header only library)
target_link_libraries(imgdoc2API PRIVATE GSL)

# link with static libCZI library
target_compile_definitions(imgdoc2API PRIVATE _LIBCZISTATICLIB)
target_link_libraries(imgdoc2API PRIVATE libCZIStatic)
target_include_directories(imgdoc2API PRIVATE ${LIBCZI_INCLUDE_DIR})

target_compile_definitions(imgdoc2API PUBLIC LIBIMGDOC2_EXPORTS)

add_custom_command(TARGET imgdoc2API POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
    $<TARGET_FILE:imgdoc2API>
    "${CMAKE_SOURCE_DIR}/dotnet/native_dlls"
)
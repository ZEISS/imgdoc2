# SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
#
# SPDX-License-Identifier: MIT

include(FetchContent)
FetchContent_Declare(
  cli11
  GIT_REPOSITORY https://github.com/CLIUtils/CLI11
  GIT_TAG        v2.2.0
)

FetchContent_MakeAvailable(cli11)

set(CONVCZI_SOURCEFILES 
    "convczi.cpp"
    "commandlineoptions.h"
    "commandlineoptions.cpp"
    "utilities.h"
    "utilities.cpp")


add_executable(convczi ${CONVCZI_SOURCEFILES}  )

set_target_properties(convczi PROPERTIES CXX_STANDARD 17)
target_compile_definitions(convczi PRIVATE _LIBCZISTATICLIB)
target_link_libraries(convczi PRIVATE libCZIStatic libimgdoc2 CLI11::CLI11)

target_include_directories(convczi PRIVATE ${LIBCZI_INCLUDE_DIR})

# this preprocessor define needs to be defined when building "SqliteImgDoc" and by users of it (if linking the static library)
target_compile_definitions(convczi PUBLIC _SQLITEIMGDOCSTATICLIB=1)


# create the include-file "ConvCZI_Config.h" where build-time-configurations will go into
IF(WIN32)
	set(ConvCZI_UNIX_ENVIRONMENT 0)
	set(ConvCZI_WIN32_ENVIRONMENT 1)
ENDIF()
IF(UNIX)
  set(ConvCZI_UNIX_ENVIRONMENT 1)
	set(ConvCZI_WIN32_ENVIRONMENT 0)
ENDIF()

configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/ConvCZI_Config.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/ConvCZI_Config.h"
  )

target_include_directories(convczi PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
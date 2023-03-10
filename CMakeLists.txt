# SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
#
# SPDX-License-Identifier: MIT

# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.7)

# Needed to enable -MT compile switch
cmake_policy(SET CMP0091 NEW)

include(FetchContent)

project("imgdoc2")

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(IMGDOC2_BUILD_CLANGTIDY "Build with Clang-Tidy" OFF)

if (IMGDOC2_BUILD_CLANGTIDY)
  # How "clang-tidy" organization works (if this option is enable here):
  # - the compiler is looking for a ".clang-tidy"-file (containing configuration) in the parent folder of each compilation unit
  # - this ".clang-tidy"-file in turn my refer to another ".clang-tidy"-file in its parent directory if the option "InheritParentConfig" is set to true
  # - _on top_ of that the option we give here with the checks-argument is applied (where we globally turn off some checkers)
  # please see -> https://clang.llvm.org/extra/clang-tidy for details
  # Note: this means that if there is no ".clang-tidy"-file (in the parent folder of a .cpp file), then no checks are done
  set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=-llvm-*,-llvmlibc-*,-fuchsia-*,-altera-*,-hicpp-*,-abseil-*")
  #set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=*,-llvm-*,-llvmlibc-*,-fuchsia-*,-altera-*,-hicpp-*,-abseil-*,-google-build-using-namespace,-modernize-use-trailing-return-type,-readability-convert-member-functions-to-static")
  #set(CMAKE_C_CLANG_TIDY "clang-tidy;-checks=*")
endif()

include(ExternalProject)

include(FetchContent)

# In this example we are picking a specific tag.
# You can also pick a specific commit, if you need to.
FetchContent_Declare(GSL
    GIT_REPOSITORY "https://github.com/microsoft/GSL"
    GIT_TAG "v4.0.0"
)

FetchContent_MakeAvailable(GSL)

option(IMGDOC2_BUILD_CONVCZI "Whether to build the convczi-tool" ON)

add_subdirectory(external/sqlite3)

add_subdirectory(libimgdoc2)
add_subdirectory(imgdoc2API)
add_subdirectory(imgdoc2cmd)

if (IMGDOC2_BUILD_CONVCZI)
  include("${CMAKE_SOURCE_DIR}/cmake/libCZI.cmake")
  FetchContent_GetProperties(libCZI)
  set(LIBCZI_INCLUDE_DIR "${libczi_SOURCE_DIR}/Src/libCZI")
  add_subdirectory(convczi)
endif()

enable_testing()
add_subdirectory(libimgdoc2_tests)





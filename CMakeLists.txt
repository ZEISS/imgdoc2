# SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
#
# SPDX-License-Identifier: MIT

# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.7)

cmake_policy(SET CMP0091 NEW) # Needed to enable -MT compile switch
cmake_policy(SET CMP0079 NEW) # necessary to make coverage build work, not sure why it is needed

include(FetchContent)

project("imgdoc2")

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(IMGDOC2_BUILD_CLANGTIDY "Build with Clang-Tidy" OFF)

# Enable code-coverage for unit-tests (only works with GCC/Clang)
# Usage is something like: cmake .. -DCMAKE_BUILD_TYPE=Debug -DIMGDOC2_CODECOV=ON -DIMGDOC2_BUILD_CONVCZI=OFF && make && make code_coverage
# This will generate a coverage-report in the build-directory (code_coverage.xml)
option(IMGDOC2_CODECOV "Build with Code-Coverage (for unit-tests)" OFF)

option(IMGDOC2_BUILD_CONVCZI "Whether to build the convczi-tool" ON)


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

if (IMGDOC2_CODECOV)
  include("${CMAKE_SOURCE_DIR}/cmake/CodeCoverage.cmake")
  append_coverage_compiler_flags_to_target(libimgdoc2_tests)
  append_coverage_compiler_flags_to_target(libimgdoc2)
  setup_target_for_coverage_gcovr_xml(
		    NAME code_coverage
        EXECUTABLE ctest
		    DEPENDENCIES   libimgdoc2 libimgdoc2_tests
		    BASE_DIRECTORY "${PROJECT_SOURCE_DIR}"
		    EXCLUDE "${PROJECT_SOURCE_DIR}/libimgdoc2_tests/*" "${PROJECT_BINARY_DIR}/_deps/*" "/usr/include/*")
endif()

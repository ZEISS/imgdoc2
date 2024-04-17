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

# we read the version number from a file
file (STRINGS "VersionNumber.txt" IMGDOC2_VERSION_NUMBER)

project("imgdoc2"
          VERSION ${IMGDOC2_VERSION_NUMBER})

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(IMGDOC2_BUILD_CLANGTIDY "Build with Clang-Tidy" OFF)

# Enable code-coverage for unit-tests (only works with GCC/Clang), generating coverage report in XML-format.
# A target with name "code_coverage_gcovr_xml" will be added which can be used to generate the coverage report.
# Usage is something like: cmake .. -DCMAKE_BUILD_TYPE=Debug -DIMGDOC2_CODECOV_GCOVRXML=ON -DIMGDOC2_BUILD_CONVCZI=OFF && make code_coverage_gcovr_xml
# This will generate a coverage-report in the build-directory (code_coverage_gcovr_xml.xml).
option(IMGDOC2_CODECOV_GCOVRXML "Build with Code-Coverage (for unit-tests) in XML-format" OFF)

# Enable code-coverage for unit-tests (only works with GCC/Clang), generating coverage report in the form of an HTML-report.
# A target with name "code_coverage_gcovr_html" will be added which can be used to generate the HTML-report.
# Usage is something like: cmake .. -DCMAKE_BUILD_TYPE=Debug -DIMGDOC2_CODECOV_GCOVRHTML=ON -DIMGDOC2_BUILD_CONVCZI=OFF && make code_coverage_gcovr_html
# This HTML-coverage-report will be found in the build-directory in the folder "code_coverage_gcovr_html".
option(IMGDOC2_CODECOV_GCOVRHTML "Build with Code-Coverage (for unit-tests) as an HTML-report" OFF)

option(IMGDOC2_BUILD_CONVCZI "Whether to build the convczi-tool" ON)

option(IMGDOC2_BUILD_NATIVEIMGDOC2CMD "Whether to build the native imgdoc2cmd-tool (currently contains throw-away experiments)" ON)

option(IMGDOC2_BUILD_UNITTESTS "Whether to build the unit-tests" ON)

# this option is only relevant if IMGDOC2_BUILD_UNITTESTS is ON, and it controls whether the unit-tests are run (for unit-test discovery) after building.
# This is problematic when cross-compiling. In that case, the unit-tests should be run on the target system.
option(IMGDOC2_RUNDISCOVER_UNITTESTS "Whether to run the unit-tests after building" ON)


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

# make libCZI available (currently used by imgdoc2API and convczi))
include("${CMAKE_SOURCE_DIR}/cmake/libCZI.cmake")
FetchContent_GetProperties(libCZI)
set(LIBCZI_INCLUDE_DIR "${libczi_SOURCE_DIR}/Src/libCZI")


add_subdirectory(libimgdoc2)
add_subdirectory(imgdoc2API)

if (IMGDOC2_BUILD_NATIVEIMGDOC2CMD)
  add_subdirectory(imgdoc2cmd)
endif()

if (IMGDOC2_BUILD_CONVCZI)
  add_subdirectory(convczi)
endif()

include (CTest)
set(MEMORYCHECK_COMMAND valgrind)
set(MEMORYCHECK_COMMAND_OPTIONS "--leak-check=yes") # check https://valgrind.org/docs/manual/manual-core.html#manual-core.options for more options
if (IMGDOC2_BUILD_UNITTESTS) 
  enable_testing()
  add_subdirectory(libimgdoc2_tests)
endif()

if (IMGDOC2_CODECOV_GCOVRXML OR IMGDOC2_CODECOV_GCOVRHTML)
  include("${CMAKE_SOURCE_DIR}/cmake/CodeCoverage.cmake")
  append_coverage_compiler_flags_to_target(libimgdoc2_tests)
  append_coverage_compiler_flags_to_target(libimgdoc2)
  target_compile_options(libimgdoc2 PRIVATE -O0)        # disable all optimizations
  target_compile_options(libimgdoc2_tests PRIVATE -O0)  # disable all optimizations
  if (IMGDOC2_CODECOV_GCOVRXML)
    setup_target_for_coverage_gcovr_xml(
		      NAME code_coverage_gcovr_xml
          EXECUTABLE ctest
		      DEPENDENCIES   libimgdoc2 libimgdoc2_tests
		      BASE_DIRECTORY "${PROJECT_SOURCE_DIR}"
		      EXCLUDE "${PROJECT_SOURCE_DIR}/libimgdoc2_tests/*" "${PROJECT_BINARY_DIR}/_deps/*" "/usr/include/*")
  endif()
  if (IMGDOC2_CODECOV_GCOVRHTML)
    setup_target_for_coverage_gcovr_html(
		    NAME code_coverage_gcovr_html
        EXECUTABLE ctest
		    DEPENDENCIES   libimgdoc2 libimgdoc2_tests
		    BASE_DIRECTORY "${PROJECT_SOURCE_DIR}"
		    EXCLUDE "${PROJECT_SOURCE_DIR}/libimgdoc2_tests/*" "${PROJECT_BINARY_DIR}/_deps/*" "/usr/include/*")
   endif()
endif()

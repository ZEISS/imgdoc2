# SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
#
# SPDX-License-Identifier: MIT

find_package(GTest QUIET)
find_package(GMock QUIET)

if ("${GTest_FOUND}" AND "${GMock_FOUND}")
    message(STATUS "Found GTest, using it.")
else()
    message(STATUS "Did not find package GTest, will attempt to fetch it locally.")

include(FetchContent)

 FetchContent_Declare(
      googletest
      GIT_REPOSITORY https://github.com/google/googletest.git
      GIT_TAG        release-1.12.1
      #GIT_TAG        4c8e6a9fe1c8dbd96f244f076c369f44414d8d32 # this is known to work (and contains the fix below)
    )
    FetchContent_GetProperties(googletest)
    if(NOT googletest_POPULATED)
        # Prevent overriding the parent project's compiler/linker
        # settings on Windows -> https://github.com/google/googletest/blob/master/googletest/README.md
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        FetchContent_Populate(googletest)

        add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
        set(GTEST_INCLUDE_DIRS ${googletest_SOURCE_DIR}/googletest/include)
        set(GTEST_BOTH_LIBRARIES ${CMAKE_BINARY_DIR}/lib/libgtest.a ${CMAKE_BINARY_DIR}/lib/libgtest_main.a)    
    endif()
endif() 


# Add source to this project's executable.
add_executable( libimgdoc2_tests 
 "db_utilities_test.cpp" 
 "query2d_test.cpp" 
 "dbdiscovery_test.cpp" 
 "propertybagutilities_test.cpp" 
 "blobdata_test.cpp" 
 "query_test.cpp" 
 "read2d_test.cpp" 
 "docinfo2d_test.cpp" 
 "query3d_test.cpp" 
 "docinfo3d_test.cpp" 
 "utilities.h" 
 "utilities.cpp" 
 "read3d_test.cpp" 
 "miscellaneous_test.cpp" 
 "documentoperation_test.cpp" 
 "metadata_test.cpp")

target_include_directories(libimgdoc2_tests PRIVATE ${GTEST_INCLUDE_DIRS})

#target_link_libraries(libimgdoc2_tests PRIVATE SQLiteCpp sqlite3)
#target_link_libraries(libimgdoc2_tests PRIVATE sqlite3)

# this preprocessor define needs to be defined when building "SqliteImgDoc" and by users of it (if linking the static library)
target_compile_definitions(libimgdoc2_tests PUBLIC _SQLITEIMGDOCSTATICLIB=1)

# Link test executable against gtest & gtest_main

if(UNIX AND NOT APPLE)
   # Linux
   find_package(Threads REQUIRED)
   message(STATUS "****** UNIX AND NOT APPLE ******")
   #target_link_libraries(libimgdoc2_v2_tests PRIVATE gtest gmock gtest_main libimgdoc2_v2 ${CMAKE_THREAD_LIBS_INIT}  ${CMAKE_DL_LIBS})
   target_link_libraries(libimgdoc2_tests PRIVATE gtest gmock gtest_main libimgdoc2 pthread dl)
elseif(UNIX AND APPLE)
   # Mac OS
   find_package(Threads REQUIRED)
   target_link_libraries(libimgdoc2_tests PRIVATE gtest gmock gtest_main libimgdoc2 pthread dl)
else()
   find_package(Threads REQUIRED)
   target_link_libraries(libimgdoc2_tests PRIVATE gtest gmock gtest_main libimgdoc2 )
endif()

#[[
# Cygwin seems to need some special treatment -> https://stackoverflow.com/questions/18784112/googletest-1-6-with-cygwin-1-7-compile-error-fileno-was-not-declared-in-this
 if(NOT CYGWIN)
    set_target_properties(libimgdoc2_tests PROPERTIES
               CXX_STANDARD 11
               CXX_EXTENSIONS OFF)
else()
    set_target_properties(libimgdoc2_tests PROPERTIES
               CXX_STANDARD 11
               CXX_EXTENSIONS ON)
endif()
]]

add_test( libimgdoc2_tests  libimgdoc2_tests  )

if (IMGDOC2_RUNDISCOVER_UNITTESTS)
  gtest_discover_tests(libimgdoc2_tests)
endif()

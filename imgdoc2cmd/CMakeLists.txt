# SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
#
# SPDX-License-Identifier: MIT

add_executable(imgdoc2cmd 
                imgdoc2cmd.cpp)

#target_include_directories(imgdoc2cmd PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../extlibs/cxxopts/>)

if(UNIX AND NOT APPLE)
   # Linux
   find_package(Threads REQUIRED)
   message(STATUS "****** UNIX AND NOT APPLE ******")
   message(STATUS "CMAKE_THREAD_LIBS_INIT=${CMAKE_THREAD_LIBS_INIT}  CMAKE_DL_LIBS=${CMAKE_DL_LIBS}")
   #target_link_libraries(imgdoc2cmd PRIVATE libimgdoc2 ${CMAKE_THREAD_LIBS_INIT}  ${CMAKE_DL_LIBS})
   target_link_libraries(imgdoc2cmd PRIVATE libimgdoc2 pthread dl)
elseif(UNIX AND APPLE)
   # Mac OS
   find_package(Threads REQUIRED)
   target_link_libraries(imgdoc2cmd PRIVATE libimgdoc2 pthread dl)
else()
   find_package(Threads REQUIRED)
   target_link_libraries(imgdoc2cmd PRIVATE  libimgdoc2 )
endif()

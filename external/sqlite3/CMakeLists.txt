# SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
#
# SPDX-License-Identifier: MIT

add_library(sqlite3 STATIC 
               sqlite3.c
               sqlite3.h
)

set_target_properties(sqlite3 PROPERTIES
    OUTPUT_NAME   sqlite3
    PUBLIC_HEADER sqlite3.h
    DEBUG_POSTFIX d
    )
    

# Configuration
# All options are defined at:
# https://www.sqlite.org/compile.html
target_compile_definitions(sqlite3 PUBLIC # inject user's options
  SQLITE_ENABLE_RTREE 
  SQLITE_ENABLE_COLUMN_METADATA
  SQLITE_USE_ALLOCA
  SQLITE_LIKE_DOESNT_MATCH_BLOBS
)

# shared libraries need PIC
set_property(TARGET sqlite3 PROPERTY POSITION_INDEPENDENT_CODE ON)
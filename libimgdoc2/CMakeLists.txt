# SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
#
# SPDX-License-Identifier: MIT

set(LibImgDoc2_Srcfiles 
         "src/public/ClassFactory.cpp"
         "src/public/CreateOptions.cpp"
         "inc/ICreateOptions.h"
         "inc/ClassFactory.h" 
         "inc/imgdoc2.h" 
         "src/db/database_configuration.h" 
         "src/db/database_configuration.cpp" 
         "src/db/database_creator.h" 
         "src/db/database_creator.cpp" 
         "src/db/IDbConnection.h" 
         "src/db/IDb.h" 
         "inc/IDocRead2d.h" 
         "inc/IDocWrite2d.h" 
         "src/db/DbFactory.h" 
         "src/db/sqlite/sqlite_DbFactory.cpp" 
         "src/db/sqlite/sqlite_DbConnection.cpp" 
         "src/db/sqlite/sqlite_DbConnection.h" 
         "inc/IDoc.h" 
         "src/doc/document.h" 
         "src/doc/document.cpp" 
         "src/db/sqlite/custom_functions.cpp"
         "src/db/sqlite/custom_functions.h" 
         "inc/types.h"  
         "inc/IDatabaseTransaction.h" 
         "inc/IDbIndexManagement.h" 
         "src/doc/documentRead2d.h" 
         "src/doc/documentRead2d.cpp" 
         "inc/LogicalPositionInfo.h" 
         "inc/utilities.h" 
         "inc/TileCoordinate.h" 
         "inc/ITileCoordinate.h" 
         "src/db/sqlite/sqlite_DbStatement.h" 
         "src/db/sqlite/sqlite_DbStatement.cpp" 
         "src/db/sqlite/ISqlite_DbStatement.h" 
         "src/doc/documentWrite2d.h"
         "src/doc/documentWrite2d.cpp"
         "inc/DataTypes.h" 
         "inc/IDataObj.h" 
         "src/db/utilities.h" 
         "src/db/utilities.cpp" 
         "inc/exceptions.h" 
         "src/public/exceptions.cpp" 
         "src/db/IDbStatement.h" 
         "inc/IDimCoordinateQueryClause.h" 
         "inc/ITIleInfoQueryClause.h"
         "inc/DimCoordinateQueryClause.h"
         "inc/IBlobOutput.h" 
         "src/doc/transactionHelper.h"
         "src/db/database_discovery.h"
         "src/db/database_discovery.cpp"
         "src/db/database_constants.h" 
         "src/db/database_constants.cpp"
         "inc/IOpenExistingOptions.h"
         "src/public/OpenExistingOptions.cpp"
         "inc/impl/BlobOutputImplementations.h"
         "inc/impl/DataObjectImplementations.h" 
         "inc/IEnvironment.h" 
         "inc/TileInfoQueryClause.h" 
         "inc/IDocInfo.h" 
         "inc/IDocQuery2d.h" 
         "inc/IDocQuery3d.h" 
         "inc/IDocWrite3d.h" 
         "inc/IDocRead3d.h" 
         "inc/DocumentType.h" 
         "src/doc/documentRead3d.h" 
         "src/doc/documentWrite3d.h" 
         "inc/BrickBaseInfo.h" 
         "src/doc/documentWrite3d.cpp"
         "src/doc/documentRead3d.cpp" 
         "src/db/database_utilities.h" 
         "src/db/database_utilities.cpp" 
         "src/doc/documentReadBase.h" 
         "src/doc/documentReadBase.cpp" 
         "inc/IDocInfo2d.h" 
         "inc/Intervals.h" 
         "inc/loglevel.h" 
         "inc/IDocumentMetadata.h" 
         "src/doc/documentMetadataWriter.h" 
         "src/doc/documentMetadataWriter.cpp"
         "src/doc/documentMetadataReader.h"
         "src/doc/documentMetadataReader.cpp" 
         "src/doc/documentMetadataBase.cpp")

add_library(libimgdoc2 STATIC
                ${LibImgDoc2_Srcfiles})

# shared libraries need PIC
set_property(TARGET libimgdoc2 PROPERTY POSITION_INDEPENDENT_CODE ON)

target_link_libraries(libimgdoc2 PUBLIC sqlite3)

# Link against the GSL interface library (i.e. header only library)
target_link_libraries(libimgdoc2 PRIVATE GSL)

set_target_properties(libimgdoc2 PROPERTIES INCLUDE_DIRECTORIES "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../external/sqlite3>;${CMAKE_CURRENT_SOURCE_DIR}/inc")

# this preprocessor define needs to be defined when building "SqliteImgDoc" and by users of it (if linking the static library)
target_compile_definitions(libimgdoc2 PUBLIC _SQLITEIMGDOCSTATICLIB=1)

target_include_directories(libimgdoc2 INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc> 
  )
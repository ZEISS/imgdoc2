// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

/// Values that represent "well known items" in the 'GENERAL'-table.
enum class GeneralTableItems
{
    kVersion,
    kTilesDataTable,    ///< An enum constant representing "Name of the 'TILESDATA'-table".
    kTilesInfoTable,    ///< An enum constant representing "Name of the 'TILESINFO'-table".
    kDocType,           ///< An enum constant representing the document type.
    kBlobTable,         ///< An enum constant representing "Name of the 'BLOB'-table".
    kSpatialIndexTable  ///< An enum constant representing the "Name of the 'Spatial-Index'-table".
};

/// Here we gather constants for the imgdoc2-database desgin. "Constant" means that this should be the
/// only place where those value appear, and this should be the only place that needs to be changed.
class DbConstants
{
public:
    /// The name of the General table ("GENERAL").
    static const char* const kGeneralTable_Name;  

    /// The name of the column with the keys for the property-bag in the General table ("Key").
    static const char* const kGeneralTable_KeyColumnName;         

    /// The name of the column with the values for the property-bag in the General table ("ValueString").
    static const char* const kGeneralTable_ValueStringColumnName; // = "ValueString"

    static const char* const kTilesInfoTable_DefaultName;         // = "TILESINFO"
    static const char* const kTilesDataTable_DefaultName;         // = "TILESDATA"
    static const char* const kTilesSpatialIndexTable_DefaultName; // = "TILESSPATIALINDEX"
    static const char* const kBlobTable_DefaultName;              // = "BLOBS"

    static const char* const kTilesDataTable_Column_Pk_DefaultName;
    static const char* const kTilesDataTable_Column_PixelWidth_DefaultName;
    static const char* const kTilesDataTable_Column_PixelHeight_DefaultName;
    static const char* const kTilesDataTable_Column_PixelType_DefaultName;
    static const char* const kTilesDataTable_Column_TileDataType_DefaultName;
    static const char* const kTilesDataTable_Column_BinDataStorageType_DefaultName;
    static const char* const kTilesDataTable_Column_BinDataId_DefaultName;


    static const char* const kTilesInfoTable_Column_Pk_DefaultName;
    static const char* const kTilesInfoTable_Column_TileX_DefaultName;
    static const char* const kTilesInfoTable_Column_TileY_DefaultName;
    static const char* const kTilesInfoTable_Column_TileW_DefaultName;
    static const char* const kTilesInfoTable_Column_TileH_DefaultName;
    static const char* const kTilesInfoTable_Column_PyramidLevel_DefaultName;
    static const char* const kTilesInfoTable_Column_TileDataId_DefaultName;

    static const char* const kBlobTable_Column_Pk_DefaultName;
    static const char* const kBlobTable_Column_Data_DefaultName;

    static const char* const kSqliteSpatialIndexTable_Column_Pk_DefaultName;
    static const char* const kSqliteSpatialIndexTable_Column_minX_DefaultName;
    static const char* const kSqliteSpatialIndexTable_Column_maxX_DefaultName;
    static const char* const kSqliteSpatialIndexTable_Column_minY_DefaultName;
    static const char* const kSqliteSpatialIndexTable_Column_maxY_DefaultName;

    static const char* const kDimensionColumnPrefix_Default;  // = "Dim_"
    static const char* const kIndexForDimensionColumnPrefix_Default; // = "IndexForDim_"


    static const char* GetGeneralTable_ItemKey(GeneralTableItems item);
};


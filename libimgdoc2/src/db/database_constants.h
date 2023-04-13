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
    kSpatialIndexTable, ///< An enum constant representing the "Name of the 'Spatial-Index'-table".
    kMetadataTable      ///< An enum constant representing the "Name of the 'Metadata'-table".
};

/// Here we gather constants for the imgdoc2-database design. "Constant" means that this should be the
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
    static const char* const kMetadataTable_DefaultName;          // = "METADATA"

    static const char* const kTilesDataTable_Column_Pk_DefaultName;
    static const char* const kTilesDataTable_Column_PixelWidth_DefaultName;
    static const char* const kTilesDataTable_Column_PixelHeight_DefaultName;
    static const char* const kTilesDataTable_Column_PixelType_DefaultName;
    static const char* const kTilesDataTable_Column_TileDataType_DefaultName;
    static const char* const kTilesDataTable_Column_BinDataStorageType_DefaultName;
    static const char* const kTilesDataTable_Column_BinDataId_DefaultName;

    static const char* const kTilesDataTable_Column_PixelDepth_DefaultName;

    static const char* const kTilesInfoTable_Column_Pk_DefaultName;
    static const char* const kTilesInfoTable_Column_TileX_DefaultName;
    static const char* const kTilesInfoTable_Column_TileY_DefaultName;
    static const char* const kTilesInfoTable_Column_TileW_DefaultName;
    static const char* const kTilesInfoTable_Column_TileH_DefaultName;
    static const char* const kTilesInfoTable_Column_PyramidLevel_DefaultName;
    static const char* const kTilesInfoTable_Column_TileDataId_DefaultName;

    static const char* const kTilesInfoTable_Column_TileZ_DefaultName;
    static const char* const kTilesInfoTable_Column_TileD_DefaultName;

    static const char* const kBlobTable_Column_Pk_DefaultName;
    static const char* const kBlobTable_Column_Data_DefaultName;

    static const char* const kSqliteSpatialIndexTable_Column_Pk_DefaultName;
    static const char* const kSqliteSpatialIndexTable_Column_minX_DefaultName;
    static const char* const kSqliteSpatialIndexTable_Column_maxX_DefaultName;
    static const char* const kSqliteSpatialIndexTable_Column_minY_DefaultName;
    static const char* const kSqliteSpatialIndexTable_Column_maxY_DefaultName;
    static const char* const kSqliteSpatialIndexTable_Column_minZ_DefaultName;
    static const char* const kSqliteSpatialIndexTable_Column_maxZ_DefaultName;

    static const char* const kMetadataTable_Column_Pk_DefaultName;
    static const char* const kMetadataTable_Column_Name_DefaultName;
    static const char* const kMetadataTable_Column_AncestorId_DefaultName;
    static const char* const kMetadataTable_Column_TypeDiscriminator_DefaultName;
    static const char* const kMetadataTable_Column_ValueDouble_DefaultName;
    static const char* const kMetadataTable_Column_ValueInteger_DefaultName;
    static const char* const kMetadataTable_Column_ValueString_DefaultName;

    static const char* const kDimensionColumnPrefix_Default;  // = "Dim_"
    static const char* const kIndexForDimensionColumnPrefix_Default; // = "IndexForDim_"

    /// Gets the "key" for the given item in the 'GENERAL'-table.
    /// \param  item    The item to query the name for.
    /// \returns    The name for the general table item key.
    static const char* GetGeneralTable_ItemKey(GeneralTableItems item);
};


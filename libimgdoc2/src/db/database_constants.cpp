// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include <stdexcept>
#include "database_constants.h"

using namespace std;

/*static*/const char* const DbConstants::kGeneralTable_Name = "GENERAL";

/*static*/const char* const DbConstants::kGeneralTable_KeyColumnName = "Key";
/*static*/const char* const DbConstants::kGeneralTable_ValueStringColumnName = "ValueString";

/*static*/const char* const DbConstants::kTilesInfoTable_DefaultName = "TILESINFO";
/*static*/const char* const DbConstants::kTilesDataTable_DefaultName = "TILESDATA";
/*static*/const char* const DbConstants::kTilesSpatialIndexTable_DefaultName = "TILESSPATIALINDEX";
/*static*/const char* const DbConstants::kBlobTable_DefaultName = "BLOBS";
/*static*/const char* const DbConstants::kMetadataTable_DefaultName = "METADATA";

/*static*/const char* const DbConstants::kTilesDataTable_Column_Pk_DefaultName = "Pk";
/*static*/const char* const DbConstants::kTilesDataTable_Column_PixelWidth_DefaultName = "PixelWidth";
/*static*/const char* const DbConstants::kTilesDataTable_Column_PixelHeight_DefaultName = "PixelHeight";
/*static*/const char* const DbConstants::kTilesDataTable_Column_PixelType_DefaultName = "PixelType";
/*static*/const char* const DbConstants::kTilesDataTable_Column_TileDataType_DefaultName = "TileDataType";
/*static*/const char* const DbConstants::kTilesDataTable_Column_BinDataStorageType_DefaultName = "BinDataStorageType";
/*static*/const char* const DbConstants::kTilesDataTable_Column_BinDataId_DefaultName = "BinDataId";

/*static*/const char* const DbConstants::kTilesDataTable_Column_PixelDepth_DefaultName = "PixelDepth";

/*static*/const char* const DbConstants::kTilesInfoTable_Column_Pk_DefaultName = "Pk";
/*static*/const char* const DbConstants::kTilesInfoTable_Column_TileX_DefaultName = "TileX";
/*static*/const char* const DbConstants::kTilesInfoTable_Column_TileY_DefaultName = "TileY";
/*static*/const char* const DbConstants::kTilesInfoTable_Column_TileW_DefaultName = "TileW";
/*static*/const char* const DbConstants::kTilesInfoTable_Column_TileH_DefaultName = "TileH";
/*static*/const char* const DbConstants::kTilesInfoTable_Column_PyramidLevel_DefaultName = "PyramidLevel";
/*static*/const char* const DbConstants::kTilesInfoTable_Column_TileDataId_DefaultName = "TileDataId";

/*static*/const char* const DbConstants::kTilesInfoTable_Column_TileZ_DefaultName = "TileZ";
/*static*/const char* const DbConstants::kTilesInfoTable_Column_TileD_DefaultName = "TileD";

/*static*/const char* const DbConstants::kBlobTable_Column_Pk_DefaultName = "Pk";
/*static*/const char* const DbConstants::kBlobTable_Column_Data_DefaultName = "Data";

/*static*/const char* const DbConstants::kSqliteSpatialIndexTable_Column_Pk_DefaultName = "id";
/*static*/const char* const DbConstants::kSqliteSpatialIndexTable_Column_minX_DefaultName = "minX";
/*static*/const char* const DbConstants::kSqliteSpatialIndexTable_Column_maxX_DefaultName = "maxX";
/*static*/const char* const DbConstants::kSqliteSpatialIndexTable_Column_minY_DefaultName = "minY";
/*static*/const char* const DbConstants::kSqliteSpatialIndexTable_Column_maxY_DefaultName = "maxY";
/*static*/const char* const DbConstants::kSqliteSpatialIndexTable_Column_minZ_DefaultName = "minZ";
/*static*/const char* const DbConstants::kSqliteSpatialIndexTable_Column_maxZ_DefaultName = "maxZ";

/*static*/const char* const DbConstants::kMetadataTable_Column_Pk_DefaultName = "Pk";
/*static*/const char* const DbConstants::kMetadataTable_Column_Name_DefaultName = "Name";
/*static*/const char* const DbConstants::kMetadataTable_Column_AncestorId_DefaultName = "AncestorId";
/*static*/const char* const DbConstants::kMetadataTable_Column_TypeDiscriminator_DefaultName = "TypeDiscriminator";
/*static*/const char* const DbConstants::kMetadataTable_Column_ValueDouble_DefaultName = "ValueDouble";
/*static*/const char* const DbConstants::kMetadataTable_Column_ValueInteger_DefaultName = "ValueInteger";
/*static*/const char* const DbConstants::kMetadataTable_Column_ValueString_DefaultName = "ValueString";

/*static*/const char* const DbConstants::kDimensionColumnPrefix_Default = "Dim_";
/*static*/const char* const DbConstants::kIndexForDimensionColumnPrefix_Default = "IndexForDim_";

/*static*/const char* DbConstants::GetGeneralTable_ItemKey(GeneralTableItems item)
{
    switch (item)
    {
        case GeneralTableItems::kVersion:
            return "Version";
        case GeneralTableItems::kTilesDataTable:
            return "TilesDataTable";
        case GeneralTableItems::kTilesInfoTable:
            return "TilesInfoTable";
        case GeneralTableItems::kDocType:
            return "DocType";
        case GeneralTableItems::kBlobTable:
            return "BlobTable";
        case GeneralTableItems::kSpatialIndexTable:
            return "SpatialIndexTable";
        case GeneralTableItems::kMetadataTable:
            return "MetadataTable";
    }

    throw std::invalid_argument("invalid argument for 'item' specified.");
}

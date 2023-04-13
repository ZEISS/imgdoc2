// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include "database_configuration.h"
#include "database_constants.h"
#include <stdexcept>

#include "database_utilities.h"

using namespace std;

void DatabaseConfigurationCommon::SetTableName(TableTypeCommon tableType, const char* name)
{
    if (name != nullptr)
    {
        this->map_tabletype_to_tablename_[tableType] = name;
    }
    else
    {
        this->map_tabletype_to_tablename_.erase(tableType);
    }
}

/*virtual*/bool DatabaseConfigurationCommon::TryGetTableName(TableTypeCommon table_type, std::string* name) const
{
    const auto iterator = this->map_tabletype_to_tablename_.find(table_type);
    if (iterator == this->map_tabletype_to_tablename_.cend())
    {
        return false;
    }

    if (name != nullptr)
    {
        *name = iterator->second;
    }

    return true;
}

std::string DatabaseConfigurationCommon::GetTableNameOrThrow(TableTypeCommon table_type) const
{
    std::string table_name;
    if (!this->TryGetTableName(table_type, &table_name))
    {
        throw std::runtime_error("table-name not present");
    }

    return table_name;
}

bool DatabaseConfigurationCommon::TryGetColumnNameOfGeneralInfoTable(int columnIdentifier, std::string* column_name) const
{
    const char* requested_column_name = nullptr;
    switch (columnIdentifier)
    {
        case kGeneralInfoTable_Column_Key:
            requested_column_name = "Key";
            break;
        case kGeneralInfoTable_Column_ValueString:
            requested_column_name = "ValueString";
            break;
        default:
            break;
    }

    if (requested_column_name == nullptr)
    {
        return false;
    }

    if (column_name != nullptr)
    {
        *column_name = requested_column_name;
    }

    return true;
}

void DatabaseConfigurationCommon::SetColumnNameForBlobTable(int column_identifier, const char* column_name)
{
    SetColumnName(this->map_blobtable_columnids_to_columnname_, column_identifier, column_name);
}

bool DatabaseConfigurationCommon::TryGetColumnNameOfBlobTable(int column_identifier, std::string* column_name) const
{
    return GetColumnName(this->map_blobtable_columnids_to_columnname_, column_identifier, column_name);
}

void DatabaseConfigurationCommon::SetColumnNameForMetadataTable(int column_identifier, const char* column_name)
{
    SetColumnName(this->map_metadatatable_columnids_to_columnname_, column_identifier, column_name);
}

bool DatabaseConfigurationCommon::TryGetColumnNameOfMetadataTable(int column_identifier, std::string* column_name) const
{
    return GetColumnName(this->map_metadatatable_columnids_to_columnname_, column_identifier, column_name);
}

std::string DatabaseConfigurationCommon::GetTableNameForTilesDataOrThrow() const
{
    return this->GetTableNameOrThrow(TableTypeCommon::TilesData);
}

std::string DatabaseConfigurationCommon::GetTableNameForTilesInfoOrThrow() const
{
    return this->GetTableNameOrThrow(TableTypeCommon::TilesInfo);
}

std::string DatabaseConfigurationCommon::GetTableNameForGeneralTableOrThrow() const
{
    return this->GetTableNameOrThrow(TableTypeCommon::GeneralInfo);
}

std::string DatabaseConfigurationCommon::GetTableNameForTilesSpatialIndexTableOrThrow() const
{
    return this->GetTableNameOrThrow(TableTypeCommon::TilesSpatialIndex);
}

std::string DatabaseConfigurationCommon::GetTableNameForBlobTableOrThrow() const
{
    return this->GetTableNameOrThrow(TableTypeCommon::Blobs);
}

std::string DatabaseConfigurationCommon::GetTableNameForMetadataTableOrThrow() const
{
    return this->GetTableNameOrThrow(TableTypeCommon::Metadata);
}

std::string DatabaseConfigurationCommon::GetColumnNameOfGeneralInfoTableOrThrow(int column_identifier) const
{
    string general_table_name;
    if (!this->TryGetColumnNameOfGeneralInfoTable(column_identifier, &general_table_name))
    {
        throw std::runtime_error("column-name not present");
    }

    return general_table_name;
}

bool DatabaseConfigurationCommon::GetIsUsingSpatialIndex() const
{
    const auto iterator = this->map_tabletype_to_tablename_.find(TableTypeCommon::TilesSpatialIndex);
    return iterator != this->map_tabletype_to_tablename_.cend();
}

bool DatabaseConfigurationCommon::GetHasBlobsTable() const
{
    const auto iterator = this->map_tabletype_to_tablename_.find(TableTypeCommon::Blobs);
    return iterator != this->map_tabletype_to_tablename_.cend();
}

bool DatabaseConfigurationCommon::GetHasMetadataTable() const
{
    const auto iterator = this->map_tabletype_to_tablename_.find(TableTypeCommon::Metadata);
    return iterator != this->map_tabletype_to_tablename_.cend();
}

bool DatabaseConfigurationCommon::IsDimensionIndexed(imgdoc2::Dimension dimension) const
{
    return this->indexed_dimensions_.find(dimension) != this->indexed_dimensions_.cend();
}

bool DatabaseConfigurationCommon::IsTileDimensionValid(imgdoc2::Dimension dimension) const
{
    return this->dimensions_.find(dimension) != this->dimensions_.cend();
}

std::string DatabaseConfigurationCommon::GetColumnNameOfBlobTableOrThrow(int column_identifier) const
{
    std::string column_name;;
    if (!this->TryGetColumnNameOfBlobTable(column_identifier, &column_name))
    {
        throw std::runtime_error("column-name not present");
    }

    return column_name;
}

std::string DatabaseConfigurationCommon::GetColumnNameOfMetadataTableOrThrow(int column_identifier) const
{
    std::string column_name;;
    if (!this->TryGetColumnNameOfMetadataTable(column_identifier, &column_name))
    {
        throw std::runtime_error("column-name not present");
    }

    return column_name;
}

/*static*/void DatabaseConfigurationCommon::SetColumnName(std::map<int, std::string>& map, int columnIdentifier, const char* column_name)
{
    if (column_name != nullptr)
    {
        map[columnIdentifier] = column_name;
    }
    else
    {
        map.erase(columnIdentifier);
    }
}

/*static*/bool DatabaseConfigurationCommon::GetColumnName(const std::map<int, std::string>& map, int columnIdentifier, std::string* column_name)
{
    const auto iterator = map.find(columnIdentifier);
    if (iterator == map.cend())
    {
        return false;
    }

    if (column_name != nullptr)
    {
        *column_name = iterator->second;
    }

    return true;
}

void DatabaseConfigurationCommon::SetDefaultColumnNamesForMetadataTable()
{
    this->SetColumnNameForMetadataTable(DatabaseConfigurationCommon::kMetadataTable_Column_Pk, DbConstants::kMetadataTable_Column_Pk_DefaultName);
    this->SetColumnNameForMetadataTable(DatabaseConfigurationCommon::kMetadataTable_Column_Name, DbConstants::kMetadataTable_Column_Name_DefaultName);
    this->SetColumnNameForMetadataTable(DatabaseConfigurationCommon::kMetadataTable_Column_AncestorId, DbConstants::kMetadataTable_Column_AncestorId_DefaultName);
    this->SetColumnNameForMetadataTable(DatabaseConfigurationCommon::kMetadataTable_Column_TypeDiscriminator, DbConstants::kMetadataTable_Column_TypeDiscriminator_DefaultName);
    this->SetColumnNameForMetadataTable(DatabaseConfigurationCommon::kMetadataTable_Column_ValueDouble, DbConstants::kMetadataTable_Column_ValueDouble_DefaultName);
    this->SetColumnNameForMetadataTable(DatabaseConfigurationCommon::kMetadataTable_Column_ValueInteger, DbConstants::kMetadataTable_Column_ValueInteger_DefaultName);
    this->SetColumnNameForMetadataTable(DatabaseConfigurationCommon::kMetadataTable_Column_ValueString, DbConstants::kMetadataTable_Column_ValueString_DefaultName);
}

// ----------------------------------------------------------------------------

/*virtual*/ [[nodiscard]] imgdoc2::DocumentType DatabaseConfiguration2D::GetDocumentType() const
{
    return imgdoc2::DocumentType::kImage2d;
}

void DatabaseConfiguration2D::SetColumnNameForTilesInfoTable(int columnIdentifier, const char* column_name)
{
    SetColumnName(this->map_tilesinfotable_columnids_to_columnname_, columnIdentifier, column_name);
}

bool DatabaseConfiguration2D::TryGetColumnNameOfTilesInfoTable(int columnIdentifier, std::string* column_name) const
{
    return GetColumnName(this->map_tilesinfotable_columnids_to_columnname_, columnIdentifier, column_name);
}

void DatabaseConfiguration2D::SetColumnNameForTilesDataTable(int columnIdentifier, const char* column_name)
{
    SetColumnName(this->map_tilesdatatable_columnids_to_columnname_, columnIdentifier, column_name);
}

bool DatabaseConfiguration2D::TryGetColumnNameOfTilesDataTable(int columnIdentifier, std::string* column_name) const
{
    return GetColumnName(this->map_tilesdatatable_columnids_to_columnname_, columnIdentifier, column_name);
}

void DatabaseConfiguration2D::SetColumnNameForTilesSpatialIndexTable(int columnIdentifier, const char* column_name)
{
    SetColumnName(this->map_tilespatialindextable_columnids_to_columnname_, columnIdentifier, column_name);
}

bool DatabaseConfiguration2D::TryGetColumnNameOfTilesSpatialIndexTable(int columnIdentifier, std::string* column_name) const
{
    return GetColumnName(this->map_tilespatialindextable_columnids_to_columnname_, columnIdentifier, column_name);
}


std::string DatabaseConfiguration2D::GetColumnNameOfTilesDataTableOrThrow(int column_identifier) const
{
    std::string column_name;
    if (!this->TryGetColumnNameOfTilesDataTable(column_identifier, &column_name))
    {
        throw std::runtime_error("column-name not present");
    }

    return column_name;
}

std::string DatabaseConfiguration2D::GetColumnNameOfTilesInfoTableOrThrow(int columnIdentifier) const
{
    std::string column_name;
    if (!this->TryGetColumnNameOfTilesInfoTable(columnIdentifier, &column_name))
    {
        throw std::runtime_error("column-name not present");
    }

    return column_name;
}

std::string DatabaseConfiguration2D::GetColumnNameOfTilesSpatialIndexTableOrThrow(int columnIdentifier) const
{
    std::string column_name;
    if (!this->TryGetColumnNameOfTilesSpatialIndexTable(columnIdentifier, &column_name))
    {
        throw std::runtime_error("column-name not present");
    }

    return column_name;
}

void DatabaseConfiguration2D::SetDefaultColumnNamesForTilesInfoTable()
{
    this->SetColumnNameForTilesInfoTable(DatabaseConfiguration2D::kTilesInfoTable_Column_Pk, DbConstants::kTilesInfoTable_Column_Pk_DefaultName/*"Pk"*/);
    this->SetColumnNameForTilesInfoTable(DatabaseConfiguration2D::kTilesInfoTable_Column_TileX, DbConstants::kTilesInfoTable_Column_TileX_DefaultName/*"TileX"*/);
    this->SetColumnNameForTilesInfoTable(DatabaseConfiguration2D::kTilesInfoTable_Column_TileY, DbConstants::kTilesInfoTable_Column_TileY_DefaultName/*"TileY"*/);
    this->SetColumnNameForTilesInfoTable(DatabaseConfiguration2D::kTilesInfoTable_Column_TileW, DbConstants::kTilesInfoTable_Column_TileW_DefaultName /*"TileW"*/);
    this->SetColumnNameForTilesInfoTable(DatabaseConfiguration2D::kTilesInfoTable_Column_TileH, DbConstants::kTilesInfoTable_Column_TileH_DefaultName/*"TileH"*/);
    this->SetColumnNameForTilesInfoTable(DatabaseConfiguration2D::kTilesInfoTable_Column_PyramidLevel, DbConstants::kTilesInfoTable_Column_PyramidLevel_DefaultName/*"PyramidLevel"*/);
    this->SetColumnNameForTilesInfoTable(DatabaseConfiguration2D::kTilesInfoTable_Column_TileDataId, DbConstants::kTilesInfoTable_Column_TileDataId_DefaultName/*"TileDataId"*/);
}

void DatabaseConfiguration2D::SetDefaultColumnNamesForTilesDataTable()
{
    this->SetColumnNameForTilesDataTable(DatabaseConfiguration2D::kTilesDataTable_Column_Pk, DbConstants::kTilesDataTable_Column_Pk_DefaultName/*"Pk"*/);
    this->SetColumnNameForTilesDataTable(DatabaseConfiguration2D::kTilesDataTable_Column_PixelWidth, DbConstants::kTilesDataTable_Column_PixelWidth_DefaultName/*"PixelWidth"*/);
    this->SetColumnNameForTilesDataTable(DatabaseConfiguration2D::kTilesDataTable_Column_PixelHeight, DbConstants::kTilesDataTable_Column_PixelHeight_DefaultName/*"PixelHeight"*/);
    this->SetColumnNameForTilesDataTable(DatabaseConfiguration2D::kTilesDataTable_Column_PixelType, DbConstants::kTilesDataTable_Column_PixelType_DefaultName/*"PixelType"*/);
    this->SetColumnNameForTilesDataTable(DatabaseConfiguration2D::kTilesDataTable_Column_TileDataType, DbConstants::kTilesDataTable_Column_TileDataType_DefaultName/*"TileDataType"*/);
    this->SetColumnNameForTilesDataTable(DatabaseConfiguration2D::kTilesDataTable_Column_BinDataStorageType, DbConstants::kTilesDataTable_Column_BinDataStorageType_DefaultName/*"BinDataStorageType"*/);
    this->SetColumnNameForTilesDataTable(DatabaseConfiguration2D::kTilesDataTable_Column_BinDataId, DbConstants::kTilesDataTable_Column_BinDataId_DefaultName/*"BinDataId"*/);
}

// ----------------------------------------------------------------------------

/*virtual*/imgdoc2::DocumentType DatabaseConfiguration3D::GetDocumentType() const
{
    return imgdoc2::DocumentType::kImage3d;
}

void DatabaseConfiguration3D::SetColumnNameForTilesInfoTable(int columnIdentifier, const char* column_name)
{
    SetColumnName(this->map_tilesinfotable_columnids_to_columnname_, columnIdentifier, column_name);
}

bool DatabaseConfiguration3D::TryGetColumnNameOfTilesInfoTable(int columnIdentifier, std::string* column_name) const
{
    return GetColumnName(this->map_tilesinfotable_columnids_to_columnname_, columnIdentifier, column_name);
}

void DatabaseConfiguration3D::SetColumnNameForTilesDataTable(int columnIdentifier, const char* column_name)
{
    SetColumnName(this->map_tilesdatatable_columnids_to_columnname_, columnIdentifier, column_name);
}

bool DatabaseConfiguration3D::TryGetColumnNameOfTilesDataTable(int columnIdentifier, std::string* column_name) const
{
    return GetColumnName(this->map_tilesdatatable_columnids_to_columnname_, columnIdentifier, column_name);
}

void DatabaseConfiguration3D::SetColumnNameForTilesSpatialIndexTable(int columnIdentifier, const char* column_name)
{
    SetColumnName(this->map_tilespatialindextable_columnids_to_columnname_, columnIdentifier, column_name);
}

bool DatabaseConfiguration3D::TryGetColumnNameOfTilesSpatialIndexTable(int columnIdentifier, std::string* column_name) const
{
    return GetColumnName(this->map_tilespatialindextable_columnids_to_columnname_, columnIdentifier, column_name);
}


std::string DatabaseConfiguration3D::GetColumnNameOfTilesDataTableOrThrow(int column_identifier) const
{
    std::string column_name;
    if (!this->TryGetColumnNameOfTilesDataTable(column_identifier, &column_name))
    {
        throw std::runtime_error("column-name not present");
    }

    return column_name;
}

std::string DatabaseConfiguration3D::GetColumnNameOfTilesInfoTableOrThrow(int columnIdentifier) const
{
    std::string column_name;
    if (!this->TryGetColumnNameOfTilesInfoTable(columnIdentifier, &column_name))
    {
        throw std::runtime_error("column-name not present");
    }

    return column_name;
}

std::string DatabaseConfiguration3D::GetColumnNameOfTilesSpatialIndexTableOrThrow(int columnIdentifier) const
{
    std::string column_name;
    if (!this->TryGetColumnNameOfTilesSpatialIndexTable(columnIdentifier, &column_name))
    {
        throw std::runtime_error("column-name not present");
    }

    return column_name;
}

void DatabaseConfiguration3D::SetDefaultColumnNamesForTilesInfoTable()
{
    this->SetColumnNameForTilesInfoTable(DatabaseConfiguration3D::kTilesInfoTable_Column_Pk, DbConstants::kTilesInfoTable_Column_Pk_DefaultName/*"Pk"*/);
    this->SetColumnNameForTilesInfoTable(DatabaseConfiguration3D::kTilesInfoTable_Column_TileX, DbConstants::kTilesInfoTable_Column_TileX_DefaultName/*"TileX"*/);
    this->SetColumnNameForTilesInfoTable(DatabaseConfiguration3D::kTilesInfoTable_Column_TileY, DbConstants::kTilesInfoTable_Column_TileY_DefaultName/*"TileY"*/);
    this->SetColumnNameForTilesInfoTable(DatabaseConfiguration3D::kTilesInfoTable_Column_TileZ, DbConstants::kTilesInfoTable_Column_TileZ_DefaultName/*"TileZ"*/);
    this->SetColumnNameForTilesInfoTable(DatabaseConfiguration3D::kTilesInfoTable_Column_TileW, DbConstants::kTilesInfoTable_Column_TileW_DefaultName/*"TileW"*/);
    this->SetColumnNameForTilesInfoTable(DatabaseConfiguration3D::kTilesInfoTable_Column_TileH, DbConstants::kTilesInfoTable_Column_TileH_DefaultName/*"TileH"*/);
    this->SetColumnNameForTilesInfoTable(DatabaseConfiguration3D::kTilesInfoTable_Column_TileD, DbConstants::kTilesInfoTable_Column_TileD_DefaultName/*"TileD"*/);
    this->SetColumnNameForTilesInfoTable(DatabaseConfiguration3D::kTilesInfoTable_Column_PyramidLevel, DbConstants::kTilesInfoTable_Column_PyramidLevel_DefaultName/*"PyramidLevel"*/);
    this->SetColumnNameForTilesInfoTable(DatabaseConfiguration3D::kTilesInfoTable_Column_TileDataId, DbConstants::kTilesInfoTable_Column_TileDataId_DefaultName/*"TileDataId"*/);
}

void DatabaseConfiguration3D::SetDefaultColumnNamesForTilesDataTable()
{
    this->SetColumnNameForTilesDataTable(DatabaseConfiguration3D::kTilesDataTable_Column_Pk, DbConstants::kTilesDataTable_Column_Pk_DefaultName/*"Pk"*/);
    this->SetColumnNameForTilesDataTable(DatabaseConfiguration3D::kTilesDataTable_Column_PixelWidth, DbConstants::kTilesDataTable_Column_PixelWidth_DefaultName/*"PixelWidth"*/);
    this->SetColumnNameForTilesDataTable(DatabaseConfiguration3D::kTilesDataTable_Column_PixelHeight, DbConstants::kTilesDataTable_Column_PixelHeight_DefaultName/*"PixelHeight"*/);
    this->SetColumnNameForTilesDataTable(DatabaseConfiguration3D::kTilesDataTable_Column_PixelDepth, DbConstants::kTilesDataTable_Column_PixelDepth_DefaultName/*"PixelDepth"*/);
    this->SetColumnNameForTilesDataTable(DatabaseConfiguration3D::kTilesDataTable_Column_PixelDepth, DbConstants::kTilesDataTable_Column_PixelDepth_DefaultName/*"PixelDepth"*/);
    this->SetColumnNameForTilesDataTable(DatabaseConfiguration3D::kTilesDataTable_Column_PixelType, DbConstants::kTilesDataTable_Column_PixelType_DefaultName/*"PixelType"*/);
    this->SetColumnNameForTilesDataTable(DatabaseConfiguration3D::kTilesDataTable_Column_TileDataType, DbConstants::kTilesDataTable_Column_TileDataType_DefaultName/*"TileDataType"*/);
    this->SetColumnNameForTilesDataTable(DatabaseConfiguration3D::kTilesDataTable_Column_BinDataStorageType, DbConstants::kTilesDataTable_Column_BinDataStorageType_DefaultName/*"BinDataStorageType"*/);
    this->SetColumnNameForTilesDataTable(DatabaseConfiguration3D::kTilesDataTable_Column_BinDataId, DbConstants::kTilesDataTable_Column_BinDataId_DefaultName/*"BinDataId"*/);
}

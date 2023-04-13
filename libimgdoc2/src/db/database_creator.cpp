// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include <sstream>
#include <string>
#include <gsl/gsl>
#include <imgdoc2.h>
#include "database_creator.h"
#include "database_configuration.h"
#include "database_constants.h"
#include "DbFactory.h"
#include "database_discovery.h"
#include "database_utilities.h"
#include "utilities.h"

using namespace std;
using namespace imgdoc2;

std::shared_ptr<DatabaseConfiguration2D> DbCreator::CreateTables2d(const imgdoc2::ICreateOptions* create_options)
{
    DbCreator::ThrowIfDocumentTypeIsNotAsSpecified(create_options, imgdoc2::DocumentType::kImage2d);

    // construct the "database configuration" based on the the create_options
    auto database_configuration = make_shared<DatabaseConfiguration2D>();
    this->Initialize2dConfigurationFromCreateOptions(database_configuration.get(), create_options);

    // TODO(JBL): make those operations a transaction
    auto sql_statement = this->GenerateSqlStatementForCreatingGeneralTable_Sqlite(database_configuration.get());
    this->db_connection_->Execute(sql_statement);

    sql_statement = this->GenerateSqlStatementForFillingGeneralTable_Sqlite(database_configuration.get());
    this->db_connection_->Execute(sql_statement);

    sql_statement = this->GenerateSqlStatementForCreatingTilesDataTable_Sqlite(database_configuration.get());
    this->db_connection_->Execute(sql_statement);

    sql_statement = this->GenerateSqlStatementForCreatingTilesInfoTable_Sqlite(database_configuration.get());
    this->db_connection_->Execute(sql_statement);

    sql_statement = this->GenerateSqlStatementForCreatingMetadataTable_Sqlite(database_configuration.get());
    this->db_connection_->Execute(sql_statement);

    if (create_options->GetUseSpatialIndex())
    {
        sql_statement = this->GenerateSqlStatementForCreatingSpatialTilesIndex_Sqlite(database_configuration.get());
        this->db_connection_->Execute(sql_statement);

        // and, add its name to the "General" table
        this->SetGeneralTableInfoForSpatialIndex(database_configuration.get());
    }

    if (create_options->GetCreateBlobTable())
    {
        sql_statement = GenerateSqlStatementForCreatingBlobTable_Sqlite(database_configuration.get());
        this->db_connection_->Execute(sql_statement);
        this->SetBlobTableNameInGeneralTable(database_configuration.get());
    }

    return database_configuration;
}

std::shared_ptr<DatabaseConfiguration3D> DbCreator::CreateTables3d(const imgdoc2::ICreateOptions* create_options)
{
    DbCreator::ThrowIfDocumentTypeIsNotAsSpecified(create_options, imgdoc2::DocumentType::kImage3d);

    // construct the "database configuration" based on the the create_options
    auto database_configuration = make_shared<DatabaseConfiguration3D>();
    this->Initialize3dConfigurationFromCreateOptions(database_configuration.get(), create_options);

    // TODO(JBL): make those operations a transaction
    auto sql_statement = this->GenerateSqlStatementForCreatingGeneralTable_Sqlite(database_configuration.get());
    this->db_connection_->Execute(sql_statement);

    sql_statement = this->GenerateSqlStatementForFillingGeneralTable_Sqlite(database_configuration.get());
    this->db_connection_->Execute(sql_statement);

    sql_statement = this->GenerateSqlStatementForCreatingTilesDataTable_Sqlite(database_configuration.get());
    this->db_connection_->Execute(sql_statement);

    sql_statement = this->GenerateSqlStatementForCreatingTilesInfoTable_Sqlite(database_configuration.get());
    this->db_connection_->Execute(sql_statement);

    sql_statement = this->GenerateSqlStatementForCreatingMetadataTable_Sqlite(database_configuration.get());
    this->db_connection_->Execute(sql_statement);

    if (create_options->GetUseSpatialIndex())
    {
        sql_statement = this->GenerateSqlStatementForCreatingSpatialTilesIndex_Sqlite(database_configuration.get());
        this->db_connection_->Execute(sql_statement);

        // and, add its name to the "General" table
        this->SetGeneralTableInfoForSpatialIndex(database_configuration.get());
    }

    if (create_options->GetCreateBlobTable())
    {
        sql_statement = GenerateSqlStatementForCreatingBlobTable_Sqlite(database_configuration.get());
        this->db_connection_->Execute(sql_statement);
        this->SetBlobTableNameInGeneralTable(database_configuration.get());
    }

    return database_configuration;
}

std::string DbCreator::GenerateSqlStatementForCreatingTilesDataTable_Sqlite(const DatabaseConfiguration2D* database_configuration)
{
    stringstream string_stream;
    string_stream << "CREATE TABLE[" << database_configuration->GetTableNameForTilesDataOrThrow() << "](" <<
        "[" << database_configuration->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_Pk) << "] INTEGER PRIMARY KEY," <<
        "[" << database_configuration->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_PixelWidth) << "] INTEGER(4) NOT NULL," <<
        "[" << database_configuration->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_PixelHeight) << "] INTEGER(4) NOT NULL," <<
        "[" << database_configuration->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_PixelType) << "] INTEGER(1) NOT NULL," <<
        "[" << database_configuration->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_TileDataType) << "] INTEGER(1) NOT NULL," <<
        "[" << database_configuration->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_BinDataStorageType) << "] INTEGER(1)," <<
        "[" << database_configuration->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_BinDataId) << "] INTEGER(8)";
    string_stream << ");";

    return string_stream.str();
}

std::string DbCreator::GenerateSqlStatementForCreatingTilesDataTable_Sqlite(const DatabaseConfiguration3D* database_configuration)
{
    stringstream string_stream;
    string_stream << "CREATE TABLE[" << database_configuration->GetTableNameForTilesDataOrThrow() << "](" <<
        "[" << database_configuration->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration3D::kTilesDataTable_Column_Pk) << "] INTEGER PRIMARY KEY," <<
        "[" << database_configuration->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration3D::kTilesDataTable_Column_PixelWidth) << "] INTEGER(4) NOT NULL," <<
        "[" << database_configuration->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration3D::kTilesDataTable_Column_PixelHeight) << "] INTEGER(4) NOT NULL," <<
        "[" << database_configuration->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration3D::kTilesDataTable_Column_PixelDepth) << "] INTEGER(4) NOT NULL," <<
        "[" << database_configuration->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration3D::kTilesDataTable_Column_PixelType) << "] INTEGER(1) NOT NULL," <<
        "[" << database_configuration->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration3D::kTilesDataTable_Column_TileDataType) << "] INTEGER(1) NOT NULL," <<
        "[" << database_configuration->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration3D::kTilesDataTable_Column_BinDataStorageType) << "] INTEGER(1)," <<
        "[" << database_configuration->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration3D::kTilesDataTable_Column_BinDataId) << "] INTEGER(8)";
    string_stream << ");";

    return string_stream.str();
}

std::string DbCreator::GenerateSqlStatementForCreatingTilesInfoTable_Sqlite(const DatabaseConfiguration2D* database_configuration)
{
    stringstream string_stream;
    // Notes:
    // * "INTEGER PRIMARY KEY" makes the column-name an alias for the RowId-column
    string_stream << "CREATE TABLE[" << database_configuration->GetTableNameForTilesInfoOrThrow() << "](" <<
        "[" << database_configuration->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_Pk) << "] INTEGER PRIMARY KEY," <<
        "[" << database_configuration->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileX) << "] DOUBLE NOT NULL," <<
        "[" << database_configuration->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileY) << "] DOUBLE NOT NULL," <<
        "[" << database_configuration->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileW) << "] DOUBLE NOT NULL," <<
        "[" << database_configuration->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileH) << "] DOUBLE NOT NULL," <<
        "[" << database_configuration->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_PyramidLevel) << "] INTEGER(1) NOT NULL,";

    string_stream << "[" << database_configuration->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileDataId) << "] INTEGER(8) NOT NULL";

    for (const auto dim : database_configuration->GetTileDimensions())
    {
        string colName = database_configuration->GetDimensionsColumnPrefix() + dim;
        string_stream << ", [" << colName << "] INTEGER(4) NOT NULL";
    }

    string_stream << ");";

    // create the indices for the "dimension tables"
    for (const auto dim : database_configuration->GetIndexedTileDimensions())
    {
        string_stream << "CREATE INDEX [" << database_configuration->GetIndexForDimensionColumnPrefix() << dim << "] ON "
            << "[" << database_configuration->GetTableNameForTilesInfoOrThrow() << "] "
            << "( [" << database_configuration->GetDimensionsColumnPrefix() << dim << "]);";
    }

    return string_stream.str();
}

std::string DbCreator::GenerateSqlStatementForCreatingTilesInfoTable_Sqlite(const DatabaseConfiguration3D* database_configuration)
{
    stringstream string_stream;
    // Notes:
    // * "INTEGER PRIMARY KEY" makes the column-name an alias for the RowId-column
    string_stream << "CREATE TABLE[" << database_configuration->GetTableNameForTilesInfoOrThrow() << "](" <<
        "[" << database_configuration->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration3D::kTilesInfoTable_Column_Pk) << "] INTEGER PRIMARY KEY," <<
        "[" << database_configuration->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration3D::kTilesInfoTable_Column_TileX) << "] DOUBLE NOT NULL," <<
        "[" << database_configuration->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration3D::kTilesInfoTable_Column_TileY) << "] DOUBLE NOT NULL," <<
        "[" << database_configuration->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration3D::kTilesInfoTable_Column_TileZ) << "] DOUBLE NOT NULL," <<
        "[" << database_configuration->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration3D::kTilesInfoTable_Column_TileW) << "] DOUBLE NOT NULL," <<
        "[" << database_configuration->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration3D::kTilesInfoTable_Column_TileH) << "] DOUBLE NOT NULL," <<
        "[" << database_configuration->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration3D::kTilesInfoTable_Column_TileD) << "] DOUBLE NOT NULL," <<
        "[" << database_configuration->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration3D::kTilesInfoTable_Column_PyramidLevel) << "] INTEGER(1) NOT NULL,";

    string_stream << "[" << database_configuration->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration3D::kTilesInfoTable_Column_TileDataId) << "] INTEGER(8) NOT NULL";

    // TODO(JBL): the code below can be refactored to be shared with the 2D case
    for (const auto dim : database_configuration->GetTileDimensions())
    {
        string colName = database_configuration->GetDimensionsColumnPrefix() + dim;
        string_stream << ", [" << colName << "] INTEGER(4) NOT NULL";
    }

    string_stream << ");";

    // create the indices for the "dimension tables"
    for (const auto dim : database_configuration->GetIndexedTileDimensions())
    {
        string_stream << "CREATE INDEX [" << database_configuration->GetIndexForDimensionColumnPrefix() << dim << "] ON "
            << "[" << database_configuration->GetTableNameForTilesInfoOrThrow() << "] "
            << "( [" << database_configuration->GetDimensionsColumnPrefix() << dim << "]);";
    }

    return string_stream.str();
}

std::string DbCreator::GenerateSqlStatementForCreatingGeneralTable_Sqlite(const DatabaseConfigurationCommon* database_configuration_common)
{
    stringstream string_stream;
    string_stream << "CREATE TABLE[" << database_configuration_common->GetTableNameForGeneralTableOrThrow() << "](" <<
        "[" << database_configuration_common->GetColumnNameOfGeneralInfoTableOrThrow(DatabaseConfigurationCommon::kGeneralInfoTable_Column_Key) << "] TEXT(40) UNIQUE," <<
        "[" << database_configuration_common->GetColumnNameOfGeneralInfoTableOrThrow(DatabaseConfigurationCommon::kGeneralInfoTable_Column_ValueString) << "] TEXT);";

    return string_stream.str();
}

void DbCreator::SetGeneralTableInfoForSpatialIndex(const DatabaseConfigurationCommon* database_configuration_common)
{
    Expects(database_configuration_common != nullptr && database_configuration_common->GetIsUsingSpatialIndex() == true);

    // insert an item into the "General"-table where we notify about the name of the "tiles-spatial-table"
    Utilities::WriteStringIntoPropertyBag(
        this->db_connection_.get(),
        database_configuration_common->GetTableNameForGeneralTableOrThrow(),
        database_configuration_common->GetColumnNameOfGeneralInfoTableOrThrow(DatabaseConfigurationCommon::kGeneralInfoTable_Column_Key),
        database_configuration_common->GetColumnNameOfGeneralInfoTableOrThrow(DatabaseConfigurationCommon::kGeneralInfoTable_Column_ValueString),
        DbConstants::GetGeneralTable_ItemKey(GeneralTableItems::kSpatialIndexTable),
        database_configuration_common->GetTableNameForTilesSpatialIndexTableOrThrow());
}

std::string DbCreator::GenerateSqlStatementForFillingGeneralTable_Sqlite(const DatabaseConfigurationCommon* database_configuration_common)
{
    stringstream string_stream;
    string_stream << "INSERT INTO [" << database_configuration_common->GetTableNameForGeneralTableOrThrow() << "]" <<
        "([" << database_configuration_common->GetColumnNameOfGeneralInfoTableOrThrow(DatabaseConfigurationCommon::kGeneralInfoTable_Column_Key) << "], " <<
        "[" << database_configuration_common->GetColumnNameOfGeneralInfoTableOrThrow(DatabaseConfigurationCommon::kGeneralInfoTable_Column_ValueString) << "])" <<
        " VALUES('" << DbConstants::GetGeneralTable_ItemKey(GeneralTableItems::kVersion) << "','" << "0.0.1-alpha" << "')," <<
        "('" << DbConstants::GetGeneralTable_ItemKey(GeneralTableItems::kTilesDataTable) << "','" << database_configuration_common->GetTableNameForTilesDataOrThrow() << "')," <<
        "('" << DbConstants::GetGeneralTable_ItemKey(GeneralTableItems::kTilesInfoTable) << "','" << database_configuration_common->GetTableNameForTilesInfoOrThrow() << "')," <<
        "('" << DbConstants::GetGeneralTable_ItemKey(GeneralTableItems::kMetadataTable) << "','" << database_configuration_common->GetTableNameForMetadataTableOrThrow() << "')," <<
        "('" << DbConstants::GetGeneralTable_ItemKey(GeneralTableItems::kDocType) << "','" << DbUtilities::GetDocTypeValueForDocumentType(database_configuration_common->GetDocumentType()) << "');";

    return string_stream.str();
}

void DbCreator::Initialize2dConfigurationFromCreateOptions(DatabaseConfiguration2D* database_configuration, const imgdoc2::ICreateOptions* create_options)
{
    database_configuration->SetDimensionColumnPrefix(DbConstants::kDimensionColumnPrefix_Default/*"Dim_"*/);
    database_configuration->SetIndexForDimensionColumnPrefix(DbConstants::kIndexForDimensionColumnPrefix_Default/*"IndexForDim_"*/);
    database_configuration->SetTableName(DatabaseConfigurationCommon::TableTypeCommon::GeneralInfo, DbConstants::kGeneralTable_Name/*"GENERAL"*/);
    database_configuration->SetTableName(DatabaseConfigurationCommon::TableTypeCommon::TilesData, DbConstants::kTilesDataTable_DefaultName/*"TILESDATA"*/);
    database_configuration->SetTableName(DatabaseConfigurationCommon::TableTypeCommon::TilesInfo, DbConstants::kTilesInfoTable_DefaultName/*"TILESINFO"*/);
    database_configuration->SetTableName(DatabaseConfigurationCommon::TableTypeCommon::Metadata, DbConstants::kMetadataTable_DefaultName);
    database_configuration->SetDefaultColumnNamesForMetadataTable();// TODO(JBl): should we make the metadata-table optional?
    database_configuration->SetDefaultColumnNamesForTilesDataTable();
    database_configuration->SetDefaultColumnNamesForTilesInfoTable();
    database_configuration->SetTileDimensions(create_options->GetDimensions().cbegin(), create_options->GetDimensions().cend());
    database_configuration->SetIndexedTileDimensions(create_options->GetIndexedDimensions().cbegin(), create_options->GetIndexedDimensions().cend());

    if (create_options->GetUseSpatialIndex())
    {
        database_configuration->SetTableName(DatabaseConfigurationCommon::TableTypeCommon::TilesSpatialIndex, DbConstants::kTilesSpatialIndexTable_DefaultName/*"TILESSPATIALINDEX"*/);
        database_configuration->SetColumnNameForTilesSpatialIndexTable(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_Pk, DbConstants::kSqliteSpatialIndexTable_Column_Pk_DefaultName/*"id"*/);
        database_configuration->SetColumnNameForTilesSpatialIndexTable(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MinX, DbConstants::kSqliteSpatialIndexTable_Column_minX_DefaultName /*"minX"*/);
        database_configuration->SetColumnNameForTilesSpatialIndexTable(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MaxX, DbConstants::kSqliteSpatialIndexTable_Column_maxX_DefaultName /*"maxX"*/);
        database_configuration->SetColumnNameForTilesSpatialIndexTable(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MinY, DbConstants::kSqliteSpatialIndexTable_Column_minY_DefaultName /*"minY"*/);
        database_configuration->SetColumnNameForTilesSpatialIndexTable(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MaxY, DbConstants::kSqliteSpatialIndexTable_Column_maxY_DefaultName /*"maxY"*/);
    }

    if (create_options->GetCreateBlobTable())
    {
        database_configuration->SetTableName(DatabaseConfigurationCommon::TableTypeCommon::Blobs, DbConstants::kBlobTable_DefaultName);
        database_configuration->SetColumnNameForBlobTable(DatabaseConfiguration2D::kBlobTable_Column_Pk, DbConstants::kBlobTable_Column_Pk_DefaultName);
        database_configuration->SetColumnNameForBlobTable(DatabaseConfiguration2D::kBlobTable_Column_Data, DbConstants::kBlobTable_Column_Data_DefaultName);
    }
}

void DbCreator::Initialize3dConfigurationFromCreateOptions(DatabaseConfiguration3D* database_configuration, const imgdoc2::ICreateOptions* create_options)
{
    database_configuration->SetDimensionColumnPrefix(DbConstants::kDimensionColumnPrefix_Default/*"Dim_"*/);
    database_configuration->SetIndexForDimensionColumnPrefix(DbConstants::kIndexForDimensionColumnPrefix_Default/*"IndexForDim_"*/);
    database_configuration->SetTableName(DatabaseConfigurationCommon::TableTypeCommon::GeneralInfo, DbConstants::kGeneralTable_Name/*"GENERAL"*/);
    database_configuration->SetTableName(DatabaseConfigurationCommon::TableTypeCommon::TilesData, DbConstants::kTilesDataTable_DefaultName/*"TILESDATA"*/);
    database_configuration->SetTableName(DatabaseConfigurationCommon::TableTypeCommon::TilesInfo, DbConstants::kTilesInfoTable_DefaultName/*"TILESINFO"*/);
    database_configuration->SetTableName(DatabaseConfigurationCommon::TableTypeCommon::Metadata, DbConstants::kMetadataTable_DefaultName);
    database_configuration->SetDefaultColumnNamesForMetadataTable();// TODO(JBl): should we make the metadata-table optional?
    database_configuration->SetDefaultColumnNamesForTilesDataTable();

    database_configuration->SetDefaultColumnNamesForTilesInfoTable();
    database_configuration->SetTileDimensions(create_options->GetDimensions().cbegin(), create_options->GetDimensions().cend());
    database_configuration->SetIndexedTileDimensions(create_options->GetIndexedDimensions().cbegin(), create_options->GetIndexedDimensions().cend());

    if (create_options->GetUseSpatialIndex())
    {
        database_configuration->SetTableName(DatabaseConfigurationCommon::TableTypeCommon::TilesSpatialIndex, DbConstants::kTilesSpatialIndexTable_DefaultName/*"TILESSPATIALINDEX"*/);
        database_configuration->SetColumnNameForTilesSpatialIndexTable(DatabaseConfiguration3D::kTilesSpatialIndexTable_Column_Pk, DbConstants::kSqliteSpatialIndexTable_Column_Pk_DefaultName/*"id"*/);
        database_configuration->SetColumnNameForTilesSpatialIndexTable(DatabaseConfiguration3D::kTilesSpatialIndexTable_Column_MinX, DbConstants::kSqliteSpatialIndexTable_Column_minX_DefaultName /*"minX"*/);
        database_configuration->SetColumnNameForTilesSpatialIndexTable(DatabaseConfiguration3D::kTilesSpatialIndexTable_Column_MaxX, DbConstants::kSqliteSpatialIndexTable_Column_maxX_DefaultName /*"maxX"*/);
        database_configuration->SetColumnNameForTilesSpatialIndexTable(DatabaseConfiguration3D::kTilesSpatialIndexTable_Column_MinY, DbConstants::kSqliteSpatialIndexTable_Column_minY_DefaultName /*"minY"*/);
        database_configuration->SetColumnNameForTilesSpatialIndexTable(DatabaseConfiguration3D::kTilesSpatialIndexTable_Column_MaxY, DbConstants::kSqliteSpatialIndexTable_Column_maxY_DefaultName /*"maxY"*/);
        database_configuration->SetColumnNameForTilesSpatialIndexTable(DatabaseConfiguration3D::kTilesSpatialIndexTable_Column_MinZ, DbConstants::kSqliteSpatialIndexTable_Column_minZ_DefaultName /*"minZ"*/);
        database_configuration->SetColumnNameForTilesSpatialIndexTable(DatabaseConfiguration3D::kTilesSpatialIndexTable_Column_MaxZ, DbConstants::kSqliteSpatialIndexTable_Column_maxZ_DefaultName /*"maxZ"*/);
    }

    if (create_options->GetCreateBlobTable())
    {
        database_configuration->SetTableName(DatabaseConfigurationCommon::TableTypeCommon::Blobs, DbConstants::kBlobTable_DefaultName);
        database_configuration->SetColumnNameForBlobTable(DatabaseConfiguration2D::kBlobTable_Column_Pk, DbConstants::kBlobTable_Column_Pk_DefaultName);
        database_configuration->SetColumnNameForBlobTable(DatabaseConfiguration2D::kBlobTable_Column_Data, DbConstants::kBlobTable_Column_Data_DefaultName);
    }
}

std::string DbCreator::GenerateSqlStatementForCreatingSpatialTilesIndex_Sqlite(const DatabaseConfiguration2D* database_configuration)
{
    Expects(database_configuration != nullptr && database_configuration->GetIsUsingSpatialIndex() == true);

    auto string_stream = ostringstream();
    string_stream << "CREATE VIRTUAL TABLE " << database_configuration->GetTableNameForTilesSpatialIndexTableOrThrow() << " USING rtree(" <<
        database_configuration->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_Pk) << "," <<         // Integer primary key
        database_configuration->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MinX) << "," <<       // Minimum X coordinate"
        database_configuration->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MaxX) << "," <<       // Maximum X coordinate"
        database_configuration->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MinY) << "," <<       // Minimum Y coordinate"
        database_configuration->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MaxY) << ");";        // Maximum Y coordinate"
    return string_stream.str();
}

std::string DbCreator::GenerateSqlStatementForCreatingSpatialTilesIndex_Sqlite(const DatabaseConfiguration3D* database_configuration)
{
    Expects(database_configuration != nullptr && database_configuration->GetIsUsingSpatialIndex() == true);

    ostringstream string_stream;
    string_stream << "CREATE VIRTUAL TABLE " << database_configuration->GetTableNameForTilesSpatialIndexTableOrThrow() << " USING rtree(" <<
        database_configuration->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration3D::kTilesSpatialIndexTable_Column_Pk) << "," <<         // Integer primary key
        database_configuration->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration3D::kTilesSpatialIndexTable_Column_MinX) << "," <<       // Minimum X coordinate"
        database_configuration->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration3D::kTilesSpatialIndexTable_Column_MaxX) << "," <<       // Maximum X coordinate"
        database_configuration->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration3D::kTilesSpatialIndexTable_Column_MinY) << "," <<       // Minimum Y coordinate"
        database_configuration->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration3D::kTilesSpatialIndexTable_Column_MaxY) << "," <<       // Maximum Y coordinate"
        database_configuration->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration3D::kTilesSpatialIndexTable_Column_MinZ) << "," <<       // Minimum Z coordinate"
        database_configuration->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration3D::kTilesSpatialIndexTable_Column_MaxZ) << ");";        // Maximum Z coordinate"
    return string_stream.str();
}

std::string DbCreator::GenerateSqlStatementForCreatingBlobTable_Sqlite(const DatabaseConfiguration2D* database_configuration)
{
    Expects(database_configuration != nullptr && database_configuration->GetHasBlobsTable() == true);

    ostringstream string_stream;
    string_stream << "CREATE TABLE [" << database_configuration->GetTableNameForBlobTableOrThrow() << "] (" <<
        "[" << database_configuration->GetColumnNameOfBlobTableOrThrow(DatabaseConfiguration2D::kBlobTable_Column_Pk) << "] INTEGER PRIMARY KEY," <<
        "[" << database_configuration->GetColumnNameOfBlobTableOrThrow(DatabaseConfiguration2D::kBlobTable_Column_Data) << "] BLOB );";

    return string_stream.str();
}

std::string DbCreator::GenerateSqlStatementForCreatingBlobTable_Sqlite(const DatabaseConfiguration3D* database_configuration)
{
    Expects(database_configuration != nullptr && database_configuration->GetHasBlobsTable() == true);

    ostringstream string_stream;
    string_stream << "CREATE TABLE [" << database_configuration->GetTableNameForBlobTableOrThrow() << "] (" <<
        "[" << database_configuration->GetColumnNameOfBlobTableOrThrow(DatabaseConfiguration3D::kBlobTable_Column_Pk) << "] INTEGER PRIMARY KEY," <<
        "[" << database_configuration->GetColumnNameOfBlobTableOrThrow(DatabaseConfiguration3D::kBlobTable_Column_Data) << "] BLOB );";

    return string_stream.str();
}

void DbCreator::SetBlobTableNameInGeneralTable(const DatabaseConfigurationCommon* database_configuration_common)
{
    ostringstream string_stream;
    string_stream << "INSERT INTO [" << database_configuration_common->GetTableNameForGeneralTableOrThrow() << "]" <<
        "([" << database_configuration_common->GetColumnNameOfGeneralInfoTableOrThrow(DatabaseConfigurationCommon::kGeneralInfoTable_Column_Key) << "], " <<
        "[" << database_configuration_common->GetColumnNameOfGeneralInfoTableOrThrow(DatabaseConfigurationCommon::kGeneralInfoTable_Column_ValueString) << "])" <<
        " VALUES('" << "BlobTable" << "','" << database_configuration_common->GetTableNameForBlobTableOrThrow() << "');";
    this->db_connection_->Execute(string_stream.str());
}

/*static*/void DbCreator::ThrowIfDocumentTypeIsNotAsSpecified(const imgdoc2::ICreateOptions* create_options, imgdoc2::DocumentType document_type)
{
    if (create_options->GetDocumentType() != document_type)
    {
        throw internal_error_exception("Document type is not as expected");
    }
}

std::string DbCreator::GenerateSqlStatementForCreatingMetadataTable_Sqlite(const DatabaseConfigurationCommon* database_configuration_common)
{
    ostringstream string_stream;
    string_stream << "CREATE TABLE [" << database_configuration_common->GetTableNameForMetadataTableOrThrow() << "] (" <<
        "[" << database_configuration_common->GetColumnNameOfMetadataTableOrThrow(DatabaseConfigurationCommon::kMetadataTable_Column_Pk) << "] INTEGER PRIMARY KEY," <<
        "[" << database_configuration_common->GetColumnNameOfMetadataTableOrThrow(DatabaseConfigurationCommon::kMetadataTable_Column_Name) << "] TEXT NOT NULL," <<
        "[" << database_configuration_common->GetColumnNameOfMetadataTableOrThrow(DatabaseConfigurationCommon::kMetadataTable_Column_AncestorId) << "] INTEGER," <<
        "[" << database_configuration_common->GetColumnNameOfMetadataTableOrThrow(DatabaseConfigurationCommon::kMetadataTable_Column_TypeDiscriminator) << "] INTEGER," <<
        "[" << database_configuration_common->GetColumnNameOfMetadataTableOrThrow(DatabaseConfigurationCommon::kMetadataTable_Column_ValueDouble) << "] REAL," <<
        "[" << database_configuration_common->GetColumnNameOfMetadataTableOrThrow(DatabaseConfigurationCommon::kMetadataTable_Column_ValueInteger) << "] INTEGER," <<
        "[" << database_configuration_common->GetColumnNameOfMetadataTableOrThrow(DatabaseConfigurationCommon::kMetadataTable_Column_ValueString) << "] TEXT," <<
        "FOREIGN KEY(" << database_configuration_common->GetColumnNameOfMetadataTableOrThrow(DatabaseConfigurationCommon::kMetadataTable_Column_AncestorId) << ") REFERENCES " <<
        database_configuration_common->GetTableNameForMetadataTableOrThrow() << "(" << database_configuration_common->GetColumnNameOfMetadataTableOrThrow(DatabaseConfigurationCommon::kMetadataTable_Column_Pk) << "),"
        "UNIQUE(" << database_configuration_common->GetColumnNameOfMetadataTableOrThrow(DatabaseConfigurationCommon::kMetadataTable_Column_Name) << "," << 
        database_configuration_common->GetColumnNameOfMetadataTableOrThrow(DatabaseConfigurationCommon::kMetadataTable_Column_AncestorId) << ") )" << ";";
    // the combination of Name and AncestorId must be unique, in other words, the names of items with the same ancestor must be unique

    return string_stream.str();
}

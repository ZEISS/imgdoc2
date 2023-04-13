// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <string>
#include <memory>
#include "IDbConnection.h"
#include "database_configuration.h"

class DbCreator
{
private:
    std::shared_ptr<IDbConnection> db_connection_;
public:
    explicit DbCreator(std::shared_ptr<IDbConnection> dbConnection) :
        db_connection_(dbConnection)
    {}

    std::shared_ptr<DatabaseConfiguration2D> CreateTables2d(const imgdoc2::ICreateOptions* create_options);
    std::shared_ptr<DatabaseConfiguration3D> CreateTables3d(const imgdoc2::ICreateOptions* create_options);
private:
    void Initialize2dConfigurationFromCreateOptions(DatabaseConfiguration2D* database_configuration, const imgdoc2::ICreateOptions* create_options);
    void Initialize3dConfigurationFromCreateOptions(DatabaseConfiguration3D* database_configuration, const imgdoc2::ICreateOptions* create_options);

    std::string GenerateSqlStatementForCreatingGeneralTable_Sqlite(const DatabaseConfigurationCommon* database_configuration_common);
    std::string GenerateSqlStatementForFillingGeneralTable_Sqlite(const DatabaseConfigurationCommon* database_configuration_common);
    std::string GenerateSqlStatementForCreatingTilesDataTable_Sqlite(const DatabaseConfiguration2D* database_configuration);
    std::string GenerateSqlStatementForCreatingTilesInfoTable_Sqlite(const DatabaseConfiguration2D* database_configuration);
    std::string GenerateSqlStatementForCreatingSpatialTilesIndex_Sqlite(const DatabaseConfiguration2D* database_configuration);
    std::string GenerateSqlStatementForCreatingBlobTable_Sqlite(const DatabaseConfiguration2D* database_configuration);

    std::string GenerateSqlStatementForCreatingTilesDataTable_Sqlite(const DatabaseConfiguration3D* database_configuration);
    std::string GenerateSqlStatementForCreatingTilesInfoTable_Sqlite(const DatabaseConfiguration3D* database_configuration);
    std::string GenerateSqlStatementForCreatingSpatialTilesIndex_Sqlite(const DatabaseConfiguration3D* database_configuration);
    std::string GenerateSqlStatementForCreatingBlobTable_Sqlite(const DatabaseConfiguration3D* database_configuration);

    /// Generates the SQL statement for creating metadata table (for SQLite).
    /// \param  database_configuration_common   The database configuration.
    /// \returns    The SQL statement for creating the metadata table.
    std::string GenerateSqlStatementForCreatingMetadataTable_Sqlite(const DatabaseConfigurationCommon* database_configuration_common);

    void SetBlobTableNameInGeneralTable(const DatabaseConfigurationCommon* database_configuration_common);

    void SetGeneralTableInfoForSpatialIndex(const DatabaseConfigurationCommon* database_configuration_common);

    static void ThrowIfDocumentTypeIsNotAsSpecified(const imgdoc2::ICreateOptions* create_options, imgdoc2::DocumentType document_type);
};

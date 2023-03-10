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

    std::shared_ptr<DatabaseConfigurationCommon> CreateTables(const imgdoc2::ICreateOptions* create_options);

private:
    void Initialize2dConfigurationFromCreateOptions(DatabaseConfiguration2D* database_configuration, const imgdoc2::ICreateOptions* create_options);

    std::string GenerateSqlStatementForCreatingGeneralTable_Sqlite(const DatabaseConfiguration2D* database_configuration);
    std::string GenerateSqlStatementForFillingGeneralTable_Sqlite(const DatabaseConfiguration2D* database_configuration);
    std::string GenerateSqlStatementForCreatingTilesDataTable_Sqlite(const DatabaseConfiguration2D* database_configuration);
    std::string GenerateSqlStatementForCreatingTilesInfoTable_Sqlite(const DatabaseConfiguration2D* database_configuration);
    std::string GenerateSqlStatementForCreatingSpatialTilesIndex_Sqlite(const DatabaseConfiguration2D* database_configuration);
    std::string GenerateSqlStatementForCreatingBlobTable_Sqlite(const DatabaseConfiguration2D* database_configuration);

    void SetBlobTableNameInGeneralTable(const DatabaseConfiguration2D* database_configuration);

    void SetGeneralTableInfoForSpatialIndex(const DatabaseConfiguration2D* database_configuration);
};

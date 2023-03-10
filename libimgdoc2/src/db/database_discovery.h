// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <vector>
#include <utility>
#include <string>
#include <memory>
#include "IDbConnection.h"
#include "database_configuration.h"

class DbDiscovery
{
private:
    std::shared_ptr<IDbConnection> db_connection_;

    std::shared_ptr<DatabaseConfigurationCommon> configuration_;
private:
    struct GeneralDataDiscoveryResult
    {
        std::string tilesdatatable_name;
        std::string tileinfotable_name;
        std::string blobtable_name;
        std::string spatial_index_table_name;

        imgdoc2::DocumentType document_type { imgdoc2::DocumentType::kInvalid };
        std::vector<imgdoc2::Dimension> dimensions;
        std::vector<imgdoc2::Dimension> indexed_dimensions;
    };

public:
    explicit DbDiscovery(std::shared_ptr<IDbConnection> dbConnection) :
        db_connection_(std::move(dbConnection))
    {}

    void DoDiscovery();

    std::shared_ptr<DatabaseConfigurationCommon> GetDatabaseConfiguration();

private:
    GeneralDataDiscoveryResult DiscoverGeneralTable();

    /// This method tries to populate the 'general_table_discovery_result' with additonal information and validates
    /// that information. On input, it is expecting only the table-names to contain information, those names
    /// are then here validated and the remaining fields are populated.
    /// In case of an (unrecoverable) error, this method will throw an exception.
    /// \param [in,out] {GeneralDataDiscoveryResult&} On input, it is expected that the table-names are filled, on exit the other fields are populated and validated.
    void Check_Tables_And_Determine_Dimensions(GeneralDataDiscoveryResult& general_table_discovery_result);

    void FillInformationForConfiguration2D(const GeneralDataDiscoveryResult& general_data_discovery_result, DatabaseConfiguration2D& configuration_2d);

    struct ExpectedColumnsInfo
    {
        explicit ExpectedColumnsInfo(const char* sz) : column_name(sz) {}
        std::string column_name;
    };
};

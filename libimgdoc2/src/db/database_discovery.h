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

/// This class is used for discovering a database and its configuration. It is checked whether
/// the database is a valid "imgdoc2" database and if so, the configuration is determined.
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
        std::string metadatatable_name;

        imgdoc2::DocumentType document_type { imgdoc2::DocumentType::kInvalid };
        std::vector<imgdoc2::Dimension> dimensions;
        std::vector<imgdoc2::Dimension> indexed_dimensions;
    };

public:
    DbDiscovery() = delete;

    /// Constructor.
    ///
    /// \param  dbConnection    The database connection.
    explicit DbDiscovery(std::shared_ptr<IDbConnection> dbConnection) :
        db_connection_(std::move(dbConnection))
    {}

    /// Executes the discovery operation. It is checked whether the database is a valid "imgdoc2" database and if so,
    /// its type and configuration is determined. In case of an error, an exception is thrown.
    void DoDiscovery();

    /// Gets the document type.
    ///
    /// \returns    The document type.
    [[nodiscard]] imgdoc2::DocumentType GetDocumentType() const;

    /// Gets database configuration, i.e. the least common denominator, the base type from which all 
    /// configuration have to derive from.
    /// \returns    The base database configuration if available; otherwise, null.
    [[nodiscard]] std::shared_ptr<DatabaseConfigurationCommon> GetDatabaseConfigurationCommon() const;

    /// Gets database configuration object for an image-2D document. If the document is not an image-2D document,
    /// an internal_error_exception is thrown.
    /// \returns   The database configuration object for an image-2D document.
    [[nodiscard]] std::shared_ptr<DatabaseConfiguration2D> GetDatabaseConfiguration2DOrThrow() const;

    /// Gets database configuration object for an image-3D document. If the document is not an image-3D document,
    /// an internal_error_exception is thrown.
    /// \returns   The database configuration object for an image-3D document.
    [[nodiscard]] std::shared_ptr<DatabaseConfiguration3D> GetDatabaseConfiguration3DOrThrow() const;

    /// Gets database configuration object for an image-2D document. If the document is not an image-2D document,
    /// null is returned.
   /// \returns   The database configuration object for an image-2D document if available; otherwise, null.
    [[nodiscard]] std::shared_ptr<DatabaseConfiguration2D> GetDatabaseConfiguration2DOrNull() const;

    /// Gets database configuration object for an image-3D document. If the document is not an image-3D document,
    /// null is returned.
    /// \returns   The database configuration object for an image-3D document if available; otherwise, null.
    [[nodiscard]] std::shared_ptr<DatabaseConfiguration3D> GetDatabaseConfiguration3DOrNull() const;
private:
    GeneralDataDiscoveryResult DiscoverGeneralTable();

    /// This method tries to populate the 'general_table_discovery_result' with additional information and validates
    /// that information. On input, it is expecting only the table-names to contain information, those names
    /// are then here validated and the remaining fields are populated.
    /// In case of an (unrecoverable) error, this method will throw an exception.
    /// \param [in,out] general_table_discovery_result  On input, it is expected that the table-names are filled, on exit the other fields are populated and validated.
    void Check_Tables_And_Determine_Dimensions(GeneralDataDiscoveryResult& general_table_discovery_result);

    void FillInformationForConfiguration2D(const GeneralDataDiscoveryResult& general_data_discovery_result, DatabaseConfiguration2D& configuration_2d);
    void FillInformationForConfiguration3D(const GeneralDataDiscoveryResult& general_data_discovery_result, DatabaseConfiguration3D& configuration_3d);

    struct ExpectedColumnsInfo
    {
        explicit ExpectedColumnsInfo(const char* sz) : column_name(sz) {}
        std::string column_name;
    };
};

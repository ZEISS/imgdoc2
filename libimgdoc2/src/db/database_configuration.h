// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <vector>
#include <unordered_set>
#include <map>
#include <algorithm>
#include <iterator>
#include <string>
#include <stdexcept>
#include <imgdoc2.h>

/// The purpose of this class is to represent all information/configuration required to operate
/// on the database. This includes
/// - names of the various tables/column in use  
/// - certain options or "information about the state" like which columns are indexed or not  
/// - options/configurations like "spatial index active or not"  
/// The idea is that "DatabaseConfigurationCommon" covers all options which are common for all supported
/// "document types", and that we have derived classes specific for a document type.
class DatabaseConfigurationCommon
{
public:
    /// Values that represent the tables (which are of relevance to us).
    enum class TableTypeCommon
    {
        GeneralInfo,
        TilesData,
        TilesInfo,
        TilesSpatialIndex,
        Blobs
    };

    static constexpr int kGeneralInfoTable_Column_Key = 1;  ///< Identifier for the "key column" in the "general" table
    static constexpr int kGeneralInfoTable_Column_ValueString = 2;  ///< Identifier for the "value(string) column" in the "general" table

    static constexpr int kBlobTable_Column_Pk = 1;
    static constexpr int kBlobTable_Column_Data = 2;
private:
    std::unordered_set<imgdoc2::Dimension> dimensions_;
    std::unordered_set<imgdoc2::Dimension> indexed_dimensions_;
    std::map<TableTypeCommon, std::string> map_tabletype_to_tablename_;
    std::string dimension_column_prefix_;
    std::string index_for_dimension_prefix_;
    std::map<int, std::string> map_blobtable_columnids_to_columnname_;
public:
    template<typename ForwardIterator>
    void SetTileDimensions(ForwardIterator begin, ForwardIterator end)
    {
        this->dimensions_.clear();
        for (auto it = begin; it != end; ++it)
        {
            this->dimensions_.insert(*it);
        }
    }

    template<typename ForwardIterator>
    void SetIndexedTileDimensions(ForwardIterator begin, ForwardIterator end)
    {
        this->indexed_dimensions_.clear();
        for (auto it = begin; it != end; ++it)
        {
            this->indexed_dimensions_.insert(*it);
        }
    }

    const std::unordered_set<imgdoc2::Dimension>& GetTileDimensions() const { return this->dimensions_; }
    const std::unordered_set<imgdoc2::Dimension>& GetIndexedTileDimensions() const { return this->indexed_dimensions_; }

    bool IsDimensionIndexed(imgdoc2::Dimension) const;

    /// Queries if the specified tile dimension is valid (for this document).
    /// \param  dimension  The dimension to check.
    /// \returns    True if the tile dimension is valid for this document, false if not.
    bool IsTileDimensionValid(imgdoc2::Dimension dimension) const;

    void SetDimensionColumnPrefix(const char* prefix) { this->dimension_column_prefix_ = prefix; }
    void SetIndexForDimensionColumnPrefix(const char* prefix) { this->index_for_dimension_prefix_ = prefix; }
    const std::string& GetDimensionsColumnPrefix() const { return this->dimension_column_prefix_; }
    const std::string& GetIndexForDimensionColumnPrefix() const { return this->index_for_dimension_prefix_; }

    void SetTableName(TableTypeCommon tableType, const char* name);

    virtual bool TryGetTableName(TableTypeCommon table_type, std::string* name) const;

    bool TryGetColumnNameOfGeneralInfoTable(int columnIdentifier, std::string* column_name) const;

    void SetColumnNameForBlobTable(int column_identifier, const char* column_name);
    bool TryGetColumnNameOfBlobTable(int column_identifier, std::string* column_name) const;

    virtual ~DatabaseConfigurationCommon() = default;
public:
    std::string GetTableNameOrThrow(TableTypeCommon table_type) const;
    std::string GetTableNameForTilesDataOrThrow() const;
    std::string GetTableNameForTilesInfoOrThrow() const;
    std::string GetTableNameForGeneralTableOrThrow() const;
    std::string GetColumnNameOfGeneralInfoTableOrThrow(int column_identifier) const;
    std::string GetColumnNameOfBlobTableOrThrow(int column_identifier) const;
    std::string GetTableNameForTilesSpatialIndexTableOrThrow() const;
    std::string GetTableNameForBlobTableOrThrow() const;

    bool GetIsUsingSpatialIndex() const;
    bool GetHasBlobsTable() const;

protected:
    static void SetColumnName(std::map<int, std::string>& map, int columnIdentifier, const char* column_name);
    static bool GetColumnName(const std::map<int, std::string>& map, int columnIdentifier, std::string* column_name);
};


/// This class is intended to capture the "state of the database configuration" for 2D-documents.
class DatabaseConfiguration2D : public DatabaseConfigurationCommon
{
private:
    std::map<int, std::string> map_tilesinfotable_columnids_to_columnname_;
    std::map<int, std::string> map_tilesdatatable_columnids_to_columnname_;
    std::map<int, std::string> map_tilespatialindextable_columnids_to_columnname_;
public:
    static constexpr int kTilesInfoTable_Column_Pk = 1;
    static constexpr int kTilesInfoTable_Column_TileX = 2;
    static constexpr int kTilesInfoTable_Column_TileY = 3;
    static constexpr int kTilesInfoTable_Column_TileW = 4;
    static constexpr int kTilesInfoTable_Column_TileH = 5;
    static constexpr int kTilesInfoTable_Column_PyramidLevel = 6;
    static constexpr int kTilesInfoTable_Column_TileDataId = 7;

    static constexpr int kTilesDataTable_Column_Pk = 1;
    static constexpr int kTilesDataTable_Column_PixelWidth = 2;
    static constexpr int kTilesDataTable_Column_PixelHeight = 3;
    static constexpr int kTilesDataTable_Column_PixelType = 4;
    static constexpr int kTilesDataTable_Column_TileDataType = 5;
    static constexpr int kTilesDataTable_Column_BinDataStorageType = 6;
    static constexpr int kTilesDataTable_Column_BinDataId = 7;

    static constexpr int kTilesSpatialIndexTable_Column_Pk = 1;
    static constexpr int kTilesSpatialIndexTable_Column_MinX = 2;
    static constexpr int kTilesSpatialIndexTable_Column_MaxX = 3;
    static constexpr int kTilesSpatialIndexTable_Column_MinY = 4;
    static constexpr int kTilesSpatialIndexTable_Column_MaxY = 5;

    /// Gets document type constant - the string for the row "DocType" in the general-table which identifies
    /// the data as "tiles-2D".
    ///
    /// \returns    The document type constant.
    const std::string& GetDocTypeConstant() const;

    void SetColumnNameForTilesInfoTable(int columnIdentifier, const char* column_name);
    bool TryGetColumnNameOfTilesInfoTable(int columnIdentifier, std::string* column_name) const;

    void SetColumnNameForTilesDataTable(int columnIdentifier, const char* column_name);
    bool TryGetColumnNameOfTilesDataTable(int columnIdentifier, std::string* column_name) const;

    void SetColumnNameForTilesSpatialIndexTable(int columnIdentifier, const char* column_name);
    bool TryGetColumnNameOfTilesSpatialIndexTable(int columnIdentifier, std::string* column_name) const;

    void SetDefaultColumnNamesForTilesInfoTable();
    void SetDefaultColumnNamesForTilesDataTable();
public:
    [[nodiscard]] std::string GetColumnNameOfTilesInfoTableOrThrow(int columnIdentifier) const;
    [[nodiscard]] std::string GetColumnNameOfTilesDataTableOrThrow(int column_identifier) const;
    [[nodiscard]] std::string GetColumnNameOfTilesSpatialIndexTableOrThrow(int columnIdentifier) const;
};

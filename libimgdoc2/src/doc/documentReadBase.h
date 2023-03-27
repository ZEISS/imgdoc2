// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <unordered_set>
#include <utility>
#include <map>
#include <functional>
#include <sstream>
#include <memory>
#include <vector>
#include <string>
#include "document.h"

/// This class contains common functionality and utilities for implementing the document-read-access classes.
class DocumentReadBase
{
private:
    std::shared_ptr<Document> document_;
protected:
    explicit DocumentReadBase(std::shared_ptr<Document> document) : document_(std::move(document))
    {}

    static void GetEntityDimensionsInternal(const std::unordered_set<imgdoc2::Dimension>& tile_dimensions, imgdoc2::Dimension* dimensions, std::uint32_t& count);

    /// Gets minimum and maximum for the specified tile dimensions. This function will use the specified functor 'func_is_dimension_valid' 
    /// to determine if a dimension is valid, and the other functor 'func_add_dimension_table_name' is used to give the column name for a dimension.
    /// The name of the table which is queried is specified by the parameter 'table_name'.
    ///
    /// \param  dimensions_to_query_for         The dimensions to query for.
    /// \param  func_is_dimension_valid         A functor which determines if a dimension is valid.
    /// \param  func_add_dimension_table_name    A functor which adds the column name for a dimension to the specified string stream.
    /// \param  table_name                      Name of the table to be queried.
    ///
    /// \returns    A map containing the min/max-information for the requested dimensions.
    std::map<imgdoc2::Dimension, imgdoc2::Int32Interval> GetMinMaxForTileDimensionInternal(
        const std::vector<imgdoc2::Dimension>& dimensions_to_query_for,
        const std::function<bool(imgdoc2::Dimension)>& func_is_dimension_valid,
        const std::function<void(std::ostringstream&, imgdoc2::Dimension)>& func_add_dimension_table_name,
        const std::string& table_name) const;

    /// Information about columns for the position and the associated extent.
    struct QueryMinMaxForXyzInfo
    {
        std::string column_name_coordinate;         /// Name of the column for the coordinate.
        std::string column_name_coordinate_extent;  /// Name of the column for the coordinate extent.
    };

    /// Creates a statement which queries for the bounding box/cuboid of all tiles/bricks. 
    /// \param  table_name  Name of the table to query (the 'TILESINFO'-table).
    /// \param  query_info  Information listing the columns for the position and the associated extent.
    /// \returns    A statement for retrieving the bounding box/cuboid of all tiles/bricks.
    [[nodiscard]] std::shared_ptr<IDbStatement> CreateQueryMinMaxForXyz(const std::string& table_name, const std::vector<QueryMinMaxForXyzInfo>& query_info) const;

    /// A utility which reads two doubles from the specified statement and sets the values in the specified interval. It uses the specified result index
    /// for reading from the statement. If the pointer 'interval' is null, the function will not read from the statement and do nothing.
    /// The returned integer is the next index to read from, or in other words - the argument 'result_index' plus two (if the pointer 'interval' is not null).
    /// \param [in,out] interval        If non-null, the interval which will be set.
    /// \param [in,out] statement       The database-result statement.
    /// \param          result_index    The index where to start to read from the database-result statement.
    /// \returns    The index (into the database-result statement) where to read the next value from.
    static int SetCoordinateBoundsValueIfNonNull(imgdoc2::DoubleInterval* interval, IDbStatement* statement, int result_index);

    [[nodiscard]] const std::shared_ptr<Document>& GetDocument() const { return this->document_; }
    [[nodiscard]] const std::shared_ptr<imgdoc2::IHostingEnvironment>& GetHostingEnvironment() const { return this->document_->GetHostingEnvironment(); }
private:
    std::shared_ptr<IDbStatement> CreateQueryMinMaxStatement(const std::vector<imgdoc2::Dimension>& dimensions, const std::function<void(std::ostringstream&, imgdoc2::Dimension)>& func_add_dimension_table_name, const std::string& table_name) const;
};

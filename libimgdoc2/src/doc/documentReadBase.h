#pragma once

#include <unordered_set>
#include <utility>
#include <map>
#include <functional>
#include <sstream>
#include <memory>
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
    std::map<imgdoc2::Dimension, imgdoc2::CoordinateBounds> GetMinMaxForTileDimensionInternal(
        const std::vector<imgdoc2::Dimension>& dimensions_to_query_for,
        const std::function<bool(imgdoc2::Dimension)>& func_is_dimension_valid,
        const std::function<void(std::ostringstream&, imgdoc2::Dimension)>& func_add_dimension_table_name, 
        const std::string& table_name) const;

    [[nodiscard]] const std::shared_ptr<Document>& GetDocument() const { return this->document_; }
    [[nodiscard]] const std::shared_ptr<imgdoc2::IHostingEnvironment>& GetHostingEnvironment() const { return this->document_->GetHostingEnvironment(); }
private:
    std::shared_ptr<IDbStatement> CreateQueryMinMaxStatement(const std::vector<imgdoc2::Dimension>& dimensions, const std::function<void(std::ostringstream&, imgdoc2::Dimension)>& func_add_dimension_table_name, const std::string& table_name) const;
};

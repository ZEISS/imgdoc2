// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include "documentReadBase.h"
#include <algorithm>

using namespace std;
using namespace imgdoc2;

/*static*/void DocumentReadBase::GetEntityDimensionsInternal(const unordered_set<imgdoc2::Dimension>& tile_dimensions, imgdoc2::Dimension* dimensions, std::uint32_t& count)
{
    if (dimensions != nullptr)
    {
        copy_n(tile_dimensions.cbegin(), min(count, static_cast<uint32_t>(tile_dimensions.size())), dimensions);
    }

    count = static_cast<uint32_t>(tile_dimensions.size());
}

std::map<imgdoc2::Dimension, imgdoc2::CoordinateBounds> DocumentReadBase::GetMinMaxForTileDimensionInternal(
        const std::vector<imgdoc2::Dimension>& dimensions_to_query_for,
        const std::function<bool(imgdoc2::Dimension)>& func_is_dimension_valid,
        const std::function<void(std::ostringstream&, imgdoc2::Dimension)>& func_add_dimension_table_name,
        const std::string& table_name) const
{
    for (const auto dimension : dimensions_to_query_for)
    {
        const bool is_valid = func_is_dimension_valid(dimension);
        if (!is_valid)
        {
            ostringstream string_stream;
            string_stream << "The dimension '" << dimension << "' is not valid.";
            throw invalid_argument_exception(string_stream.str().c_str());
        }
    }

    if (dimensions_to_query_for.empty())
    {
        return {};
    }

    const auto query_statement = this->CreateQueryMinMaxStatement(dimensions_to_query_for, func_add_dimension_table_name, table_name);

    map<imgdoc2::Dimension, imgdoc2::CoordinateBounds> result;

    // we expect exactly "2 * dimensions_to_query_for.size()" results
    const bool is_done = this->GetDocument()->GetDatabase_connection()->StepStatement(query_statement.get());
    if (!is_done)
    {
        throw internal_error_exception("database-query gave no result, this is unexpected.");
    }

    for (size_t i = 0; i < dimensions_to_query_for.size(); ++i)
    {
        CoordinateBounds coordinate_bounds;
        auto min = query_statement->GetResultInt32OrNull(i * 2);
        auto max = query_statement->GetResultInt32OrNull(i * 2 + 1);
        if (min.has_value() && max.has_value())
        {
            coordinate_bounds.minimum_value = min.value();
            coordinate_bounds.maximum_value = max.value();
        }

        result[dimensions_to_query_for[i]] = coordinate_bounds;
    }

    return result;
}

std::shared_ptr<IDbStatement> DocumentReadBase::CreateQueryMinMaxStatement(
    const std::vector<imgdoc2::Dimension>& dimensions,
    const std::function<void(std::ostringstream&, imgdoc2::Dimension)>& func_add_dimension_table_name,
    const std::string& table_name) const
{
    // preconditions:
    // - the dimensions specified must be valid
    // - the collection must not be empty

    ostringstream string_stream;
    string_stream << "SELECT ";
    bool first_iteration = true;
    for (const auto dimension : dimensions)
    {
        if (!first_iteration)
        {
            string_stream << ',';
        }

        string_stream << "MIN([";
        func_add_dimension_table_name(string_stream, dimension);
        string_stream << "]),";
        string_stream << "MAX([";
        func_add_dimension_table_name(string_stream, dimension);
        string_stream << "])";
        first_iteration = false;
    }

    string_stream << " FROM " << "[" << table_name << "];";

    auto statement = this->GetDocument()->GetDatabase_connection()->PrepareStatement(string_stream.str());
    return statement;
}

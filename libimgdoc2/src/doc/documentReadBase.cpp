// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include "documentReadBase.h"
#include <algorithm>
#include <vector>
#include <string>
#include <gsl/assert>
#include <gsl/narrow>

using namespace std;
using namespace imgdoc2;

/*static*/void DocumentReadBase::GetEntityDimensionsInternal(const unordered_set<imgdoc2::Dimension>& tile_dimensions, imgdoc2::Dimension* dimensions, std::uint32_t& count)
{
    if (dimensions != nullptr)
    {
        copy_n(tile_dimensions.cbegin(), min(count, gsl::narrow<uint32_t>(tile_dimensions.size())), dimensions);
    }

    count = gsl::narrow<uint32_t>(tile_dimensions.size());
}

std::map<imgdoc2::Dimension, imgdoc2::Int32Interval> DocumentReadBase::GetMinMaxForTileDimensionInternal(
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

    map<imgdoc2::Dimension, imgdoc2::Int32Interval> result;

    // we expect exactly "2 * dimensions_to_query_for.size()" results
    const bool is_done = this->GetDocument()->GetDatabase_connection()->StepStatement(query_statement.get());
    if (!is_done)
    {
        throw internal_error_exception("database-query gave no result, this is unexpected.");
    }

    for (int i = 0; i < gsl::narrow<int>(dimensions_to_query_for.size()); ++i)
    {
        Int32Interval coordinate_bounds;
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

std::shared_ptr<IDbStatement> DocumentReadBase::CreateQueryMinMaxForXyz(const std::string& table_name, const std::vector<QueryMinMaxForXyzInfo>& query_info) const
{
    Expects(!query_info.empty());

    ostringstream string_stream;
    string_stream << "SELECT ";
    bool first_iteration = true;
    for (const auto& info : query_info)
    {
        if (!first_iteration)
        {
            string_stream << ',';
        }

        string_stream << "MIN([";
        string_stream << info.column_name_coordinate;
        string_stream << "]),";
        string_stream << "MAX([";
        string_stream << info.column_name_coordinate << "]+[" << info.column_name_coordinate_extent;
        string_stream << "])";

        first_iteration = false;
    }

    string_stream << " FROM [" << table_name << "];";

    auto statement = this->GetDocument()->GetDatabase_connection()->PrepareStatement(string_stream.str());

    return statement;
}

/*static*/int DocumentReadBase::SetCoordinateBoundsValueIfNonNull(imgdoc2::DoubleInterval* interval, IDbStatement* statement, int result_index)
{
    if (interval != nullptr)
    {
        const auto min = statement->GetResultDoubleOrNull(result_index++);
        const auto max = statement->GetResultDoubleOrNull(result_index++);
        if (min.has_value() && max.has_value())
        {
            interval->minimum_value = min.value();
            interval->maximum_value = max.value();
        }
        else
        {
            *interval = DoubleInterval{};
        }
    }

    return result_index;
}

std::uint64_t DocumentReadBase::GetTotalTileCount(const std::string& table_name)
{
    ostringstream string_stream;
    string_stream << "SELECT COUNT(*) FROM [" << table_name << "];";
    const auto statement = this->GetDocument()->GetDatabase_connection()->PrepareStatement(string_stream.str());

    const bool is_done = this->GetDocument()->GetDatabase_connection()->StepStatement(statement.get());
    if (!is_done)
    {
        throw internal_error_exception("database-query gave no result, this is unexpected.");
    }

    const auto result = statement->GetResultInt64(0);
    return result;
}

std::map<int, std::uint64_t> DocumentReadBase::GetTileCountPerLayer(const std::string& table_name, const std::string& pyramid_level_column_name)
{
    ostringstream string_stream;
    string_stream << "SELECT [" << pyramid_level_column_name << "], COUNT(*) FROM [" << table_name << "] GROUP BY [" << pyramid_level_column_name << "];";
    const auto statement = this->GetDocument()->GetDatabase_connection()->PrepareStatement(string_stream.str());

    map<int, uint64_t> result;
    while (this->GetDocument()->GetDatabase_connection()->StepStatement(statement.get()))
    {
        const auto layer = statement->GetResultInt32(0);
        const auto count = statement->GetResultInt64(1);
        result[layer] = count;
    }

    return result;
}

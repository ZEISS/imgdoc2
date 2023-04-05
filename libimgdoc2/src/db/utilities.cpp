// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include <sstream>
#include <limits>
#include <utility>
#include <vector>
#include <tuple>
#include "utilities.h"

using namespace std;
using namespace imgdoc2;

/*static*/std::tuple<std::string, std::vector<Utilities::DataBindInfo>> Utilities::CreateWhereConditionForDimQueryClause(
    const imgdoc2::IDimCoordinateQueryClause* clause,
    const std::function<void(imgdoc2::Dimension, std::string&)>& funcGetColumnNameForDimension)
{
    vector<Utilities::DataBindInfo> databind_info;
    ostringstream string_stream;
    string column_name_for_dimension;
    bool first_dimension_iteration = true;

    for (const auto dimension : clause->GetTileDimsForClause())
    {
        const std::vector<IDimCoordinateQueryClause::RangeClause>* rangeClauses = clause->GetRangeClause(dimension);

        if (rangeClauses != nullptr)
        {
            if (!first_dimension_iteration)
            {
                string_stream << " AND ";
            }

            string_stream << "(";
            bool first_iteration = true;
            funcGetColumnNameForDimension(dimension, column_name_for_dimension);
            for (const auto rangeClause : *rangeClauses)
            {
                if (!first_iteration)
                {
                    string_stream << " OR ";
                }

                if (ProcessRangeClause(column_name_for_dimension, rangeClause, databind_info, string_stream))
                {
                    first_iteration = false;
                }
            }

            string_stream << ")";

            first_dimension_iteration = false;
        }
    }

    if (first_dimension_iteration)
    {
        // This means that the dimension-query-clause was empty, i.e. did not contain a condition.
        // In this case, we create a SQL-fragment "(TRUE)"
        string_stream << "(TRUE)";
    }

    return make_tuple(string_stream.str(), databind_info);
}

/*static*/std::tuple<std::string, std::vector<Utilities::DataBindInfo>> Utilities::CreateWhereConditionForTileInfoQueryClause(const imgdoc2::ITileInfoQueryClause* clause, const std::string& column_name_pyramidlevel)
{
    ostringstream string_stream;
    string_stream << "(";
    vector<Utilities::DataBindInfo> data_bind_infos;
    for (int no = 0;; ++no)
    {
        int value = -1;
        ComparisonOperation comparison_operator{ ComparisonOperation::Invalid };
        LogicalOperator logical_operator{ LogicalOperator::Invalid };
        const bool b = clause->GetPyramidLevelCondition(no, &logical_operator, &comparison_operator, &value);
        if (!b)
        {
            if (no == 0)
            {
                // this means we have an empty tile-info-query-clause
                return make_tuple("(TRUE)", vector<Utilities::DataBindInfo>());
            }

            string_stream << ")";
            return make_tuple(string_stream.str(), data_bind_infos);
        }

        if (no > 0)
        {
            string_stream << " " << Utilities::LogicalOperatorToString(logical_operator) << " ";
        }

        string_stream << "( [" << column_name_pyramidlevel << "] " << Utilities::ComparisonOperatorToString(comparison_operator) << " ?)";
        data_bind_infos.push_back({ DataBindInfo(value) });
    }
}

/*static*/const char* Utilities::ComparisonOperatorToString(ComparisonOperation comparison_operator)
{
    switch (comparison_operator)
    {
        case ComparisonOperation::Equal:
            return "=";
        case ComparisonOperation::NotEqual:
            return "<>";
        case ComparisonOperation::LessThan:
            return "<";
        case ComparisonOperation::LessThanOrEqual:
            return "<=";
        case ComparisonOperation::GreaterThan:
            return ">";
        case ComparisonOperation::GreaterThanOrEqual:
            return ">=";
        case ComparisonOperation::Invalid:
        default:
            throw invalid_argument("invalid operator encountered");
    }
}

/*static*/const char* Utilities::LogicalOperatorToString(imgdoc2::LogicalOperator logical_operator)
{
    switch (logical_operator)
    {
        case LogicalOperator::And:
            return "AND";
        case LogicalOperator::Or:
            return "OR";
        case LogicalOperator::Invalid:
        default:
            throw invalid_argument("invalid operator encountered");
    }
}

/*static*/std::tuple<std::string, std::vector<Utilities::DataBindInfo>> Utilities::CreateWhereStatement(const imgdoc2::IDimCoordinateQueryClause* dim_coordinate_query_clause, const imgdoc2::ITileInfoQueryClause* tileInfo_query_clause, const DatabaseConfiguration2D& database_configuration)
{
    return CreateWhereStatement(
        dim_coordinate_query_clause,
        tileInfo_query_clause,
        { database_configuration.GetDimensionsColumnPrefix() , database_configuration.GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_PyramidLevel) });
}

/*static*/std::tuple<std::string, std::vector<Utilities::DataBindInfo>> Utilities::CreateWhereStatement(const imgdoc2::IDimCoordinateQueryClause* dim_coordinate_query_clause, const imgdoc2::ITileInfoQueryClause* tileInfo_query_clause, const DatabaseConfiguration3D& database_configuration)
{
    return CreateWhereStatement(
        dim_coordinate_query_clause,
        tileInfo_query_clause,
        { database_configuration.GetDimensionsColumnPrefix() , database_configuration.GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration3D::kTilesInfoTable_Column_PyramidLevel) });
}

/*static*/bool Utilities::TryReadStringFromPropertyBag(IDbConnection* db_connection, const std::string& table_name, const std::string& key_column_name, const std::string& value_column_name, const std::string& key, std::string* output)
{
    ostringstream string_stream;
    string_stream << "SELECT [" << value_column_name << "] FROM [" << table_name << "] WHERE [" << key_column_name << "]='" << key << "';";
    const auto statement = db_connection->PrepareStatement(string_stream.str());
    if (db_connection->StepStatement(statement.get()))
    {
        if (output != nullptr)
        {
            *output = statement->GetResultString(0);
        }

        return true;
    }

    return false;
}

/*static*/void Utilities::WriteStringIntoPropertyBag(IDbConnection* db_connection, const std::string& table_name, const std::string& key_column_name, const std::string& value_column_name, const std::string& key, const std::string& value)
{
    ostringstream string_stream;
    string_stream << "INSERT OR REPLACE INTO [" << table_name << "] (" << key_column_name << "," << value_column_name << ") VALUES(? , ?);";
    const auto statement = db_connection->PrepareStatement(string_stream.str());
    int binding_index = 1;
    statement->BindString(binding_index++, key);
    statement->BindString(binding_index++, value);
    db_connection->Execute(statement.get());
}

/*static*/void Utilities::DeleteItemFromPropertyBag(IDbConnection* db_connection, const std::string& table_name, const std::string& key_column_name, const std::string& value_column_name, const std::string& key)
{
    ostringstream string_stream;
    string_stream << "DELETE FROM [" << table_name << "] WHERE [" << table_name << "].[" << key_column_name << "] = ?;";
    const auto statement = db_connection->PrepareStatement(string_stream.str());
    statement->BindString(1, key);
    db_connection->Execute(statement.get());
}

/*static*/bool Utilities::ProcessRangeClause(const string& column_name_for_dimension, const IDimCoordinateQueryClause::RangeClause& rangeClause, vector<Utilities::DataBindInfo>& databind_info, ostringstream& string_stream)
{
    bool range_clause_processed = false;
    if (rangeClause.start != numeric_limits<int>::min() && rangeClause.end != numeric_limits<int>::max())
    {
        if (rangeClause.start < rangeClause.end)
        {
            string_stream << "([" << column_name_for_dimension << "] > ? AND [" << column_name_for_dimension << "] < ?)";
            databind_info.emplace_back(DataBindInfo(rangeClause.start));
            databind_info.emplace_back(DataBindInfo(rangeClause.end));
            range_clause_processed = true;
        }
        else if (rangeClause.start == rangeClause.end)
        {
            string_stream << "([" << column_name_for_dimension << "] = ?)";
            databind_info.emplace_back(DataBindInfo(rangeClause.start));
            range_clause_processed = true;
        }
    }
    else if (rangeClause.start == numeric_limits<int>::min() && rangeClause.end != numeric_limits<int>::max())
    {
        string_stream << "([" << column_name_for_dimension << "] < ?)";
        databind_info.emplace_back(DataBindInfo(rangeClause.end));
        range_clause_processed = true;
    }
    else if (rangeClause.start != numeric_limits<int>::min() && rangeClause.end == numeric_limits<int>::max())
    {
        string_stream << "([" << column_name_for_dimension << "] > ?)";
        databind_info.emplace_back(DataBindInfo(rangeClause.start));
        range_clause_processed = true;
    }

    return range_clause_processed;
}

/*static*/std::tuple<std::string, std::vector<Utilities::DataBindInfo>> Utilities::CreateWhereStatement(const imgdoc2::IDimCoordinateQueryClause* dim_coordinate_query_clause, const imgdoc2::ITileInfoQueryClause* tileInfo_query_clause, const CreateWhereInfo& create_where_info)
{
    auto get_column_name_func =
        [&](imgdoc2::Dimension dimension, std::string& column_name)->void
    {
        column_name = create_where_info.dimension_column_prefix;
        column_name += dimension;
    };

    std::tuple<std::string, std::vector<Utilities::DataBindInfo>> return_value;

    if (dim_coordinate_query_clause != nullptr && tileInfo_query_clause != nullptr)
    {
        auto dimension_query = CreateWhereConditionForDimQueryClause(dim_coordinate_query_clause, get_column_name_func);
        auto tileinfo_query = CreateWhereConditionForTileInfoQueryClause(tileInfo_query_clause, create_where_info.column_name_pyramid_level/*database_configuration.GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_PyramidLevel)*/);
        ostringstream string_stream;
        string_stream << get<0>(dimension_query) << " AND " << get<0>(tileinfo_query);
        auto& databind_info = get<1>(dimension_query);
        std::move(get<1>(tileinfo_query).begin(), get<1>(tileinfo_query).end(), std::back_inserter(databind_info));
        return_value = make_tuple(string_stream.str(), databind_info);
    }
    else if (dim_coordinate_query_clause != nullptr && tileInfo_query_clause == nullptr)
    {
        return_value = CreateWhereConditionForDimQueryClause(dim_coordinate_query_clause, get_column_name_func);
    }
    else if (dim_coordinate_query_clause == nullptr && tileInfo_query_clause != nullptr)
    {
        return_value = CreateWhereConditionForTileInfoQueryClause(tileInfo_query_clause, create_where_info.column_name_pyramid_level/*database_configuration.GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_PyramidLevel)*/);
    }
    else
    {
        return_value = make_tuple(" (TRUE) ", std::vector<Utilities::DataBindInfo>{});
    }

    return return_value;
}

/*static*/std::tuple<std::string, std::vector<Utilities::DataBindInfo>> Utilities::CreateWhereConditionForIntersectingWithPlaneClause(const imgdoc2::Plane_NormalAndDistD& plane, const DatabaseConfiguration3D& database_configuration)
{
    const auto column_name_tile_x = database_configuration.GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration3D::kTilesInfoTable_Column_TileX);
    const auto column_name_tile_y = database_configuration.GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration3D::kTilesInfoTable_Column_TileY);
    const auto column_name_tile_z = database_configuration.GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration3D::kTilesInfoTable_Column_TileZ);
    const auto column_name_tile_w = database_configuration.GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration3D::kTilesInfoTable_Column_TileW);
    const auto column_name_tile_h = database_configuration.GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration3D::kTilesInfoTable_Column_TileH);
    const auto column_name_tile_d = database_configuration.GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration3D::kTilesInfoTable_Column_TileD);

    ostringstream string_stream;

    // The following SQL-statement is doing an intersection test between a plane and an axis-aligned-cuboid. The cuboid's coordinates
    // are read from the DB-table, and the plane's normal-representation are passed as parameters ?1-?4.
    // http://www.lighthouse3d.com/tutorials/view-frustum-culling/geometric-approach-testing-boxes-ii/
    //
    // What comes out is something like this:
    // SELECT [Pk] FROM [TILESINFO] WHERE 2*abs(-?4+([TileW]/2+[TileX])*?1+([TileH]/2+[TileY])*?2+([TileD]/2+[TileZ])*?3)<=abs(?3)*[TileD]+abs(?2)*[TileH]+abs(?1)*[TileW];
    //
    // where ?1=plane_normal.x, ?2=plane_normal.y, ?3=plane_normal.z and ?4=plane_normal.distance
    // (in the code below, we use '?' instead of '?1..' and add the parameters multiple times).
    string_stream <<
        "(2*abs(-?+([" << column_name_tile_w << "]/2+[" << column_name_tile_x << "])*?+" <<
        "([" << column_name_tile_h << "]/2+[" + column_name_tile_y << "])*?+" <<
        "([" << column_name_tile_d << "]/2+[" + column_name_tile_z << "])*?)" <<
        "<=" <<
        "abs(?)*[" << column_name_tile_d << "]+abs(?)*[" << column_name_tile_h << "]+abs(?)*[" << column_name_tile_w << "])";

    return make_tuple(
        string_stream.str(),
        std::vector<Utilities::DataBindInfo>
    {
        DataBindInfo(plane.distance),
            DataBindInfo(plane.normal.x),
            DataBindInfo(plane.normal.y),
            DataBindInfo(plane.normal.z),
            DataBindInfo(plane.normal.z),
            DataBindInfo(plane.normal.y),
            DataBindInfo(plane.normal.x)
    });
}

/*static*/int Utilities::AddDataBindInfoListToDbStatement(const std::vector<Utilities::DataBindInfo>& data_bind_info, IDbStatement* db_statement, int binding_index)
{
    for (const auto& binding_info : data_bind_info)
    {
        if (holds_alternative<int>(binding_info.value))
        {
            db_statement->BindInt32(binding_index, get<int>(binding_info.value));
        }
        else if (holds_alternative<int64_t>(binding_info.value))
        {
            db_statement->BindInt64(binding_index, get<int64_t>(binding_info.value));
        }
        else if (holds_alternative<double>(binding_info.value))
        {
            db_statement->BindDouble(binding_index, get<double>(binding_info.value));
        }
        else
        {
            throw logic_error("invalid variant");
        }

        ++binding_index;
    }

    return binding_index;
}

// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include <variant>
#include <sstream>
#include <algorithm>
#include <map>
#include <vector>
#include "documentRead2d.h"
#include "../db/utilities.h"

using namespace std;
using namespace imgdoc2;

/*virtual*/void DocumentRead2d::GetTileDimensions(imgdoc2::Dimension* dimensions, std::uint32_t& count)
{
    const auto& tile_dimensions = this->document_->GetDataBaseConfiguration2d()->GetTileDimensions();

    if (dimensions != nullptr)
    {
        copy_n(tile_dimensions.cbegin(), min(count, static_cast<uint32_t>(tile_dimensions.size())), dimensions);
    }

    count = static_cast<uint32_t>(tile_dimensions.size());
}

/*virtual*/std::map<imgdoc2::Dimension, imgdoc2::CoordinateBounds> DocumentRead2d::GetMinMaxForTileDimension(const std::vector<imgdoc2::Dimension>& dimensions_to_query_for)
{
    for (auto dimension : dimensions_to_query_for)
    {
        bool is_valid = this->document_->GetDataBaseConfiguration2d()->IsTileDimensionValid(dimension);
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

    auto query_statement = this->CreateQueryMinMaxStatement(dimensions_to_query_for);

    map<imgdoc2::Dimension, imgdoc2::CoordinateBounds> result;

    // we expect exactly "2 * dimensions_to_query_for.size()" results
    bool is_done = this->document_->GetDatabase_connection()->StepStatement(query_statement.get());
    if (!is_done)
    {
        throw internal_error_exception("database-query gave no result, this is unexpected.");
    }

    for (size_t i = 0; i < dimensions_to_query_for.size(); ++i)
    {
        CoordinateBounds coordinate_bounds;
        coordinate_bounds.minimum_value = query_statement->GetResultInt32(i * 2);
        coordinate_bounds.maximum_value = query_statement->GetResultInt32(i * 2 + 1);
        result[dimensions_to_query_for[i]] = coordinate_bounds;
    }

    return result;
}

/*virtual*/void DocumentRead2d::ReadTileInfo(imgdoc2::dbIndex idx, imgdoc2::ITileCoordinateMutate* coord, imgdoc2::LogicalPositionInfo* info, imgdoc2::TileBlobInfo* tile_blob_info)
{
    const auto query_statement = this->GetReadTileInfo_Statement(coord != nullptr, info != nullptr, tile_blob_info != nullptr);
    query_statement->BindInt64(1, idx);

    // we are expecting exactly one result, or zero in case of "not found"
    if (!this->document_->GetDatabase_connection()->StepStatement(query_statement.get()))
    {
        // this means that the tile with the specified index ('idx') was not found
        ostringstream ss;
        ss << "Request for reading tileinfo for an non-existing tile (with pk=" << idx << ")";
        throw non_existing_tile_exception(ss.str(), idx);
    }

    int result_index = 0;
    if (coord != nullptr)
    {
        coord->Clear();
        for (const auto dimension : this->document_->GetDataBaseConfiguration2d()->GetTileDimensions())
        {
            coord->Set(dimension, query_statement->GetResultInt32(result_index++));
        }
    }

    if (info != nullptr)
    {
        info->posX = query_statement->GetResultDouble(result_index++);
        info->posY = query_statement->GetResultDouble(result_index++);
        info->width = query_statement->GetResultDouble(result_index++);
        info->height = query_statement->GetResultDouble(result_index++);
        info->pyrLvl = query_statement->GetResultInt32(result_index++);
    }

    if (tile_blob_info != nullptr)
    {
        tile_blob_info->base_info.pixelWidth = query_statement->GetResultUInt32(result_index++);
        tile_blob_info->base_info.pixelHeight = query_statement->GetResultUInt32(result_index++);
        tile_blob_info->base_info.pixelType = query_statement->GetResultUInt8(result_index++);
        tile_blob_info->data_type = static_cast<DataTypes>(query_statement->GetResultInt32(result_index++));   // TODO(JBL): check whether valid enum value
    }
}

/*virtual*/void DocumentRead2d::Query(const imgdoc2::IDimCoordinateQueryClause* coordinate_clause, const imgdoc2::ITileInfoQueryClause* tileinfo_clause, const std::function<bool(imgdoc2::dbIndex)>& func)
{
    const auto query_statement = this->CreateQueryStatement(coordinate_clause, tileinfo_clause);

    while (this->document_->GetDatabase_connection()->StepStatement(query_statement.get()))
    {
        imgdoc2::dbIndex index = query_statement->GetResultInt64(0);
        const bool continue_operation = func(index);
        if (!continue_operation)
        {
            break;
        }
    }
}

/*virtual*/void DocumentRead2d::GetTilesIntersectingRect(const imgdoc2::RectangleD& rect, const imgdoc2::IDimCoordinateQueryClause* coordinate_clause, const imgdoc2::ITileInfoQueryClause* tileinfo_clause, const std::function<bool(imgdoc2::dbIndex)>& func)
{
    shared_ptr<IDbStatement> query_statement;
    if (this->document_->GetDataBaseConfiguration2d()->GetIsUsingSpatialIndex())
    {
        query_statement = this->GetTilesIntersectingRectQueryAndCoordinateAndInfoQueryClauseWithSpatialIndex(rect, coordinate_clause, tileinfo_clause);
    }
    else
    {
        query_statement = this->GetTilesIntersectingRectQueryAndCoordinateAndInfoQueryClause(rect, coordinate_clause, tileinfo_clause);
    }

    while (this->document_->GetDatabase_connection()->StepStatement(query_statement.get()))
    {
        const imgdoc2::dbIndex index = query_statement->GetResultInt64(0);
        const bool continue_operation = func(index);
        if (!continue_operation)
        {
            break;
        }
    }
}

/*virtual*/void DocumentRead2d::ReadTileData(imgdoc2::dbIndex idx, imgdoc2::IBlobOutput* data)
{
    // TODO(JBL): if following the idea of a "plug-able blob-storage component", then this operation would be affected.
    shared_ptr<IDbStatement> query_statement = this->GetReadDataQueryStatement(idx);

    // TODO(JBL): - we expect one and only one result, should raise an error otherwise
    //       - also, we need to report if no result is found
    if (this->document_->GetDatabase_connection()->StepStatement(query_statement.get()))
    {
        query_statement->GetResultBlob(0, data);
    }
    else
    {
        // this means that the tile with the specified index ('idx') was not found
        ostringstream ss;
        ss << "Request for reading tiledata for an non-existing tile (with pk=" << idx << ")";
        throw non_existing_tile_exception(ss.str(), idx);
    }

    // if we found multiple "blobs" with above query, this is a fatal error
    if (this->document_->GetDatabase_connection()->StepStatement(query_statement.get()))
    {
        ostringstream ss;
        ss << "Multiple results from 'ReadTileData'-query, which must not happen.";
        this->GetHostingEnvironment()->ReportFatalErrorAndExit(ss.str().c_str());
    }
}

shared_ptr<IDbStatement> DocumentRead2d::GetReadTileInfo_Statement(bool include_tile_coordinates, bool include_logical_position_info, bool include_tile_blob_info)
{
    // If include_tile_blob_info is false, we create a SQL-state something like this:
    // 
    //  SELECT [Dim_C],[Dim_S],[Dim_T],[Dim_M],[TileX],[TileY],[TileW],[TileH],[PyramidLevel] FROM [TILESINFO] WHERE [TileDataId] = ?1;
    //
    // The SELECT-state contains all included dimensions, then TileX, TileY, TileW, TileH, PyramidLevel.
    // If include_tile_coordinates is false, then the dimensions are not included; and if include_logical_position_info is false,
    // then the group "TileX, TileY, TileW, TileH, PyramidLevel" is not included.
    // 
    // If include_tile_blob_info is true, then a SQL-statement something like this is created:
    // 
    // SELECT [Dim_C],[Dim_S],[Dim_T],[Dim_M],[TileX],[TileY],[TileW],[TileH],[PyramidLevel],[PixelWidth],[PixelHeight],[TILESDATA].[PixelType],[TILESDATA].[BinDataStorageType]
    //    FROM [TILESINFO] LEFT JOIN[TILESDATA] ON [TILESINFO].[TileDataId] = [TILESDATA].[Pk]
    //        WHERE[TileDataId] = ?1;

    stringstream string_stream;
    string_stream << "SELECT ";

    // this is used to keep track whether an item (=column-name) as already been added (in order to add ',' when appropriate)
    bool item_has_been_added = false;

    if (include_tile_coordinates)
    {
        const auto tile_dimension = this->document_->GetDataBaseConfiguration2d()->GetTileDimensions();
        for (const auto dimension : tile_dimension)
        {
            if (item_has_been_added)
            {
                string_stream << ",";
            }

            string_stream << "[" << this->document_->GetDataBaseConfiguration2d()->GetDimensionsColumnPrefix() << dimension << "]";
            item_has_been_added = true;
        }
    }

    if (include_logical_position_info)
    {
        if (item_has_been_added)
        {
            string_stream << ',';
        }

        string_stream << "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileX) << "],"
            << "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileY) << "],"
            << "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileW) << "],"
            << "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileH) << "],"
            << "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_PyramidLevel) << "]";
        item_has_been_added = true;
    }

    if (include_tile_blob_info)
    {
        if (item_has_been_added)
        {
            string_stream << ',';
        }

        string_stream << "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_PixelWidth) << "],";
        string_stream << "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_PixelHeight) << "],";
        string_stream << "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_PixelType) << "],";
        string_stream << "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_TileDataType) << "]";
    }

    if (!include_tile_coordinates && !include_logical_position_info && !include_tile_blob_info)
    {
        // c.f. https://stackoverflow.com/questions/4253960/sql-how-to-properly-check-if-a-record-exists -> if all three clauses are not given,
        // we create a SQL-statement something like "SELECT 1 FROM [TILESINFO] WHERE [TileDataId] = ?1;" which gives a result of "1" if the
        // row exists, and an empty result otherwise
        string_stream << " 1 ";
    }

    const auto tiles_info_table_name = this->document_->GetDataBaseConfiguration2d()->GetTableNameForTilesInfoOrThrow();
    const auto tiles_data_table_name = this->document_->GetDataBaseConfiguration2d()->GetTableNameForTilesDataOrThrow();

    string_stream << " FROM [" << tiles_info_table_name << "] ";
    if (include_tile_blob_info)
    {
        string_stream << "LEFT JOIN " << tiles_data_table_name << " ON "
            << "[" << tiles_info_table_name << "].[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileDataId) << "]="
            << "[" << tiles_data_table_name << "].[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_Pk) << "] ";
    }

    string_stream << "WHERE [" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileDataId) << "]=?1;";

    auto statement = this->document_->GetDatabase_connection()->PrepareStatement(string_stream.str());
    return statement;
}

shared_ptr<IDbStatement> DocumentRead2d::CreateQueryStatement(const imgdoc2::IDimCoordinateQueryClause* coordinate_clause, const imgdoc2::ITileInfoQueryClause* tileinfo_clause)
{
    ostringstream string_stream;
    string_stream << "SELECT [" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_Pk) << "]," <<
        "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileDataId) << "] " <<
        "FROM [" << this->document_->GetDataBaseConfiguration2d()->GetTableNameForTilesInfoOrThrow() << "] " <<
        "WHERE ";

    const auto query_statement_and_binding_info = Utilities::CreateWhereStatement(coordinate_clause, tileinfo_clause, *this->document_->GetDataBaseConfiguration2d());
    string_stream << get<0>(query_statement_and_binding_info) << ";";

    auto statement = this->document_->GetDatabase_connection()->PrepareStatement(string_stream.str());

    int binding_index = 1;
    for (const auto& bind_info : get<1>(query_statement_and_binding_info))
    {
        if (holds_alternative<int>(bind_info.value))
        {
            statement->BindInt32(binding_index, get<int>(bind_info.value));
        }
        else if (holds_alternative<int64_t>(bind_info.value))
        {
            statement->BindInt64(binding_index, get<int64_t>(bind_info.value));
        }
        else if (holds_alternative<double>(bind_info.value))
        {
            statement->BindDouble(binding_index, get<double>(bind_info.value));
        }
        else
        {
            throw logic_error("invalid variant");
        }

        ++binding_index;
    }

    return statement;
}

std::shared_ptr<IDbStatement> DocumentRead2d::GetTilesIntersectingRectQueryWithSpatialIndex(const imgdoc2::RectangleD& rect)
{
    ostringstream string_stream;
    string_stream << "SELECT " << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_Pk) << " FROM " <<
        this->document_->GetDataBaseConfiguration2d()->GetTableNameForTilesSpatialIndexTableOrThrow() << " WHERE " <<
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MaxX) << ">=?1 AND " <<
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MinX) << "<=?2 AND " <<
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MaxY) << ">=?3 AND " <<
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MinY) << "<=?4";

    auto statement = this->document_->GetDatabase_connection()->PrepareStatement(string_stream.str());
    statement->BindDouble(1, rect.x);
    statement->BindDouble(2, rect.x + rect.w);
    statement->BindDouble(3, rect.y);
    statement->BindDouble(4, rect.y + rect.h);
    return statement;
}

std::shared_ptr<IDbStatement> DocumentRead2d::GetTilesIntersectingRectQuery(const imgdoc2::RectangleD& rect)
{
    ostringstream string_stream;
    string_stream << "SELECT " << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_Pk) << " FROM " <<
        this->document_->GetDataBaseConfiguration2d()->GetTableNameForTilesInfoOrThrow() << " WHERE " <<
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileX) << '+' <<
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileW) << ">=?1 AND " <<
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileX) << "<=?2 AND " <<
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileY) << '+' <<
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileH) << ">=?3 AND " <<
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileY) << "<=?4";

    auto statement = this->document_->GetDatabase_connection()->PrepareStatement(string_stream.str());
    statement->BindDouble(1, rect.x);
    statement->BindDouble(2, rect.x + rect.w);
    statement->BindDouble(3, rect.y);
    statement->BindDouble(4, rect.y + rect.h);
    return statement;
}

std::shared_ptr<IDbStatement> DocumentRead2d::GetTilesIntersectingRectQueryAndCoordinateAndInfoQueryClauseWithSpatialIndex(const imgdoc2::RectangleD& rect, const imgdoc2::IDimCoordinateQueryClause* coordinate_clause, const imgdoc2::ITileInfoQueryClause* tileinfo_clause)
{
    if (coordinate_clause == nullptr && tileinfo_clause == nullptr)
    {
        return this->GetTilesIntersectingRectQueryWithSpatialIndex(rect);
    }

    ostringstream string_stream;
    string_stream << "SELECT spatialindex." << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_Pk) << " FROM "
        << this->document_->GetDataBaseConfiguration2d()->GetTableNameForTilesSpatialIndexTableOrThrow() << " spatialindex "
        << "INNER JOIN " << this->document_->GetDataBaseConfiguration2d()->GetTableNameForTilesInfoOrThrow() << " info ON "
        << "spatialindex." << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_Pk)
        << " = info." << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_Pk)
        << " WHERE (" <<
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MaxX) << ">=? AND " <<
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MinX) << "<=? AND " <<
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MaxY) << ">=? AND " <<
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MinY) << "<=?) ";

    const auto query_statement_and_binding_info = Utilities::CreateWhereStatement(coordinate_clause, tileinfo_clause, *this->document_->GetDataBaseConfiguration2d());
    string_stream << " AND " << get<0>(query_statement_and_binding_info) << ";";

    auto statement = this->document_->GetDatabase_connection()->PrepareStatement(string_stream.str());
    int binding_index = 1;
    statement->BindDouble(binding_index++, rect.x);
    statement->BindDouble(binding_index++, rect.x + rect.w);
    statement->BindDouble(binding_index++, rect.y);
    statement->BindDouble(binding_index++, rect.y + rect.h);

    for (const auto& bind_info : get<1>(query_statement_and_binding_info))
    {
        if (holds_alternative<int>(bind_info.value))
        {
            statement->BindInt32(binding_index, get<int>(bind_info.value));
        }
        else if (holds_alternative<int64_t>(bind_info.value))
        {
            statement->BindInt64(binding_index, get<int64_t>(bind_info.value));
        }
        else if (holds_alternative<double>(bind_info.value))
        {
            statement->BindDouble(binding_index, get<double>(bind_info.value));
        }
        else
        {
            throw logic_error("invalid variant");
        }

        ++binding_index;
    }

    return statement;
}

std::shared_ptr<IDbStatement> DocumentRead2d::GetTilesIntersectingRectQueryAndCoordinateAndInfoQueryClause(const imgdoc2::RectangleD& rect, const imgdoc2::IDimCoordinateQueryClause* coordinate_clause, const imgdoc2::ITileInfoQueryClause* tileinfo_clause)
{
    if (coordinate_clause == nullptr && tileinfo_clause == nullptr)
    {
        return this->GetTilesIntersectingRectQuery(rect);
    }

    ostringstream string_stream;
    string_stream << "SELECT " << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_Pk) << " FROM "
        << this->document_->GetDataBaseConfiguration2d()->GetTableNameForTilesInfoOrThrow() << " WHERE (" <<
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileX) << '+' <<
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileW) << ">=?1 AND " <<
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileX) << "<=?2 AND " <<
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileY) << '+' <<
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileH) << ">=?3 AND " <<
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileY) << "<=?4)";

    const auto query_statement_and_binding_info = Utilities::CreateWhereStatement(coordinate_clause, tileinfo_clause, *this->document_->GetDataBaseConfiguration2d());
    string_stream << " AND " << get<0>(query_statement_and_binding_info) << ";";

    auto statement = this->document_->GetDatabase_connection()->PrepareStatement(string_stream.str());
    int binding_index = 1;
    statement->BindDouble(binding_index++, rect.x);
    statement->BindDouble(binding_index++, rect.x + rect.w);
    statement->BindDouble(binding_index++, rect.y);
    statement->BindDouble(binding_index++, rect.y + rect.h);

    for (const auto& bind_info : get<1>(query_statement_and_binding_info))
    {
        if (holds_alternative<int>(bind_info.value))
        {
            statement->BindInt32(binding_index, get<int>(bind_info.value));
        }
        else if (holds_alternative<int64_t>(bind_info.value))
        {
            statement->BindInt64(binding_index, get<int64_t>(bind_info.value));
        }
        else if (holds_alternative<double>(bind_info.value))
        {
            statement->BindDouble(binding_index, get<double>(bind_info.value));
        }
        else
        {
            throw logic_error("invalid variant");
        }

        ++binding_index;
    }

    return statement;
}

std::shared_ptr<IDbStatement> DocumentRead2d::GetReadDataQueryStatement(imgdoc2::dbIndex idx)
{
    // we create a statement like this:
    // SELECT [BLOBS].[Data]
    //    FROM [TILESDATA]
    //        LEFT JOIN[BLOBS] ON [TILESDATA].[BinDataId] = [BLOBS].[Pk]
    //        WHERE [TILESDATA].[Pk] = ?1;
    //
    // To be noted: 
    // * If the row with the specified primary key is not found (in the TILESDATA-table), then we
    //    get an empty result set.
    // * If, on the other hand, the row in TILESDATA is found, but there is no corresponding element in the
    //    [BLOBS]-table, then we a result with a null
    // 
    // This allows us to distinguish between "invalid idx" and "no blob present"

    ostringstream string_stream;
    string_stream << "SELECT [" << this->document_->GetDataBaseConfiguration2d()->GetTableNameForBlobTableOrThrow() << "]."
        << "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfBlobTableOrThrow(DatabaseConfiguration2D::kBlobTable_Column_Data) << "] "
        << "FROM [" << this->document_->GetDataBaseConfiguration2d()->GetTableNameForTilesDataOrThrow() << "] LEFT JOIN [" << this->document_->GetDataBaseConfiguration2d()->GetTableNameForBlobTableOrThrow() << "] "
        << "ON [" << this->document_->GetDataBaseConfiguration2d()->GetTableNameForTilesDataOrThrow() << "].[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_BinDataId) << "]"
        << " = [" << this->document_->GetDataBaseConfiguration2d()->GetTableNameForBlobTableOrThrow() << "].[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfBlobTableOrThrow(DatabaseConfiguration2D::kBlobTable_Column_Pk) << "]"
        << " WHERE [" << this->document_->GetDataBaseConfiguration2d()->GetTableNameForTilesDataOrThrow() << "].[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_Pk) << "] = ?1;";

    //// we create a statement like this:
    //// 
    //// SELECT [Data] FROM [BLOBS] WHERE [BLOBS].[Pk] =
    ////    (
    ////        SELECT BinDataId FROM TILESDATA WHERE Pk = 1  AND BinDataStorageType = 1
    ////    )
    ////

    auto statement = this->document_->GetDatabase_connection()->PrepareStatement(string_stream.str());
    statement->BindInt64(1, idx);
    return statement;
}

std::shared_ptr<IDbStatement> DocumentRead2d::CreateQueryMinMaxStatement(const std::vector<imgdoc2::Dimension>& dimensions)
{
    // preconditions:
    // - the dimensions specified must be valid
    // - the collection must not be empty
    
    ostringstream string_stream;
    string_stream << "SELECT ";
    bool first_iteration = true;
    for (auto dimension : dimensions)
    {
        if (!first_iteration)
        {
            string_stream << ',';
        }
        string_stream << "MIN([" << this->document_->GetDataBaseConfiguration2d()->GetDimensionsColumnPrefix() << dimension << "]),";
        string_stream << "MAX([" << this->document_->GetDataBaseConfiguration2d()->GetDimensionsColumnPrefix() << dimension << "])";
        first_iteration = false;
    }

    string_stream << " FROM " << "[" << this->document_->GetDataBaseConfiguration2d()->GetTableNameForTilesInfoOrThrow() << "];";

    auto statement = this->document_->GetDatabase_connection()->PrepareStatement(string_stream.str());
    return statement;
}

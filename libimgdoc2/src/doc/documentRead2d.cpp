// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include <variant>
#include <sstream>
#include <algorithm>
#include <map>
#include <vector>
#include <gsl/assert>
#include "documentRead2d.h"
#include "../db/utilities.h"

using namespace std;
using namespace imgdoc2;

/*virtual*/void DocumentRead2d::GetTileDimensions(imgdoc2::Dimension* dimensions, std::uint32_t& count)
{
    DocumentReadBase::GetEntityDimensionsInternal(
        this->GetDocument()->GetDataBaseConfiguration2d()->GetTileDimensions(),
        dimensions,
        count);
}

/*virtual*/void DocumentRead2d::GetTilesBoundingBox(imgdoc2::DoubleInterval* bounds_x, imgdoc2::DoubleInterval* bounds_y)
{
    if (bounds_x == nullptr && bounds_y == nullptr)
    {
        // nothing to do here
        return;
    }

    // case "no spatial index"
    const auto statement = this->CreateQueryTilesBoundingBoxStatement(bounds_x != nullptr, bounds_y != nullptr);
    if (!this->GetDocument()->GetDatabase_connection()->StepStatement(statement.get()))
    {
        throw internal_error_exception("database-query gave no result, this is unexpected.");
    }

    int result_index = 0;
    result_index = DocumentReadBase::SetCoordinateBoundsValueIfNonNull(bounds_x, statement.get(), result_index);
    DocumentReadBase::SetCoordinateBoundsValueIfNonNull(bounds_y, statement.get(), result_index);
}

/*virtual*/std::map<imgdoc2::Dimension, imgdoc2::Int32Interval> DocumentRead2d::GetMinMaxForTileDimension(const std::vector<imgdoc2::Dimension>& dimensions_to_query_for)
{
    return this->GetMinMaxForTileDimensionInternal(
        dimensions_to_query_for,
        [this](Dimension dimension)->bool { return this->GetDocument()->GetDataBaseConfiguration2d()->IsTileDimensionValid(dimension); },
        [this](ostringstream& ss, imgdoc2::Dimension dimension)->void { ss << this->GetDocument()->GetDataBaseConfiguration2d()->GetDimensionsColumnPrefix() << dimension; },
        this->GetDocument()->GetDataBaseConfiguration2d()->GetTableNameForTilesInfoOrThrow());
}

/*virtual*/std::uint64_t DocumentRead2d::GetTotalTileCount()    
{
    return DocumentReadBase::GetTotalTileCount(this->GetDocument()->GetDataBaseConfiguration2d()->GetTableNameForTilesInfoOrThrow());
}

/*virtual*/std::map<int, std::uint64_t> DocumentRead2d::GetTileCountPerLayer()
{
       return DocumentReadBase::GetTileCountPerLayer(
           this->GetDocument()->GetDataBaseConfiguration2d()->GetTableNameForTilesInfoOrThrow(),
           this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_PyramidLevel));
}

/*virtual*/void DocumentRead2d::ReadTileInfo(imgdoc2::dbIndex idx, imgdoc2::ITileCoordinateMutate* coord, imgdoc2::LogicalPositionInfo* info, imgdoc2::TileBlobInfo* tile_blob_info)
{
    const auto query_statement = this->GetReadTileInfo_Statement(coord != nullptr, info != nullptr, tile_blob_info != nullptr);
    query_statement->BindInt64(1, idx);

    // we are expecting exactly one result, or zero in case of "not found"
    if (!this->GetDocument()->GetDatabase_connection()->StepStatement(query_statement.get()))
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
        for (const auto dimension : this->GetDocument()->GetDataBaseConfiguration2d()->GetTileDimensions())
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

    while (this->GetDocument()->GetDatabase_connection()->StepStatement(query_statement.get()))
    {
        const imgdoc2::dbIndex index = query_statement->GetResultInt64(0);
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
    if (this->GetDocument()->GetDataBaseConfiguration2d()->GetIsUsingSpatialIndex())
    {
        query_statement = this->GetTilesIntersectingRectQueryAndCoordinateAndInfoQueryClauseWithSpatialIndex(rect, coordinate_clause, tileinfo_clause);
    }
    else
    {
        query_statement = this->GetTilesIntersectingRectQueryAndCoordinateAndInfoQueryClause(rect, coordinate_clause, tileinfo_clause);
    }

    while (this->GetDocument()->GetDatabase_connection()->StepStatement(query_statement.get()))
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
    const shared_ptr<IDbStatement> query_statement = this->GetReadDataQueryStatement(idx);

    // TODO(JBL): - we expect one and only one result, should raise an error otherwise
    //       - also, we need to report if no result is found
    if (this->GetDocument()->GetDatabase_connection()->StepStatement(query_statement.get()))
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
    if (this->GetDocument()->GetDatabase_connection()->StepStatement(query_statement.get()))
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
        const auto tile_dimension = this->GetDocument()->GetDataBaseConfiguration2d()->GetTileDimensions();
        for (const auto dimension : tile_dimension)
        {
            if (item_has_been_added)
            {
                string_stream << ",";
            }

            string_stream << "[" << this->GetDocument()->GetDataBaseConfiguration2d()->GetDimensionsColumnPrefix() << dimension << "]";
            item_has_been_added = true;
        }
    }

    if (include_logical_position_info)
    {
        if (item_has_been_added)
        {
            string_stream << ',';
        }

        string_stream << "[" << this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileX) << "],"
            << "[" << this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileY) << "],"
            << "[" << this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileW) << "],"
            << "[" << this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileH) << "],"
            << "[" << this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_PyramidLevel) << "]";
        item_has_been_added = true;
    }

    if (include_tile_blob_info)
    {
        if (item_has_been_added)
        {
            string_stream << ',';
        }

        string_stream << "[" << this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_PixelWidth) << "],";
        string_stream << "[" << this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_PixelHeight) << "],";
        string_stream << "[" << this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_PixelType) << "],";
        string_stream << "[" << this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_TileDataType) << "]";
    }

    if (!include_tile_coordinates && !include_logical_position_info && !include_tile_blob_info)
    {
        // c.f. https://stackoverflow.com/questions/4253960/sql-how-to-properly-check-if-a-record-exists -> if all three clauses are not given,
        // we create a SQL-statement something like "SELECT 1 FROM [TILESINFO] WHERE [TileDataId] = ?1;" which gives a result of "1" if the
        // row exists, and an empty result otherwise
        string_stream << " 1 ";
    }

    const auto tiles_info_table_name = this->GetDocument()->GetDataBaseConfiguration2d()->GetTableNameForTilesInfoOrThrow();
    const auto tiles_data_table_name = this->GetDocument()->GetDataBaseConfiguration2d()->GetTableNameForTilesDataOrThrow();

    string_stream << " FROM [" << tiles_info_table_name << "] ";
    if (include_tile_blob_info)
    {
        string_stream << "LEFT JOIN " << tiles_data_table_name << " ON "
            << "[" << tiles_info_table_name << "].[" << this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileDataId) << "]="
            << "[" << tiles_data_table_name << "].[" << this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_Pk) << "] ";
    }

    string_stream << "WHERE [" << this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileDataId) << "]=?1;";

    auto statement = this->GetDocument()->GetDatabase_connection()->PrepareStatement(string_stream.str());
    return statement;
}

shared_ptr<IDbStatement> DocumentRead2d::CreateQueryStatement(const imgdoc2::IDimCoordinateQueryClause* coordinate_clause, const imgdoc2::ITileInfoQueryClause* tileinfo_clause)
{
    ostringstream string_stream;
    string_stream << "SELECT [" << this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_Pk) << "]," <<
        "[" << this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileDataId) << "] " <<
        "FROM [" << this->GetDocument()->GetDataBaseConfiguration2d()->GetTableNameForTilesInfoOrThrow() << "] " <<
        "WHERE ";

    const auto query_statement_and_binding_info = Utilities::CreateWhereStatement(coordinate_clause, tileinfo_clause, *this->GetDocument()->GetDataBaseConfiguration2d());
    string_stream << get<0>(query_statement_and_binding_info) << ";";

    auto statement = this->GetDocument()->GetDatabase_connection()->PrepareStatement(string_stream.str());

    int binding_index = 1;
    Utilities::AddDataBindInfoListToDbStatement(get<1>(query_statement_and_binding_info), statement.get(), binding_index);

    return statement;
}

std::shared_ptr<IDbStatement> DocumentRead2d::GetTilesIntersectingRectQueryWithSpatialIndex(const imgdoc2::RectangleD& rect)
{
    ostringstream string_stream;
    string_stream << "SELECT " << this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_Pk) << " FROM " <<
        this->GetDocument()->GetDataBaseConfiguration2d()->GetTableNameForTilesSpatialIndexTableOrThrow() << " WHERE " <<
        this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MaxX) << ">=?1 AND " <<
        this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MinX) << "<=?2 AND " <<
        this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MaxY) << ">=?3 AND " <<
        this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MinY) << "<=?4";

    auto statement = this->GetDocument()->GetDatabase_connection()->PrepareStatement(string_stream.str());
    statement->BindDouble(1, rect.x);
    statement->BindDouble(2, rect.x + rect.w);
    statement->BindDouble(3, rect.y);
    statement->BindDouble(4, rect.y + rect.h);
    return statement;
}

std::shared_ptr<IDbStatement> DocumentRead2d::GetTilesIntersectingRectQuery(const imgdoc2::RectangleD& rect)
{
    ostringstream string_stream;
    string_stream << "SELECT " << this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_Pk) << " FROM " <<
        this->GetDocument()->GetDataBaseConfiguration2d()->GetTableNameForTilesInfoOrThrow() << " WHERE " <<
        this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileX) << '+' <<
        this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileW) << ">=?1 AND " <<
        this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileX) << "<=?2 AND " <<
        this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileY) << '+' <<
        this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileH) << ">=?3 AND " <<
        this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileY) << "<=?4";

    auto statement = this->GetDocument()->GetDatabase_connection()->PrepareStatement(string_stream.str());
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
    string_stream << "SELECT spatialindex." << this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_Pk) << " FROM "
        << this->GetDocument()->GetDataBaseConfiguration2d()->GetTableNameForTilesSpatialIndexTableOrThrow() << " spatialindex "
        << "INNER JOIN " << this->GetDocument()->GetDataBaseConfiguration2d()->GetTableNameForTilesInfoOrThrow() << " info ON "
        << "spatialindex." << this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_Pk)
        << " = info." << this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_Pk)
        << " WHERE (" <<
        this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MaxX) << ">=? AND " <<
        this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MinX) << "<=? AND " <<
        this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MaxY) << ">=? AND " <<
        this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MinY) << "<=?) ";

    const auto query_statement_and_binding_info = Utilities::CreateWhereStatement(coordinate_clause, tileinfo_clause, *this->GetDocument()->GetDataBaseConfiguration2d());
    string_stream << " AND " << get<0>(query_statement_and_binding_info) << ";";

    auto statement = this->GetDocument()->GetDatabase_connection()->PrepareStatement(string_stream.str());
    int binding_index = 1;
    statement->BindDouble(binding_index++, rect.x);
    statement->BindDouble(binding_index++, rect.x + rect.w);
    statement->BindDouble(binding_index++, rect.y);
    statement->BindDouble(binding_index++, rect.y + rect.h);

    Utilities::AddDataBindInfoListToDbStatement(get<1>(query_statement_and_binding_info), statement.get(), binding_index);

    return statement;
}

std::shared_ptr<IDbStatement> DocumentRead2d::GetTilesIntersectingRectQueryAndCoordinateAndInfoQueryClause(const imgdoc2::RectangleD& rect, const imgdoc2::IDimCoordinateQueryClause* coordinate_clause, const imgdoc2::ITileInfoQueryClause* tileinfo_clause)
{
    if (coordinate_clause == nullptr && tileinfo_clause == nullptr)
    {
        return this->GetTilesIntersectingRectQuery(rect);
    }

    ostringstream string_stream;
    string_stream << "SELECT " << this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_Pk) << " FROM "
        << this->GetDocument()->GetDataBaseConfiguration2d()->GetTableNameForTilesInfoOrThrow() << " WHERE (" <<
        this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileX) << '+' <<
        this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileW) << ">=?1 AND " <<
        this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileX) << "<=?2 AND " <<
        this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileY) << '+' <<
        this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileH) << ">=?3 AND " <<
        this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileY) << "<=?4)";

    const auto query_statement_and_binding_info = Utilities::CreateWhereStatement(coordinate_clause, tileinfo_clause, *this->GetDocument()->GetDataBaseConfiguration2d());
    string_stream << " AND " << get<0>(query_statement_and_binding_info) << ";";

    auto statement = this->GetDocument()->GetDatabase_connection()->PrepareStatement(string_stream.str());
    int binding_index = 1;
    statement->BindDouble(binding_index++, rect.x);
    statement->BindDouble(binding_index++, rect.x + rect.w);
    statement->BindDouble(binding_index++, rect.y);
    statement->BindDouble(binding_index++, rect.y + rect.h);

    Utilities::AddDataBindInfoListToDbStatement(get<1>(query_statement_and_binding_info), statement.get(), binding_index);

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
    string_stream << "SELECT [" << this->GetDocument()->GetDataBaseConfiguration2d()->GetTableNameForBlobTableOrThrow() << "]."
        << "[" << this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfBlobTableOrThrow(DatabaseConfiguration2D::kBlobTable_Column_Data) << "] "
        << "FROM [" << this->GetDocument()->GetDataBaseConfiguration2d()->GetTableNameForTilesDataOrThrow() << "] LEFT JOIN [" << this->GetDocument()->GetDataBaseConfiguration2d()->GetTableNameForBlobTableOrThrow() << "] "
        << "ON [" << this->GetDocument()->GetDataBaseConfiguration2d()->GetTableNameForTilesDataOrThrow() << "].[" << this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_BinDataId) << "]"
        << " = [" << this->GetDocument()->GetDataBaseConfiguration2d()->GetTableNameForBlobTableOrThrow() << "].[" << this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfBlobTableOrThrow(DatabaseConfiguration2D::kBlobTable_Column_Pk) << "]"
        << " WHERE [" << this->GetDocument()->GetDataBaseConfiguration2d()->GetTableNameForTilesDataOrThrow() << "].[" << this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_Pk) << "] = ?1;";

    // we create a statement like this:
    // 
    // SELECT [Data] FROM [BLOBS] WHERE [BLOBS].[Pk] =
    //    (
    //        SELECT BinDataId FROM TILESDATA WHERE Pk = 1  AND BinDataStorageType = 1
    //    )
    //

    auto statement = this->GetDocument()->GetDatabase_connection()->PrepareStatement(string_stream.str());
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
    for (const auto dimension : dimensions)
    {
        if (!first_iteration)
        {
            string_stream << ',';
        }
        string_stream << "MIN([" << this->GetDocument()->GetDataBaseConfiguration2d()->GetDimensionsColumnPrefix() << dimension << "]),";
        string_stream << "MAX([" << this->GetDocument()->GetDataBaseConfiguration2d()->GetDimensionsColumnPrefix() << dimension << "])";
        first_iteration = false;
    }

    string_stream << " FROM " << "[" << this->GetDocument()->GetDataBaseConfiguration2d()->GetTableNameForTilesInfoOrThrow() << "];";

    auto statement = this->GetDocument()->GetDatabase_connection()->PrepareStatement(string_stream.str());
    return statement;
}

std::shared_ptr<IDbStatement> DocumentRead2d::CreateQueryTilesBoundingBoxStatement(bool include_x, bool include_y) const
{
    Expects(include_x == true || include_y == true);

    vector<QueryMinMaxForXyzInfo> query_min_max_for_xyz_info_list;
    query_min_max_for_xyz_info_list.reserve(2);
    if (include_x)
    {
        query_min_max_for_xyz_info_list.push_back(
            QueryMinMaxForXyzInfo
            { 
                this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileX),
                this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileW) 
            });
    }

    if (include_y)
    {
        query_min_max_for_xyz_info_list.push_back(
            QueryMinMaxForXyzInfo
            {
                this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileY),
                this->GetDocument()->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileH)
            });
    }

    return this->CreateQueryMinMaxForXyz(this->GetDocument()->GetDataBaseConfiguration2d()->GetTableNameForTilesInfoOrThrow(), query_min_max_for_xyz_info_list);
}

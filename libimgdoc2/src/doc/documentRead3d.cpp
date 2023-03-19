// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include <variant>
#include <sstream>
#include <algorithm>
#include <map>
#include <vector>
#include "documentRead3d.h"
#include "../db/utilities.h"

using namespace std;
using namespace imgdoc2;

/*virtual*/void DocumentRead3d::ReadBrickInfo(imgdoc2::dbIndex idx, imgdoc2::ITileCoordinateMutate* coord, imgdoc2::LogicalPositionInfo3D* info, imgdoc2::BrickBlobInfo* brick_blob_info)
{
    const auto query_statement = this->GetReadBrickInfo_Statement(coord != nullptr, info != nullptr, brick_blob_info != nullptr);
    query_statement->BindInt64(1, idx);

    // we are expecting exactly one result, or zero in case of "not found"
    if (!this->document_->GetDatabase_connection()->StepStatement(query_statement.get()))
    {
        // this means that the tile with the specified index ('idx') was not found
        ostringstream ss;
        ss << "Request for reading brickinfo for an non-existing tile (with pk=" << idx << ")";
        throw non_existing_tile_exception(ss.str(), idx);
    }

    int result_index = 0;
    if (coord != nullptr)
    {
        coord->Clear();
        for (const auto dimension : this->document_->GetDataBaseConfiguration3d()->GetTileDimensions())
        {
            coord->Set(dimension, query_statement->GetResultInt32(result_index++));
        }
    }

    if (info != nullptr)
    {
        info->posX = query_statement->GetResultDouble(result_index++);
        info->posY = query_statement->GetResultDouble(result_index++);
        info->posZ = query_statement->GetResultDouble(result_index++);
        info->width = query_statement->GetResultDouble(result_index++);
        info->height = query_statement->GetResultDouble(result_index++);
        info->depth = query_statement->GetResultDouble(result_index++);
        info->pyrLvl = query_statement->GetResultInt32(result_index++);
    }

    if (brick_blob_info != nullptr)
    {
        brick_blob_info->base_info.pixelWidth = query_statement->GetResultUInt32(result_index++);
        brick_blob_info->base_info.pixelHeight = query_statement->GetResultUInt32(result_index++);
        brick_blob_info->base_info.pixelDepth = query_statement->GetResultUInt32(result_index++);
        brick_blob_info->base_info.pixelType = query_statement->GetResultUInt8(result_index++);
        brick_blob_info->data_type = static_cast<DataTypes>(query_statement->GetResultInt32(result_index++));   // TODO(JBL): check whether valid enum value
    }
}
//void ReadTileInfo(imgdoc2::dbIndex idx, imgdoc2::ITileCoordinateMutate* coord, imgdoc2::LogicalPositionInfo* info, imgdoc2::TileBlobInfo* tile_blob_info) override;
/*virtual*/void DocumentRead3d::Query(const imgdoc2::IDimCoordinateQueryClause* coordinate_clause, const imgdoc2::ITileInfoQueryClause* tileinfo_clause, const std::function<bool(imgdoc2::dbIndex)>& func)
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
//void GetTilesIntersectingRect(const imgdoc2::RectangleD& rect, const imgdoc2::IDimCoordinateQueryClause* coordinate_clause, const imgdoc2::ITileInfoQueryClause* tileinfo_clause, const std::function<bool(imgdoc2::dbIndex)>& func) override;
//void ReadTileData(imgdoc2::dbIndex idx, imgdoc2::IBlobOutput* data) override;

// interface IDocInfo
/*virtual*/void DocumentRead3d::GetTileDimensions(imgdoc2::Dimension* dimensions, std::uint32_t& count)
{
    throw logic_error("The method or operation is not implemented.");
}

/*virtual*/std::map<imgdoc2::Dimension, imgdoc2::CoordinateBounds> DocumentRead3d::GetMinMaxForTileDimension(const std::vector<imgdoc2::Dimension>& dimensions_to_query_for)
{
    throw logic_error("The method or operation is not implemented.");
}

shared_ptr<IDbStatement> DocumentRead3d::GetReadBrickInfo_Statement(bool include_tile_coordinates, bool include_logical_position_info, bool include_brick_blob_info)
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
        const auto tile_dimension = this->document_->GetDataBaseConfiguration3d()->GetTileDimensions();
        for (const auto dimension : tile_dimension)
        {
            if (item_has_been_added)
            {
                string_stream << ",";
            }

            string_stream << "[" << this->document_->GetDataBaseConfiguration3d()->GetDimensionsColumnPrefix() << dimension << "]";
            item_has_been_added = true;
        }
    }

    if (include_logical_position_info)
    {
        if (item_has_been_added)
        {
            string_stream << ',';
        }

        string_stream << "[" << this->document_->GetDataBaseConfiguration3d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration3D::kTilesInfoTable_Column_TileX) << "],"
            << "[" << this->document_->GetDataBaseConfiguration3d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration3D::kTilesInfoTable_Column_TileY) << "],"
            << "[" << this->document_->GetDataBaseConfiguration3d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration3D::kTilesInfoTable_Column_TileZ) << "],"
            << "[" << this->document_->GetDataBaseConfiguration3d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration3D::kTilesInfoTable_Column_TileW) << "],"
            << "[" << this->document_->GetDataBaseConfiguration3d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration3D::kTilesInfoTable_Column_TileH) << "],"
            << "[" << this->document_->GetDataBaseConfiguration3d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration3D::kTilesInfoTable_Column_TileD) << "],"
            << "[" << this->document_->GetDataBaseConfiguration3d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration3D::kTilesInfoTable_Column_PyramidLevel) << "]";
        item_has_been_added = true;
    }

    if (include_brick_blob_info)
    {
        if (item_has_been_added)
        {
            string_stream << ',';
        }

        string_stream << "[" << this->document_->GetDataBaseConfiguration3d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration3D::kTilesDataTable_Column_PixelWidth) << "],";
        string_stream << "[" << this->document_->GetDataBaseConfiguration3d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration3D::kTilesDataTable_Column_PixelHeight) << "],";
        string_stream << "[" << this->document_->GetDataBaseConfiguration3d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration3D::kTilesDataTable_Column_PixelDepth) << "],";
        string_stream << "[" << this->document_->GetDataBaseConfiguration3d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration3D::kTilesDataTable_Column_PixelType) << "],";
        string_stream << "[" << this->document_->GetDataBaseConfiguration3d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration3D::kTilesDataTable_Column_TileDataType) << "]";
    }

    if (!include_tile_coordinates && !include_logical_position_info && !include_brick_blob_info)
    {
        // c.f. https://stackoverflow.com/questions/4253960/sql-how-to-properly-check-if-a-record-exists -> if all three clauses are not given,
        // we create a SQL-statement something like "SELECT 1 FROM [TILESINFO] WHERE [TileDataId] = ?1;" which gives a result of "1" if the
        // row exists, and an empty result otherwise
        string_stream << " 1 ";
    }

    const auto tiles_info_table_name = this->document_->GetDataBaseConfiguration3d()->GetTableNameForTilesInfoOrThrow();
    const auto tiles_data_table_name = this->document_->GetDataBaseConfiguration3d()->GetTableNameForTilesDataOrThrow();

    string_stream << " FROM [" << tiles_info_table_name << "] ";
    if (include_brick_blob_info)
    {
        string_stream << "LEFT JOIN " << tiles_data_table_name << " ON "
            << "[" << tiles_info_table_name << "].[" << this->document_->GetDataBaseConfiguration3d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration3D::kTilesInfoTable_Column_TileDataId) << "]="
            << "[" << tiles_data_table_name << "].[" << this->document_->GetDataBaseConfiguration3d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration3D::kTilesDataTable_Column_Pk) << "] ";
    }

    string_stream << "WHERE [" << this->document_->GetDataBaseConfiguration3d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration3D::kTilesInfoTable_Column_TileDataId) << "]=?1;";

    auto statement = this->document_->GetDatabase_connection()->PrepareStatement(string_stream.str());
    return statement;
}

shared_ptr<IDbStatement> DocumentRead3d::CreateQueryStatement(const imgdoc2::IDimCoordinateQueryClause* coordinate_clause, const imgdoc2::ITileInfoQueryClause* tileinfo_clause)
{
    ostringstream string_stream;
    string_stream << "SELECT [" << this->document_->GetDataBaseConfiguration3d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration3D::kTilesInfoTable_Column_Pk) << "]," <<
        "[" << this->document_->GetDataBaseConfiguration3d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration3D::kTilesInfoTable_Column_TileDataId) << "] " <<
        "FROM [" << this->document_->GetDataBaseConfiguration3d()->GetTableNameForTilesInfoOrThrow() << "] " <<
        "WHERE ";

    const auto query_statement_and_binding_info = Utilities::CreateWhereStatement(coordinate_clause, tileinfo_clause, *this->document_->GetDataBaseConfiguration3d());
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

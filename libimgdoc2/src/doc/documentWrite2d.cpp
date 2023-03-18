// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include <sstream>
#include <vector> 
#include <gsl/gsl>
#include "documentWrite2d.h"
#include "transactionHelper.h"

using namespace std;
using namespace imgdoc2;

/*virtual*/imgdoc2::dbIndex DocumentWrite2d::AddTile(
    const imgdoc2::ITileCoordinate* coord,
    const imgdoc2::LogicalPositionInfo* info,
    const imgdoc2::TileBaseInfo* tileInfo,
    imgdoc2::DataTypes datatype,
    imgdoc2::TileDataStorageType storage_type,
    const imgdoc2::IDataObjBase* data)
{
    TransactionHelper<dbIndex> transaction{
        this->document_->GetDatabase_connection(),
        [&]()->dbIndex
        {
            return this->AddTileInternal(coord, info, tileInfo, datatype, storage_type, data);
        }
    };

    return transaction.Execute();
}

/*virtual*/void DocumentWrite2d::BeginTransaction()
{
    this->document_->GetDatabase_connection()->BeginTransaction();
}

/*virtual*/void DocumentWrite2d::CommitTransaction()
{
    this->document_->GetDatabase_connection()->EndTransaction(true);
}

/*virtual*/void DocumentWrite2d::RollbackTransaction()
{
    this->document_->GetDatabase_connection()->EndTransaction(false);
}

imgdoc2::dbIndex DocumentWrite2d::AddTileInternal(
    const imgdoc2::ITileCoordinate* coord,
    const imgdoc2::LogicalPositionInfo* info,
    const imgdoc2::TileBaseInfo* tileInfo,
    imgdoc2::DataTypes datatype,
    imgdoc2::TileDataStorageType storage_type,
    const imgdoc2::IDataObjBase* data)
{
    const auto tiles_data_id = this->AddTileData(tileInfo, datatype, storage_type, data);

    ostringstream string_stream;
    string_stream << "INSERT INTO [" << this->document_->GetDataBaseConfiguration2d()->GetTableNameForTilesInfoOrThrow() << "] ("
        << "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileX) << "],"
        << "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileY) << "],"
        << "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileW) << "],"
        << "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileH) << "],"
        << "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_PyramidLevel) << "],"
        << "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileDataId) << "]";

    vector<int> coordinate_values;
    coord->EnumCoordinates(
        [&](Dimension dimension, int value)->bool
        {
            string_stream << ", [" << this->document_->GetDataBaseConfiguration2d()->GetDimensionsColumnPrefix() << dimension << ']';
            coordinate_values.push_back(value);
            return true;
        });

    string_stream << ") VALUES( ?, ?, ?, ?, ?, ?";
    for (size_t i = 0; i < coordinate_values.size(); ++i)
    {
        string_stream << ", ?";
    }

    string_stream << ");";

    const auto statement = this->document_->GetDatabase_connection()->PrepareStatement(string_stream.str());
    int binding_index = 1;
    statement->BindDouble(binding_index++, info->posX);
    statement->BindDouble(binding_index++, info->posY);
    statement->BindDouble(binding_index++, info->width);
    statement->BindDouble(binding_index++, info->height);
    statement->BindInt32(binding_index++, info->pyrLvl);
    statement->BindInt64(binding_index++, tiles_data_id);

    for (const int coordinate_value : coordinate_values)
    {
        statement->BindInt32(binding_index++, coordinate_value);
    }

    const auto row_id = this->document_->GetDatabase_connection()->ExecuteAndGetLastRowId(statement.get());

    if (this->document_->GetDataBaseConfiguration2d()->GetIsUsingSpatialIndex())
    {
        this->AddToSpatialIndex(row_id, *info);
    }

    return row_id;
}

imgdoc2::dbIndex DocumentWrite2d::AddTileData(const imgdoc2::TileBaseInfo* tile_info, imgdoc2::DataTypes datatype, imgdoc2::TileDataStorageType storage_type, const imgdoc2::IDataObjBase* data)
{
    // first, add the blob data
    dbIndex blob_db_index = 0;
    if (data != nullptr)
    {
        blob_db_index = this->AddBlobData(storage_type, data);
    }

    ostringstream string_stream;
    string_stream << "INSERT INTO " << this->document_->GetDataBaseConfiguration2d()->GetTableNameForTilesDataOrThrow() << " ("
        << "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_PixelWidth) << "],"
        << "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_PixelHeight) << "],"
        << "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_PixelType) << "],"
        << "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_TileDataType) << "],"
        << "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_BinDataStorageType) << "],"
        << "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_BinDataId) << "]"
        ") VALUES( ?1, ?2, ?3, ?4, ?5, ?6);";

    const auto statement = this->document_->GetDatabase_connection()->PrepareStatement(string_stream.str());

    int binding_index = 1;
    statement->BindInt32(binding_index++, tile_info->pixelWidth);
    statement->BindInt32(binding_index++, tile_info->pixelHeight);
    statement->BindInt32(binding_index++, tile_info->pixelType);
    statement->BindInt32(binding_index++, static_cast<underlying_type_t<decltype(datatype)>>(datatype));
    if (data != nullptr)
    {
        // for data-type "zero" 
        statement->BindInt32(binding_index++, static_cast<underlying_type_t<decltype(storage_type)>>(storage_type));
        statement->BindInt64(binding_index++, blob_db_index);
    }

    const auto row_id = this->document_->GetDatabase_connection()->ExecuteAndGetLastRowId(statement.get());
    return row_id;
}

imgdoc2::dbIndex DocumentWrite2d::AddBlobData(imgdoc2::TileDataStorageType storage_type, const imgdoc2::IDataObjBase* data)
{
    Expects(data != nullptr);

    if (storage_type != TileDataStorageType::BlobInDatabase)
    {
        throw invalid_operation_exception("Storage-types other than 'blob-in-database' are not implemented.");
    }

    if (!this->document_->GetDataBaseConfiguration2d()->GetHasBlobsTable())
    {
        throw invalid_operation_exception("The database does not have a blob-table.");
    }

    const auto insert_data_statement = this->CreateInsertDataStatement(data);

    const auto row_id = this->document_->GetDatabase_connection()->ExecuteAndGetLastRowId(insert_data_statement.get());
    return row_id;
}

std::shared_ptr<IDbStatement> DocumentWrite2d::CreateInsertDataStatement(const imgdoc2::IDataObjBase* data)
{
    ostringstream string_stream;
    string_stream << "INSERT INTO [" << this->document_->GetDataBaseConfiguration2d()->GetTableNameForBlobTableOrThrow() << "] ("
        << "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfBlobTableOrThrow(DatabaseConfigurationCommon::kBlobTable_Column_Data) << "]"
        << ") VALUES( ?1 );";

    auto statement = this->document_->GetDatabase_connection()->PrepareStatement(string_stream.str());
    const void* ptr_data = nullptr;
    size_t size_data = 0;
    data->GetData(&ptr_data, &size_data);
    statement->BindBlob_Static(1, ptr_data, size_data);
    return statement;
}

void DocumentWrite2d::AddToSpatialIndex(imgdoc2::dbIndex index, const imgdoc2::LogicalPositionInfo& logical_position_info)
{
    ostringstream string_stream;
    string_stream << "INSERT INTO " << this->document_->GetDataBaseConfiguration2d()->GetTableNameForTilesSpatialIndexTableOrThrow() << " ("
        << "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_Pk) << "],"
        << "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MinX) << "],"
        << "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MaxX) << "],"
        << "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MinY) << "],"
        << "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MaxY) << "]"
        ") VALUES(?1,?2,?3,?4,?5);";

    const auto statement = this->document_->GetDatabase_connection()->PrepareStatement(string_stream.str());

    int binding_index = 1;
    statement->BindInt64(binding_index++, index);
    statement->BindDouble(binding_index++, logical_position_info.posX);
    statement->BindDouble(binding_index++, logical_position_info.posX + logical_position_info.width);
    statement->BindDouble(binding_index++, logical_position_info.posY);
    statement->BindDouble(binding_index++, logical_position_info.posY + logical_position_info.height);

    this->document_->GetDatabase_connection()->ExecuteAndGetLastRowId(statement.get());
}


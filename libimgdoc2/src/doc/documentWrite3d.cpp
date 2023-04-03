// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include <sstream>
#include <vector> 
#include <gsl/gsl>
#include "documentWrite3d.h"
#include "transactionHelper.h"

using namespace std;
using namespace imgdoc2;

/*virtual*/imgdoc2::dbIndex DocumentWrite3d::AddBrick(
            const imgdoc2::ITileCoordinate* coordinate,
            const imgdoc2::LogicalPositionInfo3D* logical_position_3d_info,
            const imgdoc2::BrickBaseInfo* brickInfo,
            imgdoc2::DataTypes data_type,
            imgdoc2::TileDataStorageType storage_type,
            const imgdoc2::IDataObjBase* data)
{
    TransactionHelper<dbIndex> transaction{
        this->document_->GetDatabase_connection(),
        [&]()->dbIndex
        {
            return this->AddBrickInternal(coordinate, logical_position_3d_info, brickInfo, data_type, storage_type, data);
        }
    };

    return transaction.Execute();
}

/*virtual*/void DocumentWrite3d::BeginTransaction()
{
    this->document_->GetDatabase_connection()->BeginTransaction();
}

/*virtual*/void DocumentWrite3d::CommitTransaction()
{
    this->document_->GetDatabase_connection()->EndTransaction(true);
}

/*virtual*/void DocumentWrite3d::RollbackTransaction()
{
    this->document_->GetDatabase_connection()->EndTransaction(false);
}

imgdoc2::dbIndex DocumentWrite3d::AddBrickInternal(
        const imgdoc2::ITileCoordinate* coordinate,
        const imgdoc2::LogicalPositionInfo3D* logical_position_info_3d,
        const imgdoc2::BrickBaseInfo* brick_base_info,
        imgdoc2::DataTypes data_type,
        imgdoc2::TileDataStorageType storage_type,
        const imgdoc2::IDataObjBase* data)
{
    const auto tiles_data_id = this->AddBrickData(brick_base_info, data_type, storage_type, data);

    ostringstream string_stream;
    string_stream << "INSERT INTO [" << this->document_->GetDataBaseConfiguration3d()->GetTableNameForTilesInfoOrThrow() << "] ("
        << "[" << this->document_->GetDataBaseConfiguration3d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration3D::kTilesInfoTable_Column_TileX) << "],"
        << "[" << this->document_->GetDataBaseConfiguration3d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration3D::kTilesInfoTable_Column_TileY) << "],"
        << "[" << this->document_->GetDataBaseConfiguration3d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration3D::kTilesInfoTable_Column_TileZ) << "],"
        << "[" << this->document_->GetDataBaseConfiguration3d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration3D::kTilesInfoTable_Column_TileW) << "],"
        << "[" << this->document_->GetDataBaseConfiguration3d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration3D::kTilesInfoTable_Column_TileH) << "],"
        << "[" << this->document_->GetDataBaseConfiguration3d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration3D::kTilesInfoTable_Column_TileD) << "],"
        << "[" << this->document_->GetDataBaseConfiguration3d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration3D::kTilesInfoTable_Column_PyramidLevel) << "],"
        << "[" << this->document_->GetDataBaseConfiguration3d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration3D::kTilesInfoTable_Column_TileDataId) << "]";

    vector<int> coordinate_values;
    coordinate->EnumCoordinates(
        [&](Dimension dimension, int value)->bool
        {
            string_stream << ", [" << this->document_->GetDataBaseConfiguration3d()->GetDimensionsColumnPrefix() << dimension << ']';
            coordinate_values.push_back(value);
            return true;
        });

    string_stream << ") VALUES( ?, ?, ?, ?, ?, ?, ?, ?";
    for (size_t i = 0; i < coordinate_values.size(); ++i)
    {
        string_stream << ", ?";
    }

    string_stream << ");";

    const auto statement = this->document_->GetDatabase_connection()->PrepareStatement(string_stream.str());
    int binding_index = 1;
    statement->BindDouble(binding_index++, logical_position_info_3d->posX);
    statement->BindDouble(binding_index++, logical_position_info_3d->posY);
    statement->BindDouble(binding_index++, logical_position_info_3d->posZ);
    statement->BindDouble(binding_index++, logical_position_info_3d->width);
    statement->BindDouble(binding_index++, logical_position_info_3d->height);
    statement->BindDouble(binding_index++, logical_position_info_3d->depth);
    statement->BindInt32(binding_index++, logical_position_info_3d->pyrLvl);
    statement->BindInt64(binding_index++, tiles_data_id);

    for (const int coordinate_value : coordinate_values)
    {
        statement->BindInt32(binding_index++, coordinate_value);
    }

    const auto row_id = this->document_->GetDatabase_connection()->ExecuteAndGetLastRowId(statement.get());

    if (this->document_->GetDataBaseConfiguration3d()->GetIsUsingSpatialIndex())
    {
        this->AddToSpatialIndex(row_id, *logical_position_info_3d);
    }

    return row_id;
}

imgdoc2::dbIndex DocumentWrite3d::AddBrickData(const imgdoc2::BrickBaseInfo* brick_base_info, imgdoc2::DataTypes data_type, imgdoc2::TileDataStorageType storage_type, const imgdoc2::IDataObjBase* data)
{
    // first, add the blob data
    dbIndex blob_db_index = 0;
    if (data != nullptr)
    {
        blob_db_index = this->AddBlobData(storage_type, data);
    }

    ostringstream string_stream;
    string_stream << "INSERT INTO " << this->document_->GetDataBaseConfiguration3d()->GetTableNameForTilesDataOrThrow() << " ("
        << "[" << this->document_->GetDataBaseConfiguration3d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration3D::kTilesDataTable_Column_PixelWidth) << "],"
        << "[" << this->document_->GetDataBaseConfiguration3d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration3D::kTilesDataTable_Column_PixelHeight) << "],"
        << "[" << this->document_->GetDataBaseConfiguration3d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration3D::kTilesDataTable_Column_PixelDepth) << "],"
        << "[" << this->document_->GetDataBaseConfiguration3d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration3D::kTilesDataTable_Column_PixelType) << "],"
        << "[" << this->document_->GetDataBaseConfiguration3d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration3D::kTilesDataTable_Column_TileDataType) << "],"
        << "[" << this->document_->GetDataBaseConfiguration3d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration3D::kTilesDataTable_Column_BinDataStorageType) << "],"
        << "[" << this->document_->GetDataBaseConfiguration3d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration3D::kTilesDataTable_Column_BinDataId) << "]"
        ") VALUES( ?1, ?2, ?3, ?4, ?5, ?6, ?7);";

    const auto statement = this->document_->GetDatabase_connection()->PrepareStatement(string_stream.str());

    int binding_index = 1;
    statement->BindInt32(binding_index++, brick_base_info->pixelWidth);
    statement->BindInt32(binding_index++, brick_base_info->pixelHeight);
    statement->BindInt32(binding_index++, brick_base_info->pixelDepth);
    statement->BindInt32(binding_index++, brick_base_info->pixelType);
    statement->BindInt32(binding_index++, static_cast<underlying_type_t<decltype(data_type)>>(data_type));
    if (data != nullptr)
    {
        // for data-type "zero" 
        statement->BindInt32(binding_index++, static_cast<underlying_type_t<decltype(storage_type)>>(storage_type));
        statement->BindInt64(binding_index++, blob_db_index);
    }

    const auto row_id = this->document_->GetDatabase_connection()->ExecuteAndGetLastRowId(statement.get());
    return row_id;
}

imgdoc2::dbIndex DocumentWrite3d::AddBlobData(imgdoc2::TileDataStorageType storage_type, const imgdoc2::IDataObjBase* data)
{
    // TODO(JBL) - combine with 2d version
    Expects(data != nullptr);

    if (storage_type != TileDataStorageType::BlobInDatabase)
    {
        throw invalid_operation_exception("Storage-types other than 'blob-in-database' are not implemented.");
    }

    if (!this->document_->GetDataBaseConfiguration3d()->GetHasBlobsTable())
    {
        throw invalid_operation_exception("The database does not have a blob-table.");
    }

    const auto insert_data_statement = this->CreateInsertDataStatement(data);

    const auto row_id = this->document_->GetDatabase_connection()->ExecuteAndGetLastRowId(insert_data_statement.get());
    return row_id;
}

std::shared_ptr<IDbStatement> DocumentWrite3d::CreateInsertDataStatement(const imgdoc2::IDataObjBase* data)
{
    // TODO(JBL) - combine with 2d version
    ostringstream string_stream;
    string_stream << "INSERT INTO [" << this->document_->GetDataBaseConfiguration3d()->GetTableNameForBlobTableOrThrow() << "] ("
        << "[" << this->document_->GetDataBaseConfiguration3d()->GetColumnNameOfBlobTableOrThrow(DatabaseConfigurationCommon::kBlobTable_Column_Data) << "]"
        << ") VALUES( ?1 );";

    auto statement = this->document_->GetDatabase_connection()->PrepareStatement(string_stream.str());
    const void* ptr_data = nullptr;
    size_t size_data = 0;
    data->GetData(&ptr_data, &size_data);
    statement->BindBlob_Static(1, ptr_data, size_data);
    return statement;
}

void DocumentWrite3d::AddToSpatialIndex(imgdoc2::dbIndex index, const imgdoc2::LogicalPositionInfo3D& logical_position_info)
{
    ostringstream string_stream;
    string_stream << "INSERT INTO " << this->document_->GetDataBaseConfiguration3d()->GetTableNameForTilesSpatialIndexTableOrThrow() << " ("
        << "[" << this->document_->GetDataBaseConfiguration3d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration3D::kTilesSpatialIndexTable_Column_Pk) << "],"
        << "[" << this->document_->GetDataBaseConfiguration3d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration3D::kTilesSpatialIndexTable_Column_MinX) << "],"
        << "[" << this->document_->GetDataBaseConfiguration3d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration3D::kTilesSpatialIndexTable_Column_MaxX) << "],"
        << "[" << this->document_->GetDataBaseConfiguration3d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration3D::kTilesSpatialIndexTable_Column_MinY) << "],"
        << "[" << this->document_->GetDataBaseConfiguration3d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration3D::kTilesSpatialIndexTable_Column_MaxY) << "],"
        << "[" << this->document_->GetDataBaseConfiguration3d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration3D::kTilesSpatialIndexTable_Column_MinZ) << "],"
        << "[" << this->document_->GetDataBaseConfiguration3d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration3D::kTilesSpatialIndexTable_Column_MaxZ) << "]"
        ") VALUES(?1,?2,?3,?4,?5,?6,?7);";

    const auto statement = this->document_->GetDatabase_connection()->PrepareStatement(string_stream.str());

    int binding_index = 1;
    statement->BindInt64(binding_index++, index);
    statement->BindDouble(binding_index++, logical_position_info.posX);
    statement->BindDouble(binding_index++, logical_position_info.posX + logical_position_info.width);
    statement->BindDouble(binding_index++, logical_position_info.posY);
    statement->BindDouble(binding_index++, logical_position_info.posY + logical_position_info.height);
    statement->BindDouble(binding_index++, logical_position_info.posZ);
    statement->BindDouble(binding_index++, logical_position_info.posZ + logical_position_info.depth);

    this->document_->GetDatabase_connection()->ExecuteAndGetLastRowId(statement.get());
}

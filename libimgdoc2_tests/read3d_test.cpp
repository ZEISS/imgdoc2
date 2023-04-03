// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../libimgdoc2/inc/imgdoc2.h"

using namespace std;
using namespace imgdoc2;
using namespace testing;

TEST(Read3d, TryReadBrickInfoForNonExistentTile)
{
    const auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetDocumentType(DocumentType::kImage3d);
    create_options->SetFilename(":memory:");
    create_options->AddDimension('M');
    create_options->SetUseSpatialIndex(false);
    create_options->SetCreateBlobTable(true);

    auto doc = ClassFactory::CreateNew(create_options.get());

    auto reader = doc->GetReader3d();

    TileCoordinate tc;
    LogicalPositionInfo3D logical_position_info;
    EXPECT_THROW(
        reader->ReadBrickInfo(1234, &tc, &logical_position_info, nullptr),
        imgdoc2::non_existing_tile_exception);
}

TEST(Read3d, CreateDocumentAddSingleTileReadTileInfoCheckForCorrectness)
{
    // here we exercise the IDocRead2d::ReadTileInfo method - we create a document, add one tile, read it again
    //  and compare the content
    constexpr size_t kBLOB_SIZE = 100;

    const auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetDocumentType(DocumentType::kImage3d);
    create_options->SetFilename(":memory:");
    create_options->AddDimension('M');
    create_options->SetUseSpatialIndex(false);
    create_options->SetCreateBlobTable(true);

    auto doc = ClassFactory::CreateNew(create_options.get());
    auto reader = doc->GetReader3d();
    auto writer = doc->GetWriter3d();

    LogicalPositionInfo3D position_info;
    BrickBaseInfo brick_base_info;
    TileCoordinate tc({ { 'M', 1 } });
    position_info.posX = 10;
    position_info.posY = 11;
    position_info.posZ = 12;
    position_info.width = 13;
    position_info.height = 43;
    position_info.depth = 15;
    position_info.pyrLvl = 0;
    brick_base_info.pixelWidth = 40;
    brick_base_info.pixelHeight = 41;
    brick_base_info.pixelDepth = 42;
    brick_base_info.pixelType = 3;

    imgdoc2::DataObjectOnHeap blobData{ kBLOB_SIZE };
    for (size_t i = 0; i < blobData.GetSizeOfData(); ++i)
    {
        uint8_t* ptrData = static_cast<uint8_t*>(blobData.GetData()) + i;
        *ptrData = static_cast<uint8_t>(i);
    }

    writer->AddBrick(&tc, &position_info, &brick_base_info, DataTypes::UNCOMPRESSED_BRICK, TileDataStorageType::BlobInDatabase, &blobData);

    CDimCoordinateQueryClause coordinate_query_clause;
    coordinate_query_clause.AddRangeClause('M', IDimCoordinateQueryClause::RangeClause{ 1, 1 });
    vector<dbIndex> indices;
    reader->Query(&coordinate_query_clause, nullptr, [&](dbIndex index)->bool {indices.push_back(index); return true; });
    ASSERT_EQ(indices.size(), 1);

    TileCoordinate tile_coordinate_read;
    LogicalPositionInfo3D logical_position_info;
    BrickBlobInfo brick_blob_info;
    reader->ReadBrickInfo(indices[0], &tile_coordinate_read, &logical_position_info, &brick_blob_info);
    ASSERT_EQ(logical_position_info.posX, position_info.posX);
    ASSERT_EQ(logical_position_info.posY, position_info.posY);
    ASSERT_EQ(logical_position_info.posZ, position_info.posZ);
    ASSERT_EQ(logical_position_info.width, position_info.width);
    ASSERT_EQ(logical_position_info.height, position_info.height);
    ASSERT_EQ(logical_position_info.depth, position_info.depth);
    ASSERT_EQ(logical_position_info.pyrLvl, position_info.pyrLvl);
    ASSERT_EQ(brick_blob_info.base_info.pixelWidth, 40);
    ASSERT_EQ(brick_blob_info.base_info.pixelHeight, 41);
    ASSERT_EQ(brick_blob_info.base_info.pixelDepth, 42);
    ASSERT_EQ(brick_blob_info.base_info.pixelType, 3);
    ASSERT_EQ(brick_blob_info.data_type, DataTypes::UNCOMPRESSED_BRICK);
    int m_read;
    ASSERT_TRUE(tile_coordinate_read.TryGetCoordinate('M', &m_read));
    ASSERT_EQ(m_read, 1);
    auto dimensions_read = tile_coordinate_read.GetDimensions();
    ASSERT_EQ(dimensions_read.size(), 1);
    ASSERT_EQ(dimensions_read[0], 'M');
}

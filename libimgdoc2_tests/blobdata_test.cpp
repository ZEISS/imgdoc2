// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>
#include "../libimgdoc2/inc/imgdoc2.h"
#include "../libimgdoc2/src/db/utilities.h"

using namespace std;
using namespace imgdoc2;

TEST(BlobData, AddTileWithBlobDataAndRetrieveItCheckForCorrectness)
{
    constexpr size_t kBLOB_SIZE = 100;
    const auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetFilename(":memory:");
    //create_options->SetFilename("d:\\test.db");
    create_options->AddDimension('M');
    create_options->SetCreateBlobTable(true);

    const auto doc = ClassFactory::CreateNew(create_options.get());
    const auto writer = doc->GetWriter2d();

    LogicalPositionInfo position_info;
    TileBaseInfo tile_info;
    const TileCoordinate tile_coordinate({ { 'M', 0} });
    position_info.posX = 0;
    position_info.posY = 0;
    position_info.width = 10;
    position_info.height = 10;
    position_info.pyrLvl = 0;
    tile_info.pixelWidth = 10;
    tile_info.pixelHeight = 10;
    tile_info.pixelType = 0;
    imgdoc2::DataObjectOnHeap blobData{ kBLOB_SIZE };
    for (size_t i = 0; i < blobData.GetSizeOfData(); ++i)
    {
        uint8_t* ptrData = static_cast<uint8_t*>(blobData.GetData()) + i;
        *ptrData = static_cast<uint8_t>(i);
    }

    writer->AddTile(
        &tile_coordinate,
        &position_info,
        &tile_info,
        DataTypes::UNCOMPRESSED_BITMAP,
        TileDataStorageType::BlobInDatabase,
        &blobData);

    const auto reader = doc->GetReader2d();
    CDimCoordinateQueryClause coordinate_query_clause;
    coordinate_query_clause.AddRangeClause('M', IDimCoordinateQueryClause::RangeClause{ 0, 0 });
    vector<dbIndex> indices;
    reader->Query(&coordinate_query_clause, nullptr, [&](dbIndex index)->bool {indices.push_back(index); return true; });
    ASSERT_EQ(indices.size(), 1);

    imgdoc2::BlobOutputOnHeap output_blob;
    reader->ReadTileData(indices[0], &output_blob);
    ASSERT_TRUE(output_blob.GetHasData());
    ASSERT_EQ(output_blob.GetSizeOfData(), kBLOB_SIZE);
    ASSERT_EQ(memcmp(output_blob.GetDataC(), blobData.GetDataC(), kBLOB_SIZE), 0);
}

TEST(BlobData, AddBrickWithBlobDataAndRetrieveItCheckForCorrectness)
{
    constexpr size_t kBLOB_SIZE = 100;
    const auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetDocumentType(DocumentType::kImage3d);
    create_options->SetFilename(":memory:");
    //create_options->SetFilename("d:\\test.db");
    create_options->AddDimension('M');
    create_options->SetCreateBlobTable(true);

    const auto doc = ClassFactory::CreateNew(create_options.get());
    const auto writer = doc->GetWriter3d();

    LogicalPositionInfo3D position_info;
    BrickBaseInfo brick_info;
    const TileCoordinate tile_coordinate({ { 'M', 0} });
    position_info.posX = 0;
    position_info.posY = 0;
    position_info.posZ = 0;
    position_info.width = 10;
    position_info.height = 10;
    position_info.depth = 10;
    position_info.pyrLvl = 0;
    brick_info.pixelWidth = 10;
    brick_info.pixelHeight = 10;
    brick_info.pixelDepth = 10;
    brick_info.pixelType = 0;
    imgdoc2::DataObjectOnHeap blobData{ kBLOB_SIZE };
    for (size_t i = 0; i < blobData.GetSizeOfData(); ++i)
    {
        uint8_t* ptrData = static_cast<uint8_t*>(blobData.GetData()) + i;
        *ptrData = static_cast<uint8_t>(i);
    }

    writer->AddBrick(
        &tile_coordinate,
        &position_info,
        &brick_info,
        DataTypes::UNCOMPRESSED_BRICK,
        TileDataStorageType::BlobInDatabase,
        &blobData);

    const auto reader = doc->GetReader3d();
    CDimCoordinateQueryClause coordinate_query_clause;
    coordinate_query_clause.AddRangeClause('M', IDimCoordinateQueryClause::RangeClause{ 0, 0 });
    vector<dbIndex> indices;
    reader->Query(&coordinate_query_clause, nullptr, [&](dbIndex index)->bool {indices.push_back(index); return true; });
    ASSERT_EQ(indices.size(), 1);

    imgdoc2::BlobOutputOnHeap output_blob;
    reader->ReadBrickData(indices[0], &output_blob);
    ASSERT_TRUE(output_blob.GetHasData());
    ASSERT_EQ(output_blob.GetSizeOfData(), kBLOB_SIZE);
    ASSERT_EQ(memcmp(output_blob.GetDataC(), blobData.GetDataC(), kBLOB_SIZE), 0);
}

TEST(BlobData, Document2dTryToReadNonExistingBlobAndCheckForError)
{
    const auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetFilename(":memory:");
    create_options->AddDimension('M');
    create_options->SetCreateBlobTable(true);

    const auto doc = ClassFactory::CreateNew(create_options.get());
    const auto reader = doc->GetReader2d();

    imgdoc2::BlobOutputOnHeap output_blob;
    EXPECT_THROW(
      reader->ReadTileData(12345, &output_blob),
      imgdoc2::non_existing_tile_exception);
}

TEST(BlobData, Document3dTryToReadNonExistingBlobAndCheckForError)
{
    const auto create_options = ClassFactory::CreateCreateOptionsSp();  // using "Sp" here so that this version also gets coverage
    create_options->SetDocumentType(DocumentType::kImage3d);
    create_options->SetFilename(":memory:");
    create_options->AddDimension('M');
    create_options->SetCreateBlobTable(true);

    const auto doc = ClassFactory::CreateNew(create_options.get());
    const auto reader = doc->GetReader3d();

    imgdoc2::BlobOutputOnHeap output_blob;
    EXPECT_THROW(
      reader->ReadBrickData(12345, &output_blob),
      imgdoc2::non_existing_tile_exception);
}

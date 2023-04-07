// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <array>
#include <random>

#include "utilities.h"
#include "../libimgdoc2/inc/imgdoc2.h"

using namespace std;
using namespace imgdoc2;
using namespace testing;

TEST(DocInfo3d, SimpleQueryTileDimensionsAndCheck)
{
    // arrange
    const auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetDocumentType(DocumentType::kImage3d);
    create_options->SetFilename(":memory:");
    create_options->AddDimension('M');
    create_options->SetUseSpatialIndex(false);
    create_options->SetCreateBlobTable(true);
    const auto doc = ClassFactory::CreateNew(create_options.get());
    const auto reader = doc->GetReader3d();

    // act
    const auto tile_dimensions = reader->GetTileDimensions();

    // assert
    ASSERT_THAT(tile_dimensions, ElementsAre('M'));
}

TEST(DocInfo3d, GetMinMaxForTileDimensionForRandomDocumentAndCheckResult)
{
    // arrange
    const auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetDocumentType(DocumentType::kImage3d);
    create_options->SetFilename(":memory:");
    create_options->AddDimension('x');
    create_options->AddDimension('p');
    create_options->SetUseSpatialIndex(false);
    create_options->SetCreateBlobTable(false);
    const auto doc = ClassFactory::CreateNew(create_options.get());
    const auto writer = doc->GetWriter3d();

    random_device rd;
    mt19937 rng(rd());
    uniform_int_distribution<int> distribution(numeric_limits<int>::min(), numeric_limits<int>::max());

    int min_x = numeric_limits<int>::max();
    int min_p = numeric_limits<int>::max();
    int max_x = numeric_limits<int>::min();
    int max_p = numeric_limits<int>::min();
    for (int i = 0; i < 100; ++i)
    {
        LogicalPositionInfo3D position_info;
        BrickBaseInfo brick_info;
        int x_coordinate = distribution(rng);
        int p_coordinate = distribution(rng);
        TileCoordinate tc({ { 'x', x_coordinate}, { 'p', p_coordinate} });
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
        writer->AddBrick(&tc, &position_info, &brick_info, DataTypes::ZERO, TileDataStorageType::Invalid, nullptr);
        min_x = min(min_x, x_coordinate);
        min_p = min(min_p, p_coordinate);
        max_x = max(max_x, x_coordinate);
        max_p = max(max_p, p_coordinate);
    }

    const auto reader = doc->GetReader3d();

    // act
    auto min_max = reader->GetMinMaxForTileDimension({ 'p', 'x' });

    // assert
    ASSERT_EQ(min_max.size(), 2);
    ASSERT_TRUE(min_max.find('x') != min_max.cend());
    ASSERT_TRUE(min_max.find('p') != min_max.cend());
    ASSERT_EQ(min_max['x'].minimum_value, min_x);
    ASSERT_EQ(min_max['x'].maximum_value, max_x);
    ASSERT_EQ(min_max['p'].minimum_value, min_p);
    ASSERT_EQ(min_max['p'].maximum_value, max_p);
}

TEST(DocInfo3d, GetMBoundingBoxForXyzForRandomDocumentAndCheckResult)
{
    // arrange
    const auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetDocumentType(DocumentType::kImage3d);
    create_options->SetFilename(":memory:");
    create_options->AddDimension('x');
    create_options->SetUseSpatialIndex(false);
    create_options->SetCreateBlobTable(false);
    const auto doc = ClassFactory::CreateNew(create_options.get());
    const auto writer = doc->GetWriter3d();

    random_device rd;
    mt19937 rng(rd());
    uniform_real_distribution<double> distribution(-1e6, 1e6);

    double min_x = numeric_limits<double>::max();
    double max_x = numeric_limits<double>::min();
    double min_y = numeric_limits<double>::max();
    double max_y = numeric_limits<double>::min();
    double min_z = numeric_limits<double>::max();
    double max_z = numeric_limits<double>::min();
    for (int i = 0; i < 100; ++i)
    {
        LogicalPositionInfo3D position_info;
        BrickBaseInfo brick_info;
        TileCoordinate tc({ { 'x', i} });
        position_info.posX = distribution(rng);
        position_info.posY = distribution(rng);
        position_info.posZ = distribution(rng);
        position_info.width = 10;
        position_info.height = 10;
        position_info.depth = 10;
        position_info.pyrLvl = 0;
        brick_info.pixelWidth = 10;
        brick_info.pixelHeight = 10;
        brick_info.pixelDepth = 10;
        brick_info.pixelType = 0;
        writer->AddBrick(&tc, &position_info, &brick_info, DataTypes::ZERO, TileDataStorageType::Invalid, nullptr);
        min_x = min(min_x, position_info.posX);
        max_x = max(max_x, position_info.posX + position_info.width);
        min_y = min(min_y, position_info.posY);
        max_y = max(max_y, position_info.posY + position_info.height);
        min_z = min(min_z, position_info.posZ);
        max_z = max(max_z, position_info.posZ + position_info.depth);
    }

    const auto reader = doc->GetReader3d();

    // act
    DoubleInterval interval_x, interval_y, interval_z;
    DoubleInterval interval_x_partial, interval_y_partial, interval_z_partial;
    reader->GetBricksBoundingBox(&interval_x, &interval_y, &interval_z);
    reader->GetBricksBoundingBox(&interval_x_partial, nullptr, nullptr);    // exercise variation for better code-coverage
    reader->GetBricksBoundingBox(nullptr, &interval_y_partial, nullptr);    // exercise variation for better code-coverage
    reader->GetBricksBoundingBox(nullptr, nullptr, &interval_z_partial);    // exercise variation for better code-coverage
    reader->GetBricksBoundingBox(nullptr, nullptr, nullptr);                // exercise variation for better code-coverage, which is quite pointless here of course

    // assert
    ASSERT_DOUBLE_EQ(interval_x.minimum_value, min_x);
    ASSERT_DOUBLE_EQ(interval_x.maximum_value, max_x);
    ASSERT_DOUBLE_EQ(interval_y.minimum_value, min_y);
    ASSERT_DOUBLE_EQ(interval_y.maximum_value, max_y);
    ASSERT_DOUBLE_EQ(interval_z.minimum_value, min_z);
    ASSERT_DOUBLE_EQ(interval_z.maximum_value, max_z);
    ASSERT_EQ(interval_x, interval_x_partial);
    ASSERT_EQ(interval_y, interval_y_partial);
    ASSERT_EQ(interval_z, interval_z_partial);
}

struct VariousNumberOfBricksFixture : public testing::TestWithParam<int> {};

TEST_P(VariousNumberOfBricksFixture, GetTotalTileCountForSimpleDocumentAndCheckResult)
{
    // arrange
    const int num_tiles = GetParam();
    const auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetDocumentType(DocumentType::kImage3d);
    create_options->SetFilename(":memory:");
    create_options->AddDimension('l');
    create_options->SetUseSpatialIndex(false);
    create_options->SetCreateBlobTable(false);
    const auto doc = ClassFactory::CreateNew(create_options.get());
    const auto writer = doc->GetWriter3d();

    // we just add the specified number of tiles to the document (at layer 0)
    for (int l = 0; l < num_tiles; ++l)
    {
        LogicalPositionInfo3D position_info_3d;
        BrickBaseInfo brick_base_info;
        TileCoordinate tc({ { 'l', l } });
        position_info_3d.posX = 0;
        position_info_3d.posY = 0;
        position_info_3d.posZ = 0;
        position_info_3d.width = 10;
        position_info_3d.height = 10;
        position_info_3d.depth = 12;
        position_info_3d.pyrLvl = 0;
        brick_base_info.pixelWidth = 10;
        brick_base_info.pixelHeight = 10;
        brick_base_info.pixelDepth = 10;
        brick_base_info.pixelType = 0;
        writer->AddBrick(&tc, &position_info_3d, &brick_base_info, DataTypes::ZERO, TileDataStorageType::Invalid, nullptr);
    }

    const auto reader = doc->GetReader3d();

    // act
    const auto number_of_tiles = reader->GetTotalTileCount();

    // assert
    EXPECT_EQ(number_of_tiles, num_tiles);
}

TEST_P(VariousNumberOfBricksFixture, GetTilePerLayerCountForSimpleDocumentAndCheckResult)
{
    // arrange
    const int num_tiles = GetParam();
    const auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetDocumentType(DocumentType::kImage3d);
    create_options->SetFilename(":memory:");
    create_options->AddDimension('l');
    create_options->SetUseSpatialIndex(false);
    create_options->SetCreateBlobTable(false);
    const auto doc = ClassFactory::CreateNew(create_options.get());
    const auto writer = doc->GetWriter3d();

    // we just add the specified number of tiles to the document on layer 0
    for (int l = 0; l < num_tiles; ++l)
    {
        LogicalPositionInfo3D position_info;
        BrickBaseInfo tileInfo;
        TileCoordinate tc({ { 'l', l } });
        position_info.posX = 0;
        position_info.posY = 0;
        position_info.posZ = 0;
        position_info.width = 10;
        position_info.height = 10;
        position_info.depth = 10;
        position_info.pyrLvl = 0;
        tileInfo.pixelWidth = 10;
        tileInfo.pixelHeight = 10;
        tileInfo.pixelDepth = 10;
        tileInfo.pixelType = 0;
        writer->AddBrick(&tc, &position_info, &tileInfo, DataTypes::ZERO, TileDataStorageType::Invalid, nullptr);
    }

    // ...and, half of them on layer 1
    for (int l = 0; l < num_tiles / 2; ++l)
    {
        LogicalPositionInfo3D position_info;
        BrickBaseInfo tileInfo;
        TileCoordinate tc({ { 'l', l } });
        position_info.posX = 0;
        position_info.posY = 0;
        position_info.posZ = 0;
        position_info.width = 10;
        position_info.height = 10;
        position_info.depth = 10;
        position_info.pyrLvl = 1;
        tileInfo.pixelWidth = 10;
        tileInfo.pixelHeight = 10;
        tileInfo.pixelDepth = 10;
        tileInfo.pixelType = 0;
        writer->AddBrick(&tc, &position_info, &tileInfo, DataTypes::ZERO, TileDataStorageType::Invalid, nullptr);
    }

    // ...and, a quarter of them on layer 2
    for (int l = 0; l < num_tiles / 4; ++l)
    {
        LogicalPositionInfo3D position_info;
        BrickBaseInfo tileInfo;
        TileCoordinate tc({ { 'l', l } });
        position_info.posX = 0;
        position_info.posY = 0;
        position_info.posZ = 0;
        position_info.width = 10;
        position_info.height = 10;
        position_info.depth = 10;
        position_info.pyrLvl = 2;
        tileInfo.pixelWidth = 10;
        tileInfo.pixelHeight = 10;
        tileInfo.pixelDepth = 10;
        tileInfo.pixelType = 0;
        writer->AddBrick(&tc, &position_info, &tileInfo, DataTypes::ZERO, TileDataStorageType::Invalid, nullptr);
    }

    const auto reader = doc->GetReader3d();

    // act
    auto number_of_tiles = reader->GetTotalTileCount();
    auto tile_count_per_layer = reader->GetTileCountPerLayer();

    // assert
    EXPECT_EQ(number_of_tiles, num_tiles + num_tiles / 2 + num_tiles / 4);

    // we expect the number of tiles per layer to be correct
    map<int, std::uint64_t> expected_tile_count_per_layer = { { 0, num_tiles }, { 1, num_tiles / 2 }, { 2, num_tiles / 4 } };
    ASSERT_TRUE(mapsEqual(tile_count_per_layer, expected_tile_count_per_layer));
}

INSTANTIATE_TEST_SUITE_P(
    DocInfo3d,
    VariousNumberOfBricksFixture,
    testing::Values(5, 13, 27));

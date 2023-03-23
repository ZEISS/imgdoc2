// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <array>
#include <random>

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
        int x_coordinate = i;
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
    reader->GetBricksBoundingBox(&interval_x, &interval_y, &interval_z);

    // assert
    ASSERT_DOUBLE_EQ(interval_x.minimum_value, min_x);
    ASSERT_DOUBLE_EQ(interval_x.maximum_value, max_x);
    ASSERT_DOUBLE_EQ(interval_y.minimum_value, min_y);
    ASSERT_DOUBLE_EQ(interval_y.maximum_value, max_y);
    ASSERT_DOUBLE_EQ(interval_z.minimum_value, min_z);
    ASSERT_DOUBLE_EQ(interval_z.maximum_value, max_z);

}

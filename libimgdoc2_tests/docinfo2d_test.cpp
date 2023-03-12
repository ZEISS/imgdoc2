// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../libimgdoc2/inc/imgdoc2.h"
#include <array>
#include <random>
#include <algorithm>

using namespace std;
using namespace imgdoc2;
using namespace testing;

TEST(DocInfo2d, SimpleQueryTileDimensionsAndCheck)
{
    // arrange
    const auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetFilename(":memory:");
    create_options->AddDimension('M');
    create_options->SetUseSpatialIndex(false);
    create_options->SetCreateBlobTable(true);
    const auto doc = ClassFactory::CreateNew(create_options.get());
    const auto reader = doc->GetReader2d();

    // act
    const auto tile_dimensions = reader->GetTileDimensions();

    // assert
    ASSERT_THAT(tile_dimensions, ElementsAre('M'));
}

TEST(DocInfo2d, MultipleDimensionsQueryTileDimensionsAndCheck)
{
    const array<imgdoc2::Dimension, 5> expected_dimensions{ 'l', 'N', 'p', 'b', 'Q' };

    // arrange
    const auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetFilename(":memory:");
    for (const auto dimension : expected_dimensions)
    {
        create_options->AddDimension(dimension);
    }

    create_options->SetUseSpatialIndex(false);
    create_options->SetCreateBlobTable(true);
    const auto doc = ClassFactory::CreateNew(create_options.get());
    const auto reader = doc->GetReader2d();

    // act
    const auto tile_dimensions = reader->GetTileDimensions();

    // assert
    ASSERT_THAT(tile_dimensions, UnorderedElementsAreArray(expected_dimensions));
}

TEST(DocInfo2d, MultipleDimensionsQueryTileDimensionsAndCheckForNoOutOfBounds)
{
    const array<imgdoc2::Dimension, 5> expected_dimensions{ 'l', 'N', 'p', 'b', 'Q' };

    // arrange
    const auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetFilename(":memory:");
    for (const auto dimension : expected_dimensions)
    {
        create_options->AddDimension(dimension);
    }

    create_options->SetUseSpatialIndex(false);
    create_options->SetCreateBlobTable(true);
    const auto doc = ClassFactory::CreateNew(create_options.get());
    const auto reader = doc->GetReader2d();

    // act
    array<imgdoc2::Dimension, 2> output_size_two{ 'x', 'x' };
    uint32_t size = 1;
    reader->GetTileDimensions(output_size_two.data(), size);

    // assert
    ASSERT_EQ(size, 5) << "We expect to see '5' reported as the number of available elements";
    ASSERT_TRUE(find(expected_dimensions.cbegin(), expected_dimensions.cbegin(), output_size_two[0]) != expected_dimensions.cend()) <<
        "The reported element should be one of the used dimensions.";
    ASSERT_EQ(output_size_two[1], 'x') << "The array must not be overwritten at the end.";
}

TEST(DocInfo2d, GetMinMaxForTileDimensionForSimpleDocumentAndCheckResult)
{
    // arrange
    const auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetFilename(":memory:");
    create_options->AddDimension('l');
    create_options->AddDimension('u');
    create_options->SetUseSpatialIndex(false);
    create_options->SetCreateBlobTable(false);
    const auto doc = ClassFactory::CreateNew(create_options.get());
    const auto writer = doc->GetWriter2d();

    LogicalPositionInfo position_info;
    TileBaseInfo tileInfo;
    TileCoordinate tc({ { 'l', 5 }, { 'u', 3} });
    position_info.posX = 0;
    position_info.posY = 0;
    position_info.width = 10;
    position_info.height = 10;
    position_info.pyrLvl = 0;
    tileInfo.pixelWidth = 10;
    tileInfo.pixelHeight = 10;
    tileInfo.pixelType = 0;
    writer->AddTile(&tc, &position_info, &tileInfo, DataTypes::ZERO, TileDataStorageType::Invalid, nullptr);

    tc = { { 'l', 2 }, { 'u', 31} };
    writer->AddTile(&tc, &position_info, &tileInfo, DataTypes::ZERO, TileDataStorageType::Invalid, nullptr);

    const auto reader = doc->GetReader2d();

    // act
    auto min_max = reader->GetMinMaxForTileDimension({ 'l', 'u' });

    // assert
    ASSERT_EQ(min_max.size(), 2);
    ASSERT_TRUE(min_max.find('l') != min_max.cend());
    ASSERT_TRUE(min_max.find('u') != min_max.cend());
    ASSERT_EQ(min_max['l'].minimum_value, 2);
    ASSERT_EQ(min_max['l'].maximum_value, 5);
    ASSERT_EQ(min_max['u'].minimum_value, 3);
    ASSERT_EQ(min_max['u'].maximum_value, 31);
}

TEST(DocInfo2d, GetMinMaxForTileDimensionForRandomDocumentAndCheckResult)
{
    // arrange
    const auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetFilename(":memory:");
    create_options->AddDimension('x');
    create_options->AddDimension('p');
    create_options->SetUseSpatialIndex(false);
    create_options->SetCreateBlobTable(false);
    const auto doc = ClassFactory::CreateNew(create_options.get());
    const auto writer = doc->GetWriter2d();

    random_device rd;
    mt19937 rng(rd());
    uniform_int_distribution<int> distribution(numeric_limits<int>::min(), numeric_limits<int>::max());

    int min_x = numeric_limits<int>::max();
    int min_p = numeric_limits<int>::max();
    int max_x = numeric_limits<int>::min();
    int max_p = numeric_limits<int>::min();
    for (int i = 0; i < 100; ++i)
    {
        LogicalPositionInfo position_info;
        TileBaseInfo tileInfo;
        int x_coordinate = distribution(rng);
        int p_coordinate = distribution(rng);
        TileCoordinate tc({ { 'x', x_coordinate}, { 'p', p_coordinate} });
        position_info.posX = 0;
        position_info.posY = 0;
        position_info.width = 10;
        position_info.height = 10;
        position_info.pyrLvl = 0;
        tileInfo.pixelWidth = 10;
        tileInfo.pixelHeight = 10;
        tileInfo.pixelType = 0;
        writer->AddTile(&tc, &position_info, &tileInfo, DataTypes::ZERO, TileDataStorageType::Invalid, nullptr);
        min_x = min(min_x, x_coordinate);
        min_p = min(min_p, p_coordinate);
        max_x = max(max_x, x_coordinate);
        max_p = max(max_p, p_coordinate);
    }

    const auto reader = doc->GetReader2d();

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

TEST(DocInfo2d, GetMinMaxForTileDimensionCallWithInvalidDimensionAndExpectException)
{
    // arrange
    const auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetFilename(":memory:");
    create_options->AddDimension('w');
    create_options->SetUseSpatialIndex(false);
    create_options->SetCreateBlobTable(false);
    const auto doc = ClassFactory::CreateNew(create_options.get());
    const auto writer = doc->GetWriter2d();

    LogicalPositionInfo position_info;
    TileBaseInfo tileInfo;
    TileCoordinate tc({ { 'w', 5 } });
    position_info.posX = 0;
    position_info.posY = 0;
    position_info.width = 10;
    position_info.height = 10;
    position_info.pyrLvl = 0;
    tileInfo.pixelWidth = 10;
    tileInfo.pixelHeight = 10;
    tileInfo.pixelType = 0;
    writer->AddTile(&tc, &position_info, &tileInfo, DataTypes::ZERO, TileDataStorageType::Invalid, nullptr);

    const auto reader = doc->GetReader2d();

    // act & assert
    EXPECT_THROW(
       reader->GetMinMaxForTileDimension({ 'K' }),
       imgdoc2::invalid_argument_exception);
}

TEST(DocInfo2d, GetMinMaxForTileDimensionCallWithEmptyArrayOfDimensionsAndCheckResult)
{
    // arrange
    const auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetFilename(":memory:");
    create_options->AddDimension('w');
    create_options->SetUseSpatialIndex(false);
    create_options->SetCreateBlobTable(false);
    const auto doc = ClassFactory::CreateNew(create_options.get());

    const auto reader = doc->GetReader2d();

    // act
    auto result = reader->GetMinMaxForTileDimension({ });

    // assert
    EXPECT_TRUE(result.empty());
}

// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <random>
#include "../libimgdoc2/inc/imgdoc2.h"

using namespace std;
using namespace imgdoc2;
using namespace testing;

TEST(Query, CreateDocumentAndQueryWithTileInfoQuery)
{
    auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetFilename(":memory:");
    create_options->AddDimension('M');
    create_options->SetUseSpatialIndex(false);
    create_options->SetCreateBlobTable(false);
    auto doc = ClassFactory::CreateNew(create_options.get());

    auto writer = doc->GetWriter2d();
    std::vector<dbIndex> indices_for_pyramid_level_3;

    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution< int > distribute(0, 20);

    for (int m = 0; m < 100; ++m)
    {
        LogicalPositionInfo position_info;
        TileBaseInfo tileInfo;
        TileCoordinate tc({ { 'M', m} });
        position_info.posX = m * 10;
        position_info.posY = 0;
        position_info.width = 10;
        position_info.height = 10;
        position_info.pyrLvl = distribute(generator);
        tileInfo.pixelWidth = 10;
        tileInfo.pixelHeight = 10;
        tileInfo.pixelType = 0;
        dbIndex index = writer->AddTile(&tc, &position_info, &tileInfo, DataTypes::ZERO, TileDataStorageType::Invalid, nullptr);
        if (position_info.pyrLvl == 3)
        {
            indices_for_pyramid_level_3.push_back(index);
        }
    }

    auto reader = doc->GetReader2d();

    CTileInfoQueryClause tile_info_query_clause;
    tile_info_query_clause.AddPyramidLevelCondition(LogicalOperator::Invalid, ComparisonOperation::Equal, 3);
    vector<dbIndex> query_result;
    reader->Query(nullptr, &tile_info_query_clause, [&query_result](dbIndex index)->bool {query_result.push_back(index); return true; });

    ASSERT_THAT(indices_for_pyramid_level_3, ContainerEq(query_result));
}

TEST(Query, CreateDocumentAndQueryWithTileInfoQuery2)
{
    auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetFilename(":memory:");
    create_options->AddDimension('M');
    create_options->SetUseSpatialIndex(false);
    create_options->SetCreateBlobTable(false);
    auto doc = ClassFactory::CreateNew(create_options.get());

    auto writer = doc->GetWriter2d();
    std::vector<dbIndex> indices_for_pyramid_levels_greater_8_or_5;

    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution< int > distribute(0, 20);

    for (int m = 0; m < 100; ++m)
    {
        LogicalPositionInfo position_info;
        TileBaseInfo tileInfo;
        TileCoordinate tc({ { 'M', m} });
        position_info.posX = m * 10;
        position_info.posY = 0;
        position_info.width = 10;
        position_info.height = 10;
        position_info.pyrLvl = distribute(generator);
        tileInfo.pixelWidth = 10;
        tileInfo.pixelHeight = 10;
        tileInfo.pixelType = 0;
        dbIndex index = writer->AddTile(&tc, &position_info, &tileInfo, DataTypes::ZERO, TileDataStorageType::Invalid, nullptr);
        if (position_info.pyrLvl > 8 || position_info.pyrLvl == 5)
        {
            indices_for_pyramid_levels_greater_8_or_5.push_back(index);
        }
    }

    auto reader = doc->GetReader2d();

    CTileInfoQueryClause tile_info_query_clause;
    tile_info_query_clause.AddPyramidLevelCondition(LogicalOperator::Invalid, ComparisonOperation::GreaterThan, 8);
    tile_info_query_clause.AddPyramidLevelCondition(LogicalOperator::Or, ComparisonOperation::Equal, 5);
    vector<dbIndex> query_result;
    reader->Query(nullptr, &tile_info_query_clause, [&query_result](dbIndex index)->bool {query_result.push_back(index); return true; });

    ASSERT_THAT(indices_for_pyramid_levels_greater_8_or_5, ContainerEq(query_result));
}

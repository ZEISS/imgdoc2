// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <array>
#include "../libimgdoc2/inc/imgdoc2.h"

using namespace std;
using namespace imgdoc2;
using namespace testing;

/// Creates a new in-memory document with the following characteristics: We have 10x10 tiles,
/// each width=height=10, in a checkerboard-arrangement of 10 row and 10 columns. Each tile
/// has an M-index, starting to count from 1.
/// \param  use_spatial_index   True if the document is to use a spatial index.
/// \returns                    The newly created in-memory "checkerboard document".
shared_ptr<IDoc> CreateCheckerboardDocument(bool use_spatial_index)
{
    const auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetFilename(":memory:");
    //create_options->SetFilename("d:\\test.db");
    create_options->AddDimension('M');
    create_options->SetUseSpatialIndex(true);
    create_options->SetCreateBlobTable(true);

    auto doc = ClassFactory::CreateNew(create_options.get());
    const auto writer = doc->GetWriter2d();

    for (int column = 0; column < 10; ++column)
    {
        for (int row = 0; row < 10; ++row)
        {
            LogicalPositionInfo position_info;
            TileBaseInfo tileInfo;
            TileCoordinate tc({ { 'M', column * 10 + row + 1 } });
            position_info.posX = column * 10;
            position_info.posY = row * 10;
            position_info.width = 10;
            position_info.height = 10;
            position_info.pyrLvl = 0;
            tileInfo.pixelWidth = 10;
            tileInfo.pixelHeight = 10;
            tileInfo.pixelType = 0;
            writer->AddTile(&tc, &position_info, &tileInfo, DataTypes::ZERO, TileDataStorageType::Invalid, nullptr);
        }
    }

    return doc;
}

/// Utility for retrieving the M-coordinate from a list of tiles. No error handling is done here.
/// \param [in]     reader          The reader object.
/// \param          keys            The PKs of the tiles to query.
/// \returns        The m index of items.
vector<int> GetMIndexOfItems(IDocRead2d* reader, const vector<dbIndex>& keys)
{
    vector<int> m_indices;
    for (const auto pk : keys)
    {
        TileCoordinate tc;
        reader->ReadTileInfo(pk, &tc, nullptr, nullptr);
        int m_index;
        tc.TryGetCoordinate('M', &m_index);
        m_indices.push_back(m_index);
    }

    return m_indices;
}

struct WithAndWithoutSpatialIndexFixture1 : public testing::TestWithParam<bool> {};

TEST_P(WithAndWithoutSpatialIndexFixture1, IndexQueryForRectAndCheckResult)
{
    // Using the 10x10 checkerboard-document, we query for tiles overlapping with the ROI (0,0,15,15).
    // We expect to find 4 tiles, with M=1, 2, 11, 12.
    const bool use_spatial_index = GetParam();
    const auto doc = CreateCheckerboardDocument(use_spatial_index);
    const auto reader = doc->GetReader2d();

    vector<dbIndex> result_indices;
    reader->GetTilesIntersectingRect(
        RectangleD{ 0, 0, 15, 15 },
        nullptr,
        nullptr,
        [&](dbIndex index)->bool
        {
            result_indices.emplace_back(index);
            return true;
        });

    const auto m_indices = GetMIndexOfItems(reader.get(), result_indices);
    EXPECT_THAT(m_indices, UnorderedElementsAre(1, 11, 2, 12));
}

INSTANTIATE_TEST_SUITE_P(
    Query2d,
    WithAndWithoutSpatialIndexFixture1,
    testing::Values(true, false));

struct WithAndWithoutSpatialIndexFixture2 : public testing::TestWithParam<bool> {};
TEST_P(WithAndWithoutSpatialIndexFixture2, IndexQueryForRectAndCheckResult)
{
    // we use a combined "ROI and coordinate-query", we look for subblocks which intersect with the rectangle (0,0,15,15) and
    // with the M-index in the range 0 to 5 (exclusive the borders), i.e. M > 0 and M < 5.
    // We expect to find two subblocks (with M-index 1 and 2).
    const bool use_spatial_index = GetParam();
    const auto doc = CreateCheckerboardDocument(use_spatial_index);
    const auto reader = doc->GetReader2d();

    CDimCoordinateQueryClause coordinate_query_clause;
    coordinate_query_clause.AddRangeClause('M', IDimCoordinateQueryClause::RangeClause{ 0, 5 });

    vector<dbIndex> result_indices;
    reader->GetTilesIntersectingRect(RectangleD{ 0, 0, 15, 15 },
        &coordinate_query_clause,
        nullptr,
        [&](dbIndex index)->bool
        {
            result_indices.emplace_back(index); return true;
        });

    const auto m_indices = GetMIndexOfItems(reader.get(), result_indices);
    EXPECT_THAT(m_indices, UnorderedElementsAre(1, 2));
}

INSTANTIATE_TEST_SUITE_P(
    Query2d,
    WithAndWithoutSpatialIndexFixture2,
    testing::Values(true, false));

struct WithAndWithoutSpatialIndexFixture3 : public testing::TestWithParam<bool> {};

TEST_P(WithAndWithoutSpatialIndexFixture3, IndexQueryForRectAndCheckResult)
{
    // Using the 10x10 checkerboard-document, we query for tiles overlapping with the ROI (0,0,21,21).
    // We expect to find 9 tiles, with M=1, 2, 3, 11, 12, 13, 21, 22, 23. 
    const bool use_spatial_index = GetParam();
    const auto doc = CreateCheckerboardDocument(use_spatial_index);
    const auto reader = doc->GetReader2d();

    vector<dbIndex> result_indices;
    reader->GetTilesIntersectingRect(
        RectangleD{ 0, 0, 21, 21 },
        nullptr,
        nullptr,
        [&](dbIndex index)->bool
        {
            result_indices.emplace_back(index); return true;
        });

    const auto m_indices = GetMIndexOfItems(reader.get(), result_indices);
    EXPECT_THAT(m_indices, UnorderedElementsAre(1, 2, 3, 11, 12, 13, 21, 22, 23));
}

INSTANTIATE_TEST_SUITE_P(
    Query2d,
    WithAndWithoutSpatialIndexFixture3,
    testing::Values(true, false));

TEST(Query2d, EmptyCoordinateQueryClauseCheckResult)
{
    // we query with an empty coordinate-query-clause, and expect that an empty clause means
    //  "no condition, all items are returned"
    const auto doc = CreateCheckerboardDocument(false);
    const auto reader = doc->GetReader2d();

    const CDimCoordinateQueryClause coordinate_query_clause;

    vector<dbIndex> result_indices;
    reader->Query(
        &coordinate_query_clause,
        nullptr,
        [&](dbIndex index)->bool
        {
            result_indices.emplace_back(index);
            return true;
        });

    // so, we expect to get all tiles in the document, and we check their correctness
    EXPECT_EQ(result_indices.size(), 100ul);
    std::array<int, 100> expected_result{};
    for (int i = 0; i < static_cast<int>(expected_result.size()); ++i)
    {
        expected_result[i] = 1 + i;
    }

    const auto m_indices = GetMIndexOfItems(reader.get(), result_indices);
    EXPECT_THAT(m_indices, UnorderedElementsAreArray(expected_result));
}

TEST(Query2d, EmptyCoordinateQueryClauseAndNonEmptyTileInfoQueryClauseAndCheckResult)
{
    // we query with an empty coordinate-query-clause, and expect that an empty clause means
    //  "no condition, all items are returned"
    const auto doc = CreateCheckerboardDocument(false);
    const auto reader = doc->GetReader2d();

    const CDimCoordinateQueryClause coordinate_query_clause;

    // since all tiles have pyramid-level 0, this clause does not reduce the number of hits
    CTileInfoQueryClause tile_info_query_clause;
    tile_info_query_clause.AddPyramidLevelCondition(LogicalOperator::Invalid, ComparisonOperation::LessThanOrEqual, 1);

    vector<dbIndex> result_indices;
    reader->Query(
        &coordinate_query_clause,
        &tile_info_query_clause,
        [&](dbIndex index)->bool
        {
            result_indices.emplace_back(index);
            return true;
        });

    // so, we expect to get all tiles in the document, and we check their correctness
    EXPECT_EQ(result_indices.size(), 100ul);
    std::array<int, 100> expected_result{};
    for (int i = 0; i < static_cast<int>(expected_result.size()); ++i)
    {
        expected_result[i] = 1 + i;
    }

    const auto m_indices = GetMIndexOfItems(reader.get(), result_indices);
    EXPECT_THAT(m_indices, UnorderedElementsAreArray(expected_result));
}

TEST(Query2d, EmptyTileInfoQueryClauseAndCheckResult)
{
    // we query with an empty tile-info-clause, and expect that an empty clause means
    //  "no condition, all items are returned"
    const auto doc = CreateCheckerboardDocument(false);
    const auto reader = doc->GetReader2d();

    // since all tiles have pyramid-level 0, this clause does not reduce the number of hits
    const CTileInfoQueryClause tile_info_query_clause;

    vector<dbIndex> result_indices;
    reader->Query(
        nullptr,
        &tile_info_query_clause,
        [&](dbIndex index)->bool
        {
            result_indices.emplace_back(index);
            return true;
        });

    // so, we expect to get all tiles in the document, and we check their correctness
    EXPECT_EQ(result_indices.size(), 100ul);
    std::array<int, 100> expected_result{};
    for (int i = 0; i < static_cast<int>(expected_result.size()); ++i)
    {
        expected_result[i] = 1 + i;
    }

    const auto m_indices = GetMIndexOfItems(reader.get(), result_indices);
    EXPECT_THAT(m_indices, UnorderedElementsAreArray(expected_result));
}

TEST(Query2d, EmptyCoordinateQueryClauseAndEmptyTileInfoQueryClauseAndCheckResult)
{
    // we query with an empty coordinate-query-clause and an empty tile-info-query, and expect that an empty clause means
    //  "no condition, all items are returned"
    const auto doc = CreateCheckerboardDocument(false);
    const auto reader = doc->GetReader2d();

    const CDimCoordinateQueryClause coordinate_query_clause;

    // since all tiles have pyramid-level 0, this clause does not reduce the number of hits
    const CTileInfoQueryClause tile_info_query_clause;

    vector<dbIndex> result_indices;
    reader->Query(
        &coordinate_query_clause,
        &tile_info_query_clause,
        [&](dbIndex index)->bool
        {
            result_indices.emplace_back(index);
            return true;
        });

    // so, we expect to get all tiles in the document, and we check their correctness
    EXPECT_EQ(result_indices.size(), 100ul);
    std::array<int, 100> expected_result{};
    for (int i = 0; i < static_cast<int>(expected_result.size()); ++i)
    {
        expected_result[i] = 1 + i;
    }

    const auto m_indices = GetMIndexOfItems(reader.get(), result_indices);
    EXPECT_THAT(m_indices, UnorderedElementsAreArray(expected_result));
}

TEST(Query2d, ReadTileInfoWithAllNullArgumentsForExistingTileAndExpectSuccess)
{
    const auto doc = CreateCheckerboardDocument(false);
    const auto reader = doc->GetReader2d();

    // now, we need to get a primary-key of one tile in the document
    dbIndex primary_key_of_arbitrary_tile;
    bool primary_key_of_arbitrary_tile_valid = false;
    reader->Query(
        nullptr,
        nullptr,
        [&](dbIndex index)->bool
        {
            primary_key_of_arbitrary_tile = index;
            primary_key_of_arbitrary_tile_valid = true;
            return false;
        });

    EXPECT_TRUE(primary_key_of_arbitrary_tile_valid);

    EXPECT_NO_THROW(
        reader->ReadTileInfo(primary_key_of_arbitrary_tile, nullptr, nullptr, nullptr));
}

TEST(Query2d, ReadTileInfoWithAllNullArgumentsForNonExistingTileAndExpectException)
{
    const auto doc = CreateCheckerboardDocument(false);
    const auto reader = doc->GetReader2d();

    // now, we need to retrieve a non-existing primary-key (of a tile), we do this by
    //  looking for the max (of all existing keys) and add one
    dbIndex non_existing_primary_key;
    bool non_existing_primary_key_valid = false;
    reader->Query(
        nullptr,
        nullptr,
        [&](dbIndex index)->bool
        {
            if (!non_existing_primary_key_valid)
            {
                non_existing_primary_key = index + 1;
                non_existing_primary_key_valid = true;
            }
            else
            {
                if (index + 1 > non_existing_primary_key)
                {
                    non_existing_primary_key = index + 1;
                }
            }

            return true;
        });

    EXPECT_TRUE(non_existing_primary_key_valid);

    EXPECT_THROW(
        reader->ReadTileInfo(non_existing_primary_key, nullptr, nullptr, nullptr),
        non_existing_tile_exception);
}

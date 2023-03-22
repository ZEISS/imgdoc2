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

/// Creates a new in-memory document with the following characteristics: We have 10x10x10 bricks,
/// each width=height=depth=10, in a checkerboard-arrangement of 10 row, 10 columns and 10 "columns in z-direction". 
/// Each brick has an M-index, starting to count from 1.
/// \param  use_spatial_index   True if the document is to use a spatial index.
/// \returns                    The newly created in-memory "checkerboard document".
static shared_ptr<IDoc> CreateCheckerboard3dDocument(bool use_spatial_index)
{
    const auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetDocumentType(DocumentType::kImage3d);
    create_options->SetFilename(":memory:");
    //create_options->SetFilename("d:\\test.db");
    create_options->AddDimension('M');
    create_options->SetUseSpatialIndex(use_spatial_index);
    create_options->SetCreateBlobTable(true);

    auto doc = ClassFactory::CreateNew(create_options.get());
    const auto writer = doc->GetWriter3d();

    for (int column = 0; column < 10; ++column)
    {
        for (int row = 0; row < 10; ++row)
        {
            for (int z = 0; z < 10; ++z)
            {
                LogicalPositionInfo3D position_info;
                BrickBaseInfo brickInfo;
                TileCoordinate tc({ { 'M', z * 10 * 10 + column * 10 + row + 1 } });
                position_info.posX = column * 10;
                position_info.posY = row * 10;
                position_info.posZ = z * 10;
                position_info.width = 10;
                position_info.height = 10;
                position_info.depth = 10;
                position_info.pyrLvl = 0;
                brickInfo.pixelWidth = 10;
                brickInfo.pixelHeight = 10;
                brickInfo.pixelDepth = 10;
                brickInfo.pixelType = 0;
                writer->AddBrick(&tc, &position_info, &brickInfo, DataTypes::ZERO, TileDataStorageType::Invalid, nullptr);
            }
        }
    }

    return doc;
}

/// Utility for retrieving the M-coordinate from a list of tiles. No error handling is done here.
/// \param [in]     reader          The reader object.
/// \param          keys            The PKs of the tiles to query.
/// \returns        The m index of items.
static vector<int> GetMIndexOfItems(IDocRead3d* reader, const vector<dbIndex>& keys)
{
    vector<int> m_indices;
    for (const auto pk : keys)
    {
        TileCoordinate tc;
        reader->ReadBrickInfo(pk, &tc, nullptr, nullptr);
        int m_index;
        tc.TryGetCoordinate('M', &m_index);
        m_indices.push_back(m_index);
    }

    return m_indices;
}

TEST(Query3d, EmptyCoordinateQueryClauseCheckResult)
{
    // we query with an empty coordinate-query-clause, and expect that an empty clause means
    //  "no condition, all items are returned"
    const auto doc = CreateCheckerboard3dDocument(false);
    const auto reader = doc->GetReader3d();

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
    EXPECT_EQ(result_indices.size(), 1000ul);
    std::array<int, 1000> expected_result{};
    for (int i = 0; i < static_cast<int>(expected_result.size()); ++i)
    {
        expected_result[i] = 1 + i;
    }

    const auto m_indices = GetMIndexOfItems(reader.get(), result_indices);
    EXPECT_THAT(m_indices, UnorderedElementsAreArray(expected_result));
}

/// This fixture is used to pass in a single parameter to the test, which is whether to use a spatial index or not.
struct Query3dWithAndWithoutSpatialIndexFixture : public testing::TestWithParam<bool> {};

TEST_P(Query3dWithAndWithoutSpatialIndexFixture, IndexQueryForCuboidAndCheckResult)
{
    // Using the 10x10x10 checkerboard-document, we query for tiles overlapping with the ROI (0,0,0,15,15,15).
   // We expect to find 8 tiles, with M=1, 2, 11, 12, 101, 102, 111, 112.
    const bool use_spatial_index = GetParam();
    const auto doc = CreateCheckerboard3dDocument(use_spatial_index);
    const auto reader = doc->GetReader3d();

    vector<dbIndex> result_indices;
    reader->GetTilesIntersectingCuboid(
        CuboidD{ 0, 0, 0, 15, 15, 15 },
        nullptr,
        nullptr,
        [&](dbIndex index)->bool
        {
            result_indices.emplace_back(index);
            return true;
        });

    const auto m_indices = GetMIndexOfItems(reader.get(), result_indices);
    EXPECT_THAT(m_indices, UnorderedElementsAre(1, 11, 2, 12, 101, 102, 111, 112));
}

TEST_P(Query3dWithAndWithoutSpatialIndexFixture, IndexQueryForCuboidAndCoordinateQueryAndCheckResult)
{
    // we use a combined "ROI and coordinate-query", we look for subblocks which intersect with the rectangle (0,0,0,15,15,15) and
    // with the M-index in the range 0 to 5 or 100 to 105 (exclusive the borders), i.e. (M > 0 and M < 5) or (M > 100 and M<105).
    // We expect to find four subblocks (with M-index 1, 2, 101 and 102).
    const bool use_spatial_index = GetParam();
    const auto doc = CreateCheckerboard3dDocument(use_spatial_index);
    const auto reader = doc->GetReader3d();

    CDimCoordinateQueryClause coordinate_query_clause;
    coordinate_query_clause.AddRangeClause('M', IDimCoordinateQueryClause::RangeClause{ 0, 5 });
    coordinate_query_clause.AddRangeClause('M', IDimCoordinateQueryClause::RangeClause{ 100, 105 });

    vector<dbIndex> result_indices;
    reader->GetTilesIntersectingCuboid(CuboidD{ 0, 0, 0, 15, 15, 15 },
        &coordinate_query_clause,
        nullptr,
        [&](dbIndex index)->bool
        {
            result_indices.emplace_back(index);
            return true;
        });

    const auto m_indices = GetMIndexOfItems(reader.get(), result_indices);
    EXPECT_THAT(m_indices, UnorderedElementsAre(1, 2, 101, 102));
}

TEST_P(Query3dWithAndWithoutSpatialIndexFixture, PlaneBrickIntersectionTestCase1)
{
    // we query with an empty coordinate-query-clause, and expect that an empty clause means
    //  "no condition, all items are returned"
    const bool use_spatial_index = GetParam();
    const auto doc = CreateCheckerboard3dDocument(use_spatial_index);
    const auto reader = doc->GetReader3d();

    // we construct a plane parallel to the X-Y-plane, and going through the point (0,0,51) -
    //  so we expect to intersect with the bricks with z=[50,60], and there should be exactly 100 of them,
    //  and they have an M-index from 501...600 (that's how we constructed the sample document)
    const auto plane = Plane_NormalAndDistD::FromThreePoints(Point3dD(0, 0, 51), Point3dD(100, 0, 51), Point3dD(100, 100, 51));

    vector<dbIndex> result_indices;
    reader->GetTilesIntersectingPlane(
        plane,
        nullptr,
        nullptr,
        [&](dbIndex index)->bool
        {
            result_indices.emplace_back(index);
            return true;
        });

    // so, we expect to get all tiles in the document, and we check their correctness
    ASSERT_EQ(result_indices.size(), 100ul);
    std::array<int, 100> expected_result{};
    for (int i = 0; i < static_cast<int>(expected_result.size()); ++i)
    {
        expected_result[i] = 501 + i;
    }

    const auto m_indices = GetMIndexOfItems(reader.get(), result_indices);
    EXPECT_THAT(m_indices, UnorderedElementsAreArray(expected_result));
}

TEST_P(Query3dWithAndWithoutSpatialIndexFixture, PlaneBrickIntersectionTestCase2)
{
    const bool use_spatial_index = GetParam();
    const auto doc = CreateCheckerboard3dDocument(use_spatial_index);
    const auto reader = doc->GetReader3d();

    // we construct a plane parallel to the X-Z-plane, and going through the point (0,51,0) -
    //  so we expect to intersect with the bricks with Y=[50,60], and there should be exactly 100 of them,
    //  and they have an M-index from 6, 16, 26, ... (that's how we constructed the sample document)
    const auto plane = Plane_NormalAndDistD::FromThreePoints(Point3dD(0, 51, 0), Point3dD(100, 51, 0), Point3dD(100, 51, 100));

    vector<dbIndex> result_indices;
    reader->GetTilesIntersectingPlane(
        plane,
        nullptr,
        nullptr,
        [&](dbIndex index)->bool
        {
            result_indices.emplace_back(index);
            return true;
        });

    // so, we expect to get all tiles in the document, and we check their correctness
    ASSERT_EQ(result_indices.size(), 100ul);
    std::array<int, 100> expected_result{};
    for (int i = 0; i < static_cast<int>(expected_result.size()); ++i)
    {
        expected_result[i] = 10 * i + 6;
    }

    const auto m_indices = GetMIndexOfItems(reader.get(), result_indices);
    EXPECT_THAT(m_indices, UnorderedElementsAreArray(expected_result));
}

TEST_P(Query3dWithAndWithoutSpatialIndexFixture, PlaneBrickIntersectionTestCase3)
{
    const bool use_spatial_index = GetParam();
    const auto doc = CreateCheckerboard3dDocument(use_spatial_index);
    const auto reader = doc->GetReader3d();

    // we construct a plane parallel to the Y-Z-plane, and going through the point (51,0,0) -
    //  so we expect to intersect with the bricks with X=[50,60], and there should be exactly 100 of them,
    //  and they have an M-index 51, 52, ..., 60, 151, 152, ..., 160, 251, 252, ..., 260, 351, ... ... 960 (that's how we constructed the sample document)
    const auto plane = Plane_NormalAndDistD::FromThreePoints(Point3dD(51, 0, 0), Point3dD(51, 100, 0), Point3dD(51, 0, 100));

    vector<dbIndex> result_indices;
    reader->GetTilesIntersectingPlane(
        plane,
        nullptr,
        nullptr,
        [&](dbIndex index)->bool
        {
            result_indices.emplace_back(index);
            return true;
        });

    // so, we expect to get all tiles in the document, and we check their correctness
    ASSERT_EQ(result_indices.size(), 100ul);
    std::array<int, 100> expected_result{};
    for (int i = 0; i < static_cast<int>(expected_result.size()); ++i)
    {
        expected_result[i] = (i / 10) * 100 + 51 + (i % 10);
    }

    const auto m_indices = GetMIndexOfItems(reader.get(), result_indices);
    EXPECT_THAT(m_indices, UnorderedElementsAreArray(expected_result));
}

TEST_P(Query3dWithAndWithoutSpatialIndexFixture, PlaneBrickIntersectionTestCase4)
{
    const bool use_spatial_index = GetParam();
    const auto doc = CreateCheckerboard3dDocument(use_spatial_index);
    const auto reader = doc->GetReader3d();

    // we construct a plane going through the X-axis and the point (0,100,100) - i.e. a plane at a 45 deg angle to the X-Y-plane.
    const auto plane = Plane_NormalAndDistD::FromThreePoints(Point3dD(0, 0, 0), Point3dD(100, 0, 0), Point3dD(0, 100, 100));

    vector<dbIndex> result_indices;
    reader->GetTilesIntersectingPlane(
        plane,
        nullptr,
        nullptr,
        [&](dbIndex index)->bool
        {
            result_indices.emplace_back(index);
            return true;
        });

    // so, we expect to get all tiles in the document, and we check their correctness
    ASSERT_EQ(result_indices.size(), 280ul);
    std::array<int, 280> expected_result
    {
        1,2,11,12,21,22,31,32,41,42,51,52,61,62,71,72,81,82,91,92,101,102,103,111,112,113,121,122,123,131,132,133,141,142,143,151,152,153,161,162,163,171,172,173,181,182,183,191,
        192,193,202,203,204,212,213,214,222,223,224,232,233,234,242,243,244,252,253,254,262,263,264,272,273,274,282,283,284,292,293,294,303,304,305,313,314,315,323,324,325,333,334,
        335,343,344,345,353,354,355,363,364,365,373,374,375,383,384,385,393,394,395,404,405,406,414,415,416,424,425,426,434,435,436,444,445,446,454,455,456,464,465,466,474,475,476,
        484,485,486,494,495,496,505,506,507,515,516,517,525,526,527,535,536,537,545,546,547,555,556,557,565,566,567,575,576,577,585,586,587,595,596,597,606,607,608,616,617,618,626,
        627,628,636,637,638,646,647,648,656,657,658,666,667,668,676,677,678,686,687,688,696,697,698,707,708,709,717,718,719,727,728,729,737,738,739,747,748,749,757,758,759,767,768,
        769,777,778,779,787,788,789,797,798,799,808,809,810,818,819,820,828,829,830,838,839,840,848,849,850,858,859,860,868,869,870,878,879,880,888,889,890,898,899,900,909,910,919,
        920,929,930,939,940,949,950,959,960,969,970,979,980,989,990,999,1000
    };

    const auto m_indices = GetMIndexOfItems(reader.get(), result_indices);
    EXPECT_THAT(m_indices, UnorderedElementsAreArray(expected_result));
}

TEST_P(Query3dWithAndWithoutSpatialIndexFixture, PlaneBrickIntersectionAndDimensionQueryTestCase1)
{
    // we query with an empty coordinate-query-clause, and expect that an empty clause means
    //  "no condition, all items are returned"
    const bool use_spatial_index = GetParam();
    const auto doc = CreateCheckerboard3dDocument(use_spatial_index);
    const auto reader = doc->GetReader3d();

    // we construct a plane parallel to the X-Y-plane, and going through the point (0,0,51) -
    //  so we expect to intersect with the bricks with z=[50,60], and there should be exactly 100 of them,
    //  and they have an M-index from 501...600 (that's how we constructed the sample document).
    // We use an additional dimension query to filter out the M-indexes that are in the range [500,558].
    const auto plane = Plane_NormalAndDistD::FromThreePoints(Point3dD(0, 0, 51), Point3dD(100, 0, 51), Point3dD(100, 100, 51));

    CDimCoordinateQueryClause coordinate_query_clause;
    coordinate_query_clause.AddRangeClause('M', IDimCoordinateQueryClause::RangeClause{ 500, 558 });

    vector<dbIndex> result_indices;
    reader->GetTilesIntersectingPlane(
        plane,
        &coordinate_query_clause,
        nullptr,
        [&](dbIndex index)->bool
        {
            result_indices.emplace_back(index);
            return true;
        });

    std::vector<int> expected_result_m_indices{};
    for (int i = 0; i < 100; ++i)
    {
        int expected_m = 500 + i;
        if (expected_m > 500 && expected_m < 558)
        {
            expected_result_m_indices.emplace_back(expected_m);
        }
    }

    const auto m_indices = GetMIndexOfItems(reader.get(), result_indices);
    EXPECT_THAT(m_indices, UnorderedElementsAreArray(expected_result_m_indices));
}

TEST_P(Query3dWithAndWithoutSpatialIndexFixture, PlaneBrickIntersectionAndDimensionQueryTestCase2)
{
    const bool use_spatial_index = GetParam();
    const auto doc = CreateCheckerboard3dDocument(use_spatial_index);
    const auto reader = doc->GetReader3d();

    // we construct a plane parallel to the X-Z-plane, and going through the point (0,51,0) -
    //  so we expect to intersect with the bricks with Y=[50,60], and there should be exactly 100 of them,
    //  and they have an M-index from 6, 16, 26, ... (that's how we constructed the sample document).
    //  We use an additional dimension query to filter out the M-indexes that are in the range [0,358].
    const auto plane = Plane_NormalAndDistD::FromThreePoints(Point3dD(0, 51, 0), Point3dD(100, 51, 0), Point3dD(100, 51, 100));

    CDimCoordinateQueryClause coordinate_query_clause;
    coordinate_query_clause.AddRangeClause('M', IDimCoordinateQueryClause::RangeClause{ 0, 358 });

    vector<dbIndex> result_indices;
    reader->GetTilesIntersectingPlane(
        plane,
        &coordinate_query_clause,
        nullptr,
        [&](dbIndex index)->bool
        {
            result_indices.emplace_back(index);
            return true;
        });

    std::vector<int> expected_result_m_indices{};
    for (int i = 0; i < 100; ++i)
    {
        int expected_m = 10 * i + 6;
        if (expected_m > 0 && expected_m < 358)
        {
            expected_result_m_indices.emplace_back(expected_m);
        }
    }

    const auto m_indices = GetMIndexOfItems(reader.get(), result_indices);
    EXPECT_THAT(m_indices, UnorderedElementsAreArray(expected_result_m_indices));
}

TEST_P(Query3dWithAndWithoutSpatialIndexFixture, PlaneBrickIntersectionAndDimensionQueryTestCase3)
{
    const bool use_spatial_index = GetParam();
    const auto doc = CreateCheckerboard3dDocument(use_spatial_index);
    const auto reader = doc->GetReader3d();

    // we construct a plane parallel to the Y-Z-plane, and going through the point (51,0,0) -
    //  so we expect to intersect with the bricks with X=[50,60], and there should be exactly 100 of them,
    //  and they have an M-index 51, 52, ..., 60, 151, 152, ..., 160, 251, 252, ..., 260, 351, ... ... 960 (that's how we constructed the sample document).
    //  We use an additional dimension query to filter out the M-indexes that are in the range [0,500].
    const auto plane = Plane_NormalAndDistD::FromThreePoints(Point3dD(51, 0, 0), Point3dD(51, 100, 0), Point3dD(51, 0, 100));

    CDimCoordinateQueryClause coordinate_query_clause;
    coordinate_query_clause.AddRangeClause('M', IDimCoordinateQueryClause::RangeClause{ 0, 500 });

    vector<dbIndex> result_indices;
    reader->GetTilesIntersectingPlane(
        plane,
        &coordinate_query_clause,
        nullptr,
        [&](dbIndex index)->bool
        {
            result_indices.emplace_back(index);
            return true;
        });

    std::vector<int> expected_result_m_indices{};
    for (int i = 0; i < 100; ++i)
    {
        int expected_m = (i / 10) * 100 + 51 + (i % 10);
        if (expected_m > 0 && expected_m < 500)
        {
            expected_result_m_indices.emplace_back(expected_m);
        }
    }

    const auto m_indices = GetMIndexOfItems(reader.get(), result_indices);
    EXPECT_THAT(m_indices, UnorderedElementsAreArray(expected_result_m_indices));
}

INSTANTIATE_TEST_SUITE_P(
    Query3d,
    Query3dWithAndWithoutSpatialIndexFixture,
    testing::Values(true, false));

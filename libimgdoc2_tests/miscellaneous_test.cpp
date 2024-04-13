// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <limits>
#include <stdexcept>

#include "../libimgdoc2/inc/imgdoc2.h"

using namespace imgdoc2;
using namespace std;
using namespace testing;

TEST(Miscellaneous, TileCoordinateEquality)
{
    TileCoordinate tile_coordinate_a{ { 'l', 5 }, { 'u', 3} };
    TileCoordinate tile_coordinate_b{ { 'l', 5 }, { 'u', 3} };
    EXPECT_EQ(tile_coordinate_a, tile_coordinate_b);

    tile_coordinate_a = { { 'l', 5 }, { 'u', 3} };
    tile_coordinate_b = { { 'x', 5 }, { 'u', 3} };
    EXPECT_NE(tile_coordinate_a, tile_coordinate_b);

    tile_coordinate_a = { { 'A', 5 }, { 'B', 3} };
    tile_coordinate_b = { { 'A', 5 }, { 'B', 3}, { 'C', 12} };
    EXPECT_NE(tile_coordinate_a, tile_coordinate_b);
    EXPECT_NE(tile_coordinate_b, tile_coordinate_a);

    tile_coordinate_a = { { 'U', 5 }, { 'V', 3}, { 'W', 53} };
    tile_coordinate_b = { { 'U', 5 }, { 'V', 3}, { 'W', 52} };
    EXPECT_NE(tile_coordinate_a, tile_coordinate_b);
    EXPECT_NE(tile_coordinate_b, tile_coordinate_a);

    tile_coordinate_a = { { 'U', 5 }, { 'V', 3}, { 'W', 53} };
    tile_coordinate_b = { { 'U', 5 }, { 'V', 3}, { 'W', 53} };
    EXPECT_EQ(tile_coordinate_a, tile_coordinate_b);
    EXPECT_EQ(tile_coordinate_b, tile_coordinate_a);

    EXPECT_FALSE(ITileCoordinate::AreEqual(nullptr, &tile_coordinate_a));
    EXPECT_FALSE(ITileCoordinate::AreEqual(&tile_coordinate_a, nullptr));
    EXPECT_FALSE(ITileCoordinate::AreEqual(nullptr, nullptr));

    EXPECT_TRUE(ITileCoordinate::AreEqual(&tile_coordinate_a, &tile_coordinate_a));
}

TEST(Miscellaneous, IsDimensionValid)
{
    EXPECT_TRUE(IsDimensionValid('a'));
    EXPECT_NO_THROW(IsDimensionValid('a'));
    EXPECT_FALSE(IsDimensionValid(numeric_limits<Dimension>::max()));
    EXPECT_THROW(ThrowIfDimensionInvalid(numeric_limits<Dimension>::max()), invalid_argument);
}

TEST(Miscellaneous, Rectangle)
{
    EXPECT_THROW(RectangleF(0, 0, -1, -1), invalid_argument);   // check that we cannot construct a rectangle with negative width or height
    EXPECT_THROW(RectangleD(0, 0, -1, -1), invalid_argument);   // check that we cannot construct a rectangle with negative width or height

    RectangleD rectangleD(0, 0, 1, 1);
    EXPECT_TRUE(rectangleD.IsPointInside(PointD(0.5, 0.5)));
    EXPECT_FALSE(rectangleD.IsPointInside(PointD(1.5, 0.5)));

    RectangleF rectangleF(0, 0, 1, 1);
    EXPECT_TRUE(rectangleF.IsPointInside(PointF(0.5f, 0.5f)));
    EXPECT_FALSE(rectangleF.IsPointInside(PointF(1.5f, 0.5f)));
}

TEST(Miscellaneous, Cuboid)
{
    EXPECT_THROW(CuboidF(0, 0, 0, 1, -1, -1), invalid_argument);   // check that we cannot construct a cuboidD with negative width, height or depth
    EXPECT_THROW(CuboidD(0, 0, 0, 0, -1, -1), invalid_argument);   // check that we cannot construct a cuboidD with negative width, height or depth

    CuboidD cuboidD(0, 0, 0, 1, 1, 1);
    EXPECT_TRUE(cuboidD.IsPointInside(Point3dD(0.5, 0.5, 0.5)));
    EXPECT_FALSE(cuboidD.IsPointInside(Point3dD(1.5, 0.5, 0.5)));

    CuboidF cuboidF(0, 0, 0, 1, 1, 1);
    EXPECT_TRUE(cuboidF.IsPointInside(Point3dF(0.5f, 0.5f, 0.5f)));
    EXPECT_FALSE(cuboidF.IsPointInside(Point3dF(1.5f, 0.5f, 0.5f)));
}

TEST(Miscellaneous, LogicalPositionInfo)
{
    const LogicalPositionInfo logical_position_info_a{ 1, 2, 3, 4 };
    const LogicalPositionInfo logical_position_info_b{ 1, 2, 3, 4, 0 };
    const LogicalPositionInfo logical_position_info_c{ 1, 2, 3, 4, 4 };
    EXPECT_EQ(logical_position_info_a, logical_position_info_b);
    EXPECT_EQ(logical_position_info_b, logical_position_info_a);
    EXPECT_NE(logical_position_info_a, logical_position_info_c);
    EXPECT_NE(logical_position_info_b, logical_position_info_c);
}

TEST(Miscellaneous, LogicalPositionInfo3D)
{
    const LogicalPositionInfo3D logical_position_info_3d_a{ 1, 2, 3, 4, 6, 7 };
    const LogicalPositionInfo3D logical_position_info_3d_b{ 1, 2, 3, 4, 6, 7, 0 };
    const LogicalPositionInfo3D logical_position_info_3d_c{ 1, 2, 3, 4, 6, 7, 4 };
    EXPECT_EQ(logical_position_info_3d_a, logical_position_info_3d_b);
    EXPECT_EQ(logical_position_info_3d_b, logical_position_info_3d_a);
    EXPECT_NE(logical_position_info_3d_a, logical_position_info_3d_c);
    EXPECT_NE(logical_position_info_3d_b, logical_position_info_3d_c);
}

TEST(Miscellaneous, BlobOutputOnHeap)
{
    BlobOutputOnHeap blob_output_on_heap1;

    EXPECT_TRUE(blob_output_on_heap1.Reserve(4));
    EXPECT_THROW((void)blob_output_on_heap1.Reserve(4), logic_error); // check that we cannot reserve more than once

    uint8_t data[4] = { 1, 2, 3, 4 };
    BlobOutputOnHeap blob_output_on_heap2;
    EXPECT_THROW((void)blob_output_on_heap2.SetData(0, sizeof(data), data), logic_error); // check that we cannot set data without reserving first

    BlobOutputOnHeap blob_output_on_heap3;
    EXPECT_TRUE(blob_output_on_heap3.Reserve(4));
    EXPECT_THROW((void)blob_output_on_heap3.SetData(1, sizeof(data), data), logic_error); // check that we cannot set data which is out of bounds
}

TEST(Miscellaneous, CDimCoordinateQueryClauseQueryNonExistingDimensionExpectNull)
{
    CDimCoordinateQueryClause dim_coordinate_query_clause;
    dim_coordinate_query_clause.AddRangeClause('q', IDimCoordinateQueryClause::RangeClause{ 1, 2 });
    const auto range_clause = dim_coordinate_query_clause.GetRangeClause('o');    // try to query a range clause which does not exist
    EXPECT_TRUE(range_clause == nullptr);
}

TEST(Miscellaneous, Document2dCheckTransactionSemantic)
{
    const auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetFilename(":memory:");
    create_options->AddDimension('p');
    create_options->SetUseSpatialIndex(false);
    create_options->SetCreateBlobTable(false);
    const auto doc = ClassFactory::CreateNew(create_options.get());
    const auto open_existing_options = ClassFactory::CreateOpenExistingOptionsUp();
    const auto writer2d = doc->GetWriter2d();

    // trying to end a transaction without starting one should throw
    EXPECT_THROW(writer2d->CommitTransaction(), database_exception);

    // trying to rollback a transaction without starting one should throw as well
    EXPECT_THROW(writer2d->RollbackTransaction(), database_exception);

    writer2d->BeginTransaction();
    // trying to start a transaction while another one is already active should throw
    EXPECT_THROW(writer2d->BeginTransaction(), database_exception);
    writer2d->CommitTransaction();

    // commiting while there is no active transaction should throw
    EXPECT_THROW(writer2d->CommitTransaction(), database_exception);

    // rollback while there is no active transaction should throw as well
    EXPECT_THROW(writer2d->CommitTransaction(), database_exception);
}

TEST(Miscellaneous, Document3dCheckTransactionSemantic)
{
    const auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetDocumentType(DocumentType::kImage3d);
    create_options->SetFilename(":memory:");
    create_options->AddDimension('p');
    create_options->SetUseSpatialIndex(false);
    create_options->SetCreateBlobTable(false);
    const auto doc = ClassFactory::CreateNew(create_options.get());
    const auto open_existing_options = ClassFactory::CreateOpenExistingOptionsUp();
    const auto writer3d = doc->GetWriter3d();

    // trying to end a transaction without starting one should throw
    EXPECT_THROW(writer3d->CommitTransaction(), database_exception);

    // trying to rollback a transaction without starting one should throw as well
    EXPECT_THROW(writer3d->RollbackTransaction(), database_exception);

    writer3d->BeginTransaction();
    // trying to start a transaction while another one is already active should throw
    EXPECT_THROW(writer3d->BeginTransaction(), database_exception);
    writer3d->CommitTransaction();

    // commiting while there is no active transaction should throw
    EXPECT_THROW(writer3d->CommitTransaction(), database_exception);

    // rollback while there is no active transaction should throw as well
    EXPECT_THROW(writer3d->CommitTransaction(), database_exception);
}

TEST(Miscellaneous, DoubleInterval)
{
    const DoubleInterval interval1{ 1.0, 2.0 };
    const DoubleInterval interval2{ 1.0, 2.0 };
    const DoubleInterval interval3{ 1.5, 2.0 };

    EXPECT_TRUE(interval1 == interval2);
    EXPECT_FALSE(interval1 == interval3);

    EXPECT_FALSE(interval1 != interval2);
    EXPECT_TRUE(interval1 != interval3);

    EXPECT_TRUE(interval1.IsValid());
    EXPECT_TRUE(interval2.IsValid());
    EXPECT_TRUE(interval3.IsValid());

    const DoubleInterval interval4;
    const DoubleInterval interval5{ 5, 3 };
    EXPECT_FALSE(interval4.IsValid());
    EXPECT_FALSE(interval5.IsValid());
    EXPECT_TRUE(interval4 == interval5);
}

TEST(Miscellaneous, Int32Interval)
{
    const Int32Interval interval1{ 10, 20 };
    const Int32Interval interval2{ 10, 20 };
    const Int32Interval interval3{ 15, 20 };

    EXPECT_TRUE(interval1 == interval2);
    EXPECT_FALSE(interval1 == interval3);

    EXPECT_FALSE(interval1 != interval2);
    EXPECT_TRUE(interval1 != interval3);

    EXPECT_TRUE(interval1.IsValid());
    EXPECT_TRUE(interval2.IsValid());
    EXPECT_TRUE(interval3.IsValid());

    const Int32Interval interval4;
    const Int32Interval interval5{ 5, 3 };
    EXPECT_FALSE(interval4.IsValid());
    EXPECT_FALSE(interval5.IsValid());
    EXPECT_TRUE(interval4 == interval5);
}

struct HostingEnvironmentCallCount
{
    uint32_t count_log{ 0 };
    uint32_t count_is_level_active{ 0 };
};

static void CallbackLog(std::intptr_t userparam, int level, const char* szMessage)
{
    const auto call_count = reinterpret_cast<HostingEnvironmentCallCount*>(userparam);  // NOLINT(performance-no-int-to-ptr)
    ++call_count->count_log;
}

static bool CallbackIsLevelActive(std::intptr_t userparam, int level)
{
    const auto call_count = reinterpret_cast<HostingEnvironmentCallCount*>(userparam);  // NOLINT(performance-no-int-to-ptr)
    ++call_count->count_is_level_active;
    return true;
}

TEST(Miscellaneous, HostingEnvironmentForFunctionPointers)
{
    // We construct an environment object that uses function pointers to call back into the functions defined above,
    //  which in turn increment the call count in the HostingEnvironmentCallCount struct. In the end, we check that
    //  the functions have been called during the document creation/adding of a tile we do here.
    HostingEnvironmentCallCount call_count;
    const auto hosting_environment = ClassFactory::CreateHostingEnvironmentForFunctionPointers(
        reinterpret_cast<std::intptr_t>(&call_count),
        CallbackLog,
        CallbackIsLevelActive,
        nullptr);

    const auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetFilename(":memory:");
    create_options->AddDimension('M');
    create_options->SetUseSpatialIndex(false);
    create_options->SetCreateBlobTable(false);
    const auto doc = ClassFactory::CreateNew(create_options.get(), hosting_environment);
    const auto writer = doc->GetWriter2d();

    LogicalPositionInfo position_info;
    TileBaseInfo tile_info;
    const TileCoordinate tile_coordinate({ { 'M', 5 } });
    position_info.posX = 0;
    position_info.posY = 0;
    position_info.width = 10;
    position_info.height = 10;
    position_info.pyrLvl = 0;
    tile_info.pixelWidth = 10;
    tile_info.pixelHeight = 10;
    tile_info.pixelType = 0;
    writer->AddTile(&tile_coordinate, &position_info, &tile_info, DataTypes::ZERO, TileDataStorageType::Invalid, nullptr);

    EXPECT_GT(call_count.count_log, 0);
    EXPECT_GT(call_count.count_is_level_active, 0);
}

TEST(Miscellaneous, GetVersionInfo)
{
    const auto version_info = ClassFactory::GetVersionInfo();
    EXPECT_GE(version_info.major, 0);
    EXPECT_GE(version_info.minor, 0);
    EXPECT_GE(version_info.patch, 0);
    EXPECT_FALSE(version_info.major == 0 && version_info.minor == 0 && version_info.patch == 0);
}

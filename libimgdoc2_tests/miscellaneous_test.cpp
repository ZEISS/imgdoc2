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

TEST(Miscellaneous, CheckTransactionSemantic)
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

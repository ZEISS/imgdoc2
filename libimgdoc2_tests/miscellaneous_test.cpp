// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>
#include <gmock/gmock.h>

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

// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <limits>
#include <stdexcept>

#include "../libimgdoc2/inc/imgdoc2.h"
#include "utilities.h"

using namespace imgdoc2;
using namespace std;
using namespace testing;


TEST(DocumentOperation, InATransactionAddTilesThenRollbackExpectOperationsToBeUndone)
{
     // arrange

     // create an empty document
    const auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetFilename(":memory:");
    create_options->AddDimension('l');
    create_options->AddDimension('u');
    create_options->SetUseSpatialIndex(false);
    create_options->SetCreateBlobTable(false);
    const auto doc = ClassFactory::CreateNew(create_options.get());
    const auto open_existing_options = ClassFactory::CreateOpenExistingOptionsUp();
    const auto writer2d = doc->GetWriter2d();

    // act

    // start a transaction, add two tiles, then rollback
    writer2d->BeginTransaction();

    // now, try to add a tile to the document
    LogicalPositionInfo position_info;
    TileBaseInfo tile_info;
    position_info.posX = 1;
    position_info.posY = 2;
    position_info.width = 3;
    position_info.height = 4;
    position_info.pyrLvl = 0;
    tile_info.pixelWidth = 10;
    tile_info.pixelHeight = 11;
    tile_info.pixelType = PixelType::Gray32Float;
    TileCoordinate tile_coordinate({ { 'l', 3}, {'u',1} });
    writer2d->AddTile(&tile_coordinate, &position_info, &tile_info, DataTypes::ZERO, TileDataStorageType::Invalid, nullptr);

    tile_coordinate = { { { 'l', 1}, {'u', 2} } };
    writer2d->AddTile(&tile_coordinate, &position_info, &tile_info, DataTypes::ZERO, TileDataStorageType::Invalid, nullptr);
    writer2d->RollbackTransaction();

    // assert

    // since we rolled back, the tile count should be zero
    //writer2d->RollbackTransaction();
    const auto reader2d = doc->GetReader2d();
    const auto total_tile_count = reader2d->GetTotalTileCount();
    EXPECT_EQ(total_tile_count, 0);
}

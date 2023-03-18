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

// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>
#include "../libimgdoc2/inc/imgdoc2.h"
#include "../libimgdoc2/src/db/utilities.h"

using namespace std;
using namespace imgdoc2;

TEST(BlobData, AddTileWithBlobDataAndRetrieveItCheckForCorrectness)
{
    const size_t kBLOB_SIZE = 100;
    auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetFilename(":memory:");
    //create_options->SetFilename("d:\\test.db");
    create_options->AddDimension('M');
    create_options->SetCreateBlobTable(true);

    auto doc = ClassFactory::CreateNew(create_options.get());
    auto writer = doc->GetWriter2d();

    LogicalPositionInfo position_info;
    TileBaseInfo tileInfo;
    TileCoordinate tc({ { 'M', 0} });
    position_info.posX = 0;
    position_info.posY = 0;
    position_info.width = 10;
    position_info.height = 10;
    position_info.pyrLvl = 0;
    tileInfo.pixelWidth = 10;
    tileInfo.pixelHeight = 10;
    tileInfo.pixelType = 0;
    imgdoc2::DataObjectOnHeap blobData{ kBLOB_SIZE };
    for (size_t i = 0; i < blobData.GetSizeOfData(); ++i)
    {
        uint8_t* ptrData = static_cast<uint8_t*>(blobData.GetData()) + i;
        *ptrData = static_cast<uint8_t>(i);
    }

    writer->AddTile(
        &tc,
        &position_info,
        &tileInfo,
        DataTypes::UNCOMPRESSED_BITMAP,
        TileDataStorageType::BlobInDatabase,
        &blobData);

    auto reader = doc->GetReader2d();
    CDimCoordinateQueryClause coordinate_query_clause;
    coordinate_query_clause.AddRangeClause('M', IDimCoordinateQueryClause::RangeClause{ 0, 0 });
    vector<dbIndex> indices;
    reader->Query(&coordinate_query_clause, nullptr, [&](dbIndex index)->bool {indices.push_back(index); return true; });
    ASSERT_EQ(indices.size(), 1);

    imgdoc2::BlobOutputOnHeap output_blob;
    reader->ReadTileData(indices[0], &output_blob);
    ASSERT_TRUE(output_blob.GetHasData());
    ASSERT_EQ(output_blob.GetSizeOfData(), kBLOB_SIZE);
    ASSERT_EQ(memcmp(output_blob.GetDataC(), blobData.GetDataC(), kBLOB_SIZE), 0);
}

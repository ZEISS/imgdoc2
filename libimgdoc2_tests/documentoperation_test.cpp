#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <limits>
#include <stdexcept>

#include "../libimgdoc2/inc/imgdoc2.h"
#include "utilities.h"

using namespace imgdoc2;
using namespace std;
using namespace testing;


TEST(DocumentOperation, OpenDocumentReadOnlyAndTryToWriteAndExpectFailure)
{
    // This is using a special filename for sqlite, which creates a database in memory and allows it to be opened
    //  again by another connection (which is what we do in the test, c.f. https://www.sqlite.org/inmemorydb.html).
    //  The memory is reclaimed when the last connection to the database closes.
    //  In addition, we use some preprocessor-trickery in order to create a unique filename for each test (by just adding filename and linenumber).
    static const char* kDocumentFileName = "file:" STRINGIFY(__FILE__) STRINGIFY(__LINE__) "memdb?mode=memory&cache=shared";

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
    open_existing_options->SetFilename(kDocumentFileName);
    open_existing_options->SetOpenReadonly(true);
    const auto doc2 = ClassFactory::OpenExisting(open_existing_options.get());
    const auto writer2d = doc2->GetWriter2d();

    // act

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
    const TileCoordinate tile_coordinate({ { 'l', 3}, {'u',1} });
    writer2d->AddTile(&tile_coordinate, &position_info, &tile_info, DataTypes::ZERO, TileDataStorageType::Invalid, nullptr);
}

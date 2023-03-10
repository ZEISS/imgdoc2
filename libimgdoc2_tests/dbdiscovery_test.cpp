// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../libimgdoc2/src/db/DbFactory.h"
#include "../libimgdoc2/src/db/database_creator.h"
#include "../libimgdoc2/src/db/database_discovery.h"
#include "../libimgdoc2/src/db/database_constants.h"

using namespace imgdoc2;
using namespace std;
using namespace testing;

TEST(DbDiscoveryTest, CreateAndDiscover2D)
{
    auto dbConnection = DbFactory::SqliteCreateNewDatabase(":memory:");
    shared_ptr<DatabaseConfiguration2D> database_configuration_from_creation;

    {
        DbCreator db_creator(dbConnection);
        auto create_options = ClassFactory::CreateCreateOptionsUp();
        create_options->AddDimension('a');
        create_options->AddDimension('b');
        create_options->AddDimension('b');
        create_options->AddDimension('c');
        create_options->AddIndexForDimension('a');
        create_options->AddIndexForDimension('c');
        auto configuration = db_creator.CreateTables(create_options.get());
        database_configuration_from_creation = dynamic_pointer_cast<DatabaseConfiguration2D>(configuration);
    }

    shared_ptr< DatabaseConfiguration2D> database_configuration_from_discovery;
    {
        DbDiscovery db_discovery(dbConnection);
        db_discovery.DoDiscovery();
        database_configuration_from_discovery = dynamic_pointer_cast<DatabaseConfiguration2D>(db_discovery.GetDatabaseConfiguration());
    }

    dbConnection.reset();

    EXPECT_TRUE(database_configuration_from_creation->GetTableNameForTilesInfoOrThrow() == database_configuration_from_discovery->GetTableNameForTilesInfoOrThrow());
    EXPECT_TRUE(database_configuration_from_creation->GetTableNameForTilesDataOrThrow() == database_configuration_from_discovery->GetTableNameForTilesDataOrThrow());
    EXPECT_TRUE(database_configuration_from_creation->GetTileDimensions().size() == database_configuration_from_discovery->GetTileDimensions().size());
    EXPECT_THAT(database_configuration_from_creation->GetTileDimensions(), UnorderedElementsAreArray(database_configuration_from_discovery->GetTileDimensions()));
    EXPECT_THAT(database_configuration_from_creation->GetIndexedTileDimensions(), UnorderedElementsAreArray(database_configuration_from_discovery->GetIndexedTileDimensions()));

    // since we didn't request a "spatial index" for creation, we don't expect to find one here
    EXPECT_THROW(database_configuration_from_creation->GetTableNameForTilesSpatialIndexTableOrThrow(), runtime_error);

    EXPECT_FALSE(database_configuration_from_creation->GetIsUsingSpatialIndex());
    EXPECT_FALSE(database_configuration_from_discovery->GetIsUsingSpatialIndex());
}

TEST(DbDiscoveryTest, CreateWithSpatialIndexAndDiscover2D)
{
    auto dbConnection = DbFactory::SqliteCreateNewDatabase(":memory:");
    shared_ptr< DatabaseConfiguration2D> database_configuration_from_creation;

    {
        DbCreator db_creator(dbConnection);
        auto create_options = ClassFactory::CreateCreateOptionsUp();
        create_options->AddDimension('a');
        create_options->AddDimension('b');
        create_options->AddDimension('c');
        create_options->AddIndexForDimension('a');
        create_options->AddIndexForDimension('c');
        create_options->SetUseSpatialIndex(true);   // request a spatial index
        auto configuration = db_creator.CreateTables(create_options.get());
        database_configuration_from_creation = dynamic_pointer_cast<DatabaseConfiguration2D>(configuration);
    }

    shared_ptr< DatabaseConfiguration2D> database_configuration_from_discovery;
    {
        DbDiscovery db_discovery(dbConnection);
        db_discovery.DoDiscovery();
        database_configuration_from_discovery = dynamic_pointer_cast<DatabaseConfiguration2D>(db_discovery.GetDatabaseConfiguration());
    }

    dbConnection.reset();

    EXPECT_TRUE(database_configuration_from_creation->GetTableNameForTilesInfoOrThrow() == database_configuration_from_discovery->GetTableNameForTilesInfoOrThrow());
    EXPECT_TRUE(database_configuration_from_creation->GetTableNameForTilesDataOrThrow() == database_configuration_from_discovery->GetTableNameForTilesDataOrThrow());
    EXPECT_TRUE(database_configuration_from_creation->GetTileDimensions().size() == database_configuration_from_discovery->GetTileDimensions().size());
    EXPECT_THAT(database_configuration_from_creation->GetTileDimensions(), UnorderedElementsAreArray(database_configuration_from_discovery->GetTileDimensions()));
    EXPECT_THAT(database_configuration_from_creation->GetIndexedTileDimensions(), UnorderedElementsAreArray(database_configuration_from_discovery->GetIndexedTileDimensions()));
    EXPECT_TRUE(database_configuration_from_creation->GetTableNameForTilesSpatialIndexTableOrThrow() == database_configuration_from_discovery->GetTableNameForTilesSpatialIndexTableOrThrow());
    EXPECT_TRUE(database_configuration_from_creation->GetIsUsingSpatialIndex());
    EXPECT_TRUE(database_configuration_from_discovery->GetIsUsingSpatialIndex());
}

TEST(DbDiscoveryTest, CreateAndAlterGeneralTableToHaveInconsistentInformationAndDiscover2D)
{
    // we create a document w/o spatial index, then (manually) modify the general-table and point
    //  to a spatial index with doesn't exists

    auto dbConnection = DbFactory::SqliteCreateNewDatabase(":memory:");
    shared_ptr<DatabaseConfiguration2D> database_configuration_from_creation;

    {
        DbCreator db_creator(dbConnection);
        auto create_options = ClassFactory::CreateCreateOptionsUp();
        create_options->AddDimension('a');
        create_options->AddDimension('b');
        create_options->AddDimension('c');
        create_options->AddIndexForDimension('a');
        create_options->AddIndexForDimension('c');
        create_options->SetUseSpatialIndex(false);
        auto configuration = db_creator.CreateTables(create_options.get());
        database_configuration_from_creation = dynamic_pointer_cast<DatabaseConfiguration2D>(configuration);
    }

    // insert into the "General"-table an element pointing to a spatial-index-table which
    //  does not exists
    ostringstream stream;
    stream << "INSERT INTO [" << database_configuration_from_creation->GetTableNameForGeneralTableOrThrow() << "]"
        << " (" << database_configuration_from_creation->GetColumnNameOfGeneralInfoTableOrThrow(DatabaseConfigurationCommon::kGeneralInfoTable_Column_Key) << ","
        << database_configuration_from_creation->GetColumnNameOfGeneralInfoTableOrThrow(DatabaseConfigurationCommon::kGeneralInfoTable_Column_ValueString) << ") VALUES("
        << "\"" << DbConstants::GetGeneralTable_ItemKey(GeneralTableItems::kSpatialIndexTable) << "\"," << "\"XYZ\"" << ");";
    dbConnection->Execute(stream.str());

    // and now - the expectation is that the "discovery" can identify the spatial-index as being
    //  not operational
    shared_ptr<DatabaseConfiguration2D> database_configuration_from_discovery;
    {
        DbDiscovery db_discovery(dbConnection);
        db_discovery.DoDiscovery();
        database_configuration_from_discovery = dynamic_pointer_cast<DatabaseConfiguration2D>(db_discovery.GetDatabaseConfiguration());
    }

    dbConnection.reset();

    EXPECT_FALSE(database_configuration_from_discovery->GetIsUsingSpatialIndex());
}

TEST(DbDiscoveryTest, CreateEmptyDataBaseAndExpectDiscover2DToReportError)
{
    auto dbConnection = DbFactory::SqliteCreateNewDatabase(":memory:");

    DbDiscovery db_discovery(dbConnection);
    EXPECT_THROW(db_discovery.DoDiscovery(), imgdoc2::discovery_exception);
}

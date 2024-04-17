// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net_UnitTests
{
    using FluentAssertions;
    using ImgDoc2Net.Implementation;
    using ImgDoc2Net.Interfaces;
    using ImgDoc2Net.Interop;
    using System;
    using System.Collections.Generic;

    [Collection(NonParallelCollectionDefinitionClass.Name)]
    public class CreateDocumentTests
    {
        [Fact]
        public void CreateNewDocumentAndCheckIfItIsOperationalAtInteropLevel()
        {
            // this test is operating on "interop"-level
            var instance = ImgDoc2ApiInterop.Instance;

            // we get the "statistics" before running our test - the statistics contains counters of active objects,
            //  and we check before leaving the test that it is where is was before (usually zero)
            var statisticsBeforeTest = instance.GetStatistics();

            var createOptionsHandle = instance.CreateCreateOptions();
            instance.CreateOptionsSetFilename(createOptionsHandle, ":memory:");
            var documentHandle = instance.CreateNewDocument(createOptionsHandle);
            Assert.NotEqual(documentHandle, IntPtr.Zero);
            var reader2dHandle = instance.DocumentGetReader2d(documentHandle);
            Assert.NotEqual(reader2dHandle, IntPtr.Zero);
            instance.DestroyCreateOptions(createOptionsHandle);
            instance.DestroyDocument(documentHandle);
            instance.DestroyReader2d(reader2dHandle);

            Assert.True(Utilities.IsActiveObjectCountEqual(statisticsBeforeTest, instance.GetStatistics()), "orphaned native imgdoc2-objects detected");
        }

        [Fact]
        public void CreateNewDocument3DAndCheckIfItIsOperationalAtInteropLevel()
        {
            // this test is operating on "interop"-level
            var instance = ImgDoc2ApiInterop.Instance;

            // we get the "statistics" before running our test - the statistics contains counters of active objects,
            //  and we check before leaving the test that it is where is was before (usually zero)
            var statisticsBeforeTest = instance.GetStatistics();

            var createOptionsHandle = instance.CreateCreateOptions();
            instance.CreateOptionsSetDocumentType(createOptionsHandle, DocumentType.Image3d);
            instance.CreateOptionsSetFilename(createOptionsHandle, ":memory:");
            var documentHandle = instance.CreateNewDocument(createOptionsHandle);
            Assert.NotEqual(documentHandle, IntPtr.Zero);
            var reader3dHandle = instance.DocumentGetReader3d(documentHandle);
            Assert.NotEqual(reader3dHandle, IntPtr.Zero);
            instance.DestroyCreateOptions(createOptionsHandle);
            instance.DestroyDocument(documentHandle);
            instance.DestroyReader3d(reader3dHandle);

            Assert.True(Utilities.IsActiveObjectCountEqual(statisticsBeforeTest, instance.GetStatistics()), "orphaned native imgdoc2-objects detected");
        }

        [Fact]
        public void CreateNewDocumentWithInvalidCreateOptionsAndExpectExceptionAtInteropLevel()
        {
            // this test is operating on "interop"-level
            var instance = ImgDoc2ApiInterop.Instance;
            var createOptionsHandle = instance.CreateCreateOptions();
            instance.CreateOptionsSetFilename(createOptionsHandle, "/../invalid:XXXXXX:");
            Assert.Throws<ImgDoc2Exception>(() => instance.CreateNewDocument(createOptionsHandle));
            instance.DestroyCreateOptions(createOptionsHandle);
        }

        [Fact]
        public void CreateNewDocumentAndCheckIfItIsOperational()
        {
            using var createOptions = new CreateOptions() { Filename = ":memory:" };
            using var document = Document.CreateNew(createOptions);
            using var reader2d = document.Get2dReader();
            Assert.NotNull(reader2d);
        }

        [Fact]
        public void CreateNewDocument3DAndCheckIfItIsOperational()
        {
            using var createOptions = new CreateOptions() { Filename = ":memory:", DocumentType = DocumentType.Image3d };
            using var document = Document.CreateNew(createOptions);
            using var reader2d = document.Get3dReader();
            Assert.NotNull(reader2d);
        }

        [Fact]
        public void CreateNewDocumentAndAddATileAndRunQueriesAtInteropLevel()
        {
            // this test is operating on "interop"-level
            var instance = ImgDoc2ApiInterop.Instance;

            // we get the "statistics" before running our test - the statistics contains counters of active objects,
            //  and we check before leaving the test that it is where is was before (usually zero)
            var statisticsBeforeTest = instance.GetStatistics();

            var createOptionsHandle = instance.CreateCreateOptions();
            instance.CreateOptionsSetFilename(createOptionsHandle, ":memory:");
            instance.CreateOptionsSetUseBlobTable(createOptionsHandle, true);
            instance.CreateOptionsAddDimension(createOptionsHandle, new Dimension('A'));
            instance.CreateOptionsAddDimension(createOptionsHandle, new Dimension('Z'));
            var documentHandle = instance.CreateNewDocument(createOptionsHandle);
            Assert.NotEqual(documentHandle, IntPtr.Zero);
            var writer2dHandle = instance.DocumentGetWriter2d(documentHandle);
            Assert.NotEqual(writer2dHandle, IntPtr.Zero);

            // add one tile with coordinate "A1Z2"
            TileCoordinate coordinate = new TileCoordinate(
                new[]
                {
                    Tuple.Create(new Dimension('A'), 1) ,
                    Tuple.Create(new Dimension('Z'), 2)
                });

            LogicalPosition logicalPosition = new LogicalPosition() { PositionX = 0, PositionY = 1, Width = 2, Height = 3, PyramidLevel = 0 };
            Tile2dBaseInfo tile2dBaseInfo = new Tile2dBaseInfo(1, 2, PixelType.Gray8);
            instance.Writer2dAddTile(writer2dHandle, coordinate, in logicalPosition, tile2dBaseInfo, DataType.Zero, IntPtr.Zero, 0);

            // now, query for "tiles with A=1" (where there is obviously one)
            var dimensionQueryClause = new DimensionQueryClause();
            dimensionQueryClause.AddCondition(new DimensionCondition() { Dimension = new Dimension('A'), RangeStart = 1, RangeEnd = 1 });

            var reader2dHandle = instance.DocumentGetReader2d(documentHandle);
            var queryResult = instance.Reader2dQuery(reader2dHandle, dimensionQueryClause, null, QueryOptions.DefaultMaxNumberOfResults);

            Assert.True(queryResult.ResultComplete);
            Assert.True(queryResult.Keys.Count == 1, "We expect to find one tile as result of the query.");

            instance.DestroyReader2d(reader2dHandle);

            // now, query for "tiles with A=5" (where there is obviously none)
            dimensionQueryClause = new DimensionQueryClause();
            dimensionQueryClause.AddCondition(new DimensionCondition() { Dimension = new Dimension('A'), RangeStart = 5, RangeEnd = 5 });

            reader2dHandle = instance.DocumentGetReader2d(documentHandle);
            queryResult = instance.Reader2dQuery(reader2dHandle, dimensionQueryClause, null, QueryOptions.DefaultMaxNumberOfResults);
            Assert.True(queryResult.ResultComplete);
            Assert.True(queryResult.Keys.Count == 0, "We expect to find no tile as result of the query.");

            instance.DestroyCreateOptions(createOptionsHandle);
            instance.DestroyDocument(documentHandle);
            instance.DestroyReader2d(reader2dHandle);
            instance.DestroyWriter2d(writer2dHandle);

            Assert.True(Utilities.IsActiveObjectCountEqual(statisticsBeforeTest, instance.GetStatistics()), "orphaned native imgdoc2-objects detected");
        }

        [Fact]
        public void CreateNewDocumentAndAddTwoTilesAndRunQueriesAtInteropLevel()
        {
            // this test is operating on "interop"-level
            var instance = ImgDoc2ApiInterop.Instance;

            // we get the "statistics" before running our test - the statistics contains counters of active objects,
            //  and we check before leaving the test that it is where is was before (usually zero)
            var statisticsBeforeTest = instance.GetStatistics();

            {
                var createOptionsHandle = instance.CreateCreateOptions();
                instance.CreateOptionsSetFilename(createOptionsHandle, ":memory:");
                instance.CreateOptionsSetUseBlobTable(createOptionsHandle, true);   // TODO(JBl) - since in the following no blob-storage is used, should we allow/ensure operation without blob-store?
                instance.CreateOptionsAddDimension(createOptionsHandle, new Dimension('A'));
                instance.CreateOptionsAddDimension(createOptionsHandle, new Dimension('B'));
                var documentHandle = instance.CreateNewDocument(createOptionsHandle);
                Assert.NotEqual(documentHandle, IntPtr.Zero);
                var writer2dHandle = instance.DocumentGetWriter2d(documentHandle);
                Assert.NotEqual(writer2dHandle, IntPtr.Zero);

                // add one tile with coordinate "A1Z2"
                TileCoordinate coordinate = new TileCoordinate(
                    new[]
                    {
                    Tuple.Create(new Dimension('A'), 1) ,
                    Tuple.Create(new Dimension('B'), 1)
                    });

                LogicalPosition logicalPosition = new LogicalPosition() { PositionX = 0, PositionY = 1, Width = 2, Height = 3, PyramidLevel = 0 };
                Tile2dBaseInfo tile2dBaseInfo = new Tile2dBaseInfo(2, 3, PixelType.Gray8);
                instance.Writer2dAddTile(writer2dHandle, coordinate, in logicalPosition, tile2dBaseInfo, DataType.Zero, IntPtr.Zero, 0);

                coordinate = new TileCoordinate(
                    new[]
                    {
                    Tuple.Create(new Dimension('A'), 1) ,
                    Tuple.Create(new Dimension('B'), 2)
                    });

                instance.Writer2dAddTile(writer2dHandle, coordinate, in logicalPosition, tile2dBaseInfo, DataType.Zero, IntPtr.Zero, 0);

                // now, query for "tiles with A=1" (where there are two)
                var dimensionQueryClause = new DimensionQueryClause();
                dimensionQueryClause.AddCondition(new DimensionCondition() { Dimension = new Dimension('A'), RangeStart = 1, RangeEnd = 1 });

                var reader2dHandle = instance.DocumentGetReader2d(documentHandle);
                var queryResult = instance.Reader2dQuery(reader2dHandle, dimensionQueryClause, null, QueryOptions.DefaultMaxNumberOfResults);

                Assert.True(queryResult.ResultComplete);
                Assert.True(queryResult.Keys.Count == 2, "We expect to find two tiles as result of the query.");

                //// now, query for "tiles with A=1 and B=2" (where there is none)
                dimensionQueryClause = new DimensionQueryClause();
                dimensionQueryClause.AddCondition(new DimensionCondition() { Dimension = new Dimension('A'), RangeStart = 1, RangeEnd = 1 });
                dimensionQueryClause.AddCondition(new DimensionCondition() { Dimension = new Dimension('B'), RangeStart = 2, RangeEnd = 2 });

                queryResult = instance.Reader2dQuery(reader2dHandle, dimensionQueryClause, null, QueryOptions.DefaultMaxNumberOfResults);
                Assert.True(queryResult.ResultComplete);
                Assert.True(queryResult.Keys.Count == 1, "We expect to find one tile as result of the query.");

                instance.DestroyCreateOptions(createOptionsHandle);
                instance.DestroyDocument(documentHandle);
                instance.DestroyReader2d(reader2dHandle);
                instance.DestroyWriter2d(writer2dHandle);
            }

            Assert.True(Utilities.IsActiveObjectCountEqual(statisticsBeforeTest, instance.GetStatistics()), "orphaned native imgdoc2-objects detected");
        }

        [Fact]
        public void CreateNewDocumentAndAddATileWithTileDataAndReadTileDataBackAndCompareAtInteropLevel()
        {
            // this test is operating on "interop"-level
            var instance = ImgDoc2ApiInterop.Instance;

            // we get the "statistics" before running our test - the statistics contains counters of active objects,
            //  and we check before leaving the test that it is where is was before (usually zero)
            var statisticsBeforeTest = instance.GetStatistics();

            {
                var createOptionsHandle = instance.CreateCreateOptions();
                instance.CreateOptionsSetFilename(createOptionsHandle, ":memory:");
                ////instance.CreateOptionsSetFilename(createOptionsHandle, "N:\\test.db");
                instance.CreateOptionsSetUseBlobTable(createOptionsHandle, true);
                instance.CreateOptionsAddDimension(createOptionsHandle, new Dimension('A'));
                instance.CreateOptionsAddDimension(createOptionsHandle, new Dimension('Z'));
                var documentHandle = instance.CreateNewDocument(createOptionsHandle);
                Assert.NotEqual(documentHandle, IntPtr.Zero);
                var writer2dHandle = instance.DocumentGetWriter2d(documentHandle);
                Assert.NotEqual(writer2dHandle, IntPtr.Zero);

                // add one tile with coordinate "A1Z2"
                TileCoordinate coordinate = new TileCoordinate(
                    new[]
                    {
                    Tuple.Create(new Dimension('A'), 1) ,
                    Tuple.Create(new Dimension('Z'), 2)
                    });

                LogicalPosition logicalPosition = new LogicalPosition() { PositionX = 0, PositionY = 1, Width = 2, Height = 3, PyramidLevel = 0 };
                Tile2dBaseInfo tile2dBaseInfo = new Tile2dBaseInfo(1, 2, PixelType.Gray8);
                byte[] tileData = new byte[5] { 1, 2, 3, 4, 5 };
                instance.Writer2dAddTile(writer2dHandle, coordinate, in logicalPosition, tile2dBaseInfo, DataType.UncompressedBitmap, tileData);

                // now, query for "tiles with A=1" (where there is obviously one)
                var dimensionQueryClause = new DimensionQueryClause();
                dimensionQueryClause.AddCondition(new DimensionCondition() { Dimension = new Dimension('A'), RangeStart = 1, RangeEnd = 1 });

                var reader2dHandle = instance.DocumentGetReader2d(documentHandle);
                var queryResult = instance.Reader2dQuery(reader2dHandle, dimensionQueryClause, null, QueryOptions.DefaultMaxNumberOfResults);

                Assert.True(queryResult.ResultComplete);
                Assert.True(queryResult.Keys.Count == 1, "We expect to find one tile as result of the query.");

                var blob = instance.Reader2dReadTileData(reader2dHandle, queryResult.Keys[0]);
                Assert.Equal(5, blob.Length);
                Assert.True(blob[0] == tileData[0] && blob[1] == tileData[1] && blob[2] == tileData[2] && blob[3] == tileData[3] && blob[4] == tileData[4]);

                instance.DestroyCreateOptions(createOptionsHandle);
                instance.DestroyDocument(documentHandle);
                instance.DestroyReader2d(reader2dHandle);
                instance.DestroyWriter2d(writer2dHandle);
            }

            Assert.True(Utilities.IsActiveObjectCountEqual(statisticsBeforeTest, instance.GetStatistics()), "orphaned native imgdoc2-objects detected");
        }

        [Fact]
        public void CreateNewDocumentAndAddATileWithTileDataAndReadTileDataBack()
        {
            // this is the same operation as the test above ("CreateNewDocumentAndAddATileWithTileDataAndReadTileDataBackAndCompareAtInteropLevel"), 
            // but operating on "interface level"

            // we get the "statistics" before running our test - the statistics contains counters of active objects,
            //  and we check before leaving the test that it is where is was before (usually zero)
            var statisticsBeforeTest = ImgDoc2ApiInterop.Instance.GetStatistics();

            {
                using var createOptions = new CreateOptions();

                createOptions.Filename = ":memory:";
                createOptions.UseSpatialIndex = true;
                createOptions.UseBlobTable = true;
                createOptions.AddDimension(new Dimension('A'));
                createOptions.AddDimension(new Dimension('Z'));

                using var document = Document.CreateNew(createOptions);
                using var writer = document.Get2dWriter();

                // add one tile with coordinate "A1Z2"
                TileCoordinate coordinate = new TileCoordinate(
                    new[]
                    {
                    Tuple.Create(new Dimension('A'), 1) ,
                    Tuple.Create(new Dimension('Z'), 2)
                    });

                LogicalPosition logicalPosition = new LogicalPosition() { PositionX = 0, PositionY = 1, Width = 2, Height = 3, PyramidLevel = 0 };
                Tile2dBaseInfo tile2dBaseInfo = new Tile2dBaseInfo(1, 2, PixelType.Gray8);
                byte[] tileData = new byte[5] { 1, 2, 3, 4, 5 };

                writer.AddTile(coordinate, logicalPosition, tile2dBaseInfo, DataType.UncompressedBitmap, tileData);

                using var reader = document.Get2dReader();
                var dimensionQueryClause = new DimensionQueryClause();
                dimensionQueryClause.AddCondition(new DimensionCondition() { Dimension = new Dimension('A'), RangeStart = 1, RangeEnd = 1 });
                var listOfTiles = reader.Query(dimensionQueryClause, null);
                Assert.True(listOfTiles.Count == 1, "We expect to find one tile as result of the query.");

                var tileDataReadFromDocument = reader.ReadTileData(listOfTiles[0]);
                Assert.Equal(5, tileDataReadFromDocument.Length);
                Assert.True(tileDataReadFromDocument[0] == tileData[0] &&
                                tileDataReadFromDocument[1] == tileData[1] &&
                                tileDataReadFromDocument[2] == tileData[2] &&
                                tileDataReadFromDocument[3] == tileData[3] &&
                                tileDataReadFromDocument[4] == tileData[4]);
            }

            Assert.True(Utilities.IsActiveObjectCountEqual(statisticsBeforeTest, ImgDoc2ApiInterop.Instance.GetStatistics()), "orphaned native imgdoc2-objects detected");
        }

        [Theory]
        [InlineData(true)]
        [InlineData(false)]
        public void CreateNewDocumentWithAndWithoutSpatialIndexAndAdd10By10TilesUseSpatialQueryAndCheckResult(bool useSpatialIndex)
        {

            // we get the "statistics" before running our test - the statistics contains counters of active objects,
            //  and we check before leaving the test that it is where is was before (usually zero)
            var statisticsBeforeTest = ImgDoc2ApiInterop.Instance.GetStatistics();

            {
                using var createOptions = new CreateOptions();

                createOptions.Filename = ":memory:";
                createOptions.UseSpatialIndex = useSpatialIndex;
                createOptions.UseBlobTable = true;
                createOptions.AddDimension(new Dimension('M'));

                using var document = Document.CreateNew(createOptions);
                using var writer = document.Get2dWriter();

                List<long> expectedTiles = new();

                for (int x = 0; x < 10; ++x)
                {
                    for (int y = 0; y < 10; ++y)
                    {
                        TileCoordinate coordinate = new TileCoordinate(
                                                              new[]
                                                              {
                                                                Tuple.Create(new Dimension('M'), y*10+x) ,
                                                              });

                        LogicalPosition logicalPosition = new LogicalPosition() { PositionX = x, PositionY = y, Width = 1, Height = 1, PyramidLevel = 0 };
                        Tile2dBaseInfo tile2dBaseInfo = new Tile2dBaseInfo(1, 1, PixelType.Gray8);
                        byte[] tileData = new byte[5] { 1, 2, 3, 4, 5 };
                        long pk = writer.AddTile(coordinate, logicalPosition, tile2dBaseInfo, DataType.UncompressedBitmap, tileData);
                        if (x >= 0 && x <= 2 && y >= 0 && y <= 2)
                        {
                            expectedTiles.Add(pk);
                        }
                    }
                }

                using var reader = document.Get2dReader();
                var listOfTiles = reader.QueryTilesIntersectingRect(new Rectangle() { X = 0, Y = 0, Width = 2, Height = 2 }, null, null);

                // check whether the two lists have the same content, irrespective of order
                expectedTiles.Should().BeEquivalentTo(listOfTiles);
            }

            Assert.True(Utilities.IsActiveObjectCountEqual(statisticsBeforeTest, ImgDoc2ApiInterop.Instance.GetStatistics()), "orphaned native imgdoc2-objects detected");
        }
    }
}

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

    [Collection(NonParallelCollectionDefinitionClass.Name)]
    public class DocInfo2dTests
    {
        [Fact]
        public void ApiInteropLevelGetTileDimensionsAndCheck()
        {
            // arrange

            // this test is operating on "interop"-level
            var instance = ImgDoc2ApiInterop.Instance;

            // we get the "statistics" before running our test - the statistics contains counters of active objects,
            //  and we check before leaving the test that it is where is was before (usually zero)
            var statisticsBeforeTest = instance.GetStatistics();

            var createOptionsHandle = instance.CreateCreateOptions();
            instance.CreateOptionsSetFilename(createOptionsHandle, ":memory:");
            instance.CreateOptionsAddDimension(createOptionsHandle, new Dimension('A'));
            instance.CreateOptionsAddDimension(createOptionsHandle, new Dimension('l'));
            instance.CreateOptionsAddDimension(createOptionsHandle, new Dimension('Z'));
            var documentHandle = instance.CreateNewDocument(createOptionsHandle);
            instance.DestroyCreateOptions(createOptionsHandle);

            Assert.NotEqual(documentHandle, IntPtr.Zero);

            var reader2dHandle = instance.DocumentGetReader2d(documentHandle);
            Assert.NotEqual(reader2dHandle, IntPtr.Zero);

            // act
            var dimensions = instance.DocInfo2dGetTileDimensions(reader2dHandle);

            instance.DestroyDocument(documentHandle);
            instance.DestroyReader2d(reader2dHandle);

            // assert
            dimensions.Should().BeEquivalentTo(new[] { new Dimension('A'), new Dimension('Z'), new Dimension('l') });

            Assert.True(Utilities.IsActiveObjectCountEqual(statisticsBeforeTest, instance.GetStatistics()), "orphaned native imgdoc2-objects detected");
        }

        [Fact]
        public void GetTileDimensionsAndCheck()
        {
            // we get the "statistics" before running our test - the statistics contains counters of active objects,
            //  and we check before leaving the test that it is where is was before (usually zero)
            var statisticsBeforeTest = ImgDoc2ApiInterop.Instance.GetStatistics();
            {
                // arrange

                using var createOptions = new CreateOptions() { Filename = ":memory:" };
                createOptions.AddDimension(new Dimension('A'));
                createOptions.AddDimension(new Dimension('F'));
                createOptions.AddDimension(new Dimension('g'));
                createOptions.AddDimension(new Dimension('l'));
                createOptions.AddDimension(new Dimension('o'));
                createOptions.AddDimension(new Dimension('q'));
                createOptions.AddDimension(new Dimension('S'));
                using var document = Document.CreateNew(createOptions);
                using var reader2d = document.Get2dReader();

                // act
                var dimensions = reader2d.GetTileDimensions();

                // assert
                dimensions.Should().BeEquivalentTo(
                    new[]
                        {
                            new Dimension('A'),
                            new Dimension('F'),
                            new Dimension('g'),
                            new Dimension('l'),
                            new Dimension('o'),
                            new Dimension('q'),
                            new Dimension('S')
                        });
            }

            Assert.True(Utilities.IsActiveObjectCountEqual(statisticsBeforeTest, ImgDoc2ApiInterop.Instance.GetStatistics()), "orphaned native imgdoc2-objects detected");
        }

        [Fact]
        public void CreateDocument1AndCallGetMinMaxForTileDimensionAndCheck()
        {
            using var createOptions = new CreateOptions() { Filename = ":memory:", UseBlobTable = true };
            createOptions.AddDimension(new Dimension('o'));
            using var document = Document.CreateNew(createOptions);
            using var reader2d = document.Get2dReader();
            using var writer2d = document.Get2dWriter();

            LogicalPosition logicalPosition = new LogicalPosition()
            {
                PositionX = 10,
                PositionY = 20,
                Width = 100,
                Height = 111,
                PyramidLevel = 0
            };

            var tileCoordinate = new TileCoordinate(new[]
                  {Tuple.Create(new Dimension('o'), 11)});

            writer2d.AddTile(
                tileCoordinate,
                in logicalPosition,
                new Tile2dBaseInfo(1, 1, PixelType.Gray8),
                DataType.Zero,
                null);

            tileCoordinate = new TileCoordinate(new[]
                  {Tuple.Create(new Dimension('o'), 141)});

            writer2d.AddTile(
                tileCoordinate,
                in logicalPosition,
                new Tile2dBaseInfo(1, 1, PixelType.Gray8),
                DataType.Zero,
                null);

            var minMax = reader2d.GetMinMaxForTileDimension(new[] { new Dimension('o') });

            minMax.Count.Should().Be(1);
            minMax[new Dimension('o')].Minimum.Should().Be(11);
            minMax[new Dimension('o')].Maximum.Should().Be(141);
        }

        [Fact]
        public void CreateDocument2AndCallGetMinMaxForTileDimensionAndCheck()
        {
            // we get the "statistics" before running our test - the statistics contains counters of active objects,
            //  and we check before leaving the test that it is where is was before (usually zero)
            var statisticsBeforeTest = ImgDoc2ApiInterop.Instance.GetStatistics();
            {
                using var createOptions = new CreateOptions() { Filename = ":memory:", UseBlobTable = true };
                createOptions.AddDimension(new Dimension('v'));
                createOptions.AddDimension(new Dimension('w'));
                using var document = Document.CreateNew(createOptions);
                using var reader2d = document.Get2dReader();
                using var writer2d = document.Get2dWriter();

                LogicalPosition logicalPosition = new LogicalPosition()
                {
                    PositionX = 10,
                    PositionY = 20,
                    Width = 100,
                    Height = 111,
                    PyramidLevel = 0
                };

                var tileCoordinate = new TileCoordinate(new[]
                      {Tuple.Create(new Dimension('v'), -41), Tuple.Create(new Dimension('w'), 11)});

                writer2d.AddTile(
                    tileCoordinate,
                    in logicalPosition,
                    new Tile2dBaseInfo(1, 1, PixelType.Gray8),
                    DataType.Zero,
                    null);

                tileCoordinate = new TileCoordinate(new[]
                      {Tuple.Create(new Dimension('v'), 14), Tuple.Create(new Dimension('w'), 61)});

                writer2d.AddTile(
                    tileCoordinate,
                    in logicalPosition,
                    new Tile2dBaseInfo(1, 1, PixelType.Gray8),
                    DataType.Zero,
                    null);

                var minMax = reader2d.GetMinMaxForTileDimension(reader2d.GetTileDimensions());

                minMax.Count.Should().Be(2);
                minMax[new Dimension('v')].Minimum.Should().Be(-41);
                minMax[new Dimension('v')].Maximum.Should().Be(14);
                minMax[new Dimension('w')].Minimum.Should().Be(11);
                minMax[new Dimension('w')].Maximum.Should().Be(61);
            }

            Assert.True(Utilities.IsActiveObjectCountEqual(statisticsBeforeTest, ImgDoc2ApiInterop.Instance.GetStatistics()), "orphaned native imgdoc2-objects detected");
        }

        [Fact]
        public void CreateEmptyDocument2AndCallGetMinMaxForTileDimensionAndCheckResultForBeingIndeterminate()
        {
            // we get the "statistics" before running our test - the statistics contains counters of active objects,
            //  and we check before leaving the test that it is where is was before (usually zero)
            var statisticsBeforeTest = ImgDoc2ApiInterop.Instance.GetStatistics();
            {
                using var createOptions = new CreateOptions() { Filename = ":memory:", UseBlobTable = true };
                createOptions.AddDimension(new Dimension('v'));
                createOptions.AddDimension(new Dimension('w'));
                using var document = Document.CreateNew(createOptions);
                using var reader2d = document.Get2dReader();

                var minMax = reader2d.GetMinMaxForTileDimension(reader2d.GetTileDimensions());
                minMax.Count.Should().Be(2);

                // the min/max should be "indeterminate" or "invalid", which means that min > max.
                var (min, max) = minMax[new Dimension('v')];
                min.Should().BeGreaterThan(max);
                (min, max) = minMax[new Dimension('w')];
                min.Should().BeGreaterThan(max);
            }

            Assert.True(Utilities.IsActiveObjectCountEqual(statisticsBeforeTest, ImgDoc2ApiInterop.Instance.GetStatistics()), "orphaned native imgdoc2-objects detected");
        }

        [Fact]
        public void CreateDocument1AndCallGetBoundingBoxAndCheck()
        {
            var statisticsBeforeTest = ImgDoc2ApiInterop.Instance.GetStatistics();
            {
                // arrange
                using var createOptions = new CreateOptions() { Filename = ":memory:", UseBlobTable = true };
                createOptions.AddDimension(new Dimension('o'));
                using var document = Document.CreateNew(createOptions);
                using var reader2d = document.Get2dReader();
                using var writer2d = document.Get2dWriter();

                LogicalPosition logicalPosition = new LogicalPosition()
                {
                    PositionX = 10,
                    PositionY = 20,
                    Width = 100,
                    Height = 111,
                    PyramidLevel = 0
                };

                var tileCoordinate = new TileCoordinate(new[]
                    {Tuple.Create(new Dimension('o'), 11)});

                writer2d.AddTile(
                    tileCoordinate,
                    in logicalPosition,
                    new Tile2dBaseInfo(1, 1, PixelType.Gray8),
                    DataType.Zero,
                    null);

                tileCoordinate = new TileCoordinate(new[]
                    {Tuple.Create(new Dimension('o'), 141)});

                writer2d.AddTile(
                    tileCoordinate,
                    in logicalPosition,
                    new Tile2dBaseInfo(1, 1, PixelType.Gray8),
                    DataType.Zero,
                    null);

                // act
                var extent = reader2d.GetBoundingBox();

                // assert
                extent.MinX.Should().Be(10);
                extent.MaxX.Should().Be(110);
                extent.MinY.Should().Be(20);
                extent.MaxY.Should().Be(131);
            }

            Assert.True(Utilities.IsActiveObjectCountEqual(statisticsBeforeTest, ImgDoc2ApiInterop.Instance.GetStatistics()), "orphaned native imgdoc2-objects detected");
        }

        [Fact]
        public void CreateEmptyDocument1AndCallGetBoundingBoxAndCheck()
        {
            var statisticsBeforeTest = ImgDoc2ApiInterop.Instance.GetStatistics();
            {
                // arrange
                using var createOptions = new CreateOptions() { Filename = ":memory:", UseBlobTable = true };
                createOptions.AddDimension(new Dimension('o'));
                using var document = Document.CreateNew(createOptions);
                using var reader2d = document.Get2dReader();

                // act
                var extent = reader2d.GetBoundingBox();

                // assert
                extent.IsValid.Should().BeFalse();
            }

            Assert.True(Utilities.IsActiveObjectCountEqual(statisticsBeforeTest, ImgDoc2ApiInterop.Instance.GetStatistics()), "orphaned native imgdoc2-objects detected");
        }

        [Fact]
        public void CreateDocumentAndCallGetTotalNumberOfTilesAndCheck()
        {
            var statisticsBeforeTest = ImgDoc2ApiInterop.Instance.GetStatistics();
            {
                // arrange
                using var createOptions = new CreateOptions() { Filename = ":memory:", UseBlobTable = true };
                createOptions.AddDimension(new Dimension('o'));
                using var document = Document.CreateNew(createOptions);
                using var reader2d = document.Get2dReader();
                using var writer2d = document.Get2dWriter();

                LogicalPosition logicalPosition = new LogicalPosition()
                {
                    PositionX = 10,
                    PositionY = 20,
                    Width = 100,
                    Height = 111,
                    PyramidLevel = 0
                };

                var tileCoordinate = new TileCoordinate(new[]
                    {Tuple.Create(new Dimension('o'), 11)});

                writer2d.AddTile(
                    tileCoordinate,
                    in logicalPosition,
                    new Tile2dBaseInfo(1, 1, PixelType.Gray8),
                    DataType.Zero,
                    null);

                tileCoordinate = new TileCoordinate(new[]
                    {Tuple.Create(new Dimension('o'), 141)});

                writer2d.AddTile(
                    tileCoordinate,
                    in logicalPosition,
                    new Tile2dBaseInfo(1, 1, PixelType.Gray8),
                    DataType.Zero,
                    null);

                // act
                var totalNumberOfTiles = reader2d.GetTotalNumberOfTiles();

                // assert
                totalNumberOfTiles.Should().Be(2);
            }

            Assert.True(Utilities.IsActiveObjectCountEqual(statisticsBeforeTest, ImgDoc2ApiInterop.Instance.GetStatistics()), "orphaned native imgdoc2-objects detected");
        }

        [Fact]
        public void CreateDocumentAndCallGetTileCountPerPyramidLayerAndCheck()
        {
            var statisticsBeforeTest = ImgDoc2ApiInterop.Instance.GetStatistics();
            {
                // arrange
                using var createOptions = new CreateOptions() { Filename = ":memory:", UseBlobTable = true };
                createOptions.AddDimension(new Dimension('o'));
                using var document = Document.CreateNew(createOptions);
                using var reader2d = document.Get2dReader();
                using var writer2d = document.Get2dWriter();

                // add 3 tiles on pyramid level 0, 2 tiles on pyramid level 1 and 1 tile on pyramid level 2
                LogicalPosition logicalPosition = new LogicalPosition()
                {
                    PositionX = 10,
                    PositionY = 20,
                    Width = 100,
                    Height = 111,
                    PyramidLevel = 0
                };

                var tileCoordinate = new TileCoordinate(new[]
                    {Tuple.Create(new Dimension('o'), 11)});
                writer2d.AddTile(
                    tileCoordinate,
                    in logicalPosition,
                    new Tile2dBaseInfo(1, 1, PixelType.Gray8),
                    DataType.Zero,
                    null);
                tileCoordinate = new TileCoordinate(new[]
                    {Tuple.Create(new Dimension('o'), 12)});
                writer2d.AddTile(
                    tileCoordinate,
                    in logicalPosition,
                    new Tile2dBaseInfo(1, 1, PixelType.Gray8),
                    DataType.Zero,
                    null);
                tileCoordinate = new TileCoordinate(new[]
                    {Tuple.Create(new Dimension('o'), 13)});
                writer2d.AddTile(
                    tileCoordinate,
                    in logicalPosition,
                    new Tile2dBaseInfo(1, 1, PixelType.Gray8),
                    DataType.Zero,
                    null);

                tileCoordinate = new TileCoordinate(new[]
                    {Tuple.Create(new Dimension('o'), 141)});
                logicalPosition.PyramidLevel = 1;
                writer2d.AddTile(
                    tileCoordinate,
                    in logicalPosition,
                    new Tile2dBaseInfo(1, 1, PixelType.Gray8),
                    DataType.Zero,
                    null);
                tileCoordinate = new TileCoordinate(new[]
                    {Tuple.Create(new Dimension('o'), 142)});
                writer2d.AddTile(
                    tileCoordinate,
                    in logicalPosition,
                    new Tile2dBaseInfo(1, 1, PixelType.Gray8),
                    DataType.Zero,
                    null);

                tileCoordinate = new TileCoordinate(new[]
                    {Tuple.Create(new Dimension('o'), 200)});
                logicalPosition.PyramidLevel = 2;
                writer2d.AddTile(
                    tileCoordinate,
                    in logicalPosition,
                    new Tile2dBaseInfo(1, 1, PixelType.Gray8),
                    DataType.Zero,
                    null);

                // act
                var tileCounterPerPyramidLayer = reader2d.GetTileCountPerPyramidLayer();

                // assert
                Assert.Equal(tileCounterPerPyramidLayer, new Dictionary<int, long>() { { 0, 3 }, { 1, 2 }, { 2, 1 } });
            }

            Assert.True(Utilities.IsActiveObjectCountEqual(statisticsBeforeTest, ImgDoc2ApiInterop.Instance.GetStatistics()), "orphaned native imgdoc2-objects detected");
        }
    }
}
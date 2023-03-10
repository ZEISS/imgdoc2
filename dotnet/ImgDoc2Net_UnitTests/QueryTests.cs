// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net_UnitTests
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;
    using FluentAssertions;
    using ImgDoc2Net.Implementation;
    using ImgDoc2Net.Interfaces;

    [Collection(NonParallelCollectionDefinitionClass.Name)]
    public class QueryTests
    {
        [Fact]
        public void CreateDocumentWriteATileReadTileCompareData()
        {
            using var createOptions = new CreateOptions() { Filename = ":memory:", UseBlobTable = true };
            createOptions.AddDimension(new Dimension('A'));
            using var document = Document.CreateNew(createOptions);
            using var reader2d = document.Get2dReader();
            using var writer2d = document.Get2dWriter();

            List<long> indicesForPyramidLevel3 = new List<long>();
            Random rng = new Random();

            for (int i = 0; i < 100; ++i)
            {
                LogicalPosition logicalPosition = new LogicalPosition()
                {
                    PositionX = 0,
                    PositionY = 1,
                    Width = 2,
                    Height = 3,
                    PyramidLevel = rng.Next(0, 20)
                };
                var testData = new byte[] { 8, 4, 3, 2, 85, 32, 9, 4, 1, 58 };
                long pkOfAddedTile = writer2d.AddTile(
                    new TileCoordinate(new[] { Tuple.Create(new Dimension('A'), i) }),
                    in logicalPosition,
                    new Tile2dBaseInfo(1, 2, PixelType.Gray8),
                    DataType.UncompressedBitmap,
                    testData);
                if (logicalPosition.PyramidLevel == 3)
                {
                    indicesForPyramidLevel3.Add(pkOfAddedTile);
                }
            }

            TileInfoQueryClause tileInfoQueryClause = new TileInfoQueryClause();
            tileInfoQueryClause.PyramidLevelConditionsModifiable.Add(
                new QueryClause()
                {
                    LogicalOperator = QueryLogicalOperator.Invalid,
                    ComparisonOperator = QueryComparisonOperator.Equal,
                    Value = 3
                });

            var result = reader2d.Query(null, tileInfoQueryClause);
            // check whether the two lists have the same content, irrespective of order
            indicesForPyramidLevel3.Should().BeEquivalentTo(result);
        }

        [Theory]
        [InlineData(false)]
        [InlineData(true)]
        public void CreateDocumentWriteTilesReadWithSpatialQueryAndCompareData(bool useSpatialIndex)
        {
            using var createOptions = new CreateOptions() { Filename = ":memory:", UseBlobTable = true, UseSpatialIndex = useSpatialIndex };
            createOptions.AddDimension(new Dimension('X'));
            createOptions.AddDimension(new Dimension('Y'));
            using var document = Document.CreateNew(createOptions);
            using var reader2d = document.Get2dReader();
            using var writer2d = document.Get2dWriter();

            // add a 10x10 grid of tiles, each tile 1x1 pixels
            for (int x = 0; x < 10; ++x)
            {
                for (int y = 0; y < 10; ++y)
                {

                    LogicalPosition logicalPosition = new LogicalPosition()
                    {
                        PositionX = x,
                        PositionY = y,
                        Width = 1,
                        Height = 1,
                        PyramidLevel = 0
                    };

                    // we put the "x and y"-index into the pixel data here
                    var testData = new byte[] { (byte)x, (byte)y };

                    long pkOfAddedTile = writer2d.AddTile(
                    new TileCoordinate(new[] { Tuple.Create(new Dimension('X'), x), Tuple.Create(new Dimension('Y'), y) }),
                    in logicalPosition,
                    new Tile2dBaseInfo(1, 1, PixelType.Gray8),
                    DataType.UncompressedBitmap,
                    testData);
                }
            }

            // we expect to find 9 tiles
            var list = reader2d.QueryTilesIntersectingRect(new Rectangle { X = 0, Y = 0, Width = 2.1, Height = 2.1 }, null, null);
            list.Should().NotBeEmpty().And.HaveCount(9);

            // TODO: read "tile info" when API is available
            List<byte[]> expected = new List<byte[]>
            {
                new byte[] { 0, 0 }, new byte[] { 1, 0 }, new byte[] { 2, 0 },
                new byte[] { 0, 1 }, new byte[] { 1, 1 }, new byte[] { 2, 1 },
                new byte[] { 0, 2 }, new byte[] { 1, 2 }, new byte[] { 2, 2 }
            };

            List<byte[]> result = new List<byte[]>();
            foreach (var id in list)
            {
                result.Add(reader2d.ReadTileData(id));
            }

            result.Should().BeEquivalentTo(expected);
        }

        [Theory]
        [InlineData(false)]
        [InlineData(true)]
        public void CreateDocumentWriteTilesReadWithSpatialQueryAndTileInfoClauseAndCompareData(bool useSpatialIndex)
        {
            using var createOptions = new CreateOptions() { Filename = ":memory:", UseBlobTable = true, UseSpatialIndex = useSpatialIndex };
            createOptions.AddDimension(new Dimension('X'));
            createOptions.AddDimension(new Dimension('Y'));
            using var document = Document.CreateNew(createOptions);
            using var reader2d = document.Get2dReader();
            using var writer2d = document.Get2dWriter();

            // add a 10x10 grid of tiles, each tile 1x1 pixels
            for (int x = 0; x < 10; ++x)
            {
                for (int y = 0; y < 10; ++y)
                {
                    LogicalPosition logicalPosition = new LogicalPosition()
                    {
                        PositionX = x,
                        PositionY = y,
                        Width = 1,
                        Height = 1,
                        PyramidLevel = y % 2 == 0 ? 0 : 1   // every second tile is "level 1"
                    };

                    // we put the "x and y"-index into the pixel data here
                    var testData = new byte[] { (byte)x, (byte)y };

                    long pkOfAddedTile = writer2d.AddTile(
                    new TileCoordinate(new[] { Tuple.Create(new Dimension('X'), x), Tuple.Create(new Dimension('Y'), y) }),
                    in logicalPosition,
                    new Tile2dBaseInfo(1, 1, PixelType.Gray8),
                    DataType.UncompressedBitmap,
                    testData);
                }
            }

            TileInfoQueryClause tileInfoQueryClause = new TileInfoQueryClause();
            tileInfoQueryClause.PyramidLevelConditionsModifiable.Add(
                new QueryClause()
                {
                    LogicalOperator = QueryLogicalOperator.Invalid,
                    ComparisonOperator = QueryComparisonOperator.Equal,
                    Value = 0
                });

            // we expect to find 6 tiles (tiles with y=1 have a pyramid-level=1, so they are not returned here by the query
            var list = reader2d.QueryTilesIntersectingRect(new Rectangle { X = 0, Y = 0, Width = 2.1, Height = 2.1 }, null, tileInfoQueryClause);
            list.Should().NotBeEmpty().And.HaveCount(6);

            // TODO: read "tile info" when API is available
            List<byte[]> expected = new List<byte[]>
            {
                new byte[] { 0, 0 }, new byte[] { 1, 0 }, new byte[] { 2, 0 },
                new byte[] { 0, 2 }, new byte[] { 1, 2 }, new byte[] { 2, 2 }
            };

            List<byte[]> result = new List<byte[]>();
            foreach (var id in list)
            {
                result.Add(reader2d.ReadTileData(id));
            }

            result.Should().BeEquivalentTo(expected);

            // and now we query for "pyramid-level=0", expecting to find 3 tiles then
            tileInfoQueryClause = new TileInfoQueryClause();
            tileInfoQueryClause.PyramidLevelConditionsModifiable.Add(
                new QueryClause()
                {
                    LogicalOperator = QueryLogicalOperator.Invalid,
                    ComparisonOperator = QueryComparisonOperator.Equal,
                    Value = 1
                });

            list = reader2d.QueryTilesIntersectingRect(new Rectangle { X = 0, Y = 0, Width = 2.1, Height = 2.1 }, null, tileInfoQueryClause);
            list.Should().NotBeEmpty().And.HaveCount(3);

            expected = new List<byte[]>
            {
                new byte[] { 0, 1 }, new byte[] { 1, 1 }, new byte[] { 2, 1 }
            };

            result = new List<byte[]>();
            foreach (var id in list)
            {
                result.Add(reader2d.ReadTileData(id));
            }

            result.Should().BeEquivalentTo(expected);
        }

        [Theory]
        [InlineData(false)]
        [InlineData(true)]
        public void CreateDocumentWriteTilesReadWithSpatialQueryAndCheckResult(bool useSpatialIndex)
        {
            using var createOptions = new CreateOptions() { Filename = ":memory:", UseBlobTable = true, UseSpatialIndex = useSpatialIndex };
            createOptions.AddDimension(new Dimension('X'));
            createOptions.AddDimension(new Dimension('Y'));
            using var document = Document.CreateNew(createOptions);
            using var reader2d = document.Get2dReader();
            using var writer2d = document.Get2dWriter();

            // add a 10x10 grid of tiles, each tile 1x1 pixels
            for (int x = 0; x < 10; ++x)
            {
                for (int y = 0; y < 10; ++y)
                {

                    LogicalPosition logicalPosition = new LogicalPosition()
                    {
                        PositionX = x,
                        PositionY = y,
                        Width = 1,
                        Height = 1,
                        PyramidLevel = 0
                    };

                    // we put the "x and y"-index into the pixel data here
                    var testData = new byte[] { (byte)x, (byte)y };

                    long pkOfAddedTile = writer2d.AddTile(
                    new TileCoordinate(new[] { Tuple.Create(new Dimension('X'), x), Tuple.Create(new Dimension('Y'), y) }),
                    in logicalPosition,
                    new Tile2dBaseInfo(1, 1, PixelType.Gray8),
                    DataType.UncompressedBitmap,
                    testData);
                }
            }

            // we expect to find 6 tiles (tiles with y=1 have a pyramid-level=1, so they are not returned here by the query
            var list = reader2d.QueryTilesIntersectingRect(new Rectangle { X = 0, Y = 0, Width = 10, Height = 10 }, null, null);
            list.Should().NotBeEmpty().And.HaveCount(100);
        }

        [Theory]
        [InlineData(false)]
        [InlineData(true)]
        public void CreateDocumentWriteTilesReadWithSpatialQueryAndDimensionClauseAndTileInfoClauseAndCheckResult(bool useSpatialIndex)
        {
            using var createOptions = new CreateOptions() { Filename = ":memory:", UseBlobTable = true, UseSpatialIndex = useSpatialIndex };
            createOptions.AddDimension(new Dimension('X'));
            createOptions.AddDimension(new Dimension('Y'));
            using var document = Document.CreateNew(createOptions);
            using var reader2d = document.Get2dReader();
            using var writer2d = document.Get2dWriter();

            // add a 10x10 grid of tiles, each tile 1x1 pixels
            for (int x = 0; x < 10; ++x)
            {
                for (int y = 0; y < 10; ++y)
                {

                    LogicalPosition logicalPosition = new LogicalPosition()
                    {
                        PositionX = x,
                        PositionY = y,
                        Width = 1,
                        Height = 1,
                        PyramidLevel = 0
                    };

                    // we put the "x and y"-index into the pixel data here
                    var testData = new byte[] { (byte)x, (byte)y };

                    long pkOfAddedTile = writer2d.AddTile(
                    new TileCoordinate(new[] { Tuple.Create(new Dimension('X'), x), Tuple.Create(new Dimension('Y'), y) }),
                    in logicalPosition,
                    new Tile2dBaseInfo(1, 1, PixelType.Gray8),
                    DataType.UncompressedBitmap,
                    testData);
                }
            }

            TileInfoQueryClause tileInfoQueryClause = new TileInfoQueryClause();
            tileInfoQueryClause.PyramidLevelConditionsModifiable.Add(
                new QueryClause()
                {
                    LogicalOperator = QueryLogicalOperator.Invalid,
                    ComparisonOperator = QueryComparisonOperator.Equal,
                    Value = 0
                });

            var dimensionQueryClause = new DimensionQueryClause();

            // TODO(Jbl): look into... whether RangeStart/RangeEnd should be inclusive or non-inclusive or what
            dimensionQueryClause.AddCondition(new DimensionCondition() { Dimension = new Dimension('X'), RangeStart = 7, RangeEnd = 9 });
            dimensionQueryClause.AddCondition(new DimensionCondition() { Dimension = new Dimension('Y'), RangeStart = 7, RangeEnd = 9 });

            // we expect to find 6 tiles (tiles with y=1 have a pyramid-level=1, so they are not returned here by the query
            var list = reader2d.QueryTilesIntersectingRect(new Rectangle { X = 0, Y = 0, Width = 10, Height = 10 }, dimensionQueryClause, tileInfoQueryClause);
            list.Should().NotBeEmpty().And.HaveCount(1);
        }

        [Fact]
        public void CreateDocumentQueryItAndCheckQueryOptions()
        {
            using var createOptions = new CreateOptions() { Filename = ":memory:", UseBlobTable = true };
            createOptions.AddDimension(new Dimension('X'));
            createOptions.AddDimension(new Dimension('Y'));
            using var document = Document.CreateNew(createOptions);
            using var reader2d = document.Get2dReader();
            using var writer2d = document.Get2dWriter();

            // add a 10x10 grid of tiles, each tile 1x1 pixels
            for (int x = 0; x < 10; ++x)
            {
                for (int y = 0; y < 10; ++y)
                {

                    LogicalPosition logicalPosition = new LogicalPosition()
                    {
                        PositionX = x,
                        PositionY = y,
                        Width = 1,
                        Height = 1,
                        PyramidLevel = 0
                    };

                    // we put the "x and y"-index into the pixel data here
                    var testData = new byte[] { (byte)x, (byte)y };

                    long pkOfAddedTile = writer2d.AddTile(
                    new TileCoordinate(new[] { Tuple.Create(new Dimension('X'), x), Tuple.Create(new Dimension('Y'), y) }),
                    in logicalPosition,
                    new Tile2dBaseInfo(1, 1, PixelType.Gray8),
                    DataType.UncompressedBitmap,
                    testData);
                }
            }

            QueryOptions queryOptions = new QueryOptions() { MaxNumbersOfResults = 10 };
            var list = reader2d.QueryTilesIntersectingRect(new Rectangle { X = 0, Y = 0, Width = 10, Height = 10 }, null, null, queryOptions);
            queryOptions.ResultWasComplete.Should().BeFalse();
            list.Should().NotBeEmpty().And.HaveCount(10);

            queryOptions = new QueryOptions() { MaxNumbersOfResults = 100 };
            list = reader2d.QueryTilesIntersectingRect(new Rectangle { X = 0, Y = 0, Width = 10, Height = 10 }, null, null, queryOptions);
            queryOptions.ResultWasComplete.Should().BeTrue();
            list.Should().NotBeEmpty().And.HaveCount(100);
        }
    }
}

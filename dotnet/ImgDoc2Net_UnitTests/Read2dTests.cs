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
    public class Read2dTests
    {
        [Fact]
        public void ReadTileDataWithInvalidKeyAndExpectError()
        {
            using var createOptions = new CreateOptions() { Filename = ":memory:", UseBlobTable = true };
            createOptions.AddDimension(new Dimension('A'));
            using var document = Document.CreateNew(createOptions);
            using var reader2d = document.Get2dReader();

            Assert.Throws<ImgDoc2Exception>(() => reader2d.ReadTileData(12345));
        }

        [Fact]
        public void CreateDocumentAddSingleTileReadTileInfoCheckForCorrectness()
        {
            using var createOptions = new CreateOptions() { Filename = ":memory:", UseBlobTable = true };
            createOptions.AddDimension(new Dimension('X'));
            createOptions.AddDimension(new Dimension('Y'));
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

            var testData = new byte[] { 1, 2, 3 };

            var tileCoordinate = new TileCoordinate(new[]
                {Tuple.Create(new Dimension('X'), 11), Tuple.Create(new Dimension('Y'), 12)});

            long pkOfAddedTile = writer2d.AddTile(
                tileCoordinate,
                in logicalPosition,
                new Tile2dBaseInfo(1, 1, PixelType.Gray8),
                DataType.UncompressedBitmap,
                testData);

            // TODO: Query without any clause will malfunction, that why we put together a query-clause here, but this
            //        should be fixed I guess
            TileInfoQueryClause tileInfoQueryClause = new TileInfoQueryClause();
            tileInfoQueryClause.PyramidLevelConditionsModifiable.Add(
                new QueryClause()
                {
                    LogicalOperator = QueryLogicalOperator.Invalid,
                    ComparisonOperator = QueryComparisonOperator.Equal,
                    Value = 0
                });

            var keys = reader2d.Query(null, tileInfoQueryClause, null);
            keys.Should().HaveCount(1);

            var tileInfo = reader2d.ReadTileInfo(keys[0]);
            tileInfo.logicalPosition.Should().BeEquivalentTo(logicalPosition);
            tileInfo.coordinate.Should().BeEquivalentTo(tileCoordinate);
        }

        [Fact]
        public void CreateEmptyDocumentAndCallTileReadTileInfoForNonExistentTileExpectException()
        {
            using var createOptions = new CreateOptions() { Filename = ":memory:", UseBlobTable = true };
            createOptions.AddDimension(new Dimension('a'));
            createOptions.AddDimension(new Dimension('b'));
            using var document = Document.CreateNew(createOptions);
            using var reader2d = document.Get2dReader();

            Assert.Throws<ImgDoc2Exception>(() => reader2d.ReadTileInfo(1234));
            Assert.Throws<ImgDoc2Exception>(() => reader2d.ReadTileCoordinate(1234));
            Assert.Throws<ImgDoc2Exception>(() => reader2d.ReadTileLogicalPosition(1234));
        }
    }
}

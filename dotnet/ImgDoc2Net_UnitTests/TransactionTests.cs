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
    public class TransactionTests
    {
        [Fact]
        public void AddTilesInATransactionThenRollbackAndCheckResult()
        {
            var statisticsBeforeTest = ImgDoc2ApiInterop.Instance.GetStatistics();
            {
                // we start a transaction, add some tiles, then while transaction is still open, we check the number of tiles in the document - 
                // where we should find the tiles we just added. Then we rollback the transaction and check again - where we then should find 
                // the initial state, i.e. no tiles in the document.

                // arrange
                using var createOptions = new CreateOptions() { Filename = ":memory:", UseBlobTable = true };
                createOptions.AddDimension(new Dimension('A'));
                using var document = Document.CreateNew(createOptions);
                using var reader2d = document.Get2dReader();
                using var writer2d = document.Get2dWriter();

                writer2d.BeginTransaction();

                for (int i = 0; i < 5; ++i)
                {
                    var logicalPosition = new LogicalPosition()
                    {
                        PositionX = 0,
                        PositionY = 1,
                        Width = 2,
                        Height = 3,
                        PyramidLevel = 0
                    };
                    var testData = new byte[] { 8, 4, 3, 2, 85, 32, 9, 4, 1, 58 };
                    writer2d.AddTile(
                        new TileCoordinate(new[] { Tuple.Create(new Dimension('A'), i) }),
                        in logicalPosition,
                        new Tile2dBaseInfo(1, 2, PixelType.Gray8),
                        DataType.UncompressedBitmap,
                        testData);
                }

                long totalNumberOfTiles = reader2d.GetTotalNumberOfTiles();
                totalNumberOfTiles.Should().Be(5);

                // act
                writer2d.RollbackTransaction();

                // assert
                totalNumberOfTiles = reader2d.GetTotalNumberOfTiles();
                totalNumberOfTiles.Should().Be(0);
            }

            Assert.True(Utilities.IsActiveObjectCountEqual(statisticsBeforeTest, ImgDoc2ApiInterop.Instance.GetStatistics()), "orphaned native imgdoc2-objects detected");
        }

        [Fact]
        public void Document2dCheckTransactionSemantic()
        {
            using var createOptions = new CreateOptions() { Filename = ":memory:", UseBlobTable = true };
            createOptions.AddDimension(new Dimension('A'));
            using var document = Document.CreateNew(createOptions);
            using var writer2d = document.Get2dWriter();

            // trying to end a transaction without starting one should throw
            Assert.Throws<ImgDoc2Exception>(() => writer2d.CommitTransaction());

            // trying to rollback a transaction without starting one should throw as well
            Assert.Throws<ImgDoc2Exception>(() => writer2d.RollbackTransaction());

            writer2d.BeginTransaction();

            // trying to start a transaction while another one is already active should throw
            Assert.Throws<ImgDoc2Exception>(() => writer2d.BeginTransaction());
            writer2d.CommitTransaction();
        }

        [Fact]
        public void Document3dCheckTransactionSemantic()
        {
            using var createOptions = new CreateOptions() { Filename = ":memory:", UseBlobTable = true, DocumentType = DocumentType.Image3d};
            createOptions.AddDimension(new Dimension('A'));
            using var document = Document.CreateNew(createOptions);
            using var writer3d = document.Get3dWriter();

            // trying to end a transaction without starting one should throw
            Assert.Throws<ImgDoc2Exception>(() => writer3d.CommitTransaction());

            // trying to rollback a transaction without starting one should throw as well
            Assert.Throws<ImgDoc2Exception>(() => writer3d.RollbackTransaction());

            writer3d.BeginTransaction();

            // trying to start a transaction while another one is already active should throw
            Assert.Throws<ImgDoc2Exception>(() => writer3d.BeginTransaction());
            writer3d.CommitTransaction();
        }
    }
}

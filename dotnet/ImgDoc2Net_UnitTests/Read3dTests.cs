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
    public class Read3dTests
    {
        [Fact]
        public void ReadTileDataWithInvalidKeyAndExpectError()
        {
            // we get the "statistics" before running our test - the statistics contains counters of active objects,
            //  and we check before leaving the test that it is where is was before (usually zero)
            var statisticsBeforeTest = ImgDoc2ApiInterop.Instance.GetStatistics();
            {
                using var createOptions = new CreateOptions() { Filename = ":memory:", UseBlobTable = true, DocumentType = DocumentType.Image3d };
                createOptions.AddDimension(new Dimension('A'));
                using var document = Document.CreateNew(createOptions);
                using var reader3d = document.Get3dReader();

                Assert.Throws<ImgDoc2Exception>(() => reader3d.ReadBrickData(12345));
            }

            Assert.True(Utilities.IsActiveObjectCountEqual(statisticsBeforeTest, ImgDoc2ApiInterop.Instance.GetStatistics()), "orphaned native imgdoc2-objects detected");
        }

        [Fact]
        public void CreateDocumentAddSingleBrickReadBrickInfoCheckForCorrectness()
        {
            // we get the "statistics" before running our test - the statistics contains counters of active objects,
            //  and we check before leaving the test that it is where is was before (usually zero)
            var statisticsBeforeTest = ImgDoc2ApiInterop.Instance.GetStatistics();
            {
                using var createOptions = new CreateOptions() { Filename = ":memory:", UseBlobTable = true, DocumentType = DocumentType.Image3d };
                createOptions.AddDimension(new Dimension('X'));
                createOptions.AddDimension(new Dimension('Y'));
                using var document = Document.CreateNew(createOptions);
                using var reader3d = document.Get3dReader();
                using var writer3d = document.Get3dWriter();

                LogicalPosition3d logicalPosition3d = new LogicalPosition3d()
                {
                    PositionX = 10,
                    PositionY = 20,
                    PositionZ = 30,
                    Width = 100,
                    Height = 111,
                    Depth = 112,
                    PyramidLevel = 0
                };

                var testData = new byte[] { 1, 2, 3 };

                var tileCoordinate = new TileCoordinate(new[]
                    {Tuple.Create(new Dimension('X'), 11), Tuple.Create(new Dimension('Y'), 12)});

                long pkOfAddedTile = writer3d.AddBrick(
                    tileCoordinate,
                    in logicalPosition3d,
                    new Brick3dBaseInfo(1, 1, 1, PixelType.Gray8),
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

                var keys = reader3d.Query(null, tileInfoQueryClause, null);
                keys.Should().HaveCount(1);

                var brickInfo = reader3d.ReadBrickInfo(keys[0]);
                brickInfo.logicalPosition.Should().BeEquivalentTo(logicalPosition3d);
                brickInfo.coordinate.Should().BeEquivalentTo(tileCoordinate);
            }

            Assert.True(Utilities.IsActiveObjectCountEqual(statisticsBeforeTest, ImgDoc2ApiInterop.Instance.GetStatistics()), "orphaned native imgdoc2-objects detected");
        }

        [Fact]
        public void CreateEmptyDocumentAndCallTileReadTileInfoForNonExistentTileExpectException()
        {
            // we get the "statistics" before running our test - the statistics contains counters of active objects,
            //  and we check before leaving the test that it is where is was before (usually zero)
            var statisticsBeforeTest = ImgDoc2ApiInterop.Instance.GetStatistics();
            {
                using var createOptions = new CreateOptions() { Filename = ":memory:", UseBlobTable = true, DocumentType = DocumentType.Image3d };

                createOptions.AddDimension(new Dimension('a'));
                createOptions.AddDimension(new Dimension('b'));
                using var document = Document.CreateNew(createOptions);
                using var reader3d = document.Get3dReader();

                Assert.Throws<ImgDoc2Exception>(() => reader3d.ReadBrickInfo(1234));
                Assert.Throws<ImgDoc2Exception>(() => reader3d.ReadBrickCoordinate(1234));
                Assert.Throws<ImgDoc2Exception>(() => reader3d.ReadBrickLogicalPosition(1234));
            }

            Assert.True(Utilities.IsActiveObjectCountEqual(statisticsBeforeTest, ImgDoc2ApiInterop.Instance.GetStatistics()), "orphaned native imgdoc2-objects detected");
        }
    }
}

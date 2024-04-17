// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace Imgdoc2cmd
{
    using SixLabors.Fonts;
    using SixLabors.ImageSharp;
    using SixLabors.ImageSharp.Drawing.Processing;
    using SixLabors.ImageSharp.Formats;
    using SixLabors.ImageSharp.PixelFormats;
    using SixLabors.ImageSharp.Processing;
    using ImgDoc2Net.Implementation;
    using ImgDoc2Net.Interfaces;

    internal partial class CommandCreateSyntheticDocument : ICommand
    {
        /// <summary> 
        /// Implementation of the command "CreateSyntheticDocument".
        /// The purpose of this is to create a new document with some synthetic content.
        /// </summary>
        public void Execute(Options options)
        {
            var dimensionBounds = Utilities.ParseStringRepresentationOfDimensionAndRange(options.CoordinateBounds);
            var tileSize = options.TileSize;

            using var createOptions = new CreateOptions();

            createOptions.Filename = options.OutputFilename;
            createOptions.UseSpatialIndex = false;
            createOptions.UseBlobTable = true;
            foreach (var element in dimensionBounds)
            {
                createOptions.AddDimension(element.Dimension);
            }

            using var document = Document.CreateNew(createOptions);
            using var writer = document.Get2dWriter();

            var rowsColumns = options.TilesRowsColumnsCount;
            float widthToSubtract = 0, heightToSubtract = 0;
            if (options.TilesOverlapUnit == Options.OverlapUnit.Pixel)
            {
                widthToSubtract = heightToSubtract = options.TilesOverlap;
            }
            else if (options.TilesOverlapUnit == Options.OverlapUnit.Percentage)
            {
                widthToSubtract = options.TilesOverlap / 100 * tileSize.tileWidth;
                heightToSubtract = options.TilesOverlap / 100 * tileSize.tileHeight;
            }

            foreach (var tileCoordinate in Utilities.EnumerateCoordinatesInBounds(dimensionBounds))
            {
                for (int y = 0; y < rowsColumns.tilesColumnCount; ++y)
                {
                    for (int x = 0; x < rowsColumns.tilesRowCount; ++x)
                    {
                        LogicalPosition logicalPosition = new LogicalPosition()
                        {
                            PositionX = (tileSize.tileWidth - widthToSubtract) * x,
                            PositionY = (tileSize.tileHeight - heightToSubtract) * y,
                            Width = tileSize.tileWidth,
                            Height = tileSize.tileHeight,
                            PyramidLevel = 0
                        };

                        Tile2dBaseInfo tile2dBaseInfo = new Tile2dBaseInfo(tileSize.tileWidth, tileSize.tileHeight, PixelType.Bgr24);
                        byte[] tileData = CreateTile(tileCoordinate, in logicalPosition, 1024, 1024);
                        long pk = writer.AddTile(tileCoordinate, logicalPosition, tile2dBaseInfo, DataType.UncompressedBitmap, tileData);
                    }
                }
            }
        }
    }

    internal partial class CommandCreateSyntheticDocument
    {
        private byte[] CreateTile(ITileCoordinate tileCoordinate, in LogicalPosition logicalPosition, int width, int height)
        {
            byte[] bitmapBuffer = new byte[width * height * 3];
            Image image = Image.WrapMemory<Bgr24>(new Memory<byte>(bitmapBuffer), 1024, 1024);
            Font font = SystemFonts.CreateFont("Arial", 10); // for scaling water mark size is largely ignored.

            string text =
                $"TileCoordinate: {Utilities.TileCoordinateToStringRepresentation(tileCoordinate)}\nLogicalPosition: {Utilities.LogicalPositionToStringRepresentation(in logicalPosition)}";

            // Measure the text size
            FontRectangle size = TextMeasurer.MeasureSize(text, new TextOptions(font));

            // Find out how much we need to scale the text to fill the space (up or down)
            float scalingFactor = Math.Min(width / size.Width, height / size.Height);

            // Create a new font
            Font scaledFont = new Font(font, scalingFactor * font.Size);

            var textOptions = new RichTextOptions(scaledFont)
            {
                Origin = new PointF(width / 2.0f, height / 2.0f),
                HorizontalAlignment = HorizontalAlignment.Center,
                VerticalAlignment = VerticalAlignment.Center
            };

            image.Mutate(
                ctx =>
                {
                    ctx.DrawText(textOptions, text, Color.Red);
                });

            return bitmapBuffer;
        }
    }
}

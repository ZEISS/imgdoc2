// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace Imgdoc2cmd
{
    using ImgDoc2Net.Implementation;
    using ImgDoc2Net.Interfaces;
    using System;
    using System.Collections.Generic;
    using System.IO;
    using SixLabors.ImageSharp;
    using SixLabors.ImageSharp.PixelFormats;

    internal class CommandQueryForTilesAndSaveTiles : CommandQueryForTilesBase
    {
        protected override void ProcessTiles(List<long> tiles, Options options)
        {
            int bitmapNumber = 0;
            foreach (var pk in tiles)
            {
                var tileInfo = this.CommandHelper.GetRead2d().ReadTileInfo(pk);
                string outputFilename = Path.Combine(options.DestinationFolder, $"tile_{bitmapNumber}.png");
                this.ProcessTile(pk, outputFilename, tileInfo);
                ++bitmapNumber;
            }
        }

        private void ProcessTile(long pk, string outputFilename, (ITileCoordinate coordinate, LogicalPosition logicalPosition, TileBlobInfo tileBlobInfo) tileInfo)
        {
            using (var image = ReadTileAndDecodeImageSharp(pk, tileInfo.tileBlobInfo))
            {
                image.SaveAsPng(outputFilename);
            }
        }

        private Image ReadTileAndDecodeImageSharp(long id, TileBlobInfo tileBlobInfo)
        {
            var tileData = this.CommandHelper.GetRead2d().ReadTileData(id);

            switch (tileBlobInfo.DataType)
            {
                case DataType.JpgXrCompressedBitmap:
                case DataType.Zstd0CcompressedBitmap:
                case DataType.Zstd1CcompressedBitmap:
                    {
                        // TODO(JBL): ImageSharp seems to require minimal stride, so we'd need to pass the desired stride here in
                        BitmapData bitmapData = ImgDoc2Global.GetDecoder().Decode(
                            tileData,
                            tileBlobInfo.DataType,
                            tileBlobInfo.Tile2dBaseInfo.PixelType, tileBlobInfo.Tile2dBaseInfo.PixelWidth,
                            tileBlobInfo.Tile2dBaseInfo.PixelHeight);
                        return CreateImageImageSharp(tileBlobInfo.Tile2dBaseInfo.PixelType, bitmapData.Data, tileBlobInfo.Tile2dBaseInfo.PixelWidth, tileBlobInfo.Tile2dBaseInfo.PixelHeight);
                    }
                case DataType.UncompressedBitmap:
                    {
                        return CreateImageImageSharp(tileBlobInfo.Tile2dBaseInfo.PixelType, tileData, tileBlobInfo.Tile2dBaseInfo.PixelWidth, tileBlobInfo.Tile2dBaseInfo.PixelHeight);
                    }
                default:
                    throw new InvalidOperationException("Could not process tile.");
            }
        }

        private static Image CreateImageImageSharp(PixelType pixelType, Memory<byte> data,int width,int height)
        {
            switch (pixelType)
            {
                case PixelType.Gray8:
                    return Image.WrapMemory<L8>(data, width, height);
                case PixelType.Gray16:
                    return Image.WrapMemory<L16>(data, width, height);
                case PixelType.Bgr24:
                    return Image.WrapMemory<Bgr24>(data, width, height);
                case PixelType.Bgr48:
                    // TODO(JBL): here we have the old problem of having to convert from Bgr48 to Rgb48
                    return Image.WrapMemory<Rgb48>(data, width, height);
                default:
                    throw new ArgumentException("Unknown pixelType");
            }
        }
    }
}

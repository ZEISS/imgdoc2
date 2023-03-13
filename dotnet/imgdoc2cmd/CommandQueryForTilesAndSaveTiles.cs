// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace Imgdoc2cmd
{
    using Imgdoc2cmd;
    using ImgDoc2Net;
    using ImgDoc2Net.Implementation;
    using ImgDoc2Net.Interfaces;
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;
    using System.Windows.Media;
    using System.Windows.Media.Imaging;

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
            var bitmapSource = ReadTileAndDecode(pk, tileInfo.tileBlobInfo);
            using (FileStream stream = new FileStream(outputFilename, FileMode.Create))
            {
                PngBitmapEncoder encoder = new PngBitmapEncoder();

                // Note/TODO: if we write instead "encoder.Frames.Add(BitmapFrame.Create(bitmapSource)" this will not work
                //             and I have no idea what is wrong
                encoder.Frames.Add(BitmapFrame.Create(bitmapSource, null, null, null));
                encoder.Save(stream);
            }
        }

        private BitmapSource ReadTileAndDecode(long id, TileBlobInfo tileBlobInfo)
        {
            var tileData = this.CommandHelper.GetRead2d().ReadTileData(id);

            switch (tileBlobInfo.DataType)
            {
                case DataType.JpgXrCompressedBitmap:
                    {
                        using Stream stream = new MemoryStream(tileData);
                        WmpBitmapDecoder decoder = new WmpBitmapDecoder(stream, BitmapCreateOptions.PreservePixelFormat, BitmapCacheOption.OnLoad);
                        if (decoder.Frames.Count < 1)
                        {
                            throw new InvalidOperationException("Could not decode JPGXR-encoded image.");
                        }

                        BitmapSource bitmapSource = decoder.Frames[0];
                        bitmapSource.Freeze();
                        return bitmapSource;
                    }
                case DataType.UncompressedBitmap:
                    {
                        BitmapSource bitmapSource = BitmapSource.Create(
                            tileBlobInfo.Tile2dBaseInfo.PixelWidth,
                            tileBlobInfo.Tile2dBaseInfo.PixelHeight,
                            96,
                            96,
                            ImgDoc2PixelTypeToWpfPixelFormats(tileBlobInfo.Tile2dBaseInfo.PixelType),
                            null,
                            tileData,
                            Utilities.GetBytesPerPel(tileBlobInfo.Tile2dBaseInfo.PixelType) * tileBlobInfo.Tile2dBaseInfo.PixelWidth);

                        return bitmapSource;
                    }
                default:
                    throw new InvalidOperationException("Could not process tile.");
            }
        }

        private static PixelFormat ImgDoc2PixelTypeToWpfPixelFormats(PixelType pixelType)
        {
            switch (pixelType)
            {
                case PixelType.Gray8:
                    return PixelFormats.Gray8;
                case PixelType.Gray16:
                    return PixelFormats.Gray16;
                case PixelType.Bgr24:
                    return PixelFormats.Bgr24;
                case PixelType.Bgr48:
                    return PixelFormats.Rgb48;
                default:
                    throw new ArgumentException("Unknown pixelType");
            }
        }
    }
}

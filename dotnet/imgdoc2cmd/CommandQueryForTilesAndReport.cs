// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace Imgdoc2cmd
{
    using ImgDoc2Net.Interfaces;
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;
    using ImgDoc2Net.Implementation;

    internal class CommandQueryForTilesAndReport : CommandQueryForTilesBase
    {
        protected override void ProcessTiles(List<long> tiles, Options options)
        {
            foreach (var pk in tiles)
            {
                var tileInfo = this.CommandHelper.GetRead2d().ReadTileInfo(pk);
                this.ProcessTile(pk, tileInfo);
            }
        }

        private void ProcessTile(long pk, (ITileCoordinate coordinate, LogicalPosition logicalPosition, TileBlobInfo tileBlobInfo) tileInfo)
        {
            StringBuilder stringBuilder = new StringBuilder();

            string indent = "  ";
            stringBuilder.AppendFormat($"Tile PK={pk}{Environment.NewLine}");

            stringBuilder.AppendFormat($"{indent}Coordinate: {Utilities.TileCoordinateToStringRepresentation(tileInfo.coordinate)}{Environment.NewLine}");
            stringBuilder.AppendFormat($"{indent}Logical Position: {Utilities.LogicalPositionToStringRepresentation(in tileInfo.logicalPosition)}{Environment.NewLine}");
            stringBuilder.AppendFormat($"{indent}Tile Blob Info: {Utilities.TileBlobInfoToStringRepresentation(in tileInfo.tileBlobInfo)}{Environment.NewLine}");

            Console.WriteLine(stringBuilder.ToString());
        }
    }
}

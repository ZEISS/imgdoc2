// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Implementation
{
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Text;
    using System.Text.RegularExpressions;
    using ImgDoc2Net.Interfaces;

    /// <summary> 
    /// A bunch of utility functions are gathered here.
    /// </summary>
    public static class Utilities
    {
        public static int GetBytesPerPel(PixelType pixelType)
        {
            switch (pixelType)
            {
                case PixelType.Gray8:
                    return 1;
                case PixelType.Gray16:
                    return 2;
                case PixelType.Bgr24:
                    return 3;
                case PixelType.Bgr48:
                    return 6;
                default:
                    throw new ArgumentException($"Invalid pixelType={pixelType}");
            }
        }

        /// <summary>   
        /// Creates a dimension-query-clause from a string representation. The syntax is:
        /// "A3,5B-3,1m9,10" - that is: a character identfying the dimension, followed by two
        /// comma separated integers.
        /// </summary>
        /// <exception cref="ArgumentException">    
        /// Thrown when one or more arguments have unsupported or illegal values. 
        /// </exception>
        /// <param name="s">    The string to be parsed. </param>
        /// <returns>   The dimension-query-clause created from the string representation. </returns>
        public static DimensionQueryClause CreateFromStringRepresentation(string s)
        {
            var regexForWholeString = new Regex("^(?:[a-zA-Z][+-]?[0-9]{1,10},[+-]?[0-9]{1,10})*$");
            var matchForWholeString = regexForWholeString.Match(s);
            if (!matchForWholeString.Success)
            {
                throw new ArgumentException("The string is not in a valid format.");
            }

            var dimensionQueryClause = new DimensionQueryClause();

            // Ok, so now that we are sure that the string is in the correct format, then match
            //  all the parts and capture the parts. I'd love to know if this is (easily) possible
            //  with just one regex-invocation.
            var regexForParts = new Regex("([a-zA-Z])([+-]?[0-9]{1,10}),([+-]?[0-9]{1,10})");
            var matches = regexForParts.Matches(s);
            foreach (Match match in matches)
            {
                var dimensionCharText = match.Groups[1].Value;
                var startText = match.Groups[2].Value;
                var endText = match.Groups[3].Value;

                int start, end;
                if (dimensionCharText.Length != 1 ||
                    !int.TryParse(startText, out start) ||
                    !int.TryParse(endText, out end))
                {
                    throw new ArgumentException("The string is not in a valid format.");
                }

                var dimensionCondition = new DimensionCondition()
                {
                    Dimension = new Dimension(dimensionCharText[0]),
                    RangeStart = start,
                    RangeEnd = end,
                };

                dimensionQueryClause.AddCondition(dimensionCondition);
            }

            return dimensionQueryClause;
        }

        public static string TileCoordinateToStringRepresentation(ITileCoordinate tileCoordinate)
        {
            StringBuilder stringBuilder = new StringBuilder(20);
            foreach (var coordinate in tileCoordinate.EnumCoordinates())
            {
                stringBuilder.AppendFormat($"{coordinate.Item1.Id}{coordinate.Item2}");
            }

            return stringBuilder.ToString();
        }

        public static string LogicalPositionToStringRepresentation(in LogicalPosition logicalPosition)
        {
            return
                $"PyramidLevel={logicalPosition.PyramidLevel} ({logicalPosition.PositionX},{logicalPosition.PositionY},{logicalPosition.Width},{logicalPosition.Height})";
        }

        public static string PixelTypeToInformalString(PixelType pixelType)
        {
            switch (pixelType)
            {
                case PixelType.Gray8:
                    return "Gray8";
                case PixelType.Gray16:
                    return "Gray16";
                case PixelType.Bgr24:
                    return "Bgr24";
                case PixelType.Bgr48:
                    return "Bgr48";
                case PixelType.Gray32Float:
                    return "Gray32Float";
            }

            throw new ArgumentException($"Invalid PixelType encountered.");
        }

        public static string DataTypeToInformalString(DataType dataType)
        {
            switch (dataType)
            {
                case DataType.UncompressedBitmap:
                    return "uncompressed-bitmap";
                case DataType.JpgXrCompressedBitmap:
                    return "JPGXR-compressed-bitmap";
                case DataType.Zero:
                    return "Zero";
                case DataType.UncompressedBrick:
                    return "uncompressed-brick";
            }

            throw new ArgumentException("Invalid DataType encountered.");
        }

        public static string Tile2dBaseInfoToStringRepresentation(in Tile2dBaseInfo tile2dBaseInfo)
        {
            return
                $"({tile2dBaseInfo.PixelWidth},{tile2dBaseInfo.PixelHeight}) {PixelTypeToInformalString(tile2dBaseInfo.PixelType)}";
        }

        public static string TileBlobInfoToStringRepresentation(in TileBlobInfo tileBlobInfo)
        {
            return
                $"{Tile2dBaseInfoToStringRepresentation(tileBlobInfo.Tile2dBaseInfo)} DataType={DataTypeToInformalString(tileBlobInfo.DataType)}";
        }
    }
}

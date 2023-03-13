// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Implementation
{
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Linq;
    using System.Text;
    using System.Text.RegularExpressions;
    using ImgDoc2Net.Interfaces;

    /// <summary> 
    /// A bunch of utility functions are gathered here.
    /// </summary>
    public static partial class Utilities
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
        /// Parse a string in the format "T1-10Z2" (i.e. dimension identifier, followed by either an interval
        /// or a single number) into a list containing the dimension, start and end.
        /// </summary>
        /// <exception cref="ArgumentException"> 
        /// Thrown when the string is determined to have an invalid format.</exception>
        /// <param name="s"> The string to be parsed.</param>
        /// <returns> A list containing the dimension, start and end.</returns>
        public static List<(Dimension Dimension, int Start, int End)> ParseStringRepresentationOfDimensionAndRange(string s)
        {
            var parts = Utilities.SplitStringAtDimensionIdentifiers(s);
            var result = new List<ValueTuple<Dimension, int, int>>();
            var regexForStartEnd = new Regex(@"^\s*([a-zA-Z])\s*([+-]?[0-9]{1,10})\s*-\s*([+-]?[0-9]{1,10})\s*$");
            var regexForSingleValue = new Regex(@"^\s*([a-zA-Z])\s*([+-]?[0-9]{1,10})\s*$");
            foreach (var part in parts)
            {
                // first, we try to parse a string of the form <dimension-char> <start> - <end>
                var match = regexForStartEnd.Match(part);
                if (match.Success)
                {
                    var dimension = match.Groups[1].Value;
                    var start = match.Groups[2].Value;
                    var end = match.Groups[3].Value;
                    result.Add((new Dimension(dimension[0]), int.Parse(start), int.Parse(end)));
                }
                else
                {
                    match = regexForSingleValue.Match(part);
                    if (match.Success)
                    {
                        var dimension = match.Groups[1].Value;
                        int value = int.Parse(match.Groups[2].Value);
                        result.Add((new Dimension(dimension[0]), value, value));
                    }
                    else
                    {
                        throw new ArgumentException($"Invalid format encountered: \"{part}\".");
                    }
                }
            }

            return result;
        }

        /// <summary> 
        /// Enumerates tile-coordinates as defined by the ranges in the range collection. The enumeration starts 
        /// with the first item in the range, then the second is incremented etc.
        /// The specified range collection is checked whether there is a duplicate dimension included. If this is the
        /// case, an InvalidArgument-exception is thrown. Also, if the start index is behind the end index, an
        /// InvalidArgument-exception is thrown.
        /// </summary>
        /// <param name="range"> The range collection.</param>
        /// <returns> 
        /// An enumerator that allows foreach to be used to process the coordinates in bounds in
        /// this collection.
        /// </returns>
        public static IEnumerable<TileCoordinate> EnumerateCoordinatesInBounds(IReadOnlyList<(Dimension Dimension, int Start, int End)> range)
        {
            ThrowIfDuplicateDimension();

            int countOfElements = range.Count;
            int[] values = new int[countOfElements];

            // set the 'start value' of the coordinate
            for (int i = 0; i < countOfElements; ++i)
            {
                if (range[i].End < range[i].Start)
                {
                    throw new ArgumentException($"End-index ({range[i].End}) before Start-index ({range[i].Start}) for dimension '{range[i].Dimension.Id}'.");
                }

                values[i] = range[i].Start;
            }

            // Note: c.f. https://learn.microsoft.com/en-us/dotnet/csharp/programming-guide/classes-and-structs/local-functions#local-functions-and-exceptions, this
            //        is the recommended way in order to ensure that 'exceptions are thrown when one would expect them' (namely: when the iterator is created)
            return CoordinatesInBoundsIterator();

            bool Increment()
            {
                for (int no = 0; no < range.Count; no++)
                {
                    ++values[no];
                    if (values[no] <= range[no].End)
                    {
                        return true;
                    }

                    values[no] = range[no].Start;
                }

                return false;
            }

            IEnumerable<TileCoordinate> CoordinatesInBoundsIterator()
            {
                while (true)
                {
                    TileCoordinate coordinate = new TileCoordinate(Enumerable.Range(0, countOfElements).Select(i => Tuple.Create(range[i].Dimension, values[i])));
                    yield return coordinate;
                    if (!Increment())
                    {
                        break;
                    }
                }
            }

            void ThrowIfDuplicateDimension()
            {
                SortedSet<Dimension> sortedSet = new SortedSet<Dimension>();
                foreach (var item in range)
                {
                    if (!sortedSet.Add(item.Dimension))
                    {
                        throw new ArgumentException($"Duplicate Dimension '{item.Dimension.Id}' was given.");
                    }
                }
            }
        }

        /// <summary>   
        /// Creates a dimension-query-clause from a string representation. The syntax is:
        /// "A3,5B-3,1m9,10" - that is: a character identifying the dimension, followed by two
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

                if (dimensionCharText.Length != 1 ||
                    !int.TryParse(startText, out int start) ||
                    !int.TryParse(endText, out int end))
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
                $"({logicalPosition.PositionX},{logicalPosition.PositionY},{logicalPosition.Width},{logicalPosition.Height}) PyramidLevel={logicalPosition.PyramidLevel}";
        }

        /// <summary> Convert the specified pixel type to an informal string.</summary>
        /// <exception cref="ArgumentException"> 
        /// Thrown if the pixel type is invalid or unknown.
        /// </exception>
        /// <param name="pixelType"> The pixel type.</param>
        /// <returns> The informal string describing the pixel type.</returns>
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

        /// <summary> 
        /// Get an informal string representation for the specified data-type enum.
        /// </summary>
        /// <exception cref="ArgumentException"> Thrown when one or more arguments have unsupported or
        ///     illegal values.</exception>
        /// <param name="dataType"> The data-type enum to get a string representation for.</param>
        /// <returns> The informal string.</returns>
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

        /// <summary> Query if the specified character is valid 'dimension'.</summary>
        /// <param name="c"> The character to test.</param>
        /// <returns> True if the char is valid dimension identifier; false otherwise.</returns>
        public static bool IsValidDimension(char c)
        {
            return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
        }
    }

    /// <summary> 
    /// In this part we find internally used functions and utilities.
    /// </summary>
    public static partial class Utilities
    {
        /// <summary> Splits the specified string at "characters which are dimension identifiers".</summary>
        /// <exception cref="ArgumentException"> 
        /// If the first non-white-space character is not a valid dimension identifier, the string
        /// is regarded invalid, and an exception is thrown.
        /// </exception>
        /// <param name="sourceString"> The string to be split.</param>
        /// <returns> A list containing the split strings.</returns>
        internal static List<string> SplitStringAtDimensionIdentifiers(string sourceString)
        {
            List<string> result = new List<string>();
            ReadOnlySpan<char> span = sourceString.AsSpan();

            int startIndex = -1;
            for (int i = 0; i < span.Length; ++i)
            {
                char c = span[i];
                if (!char.IsWhiteSpace(c))
                {
                    if (Utilities.IsValidDimension(c))
                    {
                        if (startIndex < 0)
                        {
                            startIndex = i;
                        }
                        else
                        {
                            result.Add(span.Slice(startIndex, i - startIndex).ToString());
                            startIndex = i;
                        }
                    }
                    else if (startIndex == -1)
                    {
                        // we require that the first (non-white-space character) is a 'valid dimension', otherwise
                        // we throw an exception
                        throw new ArgumentException("The first non-whitespace character is not a valid dimension-identifier.");
                    }
                }
            }

            if (startIndex != span.Length && startIndex >= 0)
            {
                string part = span.Slice(startIndex, span.Length - startIndex).ToString();
                result.Add(part);
            }

            return result;
        }
    }
}

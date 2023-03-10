// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interfaces
{
    /// <summary> 
    ///  This gives the tile blob information - the base information we can provide about the bitmap
    ///  contained in the corresponding blob. This information is part of the database and is available
    ///  without inspecting/decoding the blob itself.
    /// </summary>
    public class TileBlobInfo
    {
        /// <summary> Gets or sets the tile base info, i.e. width, height and pixel type..</summary>
        /// <value> Information describing the 2d tile.</value>
        public Tile2dBaseInfo Tile2dBaseInfo { get; set; }

        /// <summary> Gets or sets the data type.</summary>
        /// <value> The type of the data.</value>
        public DataType DataType { get; set; }
    }
}

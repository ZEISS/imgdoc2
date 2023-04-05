// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interfaces
{
    /// <summary> 
    ///  This gives the brick blob information - the base information we can provide about the bitmap
    ///  contained in the corresponding blob. This information is part of the database and is available
    ///  without inspecting/decoding the blob itself.
    /// </summary>
    public class BrickBlobInfo
    {
        /// <summary> Gets or sets the brick base info, i.e. width, height, depth and pixel type.</summary>
        /// <value> Information describing the 3d brick.</value>
        public Brick3dBaseInfo Brick3dBaseInfo { get; set; }

        /// <summary> Gets or sets the data type.</summary>
        /// <value> The type of the data.</value>
        public DataType DataType { get; set; }
    }
}

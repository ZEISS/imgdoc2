// SPDX-FileCopyrightText: 2024 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interfaces
{
    using System;

    /// <summary> Structure representing a (2D) bitmap. </summary>
    public struct BitmapData
    {
        /// <summary> Gets or sets the width in pixels.</summary>
        /// <value> The width in pixels.</value>
        public int Width { get; set; }

        /// <summary> Gets or sets the height in pixels.</summary>
        /// <value> The height in pixels.</value>
        public int Height { get; set; }

        /// <summary> Gets or sets the stride (in bytes).</summary>
        /// <value> The stride.</value>
        public int Stride { get; set; }

        /// <summary> Gets or sets the pixel type.</summary>
        /// <value> The pixel type.</value>
        public PixelType PixelType { get; set; }

        /// <summary> Gets or sets the pixel data.</summary>
        /// <value> The pixel data.</value>
        public Memory<byte> Data { get; set; }
    }
}

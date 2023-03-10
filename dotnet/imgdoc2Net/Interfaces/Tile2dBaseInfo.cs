// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interfaces
{
    /// <summary> 
    /// The "base information" about a tile - the width, the height (in pixels)
    /// and the pixel type.
    /// </summary>
    public class Tile2dBaseInfo
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="Tile2dBaseInfo"/> class. 
        /// </summary>
        ///
        /// <param name="width">     The width.</param>
        /// <param name="height">    The height.</param>
        /// <param name="pixelType"> The pixel type.</param>
        public Tile2dBaseInfo(int width, int height, PixelType pixelType)
        {
            this.PixelWidth = width;
            this.PixelHeight = height;
            this.PixelType = pixelType;
        }

        /// <summary> Gets or sets the width (in units of pixel).</summary>
        ///
        /// <value> The width (in units of pixel).</value>
        public int PixelWidth { get; set; }

        /// <summary> Gets or sets the height (in units of pixel).</summary>
        ///
        /// <value> The height (in units of pixel).</value>
        public int PixelHeight { get; set; }

        /// <summary> Gets or sets the pixel type.</summary>
        ///
        /// <value> The pixel type.</value>
        public PixelType PixelType { get; set; }
    }
}

// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interfaces
{
    /// <summary> 
    /// The "base information" about a brick - the width, the height and the depth (in pixels)
    /// and the pixel type.
    /// </summary>
    public class Brick3dBaseInfo
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="Brick3dBaseInfo"/> class.
        /// </summary>
        ///
        /// <param name="width">     The width.</param>
        /// <param name="height">    The height.</param>
        /// <param name="depth">     The depth.</param>
        /// <param name="pixelType"> The pixel type.</param>
        public Brick3dBaseInfo(int width, int height, int depth, PixelType pixelType)
        {
            this.PixelWidth = width;
            this.PixelHeight = height;
            this.PixelDepth = depth;
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

        /// <summary> Gets or sets the depth (in units of pixel).</summary>
        ///
        /// <value> The depth (in units of pixel).</value>
        public int PixelDepth { get; set; }

        /// <summary> Gets or sets the pixel type.</summary>
        ///
        /// <value> The pixel type.</value>
        public PixelType PixelType { get; set; }
    }
}

// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interfaces
{
    /// <summary> 
    /// This class defines an axis-aligned 2d rectangle.
    /// </summary>
    public class Rectangle
    {
        /// <summary> Gets or sets the x coordinate of the edge point.</summary>
        /// <value> The x coordinate.</value>
        public double X { get; set; }

        /// <summary> Gets or sets the y coordinate of the edge point.</summary>
        /// <value> The y coordinate.</value>
        public double Y { get; set; }

        /// <summary> Gets or sets the width.</summary>
        /// <value> The width.</value>
        public double Width { get; set; }

        /// <summary> Gets or sets the height.</summary>
        /// <value> The height.</value>
        public double Height { get; set; }
    }
}

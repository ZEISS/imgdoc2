// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interfaces
{
    /// <summary>   
    /// This defines the logical position of a brick. 
    /// </summary>
    public struct LogicalPosition3d
    {
        /// <summary>   Gets or sets the x-position in pixel-coordinate-system of the front-top-left corner of the brick. </summary>
        /// <value type="double">   The x-position in pixel-coordinate-system of the front-top-left corner of the brick. </value>
        public double PositionX { get; set; }

        /// <summary>   Gets or sets the y-position in pixel-coordinate-system of the front-top-left corner of the brick. </summary>
        /// <value type="double">   The y-position in pixel-coordinate-system of the front-top-left corner of the brick. </value>
        public double PositionY { get; set; }

        /// <summary>   Gets or sets the z-position in pixel-coordinate-system of the front-top-left corner of the brick. </summary>
        /// <value type="double">   The z-position in pixel-coordinate-system of the front-top-left corner of the brick. </value>
        public double PositionZ { get; set; }

        /// <summary>   Gets or sets the width in pixel-coordinate-system of the brick. </summary>
        /// <value type="double">   The width in pixel-coordinate-system of the brick. </value>
        public double Width { get; set; }

        /// <summary>   Gets or sets the height in pixel-coordinate-system of the brick. </summary>
        /// <value type="double">   The height in pixel-coordinate-system of the brick. </value>
        public double Height { get; set; }

        /// <summary>   Gets or sets the depth in pixel-coordinate-system of the brick. </summary>
        /// <value type="double">   The depth in pixel-coordinate-system of the brick. </value>
        public double Depth { get; set; }

        /// <summary>   Gets or sets the pyramid level of the tile. </summary>
        /// <value type="int">  The pyramid level of the tile. </value>
        public int PyramidLevel { get; set; }
    }
}

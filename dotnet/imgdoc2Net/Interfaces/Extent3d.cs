// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interfaces
{
    /// <summary>   
    /// This class is used to represent the "minimum and the maximum of the value range for
    /// the x-, y- and z-position". 
    /// If MinX > MaxX (or MinY > MaxY or MinZ > MaxZ), then the value is indeterminate.
    /// </summary>
    public class Extent3d
    {
        /// <summary>   Gets or sets the minimum x coordinate value. </summary>
        /// <value> The minimum x coordinate value. </value>
        public double MinX { get; set; } = double.MaxValue;

        /// <summary>   Gets or sets the maximum x coordinate value. </summary>
        /// <value> The maximum x coordinate value. </value>
        public double MaxX { get; set; } = double.MinValue;

        /// <summary>   Gets or sets the minimum y coordinate value. </summary>
        /// <value> The minimum y coordinate value. </value>
        public double MinY { get; set; } = double.MaxValue;

        /// <summary>   Gets or sets the maximum y coordinate value. </summary>
        /// <value> The maximum y coordinate value. </value>
        public double MaxY { get; set; } = double.MinValue;

        /// <summary>   Gets or sets the minimum z coordinate value. </summary>
        /// <value> The minimum z coordinate value. </value>
        public double MinZ { get; set; } = double.MaxValue;

        /// <summary>   Gets or sets the maximum z coordinate value. </summary>
        /// <value> The maximum z coordinate value. </value>
        public double MaxZ { get; set; } = double.MinValue;

        /// <summary>   Gets a value indicating whether the extent for the x coordinate is valid. </summary>
        /// <value> True if the extent for the x coordinate is valid, false if not. </value>
        public bool IsExtentXValid
        {
            get { return this.MinX <= this.MaxX; }
        }

        /// <summary>   Gets a value indicating whether the extent for the z coordinate is valid. </summary>
        /// <value> True if the extent for the y coordinate is valid, false if not. </value>
        public bool IsExtentYValid
        {
            get { return this.MinY <= this.MaxY; }
        }

        /// <summary>   Gets a value indicating whether the extent for the z coordinate is valid. </summary>
        /// <value> True if the extent for the z coordinate is valid, false if not. </value>
        public bool IsExtentZValid
        {
            get { return this.MinY <= this.MaxY; }
        }

        /// <summary>   Gets a value indicating whether the extent is valid (which means that both x and y are valid). </summary>
        /// <value> True if the extent is valid, false if not. </value>
        public bool IsValid
        {
            get { return this.IsExtentXValid && this.IsExtentYValid && this.IsExtentZValid; }
        }
    }
}

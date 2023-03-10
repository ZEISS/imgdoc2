// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interfaces
{
    using ImgDoc2Net.Implementation;

    /// <summary> 
    /// Structure defining a range clause for a dimension.
    /// </summary>
    public struct DimensionCondition
    {
        /// <summary> Gets or sets the dimension.</summary>
        ///
        /// <value> The dimension.</value>
        public Dimension Dimension { get; set; }

        /// <summary> Gets or sets the start value of the range (inclusive).</summary>
        ///
        /// <value> The start value of the range (inclusive).</value>
        public int RangeStart { get; set; }

        /// <summary> Gets or sets the end value of the range (inclusive).</summary>
        ///
        /// <value> The end value of the range (inclusive).</value>
        public int RangeEnd { get; set; }
    }
}

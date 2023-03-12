// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Implementation
{
    using System;

    /// <summary>   This structure is representing a 'dimension' in imgdoc2. </summary>
    public struct Dimension : IComparable<Dimension>
    {
        /// <summary> The dimension identifier.</summary>
        public char Id;

        /// <summary>
        /// Initializes a new instance of the <see cref="Dimension"/> struct.
        /// </summary>
        ///
        /// <param name="c"> The character identifying the dimension. Valid values are a-z and A-Z. </param>
        public Dimension(char c)
        {
            Dimension.ThrowIfInvalidChar(c);
            this.Id = c;
        }

        /// <summary> Compares this Dimension object to another to determine their relative ordering.</summary>
        /// <param name="other"> Another instance to compare.</param>
        /// <returns> Negative if this object is less than the other, 0 if they are equal, or positive if
        ///     this is greater.</returns>
        public int CompareTo(Dimension other)
        {
            return this.Id.CompareTo(other.Id);
        }

        private static void ThrowIfInvalidChar(char c)
        {
            if (!Utilities.IsValidDimension(c))
            {
                throw new ArgumentException("Dimension must be a-zA-Z.");
            }
        }
    }
}

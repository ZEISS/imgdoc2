// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Implementation
{
    using System;

    /// <summary>   This structure is representing a 'dimension' in imgdoc2. </summary>
    public struct Dimension
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

        private static void ThrowIfInvalidChar(char c)
        {
            if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')))
            {
                throw new ArgumentException("Dimension must be a-zA-Z.");
            }
        }
    }
}

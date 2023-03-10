// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Implementation
{
    #region using directives

    using System;
    using System.Collections.Generic;
    using System.Linq;
    using ImgDoc2Net.Interfaces;

    #endregion

    /// <summary>
    /// Implementation of the IPlaneCoordinate - interface. This class is immutable.
    /// </summary>
    public partial class TileCoordinate : ITileCoordinate
    {
        private readonly Dictionary<Dimension, int> dict = new Dictionary<Dimension, int>();

        /// <summary>
        /// Initializes a new instance of the <see cref="TileCoordinate"/> class.
        /// </summary>
        public TileCoordinate()
            : this(null)
        {
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="TileCoordinate"/> class.
        /// </summary>
        /// <param name="initial">The initial.</param>
        public TileCoordinate(IEnumerable<Tuple<Dimension, int>> initial)
        {
            if (initial != null)
            {
                foreach (var tuple in initial)
                {
                    this.dict.Add(tuple.Item1, tuple.Item2);
                }
            }
        }

        /// <summary>
        /// Tries the get the specified coordinate.
        /// </summary>
        /// <param name="dimension">The dimension.</param>
        /// <param name="coordinate">The coordinate.</param>
        /// <returns>True if the specified coordinate could be retrieved, false otherwise.</returns>
        public bool TryGetCoordinate(Dimension dimension, out int coordinate)
        {
            return this.dict.TryGetValue(dimension, out coordinate);
        }

        /// <summary>
        /// Enumerates the coordinates.
        /// </summary>
        /// <returns>An enumerator for the dimension-coordinate-pairs.</returns>
        public IEnumerable<Tuple<Dimension, int>> EnumCoordinates()
        {
            foreach (var kv in this.dict)
            {
                yield return new Tuple<Dimension, int>(kv.Key, kv.Value);
            }
        }

        /// <summary>
        /// Determines whether the specified <see cref="System.Object" />, is equal to this instance.
        /// </summary>
        /// <param name="obj">The <see cref="System.Object" /> to compare with this instance.</param>
        /// <returns>
        /// <c>True</c> if the specified <see cref="System.Object" /> is equal to this instance; otherwise, <c>false</c>.
        /// </returns>
        public override bool Equals(object obj)
        {
            return this.Equals(obj as TileCoordinate);
        }

        /// <summary>
        /// Returns a hash code for this instance.
        /// </summary>
        /// <remarks>
        /// I guess it would be possible to create a better hash-function here. However - if we use the actual data in the hash-function,
        /// this requires that the class is immutable (the hash-code must not change).
        /// </remarks>
        /// <returns>
        /// A hash code for this instance, suitable for use in hashing algorithms and data structures like a hash table. 
        /// </returns>
        public override int GetHashCode()
        {
            int h = 0;
            foreach (var c in this.dict)
            {
                h ^= (int)c.Key.Id;
                h ^= c.Value;
            }

            return h;
        }

        /// <summary>
        /// Indicates whether the current object is equal to another object of the same type.
        /// </summary>
        /// <param name="other">An object to compare with this object.</param>
        /// <returns>
        /// True if the current object is equal to the <paramref name="other" /> parameter; otherwise, false.
        /// </returns>
        public bool Equals(ITileCoordinate other)
        {
            if (object.ReferenceEquals(this, other))
            {
                return true;
            }

            if (object.ReferenceEquals(null, other))
            {
                return false;
            }

            int numberOfDimensions = this.dict.Count;

            var list = other.EnumCoordinates().ToList();

            if (list.Count != numberOfDimensions)
            {
                return false;
            }

            foreach (var tuple in list)
            {
                bool b = this.TryGetCoordinate(tuple.Item1, out int value);
                if (b == false)
                {
                    return false;
                }

                if (value != tuple.Item2)
                {
                    return false;
                }
            }

            return true;
        }
    }
}
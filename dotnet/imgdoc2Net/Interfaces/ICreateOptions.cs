// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interfaces
{
    using System;
    using System.Collections.Generic;
    using System.Text;
    using ImgDoc2Net.Implementation;

    /// <summary> Options for opening (or creating) an imgdoc2-document. </summary>
    public interface ICreateOptions : IDisposable
    {
        /// <summary> Gets the filename of the file.</summary>
        ///
        /// <value> The filename.</value>
        string Filename { get; }

        /// <summary> Gets a value indicating whether the database should be created containing a spatial index. </summary>
        ///
        /// <value> True if to construct with a spatial index; false otherwise. </value>
        bool UseSpatialIndex { get; }

        /// <summary> Gets a value indicating whether the database should be created containing a blob table.</summary>
        ///
        /// <value> True if a blob table is to be created; false if not.</value>
        bool UseBlobTable { get; }

        /// <summary> Adds a dimension.</summary>
        ///
        /// <param name="dimension"> The dimension to be added.</param>
        void AddDimension(Dimension dimension);

        /// <summary> Adds a dimension for which an index is to be created.</summary>
        ///
        /// <param name="dimension"> The dimension to be added for which an index is to be created.</param>
        void AddIndexedDimension(Dimension dimension);
    }
}

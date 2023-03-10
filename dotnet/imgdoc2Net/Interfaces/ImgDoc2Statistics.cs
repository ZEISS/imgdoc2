// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interfaces
{
    /// <summary> 
    /// Statistics provided by the interop-layer. This is intended for debugging purposes.
    /// </summary>
    public class ImgDoc2Statistics
    {
        /// <summary> Gets or sets the number of currently existing "create options" objects.</summary>
        ///
        /// <value> The total number of currently existing "create options" objects.</value>
        public int NumberOfCreateOptionsObjectsActive { get; set; }

        /// <summary> Gets or sets the number of currently existing "open existing options" objects.</summary>
        ///
        /// <value> The total number of currently existing "open existing options" objects.</value>
        public int NumberOfOpenExistingOptionsObjectsActive { get; set; }

        /// <summary> Gets or sets the number of currently existing "document" objects.</summary>
        ///
        /// <value> The total number of currently existing "document" objects.</value>
        public int NumberOfDocumentObjectsActive { get; set; }

        /// <summary> Gets or sets the number of currently existing "reader 2d" objects.</summary>
        ///
        /// <value> The total number of currently existing "reader 2d" objects.</value>
        public int NumberOfReader2dObjectsActive { get; set; }

        /// <summary> Gets or sets the number of currently existing "writer 2d" objects.</summary>
        ///
        /// <value> The total number of currently existing "writer 2d" objects.</value>
        public int NumberOfWriter2dObjectsActive { get; set; }
    }
}

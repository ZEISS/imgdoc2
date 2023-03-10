// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interfaces
{
    using System;
    using System.Collections.Generic;
    using System.Text;

    /// <summary> 
    /// This interface is representing a 'document'.
    /// Depending on the type of the document, objects for interacting with it can be created.
    /// </summary>
    public interface IDocument : IDisposable
    {
        /// <summary> 
        /// Try to get a "reader object" for a 2D-document. This method may return null
        /// if such an object cannot be constructed.
        /// </summary>
        ///
        /// <returns> The 2D reader object.</returns>
        IRead2d Get2dReader();

        /// <summary> 
        /// Try to get a "write object" for a 2D-document. This method may return null
        /// if such an object cannot be constructed.
        /// </summary>
        ///
        /// <returns> The 2D writer object.</returns>
        IWrite2d Get2dWriter();
    }
}

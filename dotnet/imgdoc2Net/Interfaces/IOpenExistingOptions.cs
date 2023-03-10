// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interfaces
{
    using System;

    /// <summary> This interface gathers all parameters for the operation of "opening an existing file".</summary>
    public interface IOpenExistingOptions : IDisposable
    {
        /// <summary> Gets the filename of the file.</summary>
        ///
        /// <value> The filename.</value>
        string Filename { get; }

        /// <summary> Gets a value indicating whether the file is to opened as "readonly".</summary>
        ///
        /// <value> True file is to opened as "readonly"; false otherwise.</value>
        bool Readonly { get; }
    }
}

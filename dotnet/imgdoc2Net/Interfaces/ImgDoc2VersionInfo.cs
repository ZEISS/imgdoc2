// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interfaces
{
    /// <summary> Version information about the ImgDoc2Net library.</summary>
    public class ImgDoc2VersionInfo
    {
        /// <summary> Gets or sets the native library version.</summary>
        /// <value> The native library version.</value>
        public ImgDoc2NativeLibraryVersionInfo NativeLibraryVersion { get; set; }

        /// <summary> Gets or sets information describing the managed image document 2 library version.</summary>
        /// <value> Information describing the managed image document 2 library version.</value>
        public ImgDoc2ManagedLibraryVersionInfo ManagedImgDoc2LibraryVersionInfo { get; set; }
    }
}

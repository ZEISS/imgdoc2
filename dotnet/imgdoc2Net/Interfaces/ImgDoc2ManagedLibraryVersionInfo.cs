// SPDX-FileCopyrightText: 2024 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interfaces
{
    /// <brief> 
    /// Version information about the managed imgdoc2 interop library.
    /// </brief>
    public struct ImgDoc2ManagedLibraryVersionInfo
    {
        /// <summary> Gets or sets the major version.</summary>
        /// <value> The major version.</value>
        public int Major { get; set; }
        
        /// <summary> Gets or sets the minor version.</summary>
        /// <value> The minor version.</value>
        public int Minor { get; set; }
        
        /// <summary> Gets or sets the patch version.</summary>
        /// <value> The patch version.</value>
        public int Patch { get; set; }
        
        /// <summary> Gets or sets the reversion - this is always zero.</summary>
        /// <value> The reversion - always 0.</value>
        public int Revision { get; set; }
        
        /// <summary> Gets or sets the type of the build - an information string identifying the build type, e.g. "Debug" or "Release".</summary>
        /// <value> The build type.</value>
        public string BuildType { get; set; }
    }
}

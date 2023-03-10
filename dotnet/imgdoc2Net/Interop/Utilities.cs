// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interop
{
    using System;

    /// <summary> A bunch of utilities.</summary>
    internal static class Utilities
    {
        /// <summary> Gets a value indicating if the current platform is Linux.</summary>
        ///
        /// <returns> True if running on a Linux platform; false otherwise.</returns>
        public static bool IsLinux()
        {
            // http://stackoverflow.com/a/5117005/358336
            int p = (int)Environment.OSVersion.Platform;
            return (p == 4) || (p == 6) || (p == 128);
        }
    }
}

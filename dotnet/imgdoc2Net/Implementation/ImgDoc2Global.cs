// SPDX-FileCopyrightText: 2024 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Implementation
{
    using System;
    using System.Reflection;
    using ImgDoc2Net.Interfaces;
    using ImgDoc2Net.Interop;

    /// <summary> 
    /// Support functions and other context-free functions are provided by this class.
    /// </summary>
    public static class ImgDoc2Global
    {
        private static readonly Lazy<IDecoding> Decoder = new Lazy<IDecoding>(() => new Decoding());

        /// <summary> Gets version information.</summary>
        /// <returns> The version information.</returns>
        public static ImgDoc2VersionInfo GetVersionInfo()
        {
            ImgDoc2NativeLibraryVersionInfo nativeLibraryVersion = ImgDoc2ApiInterop.Instance.GetNativeLibraryVersionInfo();

            // Get the currently executing assembly
            Assembly assembly = Assembly.GetExecutingAssembly();

            // Get the version of the assembly
            Version managedVersion = assembly.GetName().Version;

            return new ImgDoc2VersionInfo
            {
                NativeLibraryVersion = nativeLibraryVersion,
                ManagedImgDoc2LibraryVersionInfo = new ImgDoc2ManagedLibraryVersionInfo()
                {
                    Major = managedVersion.Major,
                    Minor = managedVersion.Minor,
                    Patch = managedVersion.Build,
                    Revision = managedVersion.Revision,
#if DEBUG
                    BuildType = "Debug",
#else
                    BuildType = "Release",
#endif
                },
            };
        }

        /// <summary> Gets a decoder object.</summary>
        /// <returns> The decoder object.</returns>
        public static IDecoding GetDecoder()
        {
            return ImgDoc2Global.Decoder.Value;
        }
    }
}
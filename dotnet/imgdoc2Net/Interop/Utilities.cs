// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interop
{
    using System;
    using System.Runtime.InteropServices;
    using System.Text;

    /// <summary> A bunch of utilities.</summary>
    internal static class Utilities
    {
        /// <summary> Gets a value indicating if the current platform is Linux.</summary>
        ///
        /// <returns> True if running on a Linux platform; false otherwise.</returns>
        public static bool IsLinux()
        {
            return RuntimeInformation.IsOSPlatform(OSPlatform.Linux);
        }

        /// <summary> Gets a value indicating if the current platform is Windows.</summary>
        ///
        /// <returns> True if running on a Windows platform; false otherwise.</returns>
        public static bool IsWindows()
        {
            return RuntimeInformation.IsOSPlatform(OSPlatform.Windows);
        }

        /// <summary> Query if the CPU architecture (of the process that is currently executing) is x64.</summary>
        /// <returns> True if CPU architecture is x64, false if not.</returns>
        public static bool IsCpuArchitectureX64()
        {
            // Get the process architecture
            var processArchitecture = RuntimeInformation.ProcessArchitecture;

            return processArchitecture == Architecture.X64;
        }

        /// <summary> Query if the CPU architecture (of the process that is currently executing) is ARM64.</summary>
        /// <returns> True if CPU architecture is ARM64, false if not.</returns>
        public static bool IsCpuArchitectureArm64()
        {
            // Get the process architecture
            var processArchitecture = RuntimeInformation.ProcessArchitecture;

            return processArchitecture == Architecture.Arm64;
        }

        /// <brief> 
        /// Convert a UTF-8 string (given as a span of bytes) into a .NET string.
        /// </brief>
        /// <exception cref="ArgumentNullException"> Thrown when one or more required arguments are null.</exception>
        /// <param name="utf8Span"> The UTF8-encoded string to be converted.</param>
        /// <returns> The converted string.</returns>
        public static string ConvertFromUtf8Span(ReadOnlySpan<byte> utf8Span)
        {
            unsafe
            {
                fixed (byte* ptr = utf8Span)
                {
                    IntPtr utf8Pointer = (IntPtr)ptr;
                    return ConvertFromUtf8IntPtr(utf8Pointer, utf8Span.Length);
                }
            }
        }

        /// <brief> 
        /// Convert a UTF-8 string (given by a pointer and a length) into a .NET string.
        /// </brief>
        /// <exception cref="ArgumentNullException"> Thrown when one or more required arguments are null.</exception>
        /// <param name="utf8Pointer"> Pointer to the UTF8-encoded string to be converted.</param>
        /// <param name="length">      The length of the input string in bytes.</param>
        /// <returns> The converted string.</returns>
        public static string ConvertFromUtf8IntPtr(IntPtr utf8Pointer, int length)
        {
            if (utf8Pointer == IntPtr.Zero)
            {
                throw new ArgumentNullException(nameof(utf8Pointer), "Pointer is null.");
            }

            if (length < 0)
            {
                throw new ArgumentOutOfRangeException(nameof(length), "Length is negative.");
            }

            if (length == 0)
            {
                return string.Empty;
            }

            // Copy data from unmanaged memory to a managed byte array
            byte[] buffer = new byte[length];
            Marshal.Copy(utf8Pointer, buffer, 0, length);

            // Convert the byte array to a string
            return Encoding.UTF8.GetString(buffer);
        }

        /// <brief> Convert a zero-terminated UTF-8 string from an IntPtr to a .NET string.</brief>
        /// <exception cref="ArgumentNullException"> Thrown when one or more required arguments are null.</exception>
        /// <param name="utf8Pointer"> Pointer to the UTF8-encoded string to be converted.</param>
        /// <returns> The converted string.</returns>
        public static string ConvertFromUtf8IntPtrZeroTerminated(IntPtr utf8Pointer)
        {
            // with .NETCore 2.1 we could use the following code instead:
            //  return Marshal.PtrToStringUTF8(utf8Pointer);
            if (utf8Pointer == IntPtr.Zero)
            {
                throw new ArgumentNullException(nameof(utf8Pointer), "Pointer is null.");
            }

            // Determine the length of the string
            int length = 0;
            while (Marshal.ReadByte(utf8Pointer, length) != 0)
            {
                length++;
            }

            return Utilities.ConvertFromUtf8IntPtr(utf8Pointer, length);
        }
    }
}

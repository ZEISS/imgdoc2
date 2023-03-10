// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interop
{
    using System;
    using System.Collections.Generic;
    using System.Runtime.InteropServices;
    using System.Text;
    using System.Xml.Linq;

    /// <summary>   
    /// This class is implementing "loading the native DLL" on Windows. 
    /// </summary>
    internal partial class DllLoaderWindows : DllLoader
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="DllLoaderWindows"/> class. 
        /// </summary>
        /// <param name="filename"> Filename of the DLL/dynlib to be loaded.</param>
        public DllLoaderWindows(string filename) 
            : base(filename)
        {
        }

        /// <inheritdoc/>
        public override IntPtr GetProcAddress(string functionName)
        {
            this.ThrowIfNotOperational();
            IntPtr addressOfFunction = DllLoaderWindows.GetProcAddress(this.DllHandle, functionName);
            return addressOfFunction;
        }

        /// <inheritdoc/>
        protected override IntPtr LoadDynamicLibrary(string filename)
        {
            return DllLoaderWindows.LoadLibrary(this.Filename);
        }
    }

    /// <content>
    /// Declaration of kernel32-functions LoadLibrary/GetProcAddress et al.
    /// </content>
    internal partial class DllLoaderWindows
    {
        [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
        private static extern IntPtr LoadLibrary(string dllToLoad);

        [DllImport("kernel32", CharSet = CharSet.Ansi, SetLastError = true, EntryPoint = "GetProcAddress")]
        private static extern IntPtr GetProcAddress(IntPtr handleModule, string procedureName);

        [DllImport("kernel32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool FreeLibrary(IntPtr hModule);
    }
}

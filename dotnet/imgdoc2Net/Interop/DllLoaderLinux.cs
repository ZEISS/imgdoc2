// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interop
{
    using System;
    using System.Collections.Generic;
    using System.Runtime.InteropServices;
    using System.Text;

    /// <summary>   
    /// This class is implementing "loading the native DLL" on Linux platform. 
    /// </summary>
    internal partial class DllLoaderLinux : DllLoader
    {
        private int importToUse;

        /// <summary>
        /// Initializes a new instance of the <see cref="DllLoaderLinux"/> class.
        /// </summary>
        /// <param name="filename"> Filename of the DLL/dynlib to be loaded.</param>
        public DllLoaderLinux(string filename) 
            : base(filename)
        {
        }

        /// <inheritdoc/>
        public override IntPtr GetProcAddress(string functionName)
        {
            this.ThrowIfNotOperational();
            IntPtr addressOfFunction = this.importToUse == 1 ? DllLoaderLinux.dlsym1(this.DllHandle, functionName) : DllLoaderLinux.dlsym2(this.DllHandle, functionName);
            return addressOfFunction;
        }

        /// <inheritdoc/>
        protected override IntPtr LoadDynamicLibrary(string filename)
        {
            IntPtr dllHandle;
            try
            {
                dllHandle = DllLoaderLinux.dlopen1(this.Filename, RTLD_NOW);
                this.importToUse = 1;
            }
            catch (DllNotFoundException)
            {
                dllHandle = DllLoaderLinux.dlopen2(this.Filename, RTLD_NOW);
                this.importToUse = 2;
            }

            return dllHandle;
        }
    }

    /// <content>   
    /// Declaration of Linux-functions for loading a dynamic library.
    /// </content>
    internal partial class DllLoaderLinux
    {
#pragma warning disable SA1310 // Field names should not contain underscore
        private const int RTLD_NOW = 2; // for dlopen's flags 
#pragma warning restore SA1310 // Field names should not contain underscore

        [DllImport("libdl.so", EntryPoint = "dlopen")]
#pragma warning disable SA1300 // Element should begin with upper-case letter
        private static extern IntPtr dlopen1(string filename, int flags);

        [DllImport("libdl.so.2", EntryPoint = "dlopen")]
        private static extern IntPtr dlopen2(string filename, int flags);

        [DllImport("libdl.so", EntryPoint = "dlsym")]
        private static extern IntPtr dlsym1(IntPtr handle, string symbol);

        [DllImport("libdl.so.2", EntryPoint = "dlsym")]
        private static extern IntPtr dlsym2(IntPtr handle, string symbol);

        [DllImport("libdl.so", EntryPoint = "dlclose")]
        private static extern IntPtr dlclose1(IntPtr handle);

        [DllImport("libdl.so.2", EntryPoint = "dlclose")]
        private static extern IntPtr dlclose2(IntPtr handle);
#pragma warning restore SA1300 // Element should begin with upper-case letter
    }
}

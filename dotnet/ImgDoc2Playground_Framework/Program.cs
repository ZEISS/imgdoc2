// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Playground_Framework
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;

    using ImgDoc2Net.Interop;

    internal class Program
    {
        static void Main(string[] args)
        {
            var instance = ImgDoc2ApiInterop.Instance;

            var handle = instance.CreateCreateOptions();
            instance.CreateOptionsSetFilename(handle, "TESTTEXT");
            instance.DestroyCreateOptions(handle);
        }
    }
}

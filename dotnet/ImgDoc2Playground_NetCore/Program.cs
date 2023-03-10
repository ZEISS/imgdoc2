// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

// See https://aka.ms/new-console-template for more information
using ImgDoc2Net.Interop;


Console.WriteLine("Hello, World!");
var instance = ImgDoc2ApiInterop.Instance;
var handle = instance.CreateCreateOptions();
instance.CreateOptionsSetFilename(handle, "TESTTEXT");
string s = instance.CreateOptionsGetFilename(handle);

instance.CreateOptionsSetFilename(handle, "ÄÖÜäöüß 如果您选择“全部接受");
s = instance.CreateOptionsGetFilename(handle);

instance.DestroyCreateOptions(handle);

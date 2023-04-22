// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net_UnitTests
{
    using FluentAssertions;
    using ImgDoc2Net.Implementation;
    using ImgDoc2Net.Interfaces;
    using ImgDoc2Net.Interop;
    using System;

    [Collection(NonParallelCollectionDefinitionClass.Name)]
    public class HandleValidityCheckTests
    {
        [Fact]
        public void HandleValidityCheck_WhenHandleIsInvalid_ThrowsException()
        {
            // this test is operating on "interop"-level
            var instance = ImgDoc2ApiInterop.Instance;
            var createOptionsHandle = instance.CreateCreateOptions();
            instance.CreateOptionsSetFilename(createOptionsHandle, ":memory:");
            var handle = instance.CreateNewDocument(createOptionsHandle);
            instance.DestroyDocument(handle+1);
            instance.DestroyCreateOptions(createOptionsHandle);
        }
    }
}
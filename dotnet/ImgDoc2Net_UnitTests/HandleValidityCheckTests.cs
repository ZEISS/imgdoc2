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
        public void OpenExistingOptionsHandle_WhenDestroyedTwice_ThrowsException()
        {
            var instance = ImgDoc2ApiInterop.Instance;
            var handle = instance.CreateOpenExistingOptions();
            instance.DestroyOpenExistingOptions(handle);

            // expect exception on calling DestroyOpenExistingOptions() twice
            Assert.Throws<ImgDoc2Exception>(() => instance.DestroyOpenExistingOptions(handle));
        }

        [Fact]
        public void CreateOptionsHandle_WhenDestroyedTwice_ThrowsException()
        {
            var instance = ImgDoc2ApiInterop.Instance;
            var handle = instance.CreateCreateOptions();
            instance.DestroyCreateOptions(handle);

            // expect exception on calling DestroyOpenExistingOptions() twice
            Assert.Throws<ImgDoc2Exception>(() => instance.DestroyCreateOptions(handle));
        }

        [Fact]
        public void UseWrongTypeOfHandle_ThrowsException()
        {
            var instance = ImgDoc2ApiInterop.Instance;
            var handle = instance.CreateCreateOptions();

            // when passing this "create-options"-handle to a function that expects an "open-existing-options"-handle,
            // we expect an exception
            Assert.Throws<ImgDoc2Exception>(() => instance.DestroyOpenExistingOptions(handle));

            instance.DestroyCreateOptions(handle);
        }

        [Fact]
        public void HandleValidityCheck_WhenHandleIsInvalid_ThrowsException()
        {
            // this test is operating on "interop"-level
            var instance = ImgDoc2ApiInterop.Instance;
            var createOptionsHandle = instance.CreateCreateOptions();
            instance.CreateOptionsSetFilename(createOptionsHandle, ":memory:");
            var handle = instance.CreateNewDocument(createOptionsHandle);

            // we add 1 to the handle, so this is then for sure an invalid handle, and we expect an exception
            Assert.Throws<ImgDoc2Exception>(() => instance.DestroyDocument(handle + 1));

            instance.DestroyDocument(handle);
            instance.DestroyCreateOptions(createOptionsHandle);
        }
    }
}
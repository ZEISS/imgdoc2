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
    using System.Collections.Generic;

    [Collection(NonParallelCollectionDefinitionClass.Name)]
    public class ImgDoc2ApiInteropTests
    {
        [Fact]
        public void BlobOutputOnByteArrayExpectExceptionIfNotInitialized()
        {
            var blobOnByteArray = new ImgDoc2ApiInterop.BlobOutputOnByteArray();

            unsafe
            {
                byte* buffer = stackalloc byte[5];
                Assert.Throws<InvalidOperationException>(() => blobOnByteArray.SetData(0, 5, new IntPtr(buffer)));
            }
        }

        [Fact]
        public void BlobOutputOnByteArrayExpectExceptionIfInitializedMultipleTimes()
        {
            var blobOnByteArray = new ImgDoc2ApiInterop.BlobOutputOnByteArray();
            blobOnByteArray.SetSize(2);
            Assert.Throws<InvalidOperationException>(() => blobOnByteArray.SetSize(2));
        }

        [Fact]
        public void BlobOutputOnByteArrayExpectExceptionIfSetDataIsOutOfBounds()
        {
            var blobOnByteArray = new ImgDoc2ApiInterop.BlobOutputOnByteArray();
            blobOnByteArray.SetSize(2);

            Assert.Throws<InvalidOperationException>(() => blobOnByteArray.SetSize(2));

            bool b;
            unsafe
            {
                byte* buffer = stackalloc byte[2];
                buffer[0] = 1;
                buffer[1] = 2;
                b = blobOnByteArray.SetData(0, 2, new IntPtr(buffer));
            }

            b.Should().BeTrue();
            blobOnByteArray.Buffer[0].Should().Be(1);
            blobOnByteArray.Buffer[1].Should().Be(2);

            unsafe
            {
                byte* buffer = stackalloc byte[2];
                buffer[0] = 1;
                buffer[1] = 2;
                Assert.Throws<ArgumentException>(() => blobOnByteArray.SetData(1, 2, new IntPtr(buffer)));
            }
        }
    }
}

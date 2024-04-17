// SPDX-FileCopyrightText: 2024 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net_UnitTests
{
    using FluentAssertions;
    using ImgDoc2Net.Implementation;

    [Collection(NonParallelCollectionDefinitionClass.Name)]
    public class ImgDoc2GlobalTests
    {
        [Fact]
        public void GetVersionInformationAndCheckForPlausibility()
        {
            var versionInfo = ImgDoc2Global.GetVersionInfo();
            versionInfo.NativeLibraryVersion.Major.Should().BeGreaterOrEqualTo(0);
            versionInfo.NativeLibraryVersion.Minor.Should().BeGreaterOrEqualTo(0);
            versionInfo.NativeLibraryVersion.Patch.Should().BeGreaterOrEqualTo(0);

            (versionInfo.NativeLibraryVersion is {Major: 0, Minor: 0, Patch: 0}).Should().BeFalse();
            
            versionInfo.ManagedImgDoc2LibraryVersionInfo.Major.Should().BeGreaterOrEqualTo(0);
            versionInfo.ManagedImgDoc2LibraryVersionInfo.Minor.Should().BeGreaterOrEqualTo(0);
            versionInfo.ManagedImgDoc2LibraryVersionInfo.Patch.Should().BeGreaterOrEqualTo(0);
            versionInfo.ManagedImgDoc2LibraryVersionInfo.Revision.Should().BeGreaterOrEqualTo(0);

            (versionInfo.ManagedImgDoc2LibraryVersionInfo is { Major: 0, Minor: 0, Patch: 0 }).Should().BeFalse();
        }
    }
}

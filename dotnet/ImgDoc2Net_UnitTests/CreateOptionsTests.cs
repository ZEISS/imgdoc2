// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net_UnitTests
{
    using ImgDoc2Net.Implementation;
    using ImgDoc2Net.Interop;

    [Collection(NonParallelCollectionDefinitionClass.Name)]
    public class CreateOptionsTests
    {
        [Fact]
        public void SetFilenameAndGetItAndCompareResult()
        {
            const string Filename = "TESTTEXT";
            var instance = ImgDoc2ApiInterop.Instance;

            // we get the "statistics" before running our test - the statistics contains counters of active objects,
            //  and we check before leaving the test that it is where is was before (usually zero)
            var statisticsBeforeTest = instance.GetStatistics();

            var handle = instance.CreateCreateOptions();
            instance.CreateOptionsSetFilename(handle, Filename);

            string s = instance.CreateOptionsGetFilename(handle);

            Assert.Equal(expected: Filename, actual: s);
            instance.DestroyCreateOptions(handle);

            Assert.True(Utilities.IsActiveObjectCountEqual(statisticsBeforeTest, instance.GetStatistics()), "orphaned native imgdoc2-objects detected");
        }

        [Fact]
        public void SetFilenameWithUnicodeCharactersAndGetItAndCompareResult()
        {
            const string Filename = "ÄÖÜäöüß 馬匹背負著馬鞍";

            var instance = ImgDoc2ApiInterop.Instance;
            var handle = instance.CreateCreateOptions();
            instance.CreateOptionsSetFilename(handle, Filename);
            string s = instance.CreateOptionsGetFilename(handle);
            Assert.Equal(expected: Filename, actual: s);
            instance.DestroyCreateOptions(handle);
        }

        [Fact]
        public void SetFilenameVeryLongTextAndGetItAndCompareResult()
        {
            string filename = string.Concat(Enumerable.Repeat("Testtext12", 1000));

            var instance = ImgDoc2ApiInterop.Instance;
            var handle = instance.CreateCreateOptions();
            instance.CreateOptionsSetFilename(handle, filename);
            string s = instance.CreateOptionsGetFilename(handle);
            Assert.Equal(s, filename);
            instance.DestroyCreateOptions(handle);
        }

        [Fact]
        public void SetUSeSpatialIndexAndGetItAndCompareResult()
        {
            var instance = ImgDoc2ApiInterop.Instance;
            var handle = instance.CreateCreateOptions();
            instance.CreateOptionsSetUseSpatialIndex(handle, true);
            Assert.True(instance.CreateOptionsGetUseSpatialIndex(handle));
            instance.CreateOptionsSetUseSpatialIndex(handle, false);
            Assert.False(instance.CreateOptionsGetUseSpatialIndex(handle));
            instance.DestroyCreateOptions(handle);
        }

        [Fact]
        public void AddDimensionsAndCheckForCorrectResult()
        {
            var instance = ImgDoc2ApiInterop.Instance;
            var handle = instance.CreateCreateOptions();
            instance.CreateOptionsAddDimension(handle, new Dimension('A'));
            instance.CreateOptionsAddDimension(handle, new Dimension('B'));

            var dimensions = instance.CreateOptionsGetDimensions(handle);

            Assert.Equal(2, dimensions.Length);
            Assert.Contains(new Dimension('A'), dimensions);
            Assert.Contains(new Dimension('B'), dimensions);
            instance.DestroyCreateOptions(handle);
        }

        [Fact]
        public void AddIndexedDimensionsAndCheckForCorrectResult()
        {
            var instance = ImgDoc2ApiInterop.Instance;
            var handle = instance.CreateCreateOptions();
            instance.CreateOptionsAddIndexedDimension(handle, new Dimension('C'));
            instance.CreateOptionsAddIndexedDimension(handle, new Dimension('P'));

            var dimensions = instance.CreateOptionsGetIndexedDimensions(handle);

            Assert.Equal(2, dimensions.Length);
            Assert.Contains(new Dimension('C'), dimensions);
            Assert.Contains(new Dimension('P'), dimensions);
            instance.DestroyCreateOptions(handle);
        }

        [Fact]
        public void CheckEmptyObjectForImageDimensionsForCorrectResult()
        {
            var instance = ImgDoc2ApiInterop.Instance;
            var handle = instance.CreateCreateOptions();
            var dimensions = instance.CreateOptionsGetDimensions(handle);
            Assert.True(dimensions != null && dimensions.Length == 0);
            instance.DestroyCreateOptions(handle);
        }

        [Fact]
        public void CheckEmptyObjectForIndexedImageDimensionsForCorrectResult()
        {
            var instance = ImgDoc2ApiInterop.Instance;
            var handle = instance.CreateCreateOptions();
            var dimensions = instance.CreateOptionsGetIndexedDimensions(handle);
            Assert.True(dimensions != null && dimensions.Length == 0);
            instance.DestroyCreateOptions(handle);
        }

        [Fact]
        public void AddInvalidDimensionIdentifierAndExpectException()
        {
            var instance = ImgDoc2ApiInterop.Instance;
            var handle = instance.CreateCreateOptions();
            Dimension dimension = new Dimension();
            Assert.Throws<ImgDoc2Exception>(() => instance.CreateOptionsAddDimension(handle, dimension));
            instance.DestroyCreateOptions(handle);
        }

        [Fact]
        public void AddIndexedInvalidDimensionIdentifierAndExpectException()
        {
            var instance = ImgDoc2ApiInterop.Instance;
            var handle = instance.CreateCreateOptions();
            Dimension dimension = new Dimension();
            Assert.Throws<ImgDoc2Exception>(() => instance.CreateOptionsAddIndexedDimension(handle, dimension));
            instance.DestroyCreateOptions(handle);
        }
    }
}

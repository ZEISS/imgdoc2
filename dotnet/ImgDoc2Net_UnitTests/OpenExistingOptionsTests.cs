// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net_UnitTests
{
    using ImgDoc2Net.Implementation;
    using ImgDoc2Net.Interop;

    [Collection(NonParallelCollectionDefinitionClass.Name)]
    public class OpenExistingOptionsTests
    {
        [Fact]
        public void SetFilenameAndGetItAndCompareResult()
        {
            const string filename = "TESTTEXT";
            var instance = ImgDoc2ApiInterop.Instance;

            // we get the "statistics" before running our test - the statistics contains counters of active objects,
            //  and we check before leaving the test that it is where is was before (usually zero)
            var statisticsBeforeTest = instance.GetStatistics();

            var handle = instance.CreateOpenExistingOptions();
            instance.OpenExistingOptionsSetFilename(handle, filename);

            string s = instance.OpenExistingOptionsGetFilename(handle);

            Assert.Equal(expected: filename, actual: s);
            instance.DestroyOpenExistingOptions(handle);

            Assert.True(Utilities.IsActiveObjectCountEqual(statisticsBeforeTest, instance.GetStatistics()), "orphaned native imgdoc2-objects detected");
        }

        [Fact]
        public void UsingOpenExistingOptionsSetFilenameAndGetItAndCompareResult()
        {
            const string filename = "TESTTEXTÄÖÜß";

            string filenameFromObject;
            using (var openExistingOption = new OpenExistingOptions())
            {
                openExistingOption.Filename = filename;
                filenameFromObject = openExistingOption.Filename;
            }

            Assert.Equal(expected: filename, actual: filenameFromObject);
        }
    }
}

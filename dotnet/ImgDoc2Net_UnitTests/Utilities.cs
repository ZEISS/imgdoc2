// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net_UnitTests
{
    using ImgDoc2Net.Interfaces;

    internal static class Utilities
    {
        public static bool IsActiveObjectCountEqual(ImgDoc2Statistics statisticsBefore, ImgDoc2Statistics statisticsAfter)
        {
            if (statisticsBefore.NumberOfCreateOptionsObjectsActive == statisticsAfter.NumberOfCreateOptionsObjectsActive &&
                statisticsBefore.NumberOfOpenExistingOptionsObjectsActive == statisticsAfter.NumberOfOpenExistingOptionsObjectsActive &&
                statisticsBefore.NumberOfDocumentObjectsActive == statisticsAfter.NumberOfDocumentObjectsActive &&
                statisticsBefore.NumberOfReader2dObjectsActive == statisticsAfter.NumberOfReader2dObjectsActive &&
                statisticsBefore.NumberOfWriter2dObjectsActive == statisticsAfter.NumberOfWriter2dObjectsActive &&
                statisticsBefore.NumberOfReader3dObjectsActive == statisticsAfter.NumberOfReader3dObjectsActive &&
                statisticsBefore.NumberOfWriter3dObjectsActive == statisticsAfter.NumberOfWriter3dObjectsActive)
            {
                return true;
            }

            return false;
        }
    }
}

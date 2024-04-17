// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net_UnitTests
{
    /// <summary>
    /// This class and its attribute is used to enforce serial execution of unit-tests. Serial execution
    /// is required because we check for leaking native objects in some tests by comparing the active object
    /// count before and after the test.
    /// C.f. https://tsuyoshiushio.medium.com/controlling-the-serial-and-parallel-test-on-xunit-6174326da196.
    ///  </summary>
    [CollectionDefinition(NonParallelCollectionDefinitionClass.Name, DisableParallelization = true)]
    public class NonParallelCollectionDefinitionClass
    {
        /// <summary> The name of the definition class.</summary>
        public const string Name = "Non-Parallel Collection";
    }
}

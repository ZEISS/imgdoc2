// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net_UnitTests
{
    using FluentAssertions;
    using ImgDoc2Net.Implementation;
    using ImgDoc2Net.Interfaces;
    using System;
    using System.Collections.Generic;
    using System.Linq;

    public class UtilitiesTest
    {
        [Fact]
        public void CreateDimensionQueryFromStringRepresentationAndCheckResult()
        {
            var dimensionQueryClause = ImgDoc2Net.Implementation.Utilities.CreateFromStringRepresentation("A3,5C3,9X-1,-3");

            var clauses = dimensionQueryClause.EnumConditions().ToList();

            var expectedResult = new List<DimensionCondition>()
            {
                new DimensionCondition(){Dimension=new Dimension('A'), RangeStart=3, RangeEnd=5},
                new DimensionCondition(){Dimension=new Dimension('C'), RangeStart=3, RangeEnd=9},
                new DimensionCondition(){Dimension=new Dimension('X'), RangeStart=-1, RangeEnd=-3},
            };

            clauses.Should().BeEquivalentTo(expectedResult);
        }

        [Fact]
        public void CreateDimensionQueryFromEmptyStringRepresentationAndCheckResult1()
        {
            var dimensionQueryClause = ImgDoc2Net.Implementation.Utilities.CreateFromStringRepresentation("");
            var clauses = dimensionQueryClause.EnumConditions().ToList();

            clauses.Should().BeEquivalentTo(Array.Empty<DimensionCondition>());
        }

        [Theory]
        [InlineData("ABC")]
        [InlineData("A,BC")]
        [InlineData("A3,BC")]
        [InlineData("A3,4B33333333333333333333333,3C")]
        [InlineData("ä3,4")]
        [InlineData("33,4")]
        public void CreateDimensionQueryFromInvalidStringRepresentationAndExpectException(string textRepresentation)
        {
            Assert.Throws<ArgumentException>(() => ImgDoc2Net.Implementation.Utilities.CreateFromStringRepresentation(textRepresentation));
        }
    }
}

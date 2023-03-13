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

        [Theory]
        [InlineData("A5Y4z75343-43@ä+", new string[] { "A5", "Y4", "z75343-43@ä+" })]
        [InlineData("r5Y4z", new string[] { "r5", "Y4", "z" })]
        [InlineData("A5p", new string[] { "A5", "p" })]
        [InlineData("z5", new string[] { "z5" })]
        public void SplitStringAtDimensionIdentifiersAndCheckResult(string text, string[] expectedResult)
        {
            var result = ImgDoc2Net.Implementation.Utilities.SplitStringAtDimensionIdentifiers(text);
            result.Should().Equal(expectedResult);
        }

        [Theory]
        [InlineData("Ökd")]
        [InlineData("5Y4z")]
        [InlineData("5p")]
        public void CallSplitStringAtDimensionIdentifiersWithInvalidStringAndExpectException(string text)
        {
            Assert.Throws<ArgumentException>(() => ImgDoc2Net.Implementation.Utilities.SplitStringAtDimensionIdentifiers(text));
        }

        [Theory]
        [InlineData("z5", new char[] { 'z' }, new int[] { 5 }, new int[] { 5 })]
        [InlineData("z5t6M9", new char[] { 'z', 't', 'M' }, new int[] { 5, 6, 9 }, new int[] { 5, 6, 9 })]
        [InlineData("z5-6", new char[] { 'z' }, new int[] { 5 }, new int[] { 6 })]
        [InlineData("z5-6m-4-5", new char[] { 'z', 'm' }, new int[] { 5, -4 }, new int[] { 6, 5 })]
        [InlineData("z 5 - 6    m -4 -   5  ", new char[] { 'z', 'm' }, new int[] { 5, -4 }, new int[] { 6, 5 })]
        [InlineData("l -5 - -3    m -4 -   -1  ", new char[] { 'l', 'm' }, new int[] { -5, -4 }, new int[] { -3, -1 })]
        [InlineData("  ", new char[] { }, new int[] { }, new int[] { })]
        public void CallParseStringRepresentationOfDimensionAndRangeAndCheckResult(string text, char[] expectedDimensions, int[] expectedStart, int[] expectedEnd)
        {
            var result = ImgDoc2Net.Implementation.Utilities.ParseStringRepresentationOfDimensionAndRange(text);
            var expectedResult = CreateList(expectedDimensions, expectedStart, expectedEnd);
            result.Should().Equal(expectedResult);

            static List<ValueTuple<Dimension, int, int>> CreateList(char[] expectedDimensions, int[] expectedStart, int[] expectedEnd)
            {
                List<ValueTuple<Dimension, int, int>> list = new List<ValueTuple<Dimension, int, int>>(expectedDimensions.Length);
                for (int index = 0; index < expectedDimensions.Length; ++index)
                {
                    list.Add(ValueTuple.Create(new Dimension(expectedDimensions[index]), expectedStart[index], expectedEnd[index]));
                }

                return list;
            }
        }

        [Theory]
        [InlineData("#-5")]
        [InlineData("a5,")]
        [InlineData("a5-,")]
        [InlineData("abc")]
        [InlineData("ab4c3")]
        [InlineData("a4-5.,b4c3")]
        [InlineData("X")]
        [InlineData("X--4")]
        public void CallParseStringRepresentationOfDimensionAndRangeWithInvalidStringAndExpectExceptiopn(string text)
        {
            Assert.Throws<ArgumentException>(() => ImgDoc2Net.Implementation.Utilities.ParseStringRepresentationOfDimensionAndRange(text));
        }

        [Fact]
        public void EnumerateCoordinatesInBoundsAndCheckResult()
        {
            var bounds = ImgDoc2Net.Implementation.Utilities.ParseStringRepresentationOfDimensionAndRange("w0-2x1-2");
            var list = ImgDoc2Net.Implementation.Utilities.EnumerateCoordinatesInBounds(bounds);

            var expectedResult = new List<TileCoordinate>
            {
                new TileCoordinate(new[] {Tuple.Create(new Dimension('w'), 0), Tuple.Create(new Dimension('x'), 1)}),
                new TileCoordinate(new[] {Tuple.Create(new Dimension('w'), 1), Tuple.Create(new Dimension('x'), 1)}),
                new TileCoordinate(new[] {Tuple.Create(new Dimension('w'), 2), Tuple.Create(new Dimension('x'), 1)}),
                new TileCoordinate(new[] {Tuple.Create(new Dimension('w'), 0), Tuple.Create(new Dimension('x'), 2)}),
                new TileCoordinate(new[] {Tuple.Create(new Dimension('w'), 1), Tuple.Create(new Dimension('x'), 2)}),
                new TileCoordinate(new[] {Tuple.Create(new Dimension('w'), 2), Tuple.Create(new Dimension('x'), 2)}),
            };

            list.Should().BeEquivalentTo(expectedResult);
        }

        [Fact]
        public void EnumerateCoordinatesInBoundsWithInvalidCollectionDuplicateDimensionAndExpectException()
        {
            var bounds = ImgDoc2Net.Implementation.Utilities.ParseStringRepresentationOfDimensionAndRange("w0-2w1-2");
            Assert.Throws<ArgumentException>(() => ImgDoc2Net.Implementation.Utilities.EnumerateCoordinatesInBounds(bounds));
        }

        [Fact]
        public void EnumerateCoordinatesInBoundsWithInvalidCollectionEndBeforeStartAndExpectException()
        {
            var bounds = ImgDoc2Net.Implementation.Utilities.ParseStringRepresentationOfDimensionAndRange("w3--2");
            Assert.Throws<ArgumentException>(() => ImgDoc2Net.Implementation.Utilities.EnumerateCoordinatesInBounds(bounds));
        }
    }
}

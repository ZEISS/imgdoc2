// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <cmath>

namespace imgdoc2
{
    /// Determine whether the two specified numbers are "approximately equal".
    /// This function uses the larger of the two values and multiples it by epsilon to determine the margin of error.
    /// See https://www.boost.org/doc/libs/1_36_0/libs/test/doc/html/utf/testing-tools/floating_point_comparison.html .
    /// \tparam t Generic type parameter.
    /// \param a       The first number to compare.
    /// \param b       The second number to compare.
    /// \param epsilon The factor to be multiplied with larger of the two values to give the allowed margin of error.
    /// \returns True if numbers are "approximately equal"; false otherwise.
    template <typename t>
    inline bool approximatelyEqual(t a, t b, t epsilon)
    {
        return std::fabs(a - b) <= ((std::fabs(a) < std::fabs(b) ? std::fabs(b) : std::fabs(a)) * epsilon);
    }

    /// Determine whether the two specified numbers are "essentially equal".
    /// This function uses the smaller of the two values and multiples it by epsilon to determine the margin of error.
    /// See https://www.boost.org/doc/libs/1_36_0/libs/test/doc/html/utf/testing-tools/floating_point_comparison.html .
    /// \tparam t Generic type parameter.
    /// \param a       The first number to compare.
    /// \param b       The second number to compare.
    /// \param epsilon The factor to be multiplied with smaller of the two values to give the allowed margin of error.
    /// \returns True if numbers are "essentially equal"; false otherwise.
    template <typename t>
    inline bool essentiallyEqual(t a, t b, t epsilon)
    {
        return std::fabs(a - b) <= ((std::fabs(a) > std::fabs(b) ? std::fabs(b) : std::fabs(a)) * epsilon);
    }

    /// Test whether the first argument is "definitely" greater than the second, i.e. the difference is greater than the specified epsilon.
    /// \tparam t Generic type parameter.
    /// \param  a       The first number to compare.
    /// \param  b       The second number to compare.
    /// \param  epsilon The factor to be multiplied with smaller of the two values to give the allowed margin of error.
    /// \returns True if first number is greater than second; false otherwise.
    template <typename t>
    inline bool definitelyGreaterThan(t a, t b, t epsilon)
    {
        return (a - b) > ((std::fabs(a) < std::fabs(b) ? std::fabs(b) : std::fabs(a)) * epsilon);
    }

    /// Test whether the first argument is "definitely" less than the second, i.e. the difference is greater than the specified epsilon.
    /// \tparam t Generic type parameter.
    /// \param  a       The first number to compare.
    /// \param  b       The second number to compare.
    /// \param  epsilon The factor to be multiplied with smaller of the two values to give the allowed margin of error.
    /// \returns True if first number is less than second; false otherwise.
    template <typename t>
    inline bool definitelyLessThan(t a, t b, t epsilon)
    {
        return (b - a) > ((std::fabs(a) < std::fabs(b) ? std::fabs(b) : std::fabs(a)) * epsilon);
    }

}

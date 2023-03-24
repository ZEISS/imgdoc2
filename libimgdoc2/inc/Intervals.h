#pragma once

#include <limits>

namespace imgdoc2
{
    /// This structure gathers a minimum and maximum value. If minimum is greater than maximum, this means 
    /// that the bounds is invalid.
    struct DoubleInterval
    {
        double minimum_value{ std::numeric_limits<double>::max() };  ///< The minimum value.
        double maximum_value{ std::numeric_limits<double>::lowest() };  ///< The maximum value.

        /// <summary>   Query if this object is valid. </summary>
        /// <returns>   True if valid, false if not. </returns>
        [[nodiscard]] bool IsValid() const
        {
            return this->minimum_value <= this->maximum_value;
        }

        /// <summary>   Equality operator. </summary>
        /// <remarks>   In case the both are 'not valid', then they are considered equal (irrespective of the actual values). </remarks>
        /// <param name="rhs">  The right hand side. </param>
        /// <returns>   True if the parameters are considered equivalent. </returns>
        bool operator==(const DoubleInterval& rhs) const
        {
            if (this->minimum_value == rhs.minimum_value && this->maximum_value == rhs.maximum_value)
            {
                return true;
            }

            // if both are invalid, then we consider them as equal - otherwise, they are unequal
            return !this->IsValid() && !rhs.IsValid();
        }

        /// <summary>   Inequality operator. </summary>
        /// <remarks>   In case the both are 'not valid', then they are considered equal (irrespective of the actual values). </remarks>
        /// <param name="rhs">  The right hand side. </param>
        /// <returns>   True if the parameters are not considered equivalent. </returns>
        bool operator!=(const DoubleInterval& rhs) const
        {
            return !(*this == rhs);
        }
    };

    /// This structure gathers a minimum and maximum value. If minimum is greater than maximum, this means 
    /// that the bounds is invalid.
    struct Int32Interval
    {
        std::int32_t minimum_value{ std::numeric_limits<std::int32_t>::max() };  ///< The minimum value.
        std::int32_t maximum_value{ std::numeric_limits<std::int32_t>::min() };  ///< The maximum value.

        /// <summary>   Query if this object is valid. </summary>
        /// <returns>   True if valid, false if not. </returns>
        [[nodiscard]] bool IsValid() const
        {
            return this->minimum_value <= this->maximum_value;
        }

        /// <summary>   Equality operator. </summary>
        /// <remarks>   In case the both are 'not valid', then they are considered equal (irrespective of the actual values). </remarks>
        /// <param name="rhs">  The right hand side. </param>
        /// <returns>   True if the parameters are considered equivalent. </returns>
        bool operator==(const Int32Interval& rhs) const
        {
            if (this->minimum_value == rhs.minimum_value && this->maximum_value == rhs.maximum_value)
            {
                return true;
            }

            // if both are invalid, then we consider them as equal - otherwise, they are unequal
            return !this->IsValid() && !rhs.IsValid();
        }

        /// <summary>   Inequality operator. </summary>
        /// <remarks>   In case the both are 'not valid', then they are considered equal (irrespective of the actual values). </remarks>
        /// <param name="rhs">  The right hand side. </param>
        /// <returns>   True if the parameters are not considered equivalent. </returns>
        bool operator!=(const Int32Interval& rhs) const
        {
            return !(*this == rhs);
        }
    };
}  // namespace imgdoc2

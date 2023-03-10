// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interfaces
{
    /// <summary> 
    /// The options for a query operation.
    /// </summary>
    public class QueryOptions
    {
        /// <summary> The default maximum number of results.</summary>
        public const int DefaultMaxNumberOfResults = 256;

        /// <summary> Gets or sets the maximum numbers of results.</summary>
        /// <value> The maximum numbers of results.</value>
        public int MaxNumbersOfResults { get; set; } = DefaultMaxNumberOfResults;

        /// <summary>   
        /// Gets or sets a value indicating whether the result was complete.
        /// This property will be set by the query-operation, and it indicates whether
        /// all results could be retrieved.
        /// </summary>
        /// <remarks>
        /// I am not expecting this to be a problem, I mean - that we provide a reasonable max number of results.
        /// If this turns out to be a problem, it would of course be possible to provide a better interop-implementation
        /// here.
        /// </remarks>
        /// <value type="bool"> True if the result set is complete, false if not. </value>
        public bool ResultWasComplete { get; set; }
    }
}

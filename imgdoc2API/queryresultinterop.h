// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <imgdoc2.h>

/// This struct is used to return the result of are IDocRead2d-query call.
/// On input, the property "element_count" specifies the size of the struct,
/// i.e. how many elements it can hold (or: how big the allocated memory is
/// so that it can hold indices). On return, "element_count" gives the number
/// of actual indices present.
/// The property "more_results_available" indicates whether the capacity was
/// sufficient, i.e. if there would have been more results available (which do
/// not fit into the structure).
#pragma pack(push, 4)
struct QueryResultInterop
{
    std::uint32_t element_count;    ///< On input, the number of elements for which space is allocated, on output the number of valid elements (always smaller than the input).
    std::uint32_t more_results_available;   ///< If 0 it means that all results could be placed into this structure, otherwise not all the results could be fitted in.
    imgdoc2::dbIndex indices[];
};
#pragma  pack(pop)

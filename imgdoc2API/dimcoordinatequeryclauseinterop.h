// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <imgdoc2.h>

#pragma pack(push, 1)
struct DimensionQueryRangeClauseInterop
{
    imgdoc2::Dimension dimension;
    int start;
    int end;
};

struct DimensionQueryClauseInterop
{
    int element_count;
    DimensionQueryRangeClauseInterop query_range_clause[];
};
#pragma pack(pop)

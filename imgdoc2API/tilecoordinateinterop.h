// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <imgdoc2.h>

#pragma pack(push, 4)
struct DimensionAndValueInterop
{
    imgdoc2::Dimension dimension;
    int value;
};
#pragma pack(pop)

#pragma pack(push, 4)
struct TileCoordinateInterop
{
    int number_of_elements;
    DimensionAndValueInterop values[];
};
#pragma pack(pop)

// SPDX-FileCopyrightText: 2024 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include "allocationobject.h"

#pragma pack(push, 4)
struct DecodedImageResultInterop
{
    std::uint32_t stride;
    AllocationObject bitmap;
};
#pragma pack(pop)

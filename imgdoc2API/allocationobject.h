// SPDX-FileCopyrightText: 2024 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdint>

#pragma pack(push, 4)
/// This struct is used to model dynamic memory allocations done on the caller site.
/// The idea is that a function pointer is passed into imgdoc2API, which is then called
/// to do an allocation. This callback then returns this structure, where the pointer_to_memory
/// is the pointer to the allocated memory (and this will be used by imgdoc2API), and the handle
/// is an opaque value to imgdoc2API.
struct AllocationObject
{
    void* pointer_to_memory;    ///< Pointer to the allocated memory.
    std::intptr_t handle;       ///< An opaque value to imgdoc2API.
};
#pragma pack(pop)

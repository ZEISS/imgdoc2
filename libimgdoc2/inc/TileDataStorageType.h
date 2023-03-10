// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

namespace imgdoc2
{
  enum class TileDataStorageType : std::uint8_t
  {
    Invalid = 0,
    BlobInDatabase = 1,
  };
}

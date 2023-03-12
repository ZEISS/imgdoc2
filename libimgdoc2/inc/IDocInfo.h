// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <vector>
#include <map>
#include <cstdint>

namespace imgdoc2
{
    struct CoordinateBounds
    {
        int minimum_value;
        int maximum_value;
    };

    class IDocInfo
    {
    public:
        virtual ~IDocInfo() = default;

        virtual void GetTileDimensions(imgdoc2::Dimension* dimensions, std::uint32_t& count) = 0;

        virtual std::map<imgdoc2::Dimension, imgdoc2::CoordinateBounds> GetMinMaxForTileDimension(const std::vector<imgdoc2::Dimension>& dimensions_to_query_for) = 0;

    public:
        std::vector<imgdoc2::Dimension> GetTileDimensions()
        {
            std::uint32_t count = 0;
            this->GetTileDimensions(nullptr, count);
            std::vector<imgdoc2::Dimension> dimensions(count);
            this->GetTileDimensions(dimensions.data(), count);
            return dimensions;
        }
    };
}

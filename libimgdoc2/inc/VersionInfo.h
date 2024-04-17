// SPDX-FileCopyrightText: 2024 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdint>
#include <string>

namespace imgdoc2
{
    /// This structure gathers build-time information about the version of the library.
    struct VersionInfo
    {
        std::uint32_t major{ 0 };   ///< The major version number. We use the semantic versioning scheme (https://semver.org/).
        std::uint32_t minor{ 0 };   ///< The minor version number. We use the semantic versioning scheme (https://semver.org/).
        std::uint32_t patch{ 0 };   ///< The patch version number. We use the semantic versioning scheme (https://semver.org/).

        /// The compiler identification. This is a free-form string.
        std::string compiler_identification;

        std::string build_type;

        /// The URL of the repository - if available.
        std::string repository_url;

        /// The branch - if available.
        std::string repository_branch;

        /// The tag or hash of the repository - if available.
        std::string repository_tag;
    };
}

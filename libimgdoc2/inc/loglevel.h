// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

namespace imgdoc2
{
    /// The log levels are defined here (see also the IHostingEnvironment interface).
    class LogLevel
    {
    public:
        static constexpr int Fatal = 0;
        static constexpr int Error = 1;
        static constexpr int Warn = 2;
        static constexpr int Info = 3;
        static constexpr int Trace = 4;
        static constexpr int Debug = 5;

        static constexpr int Sql = 32;  ///< This level will log all SQL-statement sent to the database.
    };
}

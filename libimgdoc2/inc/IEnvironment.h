// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

namespace imgdoc2
{
    /// In this class functionality "to be provided by a hosting environment" ist gathered.
    /// The prime example is "debug logging", which is a cross-cutting concern, and it is allowed
    /// to route debug print to the hosting environment.
    class IHostingEnvironment
    {
    public:
        /// This method is used for debug output. It may be called concurrently.
        /// For possible values of the log level, see the constants "LogLevel".
        ///
        /// \param  level   The log level (c.f. constants "LogLevel..." for possible values).
        /// \param  message The message text (in UTF8-encoding).
        virtual void Log(int level, const char* message) = 0;

        /// Queries if the specified log level is active. This method may be called concurrently.
        /// For possible values of the log level, see the constants "LogLevel".
        /// \param  level The log level.
        /// \returns True if the log level is active, false if not.
        virtual bool IsLogLevelActive(int level) = 0;

        /// Report fatal error and terminate the application. This is to be used for fatal error,
        /// where no recovery is possible. Obviously, this should be the last resort.
        /// \param  message The message.
        virtual void ReportFatalErrorAndExit(const char* message) = 0;

        virtual ~IHostingEnvironment() = default;
    public:
        // no copy and no move
        IHostingEnvironment() = default;
        IHostingEnvironment(const IHostingEnvironment&) = delete;             // copy constructor
        IHostingEnvironment& operator=(const IHostingEnvironment&) = delete;  // copy assignment
        IHostingEnvironment(IHostingEnvironment&&) = delete;                  // move constructor
        IHostingEnvironment& operator=(IHostingEnvironment&&) = delete;       // move assignment
    };
}

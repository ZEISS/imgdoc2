// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <memory>
#include "ICreateOptions.h"
#include "IOpenExistingOptions.h"
#include "IEnvironment.h"
#include "VersionInfo.h"

namespace imgdoc2
{
    class ICreateOptions;
    class IDoc;

    /// Class factory creating objects implemented in the imgdoc2-library.
    class ClassFactory
    {
    public:
        /// Gets built-time information about the libimgdoc2 library.
        ///
        /// \returns    The version information.
        static VersionInfo GetVersionInfo();

        /// Creates an options-object for creating a new imgdoc2-document.
        /// \returns    Pointer to a newly create options-object.
        static imgdoc2::ICreateOptions* CreateCreateOptionsPtr();

        /// Creates an options-object for creating a new imgdoc2-document.
        /// \returns    Unique-pointer of a newly create options-object.
        static std::unique_ptr<imgdoc2::ICreateOptions> CreateCreateOptionsUp();

        /// Creates an options-object for creating a new imgdoc2-document.
        /// \returns    Shared-pointer of a newly created options-object.
        static std::shared_ptr<imgdoc2::ICreateOptions> CreateCreateOptionsSp();

        /// Creates an instance of the options-object for opening an existing file.
        /// \returns Null if it fails, else the newly created "open existing options" object.
        static imgdoc2::IOpenExistingOptions* CreateOpenExistingOptions();

        /// Creates an instance of the options-object for opening an existing file.
        /// \returns Unique-pointer of a newly create options-object.
        static std::unique_ptr<imgdoc2::IOpenExistingOptions> CreateOpenExistingOptionsUp();

        /// Creates an instance of the options-object for opening an existing file.
        /// \returns Shared-pointer of a newly create options-object.
        static std::shared_ptr<imgdoc2::IOpenExistingOptions> CreateOpenExistingOptionsSp();

        /// Creates a "standard" hosting environment.
        /// \returns The newly created "standard" hosting environment object.
        static std::shared_ptr<IHostingEnvironment> CreateStandardHostingEnvironment();

        /// Creates a "null" hosting environment - which does nothing for logging.
        /// \returns The newly created null hosting environment object.
        static std::shared_ptr<IHostingEnvironment> CreateNullHostingEnvironment();

        /// Creates hosting environment which routes the methods to the provided plan C-function pointers.
        /// \param          userparam                  The user-parameter (which is passed to the functions).
        /// \param [in]     pfnLog                     If non-null, function for the Log method.
        /// \param [in]     pfnIsLevelActive           If non-null, function for the IsLogLevelActive method.
        /// \param [in]     pfnReportFatalErrorAndExit If non-null, function for the ReportFatalErrorAndExit method.
        /// \returns The newly created hosting environment object (using the provided function pointers).
        static std::shared_ptr<IHostingEnvironment> CreateHostingEnvironmentForFunctionPointers(
            std::intptr_t userparam,
            void (*pfnLog)(std::intptr_t userparam, int level, const char* szMessage),
            bool (*pfnIsLevelActive)(std::intptr_t userparam, int level),
            void (*pfnReportFatalErrorAndExit)(std::intptr_t userparam, const char* szMessage));

        /// Creates a new imgdoc2-document. A new database is constructed.
        /// \param [in] create_options  The options controlling the operation.
        /// \param      environment     (Optional) The hosting environment object.
        /// \returns    The newly created imgdoc2-document.
        static std::shared_ptr<imgdoc2::IDoc> CreateNew(imgdoc2::ICreateOptions* create_options, std::shared_ptr<IHostingEnvironment> environment = nullptr);

        /// Creates a new imgdoc2-document for an existing file.
        /// \param [in] open_existing_options   The options controlling the operation.
        /// \param      environment             (Optional) The hosting environment object.
        /// \returns    The newly created imgdoc2-document.
        static std::shared_ptr<imgdoc2::IDoc> OpenExisting(imgdoc2::IOpenExistingOptions* open_existing_options, std::shared_ptr<IHostingEnvironment> environment = nullptr);
    };
}

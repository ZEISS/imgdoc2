// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include "ClassFactory.h"

#include <charconv>
#include <cstdlib>
#include "../doc/document.h"
#include "../src/db/DbFactory.h"
#include "../src/db/database_creator.h"
#include "../src/db/database_discovery.h"

#include <libimgdoc2_config.h>

using namespace std;
using namespace imgdoc2;

class StandardHostingEnvironment : public IHostingEnvironment
{
public:
    void Log(int level, const char* message) override
    {
        // TODO(JBL): on Windows, let's use DebugOutputString,
        //            on Linux - I am not sure what to do
    }

    bool IsLogLevelActive(int level) override
    {
        return true;
    }

    void ReportFatalErrorAndExit(const char* message) override
    {
        abort();
    }
};

class NullHostingEnvironment : public IHostingEnvironment
{
public:
    void Log(int level, const char* message) override
    {
    }

    bool IsLogLevelActive(int level) override
    {
        return false;
    }

    void ReportFatalErrorAndExit(const char* message) override
    {
        abort();
    }
};

class HostingEnvironmentOnFunctionPointers : public IHostingEnvironment
{
private:
    std::intptr_t userparam_;
    void (*pfnLog_)(std::intptr_t userparam, int level, const char* szMessage);
    bool (*pfnIsLevelActive_)(std::intptr_t userparam, int level);
    void (*pfnReportFatalErrorAndExit_)(std::intptr_t userparam, const char* szMessage);
public:
    explicit HostingEnvironmentOnFunctionPointers(std::intptr_t userparam,
        void (*pfnLog)(std::intptr_t userparam, int level, const char* szMessage),
        bool (*pfnIsLevelActive)(std::intptr_t userparam, int level),
        void (*pfnReportFatalErrorAndExit)(std::intptr_t userparam, const char* szMessage))
        : userparam_(userparam),
        pfnLog_(pfnLog),
        pfnIsLevelActive_(pfnIsLevelActive),
        pfnReportFatalErrorAndExit_(pfnReportFatalErrorAndExit)
    {}

    void Log(int level, const char* message) override
    {
        if (this->pfnLog_ != nullptr)
        {
            this->pfnLog_(this->userparam_, level, message);
        }
    }

    bool IsLogLevelActive(int level) override
    {
        if (this->pfnIsLevelActive_ != nullptr)
        {
            return this->pfnIsLevelActive_(this->userparam_, level);
        }

        return false;
    }

    void ReportFatalErrorAndExit(const char* message) override
    {
        if (this->pfnReportFatalErrorAndExit_ != nullptr)
        {
            this->pfnReportFatalErrorAndExit_(this->userparam_, message);
        }

        abort();
    }
};

//------------------------------------------------------------------------

static std::uint32_t convert_to_uint32(const char* str)
{
    std::uint32_t result = 0;
    auto [ptr, ec] = std::from_chars(str, str + strlen(str), result);
    if (ec == std::errc::invalid_argument || ec == std::errc::result_out_of_range || *ptr != '\0')
    {
        return (numeric_limits<uint32_t>::max)();
    }

    return result;
}

/*static*/VersionInfo imgdoc2::ClassFactory::GetVersionInfo()
{
    VersionInfo version_info;
    version_info.major = convert_to_uint32(LIBIMGDOC2_VERSION_MAJOR);
    version_info.minor = convert_to_uint32(LIBIMGDOC2_VERSION_MINOR);
    version_info.patch = convert_to_uint32(LIBIMGDOC2_VERSION_PATCH);
    version_info.compiler_identification = LIBIMGDOC2_CXX_COMPILER_IDENTIFICATION;
    version_info.build_type = LIBIMGDOC2_BUILD_TYPE;
    version_info.repository_url = LIBIMGDOC2_REPOSITORYREMOTEURL;
    version_info.repository_branch = LIBIMGDOC2_REPOSITORYBRANCH;
    version_info.repository_tag = LIBIMGDOC2_REPOSITORYHASH;

    return version_info;
}

/*static*/std::shared_ptr<imgdoc2::IDoc> imgdoc2::ClassFactory::CreateNew(imgdoc2::ICreateOptions* create_options, std::shared_ptr<IHostingEnvironment> environment)
{
    // TODO(JBL): here would be the place where we'd allow for "other databases than Sqlite", for the time being,
    //            we just deal with Sqlite here
    auto db_connection = DbFactory::SqliteCreateNewDatabase(create_options->GetFilename().c_str(), environment);

    // check pre-conditions
    // TODO(JBL)

    // tweak settings
    switch (create_options->GetDocumentType())  // NOLINT(clang-diagnostic-switch)
    {
        case DocumentType::kImage2d:
        {
            DbCreator db_creator(db_connection);
            const auto database_configuration_2d = db_creator.CreateTables2d(create_options);

            if (database_configuration_2d)
            {
                return make_shared<Document>(db_connection, database_configuration_2d);
            }

            break;
        }
        case DocumentType::kImage3d:
        {
            DbCreator db_creator(db_connection);
            const auto database_configuration_3d = db_creator.CreateTables3d(create_options);

            if (database_configuration_3d)
            {
                return make_shared<Document>(db_connection, database_configuration_3d);
            }

            break;
        }
        case DocumentType::kInvalid:
            break;
    }

    return {};
}

/*static*/std::shared_ptr<imgdoc2::IDoc> imgdoc2::ClassFactory::OpenExisting(imgdoc2::IOpenExistingOptions* open_existing_options, std::shared_ptr<IHostingEnvironment> environment)
{
    // TODO(JBL): here would be the place where we'd allow for "other databases than Sqlite", for the time being,
    //            we just deal with Sqlite here
    auto db_connection = DbFactory::SqliteOpenExistingDatabase(open_existing_options->GetFilename().c_str(), open_existing_options->GetOpenReadonly(), environment);

    DbDiscovery database_discovery{ db_connection };
    database_discovery.DoDiscovery();

    const auto database_configuration_2d = database_discovery.GetDatabaseConfiguration2DOrNull();
    if (database_configuration_2d)
    {
        return make_shared<Document>(db_connection, database_configuration_2d);
    }

    const auto database_configuration_3d = database_discovery.GetDatabaseConfiguration3DOrNull();
    if (database_configuration_3d)
    {
        return make_shared<Document>(db_connection, database_configuration_3d);
    }

    return {};
}

/*static*/std::shared_ptr<IHostingEnvironment> imgdoc2::ClassFactory::CreateStandardHostingEnvironment()
{
    return make_shared<StandardHostingEnvironment>();
}

/*static*/std::shared_ptr<IHostingEnvironment> imgdoc2::ClassFactory::CreateNullHostingEnvironment()
{
    return make_shared<NullHostingEnvironment>();
}

/*static*/std::shared_ptr<IHostingEnvironment> imgdoc2::ClassFactory::CreateHostingEnvironmentForFunctionPointers(
    std::intptr_t userparam,
    void (*pfnLog)(std::intptr_t userparam, int level, const char* szMessage),
    bool (*pfnIsLevelActive)(std::intptr_t userparam, int level),
    void (*pfnReportFatalErrorAndExit)(std::intptr_t userparam, const char* szMessage))
{
    return make_shared<HostingEnvironmentOnFunctionPointers>(
        userparam,
        pfnLog,
        pfnIsLevelActive,
        pfnReportFatalErrorAndExit);
}

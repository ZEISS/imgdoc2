// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include <string>
#include <unordered_set>
#include <imgdoc2.h>

using namespace imgdoc2;

class OpenExistingOptions : public imgdoc2::IOpenExistingOptions
{
private:
    std::string     filename_;
    bool            read_only_{false};
public:
    OpenExistingOptions() = default;

    void SetFilename(const char* filename) override
    {
        this->filename_ = filename;
    }

    void SetOpenReadonly(bool read_only) override
    {
        this->read_only_ = read_only;
    }

    [[nodiscard]] bool GetOpenReadonly() const override
    {
        return this->read_only_;
    }

    [[nodiscard]] const std::string& GetFilename() const override
    {
        return this->filename_;
    }
};

/*static*/IOpenExistingOptions* imgdoc2::ClassFactory::CreateOpenExistingOptions()
{
    return new OpenExistingOptions();
}

// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include <imgdoc2.h>
#include <string>
#include <unordered_set>
#include <sstream>

using namespace std;
using namespace imgdoc2;

class CreateOptions : public imgdoc2::ICreateOptions
{
private:
    imgdoc2::DocumentType document_type_ = imgdoc2::DocumentType::kImage2d;
    std::string     filename_;
    std::unordered_set<Dimension> dimensions_;
    std::unordered_set<Dimension> dimensionsToIndex_;
    bool            use_spatial_index_ = false;
    bool            create_blob_table_ = false;
public:
    CreateOptions() = default;

    void SetDocumentType(imgdoc2::DocumentType document_type) override
    {
        this->document_type_ = document_type;
    }

    [[nodiscard]] imgdoc2::DocumentType GetDocumentType() const override
    {
        return this->document_type_;
    }

    void SetFilename(const char* filename) override
    {
        this->filename_ = filename;
    }

    [[nodiscard]] const std::string& GetFilename() const override
    {
        return this->filename_;
    }

    void SetUseSpatialIndex(bool use_spatial_index) override
    {
        this->use_spatial_index_ = use_spatial_index;
    }

    void SetCreateBlobTable(bool create_blob_table) override
    {
        this->create_blob_table_ = create_blob_table;
    }

    [[nodiscard]] bool GetUseSpatialIndex() const override
    {
        return this->use_spatial_index_;
    }

    void AddDimension(Dimension dim) override
    {
        ThrowIfDimensionInvalid(dim);
        this->dimensions_.emplace(dim);
    }

    void AddIndexForDimension(Dimension dim) override
    {
        ThrowIfDimensionInvalid(dim);
        this->dimensionsToIndex_.emplace(dim);
    }

    [[nodiscard]] const std::unordered_set<Dimension>& GetDimensions() const override
    {
        return this->dimensions_;
    }

    [[nodiscard]] const std::unordered_set<imgdoc2::Dimension>& GetIndexedDimensions() const override
    {
        return this->dimensionsToIndex_;
    }

    [[nodiscard]] bool GetCreateBlobTable() const override
    {
        return this->create_blob_table_;
    }
};

/*static*/ICreateOptions* imgdoc2::ClassFactory::CreateCreateOptionsPtr()
{
    return new CreateOptions();
}

/*static*/std::unique_ptr<imgdoc2::ICreateOptions> imgdoc2::ClassFactory::CreateCreateOptionsUp()
{
    return make_unique<CreateOptions>();
}

/*static*/std::shared_ptr<imgdoc2::ICreateOptions> imgdoc2::ClassFactory::CreateCreateOptionsSp()
{
    return make_shared<CreateOptions>();
}

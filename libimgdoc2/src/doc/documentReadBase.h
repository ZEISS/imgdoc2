#pragma once

#include <unordered_set>
#include <utility>
#include "document.h"

/// This class contains common functionality and utilities for implementing the document-read-access classes.
class DocumentReadBase
{
private:
    std::shared_ptr<Document> document_;
protected:
    explicit DocumentReadBase(std::shared_ptr<Document> document) : document_(std::move(document))
    {}

    static void GetEntityDimensionsInternal(const std::unordered_set<imgdoc2::Dimension>& tile_dimensions, imgdoc2::Dimension* dimensions, std::uint32_t& count);

    [[nodiscard]] const std::shared_ptr<Document>& GetDocument() const { return this->document_; }
    [[nodiscard]] const std::shared_ptr<imgdoc2::IHostingEnvironment>& GetHostingEnvironment() const { return this->document_->GetHostingEnvironment(); }
};

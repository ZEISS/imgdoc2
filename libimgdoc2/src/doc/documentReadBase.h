#pragma once

#include "document.h"

/// This class contains common functionality and utilities for implementing the document-read-access classes.
class DocumentReadBase
{
private:
    std::shared_ptr<Document> document_;
protected:
    explicit DocumentReadBase(std::shared_ptr<Document> document) : document_(std::move(document))
    {}

    [[nodiscard]] const std::shared_ptr<Document>& GetDocument() const { return this->document_; }
    [[nodiscard]] const std::shared_ptr<imgdoc2::IHostingEnvironment>& GetHostingEnvironment() const { return this->document_->GetHostingEnvironment(); }
};

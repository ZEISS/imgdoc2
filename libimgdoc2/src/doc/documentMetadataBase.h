#pragma once

#include <limits>
#include <vector>
#include <optional>
#include <memory>
#include <type_traits>
#include "IDocumentMetadata.h"
#include "../db/IDbStatement.h"
#include "document.h"

class DocumentMetadataBase
{
protected:
    std::shared_ptr<Document> document_;

    explicit DocumentMetadataBase(std::shared_ptr<Document> document) : document_(std::move(document))
    {}

    /// Values that are used for the field "TypeDiscriminator" in the database table.
    enum class DatabaseDataTypeValue : int
    {
        invalid = std::numeric_limits< std::underlying_type_t<DatabaseDataTypeValue>>::max(),
        null = 0,
        int32 = 1,
        uint32 = 2,
        doublefloat = 3,
        singlefloat = 4,
        utf8string = 5,
        json = 6,
    };

    //void BindAncestorId(const std::shared_ptr<IDbStatement>& database_statement, int binding_index, const std::optional<imgdoc2::dbIndex>& parent);
    int BindTypeDiscriminatorAndData(const std::shared_ptr<IDbStatement>& database_statement, int binding_index, DatabaseDataTypeValue type, const imgdoc2::IDocumentMetadata::metadata_item_variant& value);

    static DatabaseDataTypeValue DetermineDatabaseDataTypeValueOrThrow(imgdoc2::DocumentMetadataType type, const imgdoc2::IDocumentMetadata::metadata_item_variant& value);
    static DatabaseDataTypeValue DetermineDatabaseDataTypeValue(imgdoc2::DocumentMetadataType type, const imgdoc2::IDocumentMetadata::metadata_item_variant& value);

    std::vector<imgdoc2::dbIndex> GetNodeIdsForPath(const std::string& path);
private:
    std::shared_ptr<IDbStatement> CreateQueryForNodeIdsForPath(const std::vector<std::string_view>& path_parts);
};

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

    /// Parse the path string, i.e. split it at the delimiter '/'. The number of parts is returned in
    /// 'count_of_parts_in_path' if it is not null. Then the database is queried, and we return a vector
    /// corresponding to the parts of the path. The vector contains the primary key for those nodes. If
    /// a part cannot be found, then the query stops at this point (and the vector returned contains less
    /// elements than the number of parts in the path). So, only if the complete path can be resolved, the
    /// size of the returned vector is equal to the number of parts in the path.
    ///
    /// \param          path                    The path to be mapped.
    /// \param [in,out] count_of_parts_in_path  If non-null, the parts as determined by the path string.
    ///
    /// \returns    The primary keys of the nodes which could be mapped.
    std::vector<imgdoc2::dbIndex> GetNodeIdsForPath(const std::string& path, size_t* count_of_parts_in_path);

    bool TryMapPathAndGetTerminalNode(const std::string& path,imgdoc2::dbIndex* terminal_node_id);
private:
    std::shared_ptr<IDbStatement> CreateQueryForNodeIdsForPath(const std::vector<std::string_view>& path_parts);
};

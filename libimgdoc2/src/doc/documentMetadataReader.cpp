#include "documentMetadataReader.h"

using namespace std;
using namespace imgdoc2;

/*virtual*/imgdoc2::DocumentMetadataItem DocumentMetadataReader::GetItem(imgdoc2::dbIndex idx, imgdoc2::DocumentMetadataItemFlags flags)
{
    auto statement = this->CreateStatementForRetrievingItem(flags);
    statement->BindInt64(1, idx);

    if (!this->document_->GetDatabase_connection()->StepStatement(statement.get()))
    {
        // this means that the tile with the specified index ('idx') was not found
        ostringstream ss;
        ss << "Request for reading a non-existing item (with pk=" << idx << ")";
        throw non_existing_tile_exception(ss.str(), idx);
    }

    DocumentMetadataItem item = this->RetrieveDocumentMetadataItemFromStatement(statement, flags);
    /*DocumentMetadataItem item;
    if ((flags & DocumentMetadataItemFlags::NameValid) == DocumentMetadataItemFlags::NameValid)
    {
        item.name = statement->GetResultString(0);
    }

    if ((flags & DocumentMetadataItemFlags::DocumentMetadataTypeAndValueValid) == DocumentMetadataItemFlags::DocumentMetadataTypeAndValueValid)
    {
        const auto database_item_type_value = statement->GetResultInt32(1);
        switch (database_item_type_value)
        {
            case DatabaseDataTypeValue::null:
                item.value = std::monostate();
                item.type = DocumentMetadataType::Null;
                break;
            case DatabaseDataTypeValue::int32:
                item.value = IDocumentMetadataWrite::metadata_item_variant(statement->GetResultInt32(3));
                item.type = DocumentMetadataType::Int32;
                break;
            case DatabaseDataTypeValue::doublefloat:
                item.value = IDocumentMetadataWrite::metadata_item_variant(statement->GetResultDouble(2));
                item.type = DocumentMetadataType::Double;
                break;
            case DatabaseDataTypeValue::utf8string:
                item.value = IDocumentMetadataWrite::metadata_item_variant(statement->GetResultString(4));
                item.type = DocumentMetadataType::Text;
                break;
            case DatabaseDataTypeValue::json:
                item.value = IDocumentMetadataWrite::metadata_item_variant(statement->GetResultString(4));
                item.type = DocumentMetadataType::Json;
                break;
            default:
                throw runtime_error("DocumentMetadataReader::GetItem: Unknown data type");
        }
    }*/

    return item;
}

/*virtual*/imgdoc2::DocumentMetadataItem DocumentMetadataReader::GetItemForPath(const std::string& path, imgdoc2::DocumentMetadataItemFlags flags)
{
    imgdoc2::dbIndex idx;
    const bool success = this->TryMapPathAndGetTerminalNode(path, &idx);
    if (success)
    {
        return this->GetItem(idx, flags);
    }

    throw runtime_error("Error in DocumentMetadataReader::GetItemForPath");
}

void DocumentMetadataReader::EnumerateItems(
  std::optional<imgdoc2::dbIndex> parent,
  bool recursive,
  DocumentMetadataItemFlags flags,
  std::function<bool(imgdoc2::dbIndex, const DocumentMetadataItem& item)> callback)
{
    const auto statement = this->CreateStatementForEnumerateAllItemsWithAncestorAndDataBind(recursive, parent);

    while (this->document_->GetDatabase_connection()->StepStatement(statement.get()))
    {
        const imgdoc2::dbIndex index = statement->GetResultInt64(0);
        DocumentMetadataItem document_metadata_item = this->RetrieveDocumentMetadataItemFromStatement(statement, flags);
        const bool continue_operation = callback(index, document_metadata_item);
        if (!continue_operation)
        {
            break;
        }
    }
}

void DocumentMetadataReader::EnumerateItemsForPath(
  const std::string& path,
  bool recursive,
  DocumentMetadataItemFlags flags,
  std::function<bool(imgdoc2::dbIndex, const DocumentMetadataItem& item)> callback)
{
    throw runtime_error("DocumentMetadataReader::EnumerateItems");
}

std::shared_ptr<IDbStatement> DocumentMetadataReader::CreateStatementForRetrievingItem(imgdoc2::DocumentMetadataItemFlags flags)
{
    ostringstream string_stream;
    string_stream << "SELECT Pk, Name,TypeDiscriminator,ValueDouble,ValueInteger,ValueString FROM [" << "METADATA" << "] WHERE " <<
        "[" << "Pk" << "] = ?1;";
    auto statement = this->document_->GetDatabase_connection()->PrepareStatement(string_stream.str());
    return statement;
}

std::shared_ptr<IDbStatement> DocumentMetadataReader::CreateStatementForEnumerateAllItemsWithAncestorAndDataBind(bool recursive, std::optional<imgdoc2::dbIndex> parent)
{
    const bool parent_has_value = parent.has_value();
    ostringstream string_stream;

    if (recursive)
    {
        string_stream <<
            "WITH RECURSIVE cte AS(" <<
            "SELECT Pk, Name, AncestorId, TypeDiscriminator, ValueDouble, ValueInteger, ValueString  " <<
            "FROM METADATA ";

        if (parent_has_value)
        {
            string_stream << "WHERE AncestorId = ?1 ";
        }
        else
        {
            string_stream << "WHERE AncestorId IS NULL ";
        }

        string_stream <<
            "UNION ALL " <<
            "SELECT c.Pk, c.Name, c.AncestorId,c.TypeDiscriminator, c.ValueDouble, c.ValueInteger, c.ValueString " <<
            "FROM METADATA c " <<
            "JOIN cte ON c.AncestorId = cte.Pk " <<
            ") " <<
            "SELECT Pk, Name,TypeDiscriminator, ValueDouble, ValueInteger, ValueString FROM cte;";
    }
    else
    {
        string_stream <<
            "SELECT Pk, Name, TypeDiscriminator, ValueDouble, ValueInteger, ValueString FROM METADATA ";

        if (parent_has_value)
        {
            string_stream << "WHERE AncestorId = ?1 ";
        }
        else
        {
            string_stream << "WHERE AncestorId IS NULL ";
        }
    }

    auto statement = this->document_->GetDatabase_connection()->PrepareStatement(string_stream.str());
    if (parent_has_value)
    {
        statement->BindInt64(1, parent.value());
    }

    return statement;
}

imgdoc2::DocumentMetadataItem DocumentMetadataReader::RetrieveDocumentMetadataItemFromStatement(const std::shared_ptr<IDbStatement>& statement, imgdoc2::DocumentMetadataItemFlags flags)
{
    DocumentMetadataItem item;
    if ((flags & DocumentMetadataItemFlags::PrimaryKeyValid) == DocumentMetadataItemFlags::PrimaryKeyValid)
    {
        item.primary_key = statement->GetResultInt64(0);
    }

    if ((flags & DocumentMetadataItemFlags::NameValid) == DocumentMetadataItemFlags::NameValid)
    {
        item.name = statement->GetResultString(1);
    }

    if ((flags & DocumentMetadataItemFlags::DocumentMetadataTypeAndValueValid) == DocumentMetadataItemFlags::DocumentMetadataTypeAndValueValid)
    {
        const auto database_item_type_value = statement->GetResultInt32(2);
        switch (static_cast<DatabaseDataTypeValue>(database_item_type_value))
        {
            case DatabaseDataTypeValue::null:
                item.value = std::monostate();
                item.type = DocumentMetadataType::Null;
                break;
            case DatabaseDataTypeValue::int32:
                item.value = IDocumentMetadataWrite::metadata_item_variant(statement->GetResultInt32(4));
                item.type = DocumentMetadataType::Int32;
                break;
            case DatabaseDataTypeValue::doublefloat:
                item.value = IDocumentMetadataWrite::metadata_item_variant(statement->GetResultDouble(3));
                item.type = DocumentMetadataType::Double;
                break;
            case DatabaseDataTypeValue::utf8string:
                item.value = IDocumentMetadataWrite::metadata_item_variant(statement->GetResultString(5));
                item.type = DocumentMetadataType::Text;
                break;
            case DatabaseDataTypeValue::json:
                item.value = IDocumentMetadataWrite::metadata_item_variant(statement->GetResultString(5));
                item.type = DocumentMetadataType::Json;
                break;
            default:
                throw runtime_error("DocumentMetadataReader::GetItem: Unknown data type");
        }
    }
    return item;
}

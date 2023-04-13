// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include "documentMetadataReader.h"
#include <string>

using namespace std;
using namespace imgdoc2;

/*virtual*/imgdoc2::DocumentMetadataItem DocumentMetadataReader::GetItem(imgdoc2::dbIndex primary_key, imgdoc2::DocumentMetadataItemFlags flags)
{
    DocumentMetadataItem item;

    // special case: if no flags are specified, we should just check if the item exists
    if (flags == DocumentMetadataItemFlags::None)
    {
        const bool exists = this->CheckIfItemExists(primary_key);
        if (!exists)
        {
            ostringstream ss;
            ss << "The requested item (with pk=" << primary_key << ") does not exist";
            throw non_existing_item_exception(ss.str(), primary_key);
        }

        return item;
    }

    // check if we have to "retrieve data from the item" (i.e. if the caller wants to have the name, the type, or the value of the item)
    if ((flags & (DocumentMetadataItemFlags::kPrimaryKeyValid | DocumentMetadataItemFlags::kNameValid | DocumentMetadataItemFlags::kDocumentMetadataTypeAndValueValid)) != DocumentMetadataItemFlags::None)
    {
        const auto statement = this->CreateStatementForRetrievingItem(flags);
        statement->BindInt64(1, primary_key);

        if (!this->GetDocument()->GetDatabase_connection()->StepStatement(statement.get()))
        {
            // this means that the tile with the specified index ('primary_key') was not found
            ostringstream ss;
            ss << "Request for reading a non-existing item (with pk=" << primary_key << ")";
            throw non_existing_item_exception(ss.str(), primary_key);
        }

        // Note that "CreateStatementForRetrievingItem" does not retrieve the "complete path" of the item, we need to do this separately
        //  for the time being, so we need to remove the "kCompletePath" flag from the flags parameter
        item = this->RetrieveDocumentMetadataItemFromStatement(statement, flags & ~DocumentMetadataItemFlags::kCompletePath, "");
    }

    // check if we have to "retrieve the complete path" (i.e. if the caller wants to have the complete path of the item)
    if ((flags & DocumentMetadataItemFlags::kCompletePath) == DocumentMetadataItemFlags::kCompletePath)
    {
        if (!this->GetPathForNode(primary_key, item.complete_path))
        {
            ostringstream ss;
            ss << "Request for reading the path of a non-existing item (with pk=" << primary_key << ")";
            throw non_existing_item_exception(ss.str(), primary_key);
        }

        item.flags = item.flags | DocumentMetadataItemFlags::kCompletePath;
    }

    return item;
}

/*virtual*/imgdoc2::DocumentMetadataItem DocumentMetadataReader::GetItemForPath(const std::string& path, imgdoc2::DocumentMetadataItemFlags flags)
{
    optional<imgdoc2::dbIndex> idx;
    const bool success = this->TryMapPathAndGetTerminalNode(path, &idx);
    if (success && idx.has_value())
    {
        // note: we require to have a valid node-id here, an "empty" optional (which would mean "empty path") is not valid in this method
        return this->GetItem(idx.value(), flags);
    }

    ostringstream string_stream;
    string_stream << "The path '" << path << "' does not exist.";
    throw invalid_path_exception(string_stream.str());
}

void DocumentMetadataReader::EnumerateItems(
  std::optional<imgdoc2::dbIndex> parent,
  bool recursive,
  DocumentMetadataItemFlags flags,
  const std::function<bool(imgdoc2::dbIndex, const DocumentMetadataItem& item)>& func)
{
    string path_of_parent_node;

    // if we are to retrieve to first retrieve the path to the 'parent'-node
    if (parent.has_value() && (flags & DocumentMetadataItemFlags::kCompletePath) == DocumentMetadataItemFlags::kCompletePath)
    {
        if (!this->GetPathForNode(parent.value(), path_of_parent_node))
        {
            ostringstream ss;
            ss << "Request for reading the path of a non-existing item (with pk=" << parent.value() << ")";
            throw non_existing_item_exception(ss.str(), parent.value());
        }

        path_of_parent_node += DocumentMetadataBase::kPathDelimiter_;
    }

    this->InternalEnumerateItems(parent, path_of_parent_node, recursive, flags, func);
}

void DocumentMetadataReader::EnumerateItemsForPath(
  const std::string& path,
  bool recursive,
  DocumentMetadataItemFlags flags,
  const std::function<bool(imgdoc2::dbIndex, const DocumentMetadataItem& item)>& func)
{
    optional<imgdoc2::dbIndex> idx;
    const bool success = this->TryMapPathAndGetTerminalNode(path, &idx);
    if (success)
    {
        this->InternalEnumerateItems(idx, path, recursive, flags, func);
        return;
    }

    ostringstream string_stream;
    string_stream << "The path '" << path << "' does not exist.";
    throw invalid_path_exception(string_stream.str());
}

void DocumentMetadataReader::InternalEnumerateItems(
        std::optional<imgdoc2::dbIndex> parent,
        const std::string& path_of_parent,
        bool recursive,
        imgdoc2::DocumentMetadataItemFlags flags,
        const std::function<bool(imgdoc2::dbIndex, const imgdoc2::DocumentMetadataItem& item)>& func)
{
    const auto statement = this->CreateStatementForEnumerateAllItemsWithAncestorAndDataBind(
        recursive,
        (flags & DocumentMetadataItemFlags::kCompletePath) == DocumentMetadataItemFlags::kCompletePath,
        parent);

    bool at_least_one_item_found = false;
    while (this->GetDocument()->GetDatabase_connection()->StepStatement(statement.get()))
    {
        at_least_one_item_found = true;
        const imgdoc2::dbIndex index = statement->GetResultInt64(0);
        DocumentMetadataItem document_metadata_item = this->RetrieveDocumentMetadataItemFromStatement(statement, flags, path_of_parent);
        const bool continue_operation = func(index, document_metadata_item);
        if (!continue_operation)
        {
            break;
        }
    }

    if (!at_least_one_item_found && parent.has_value())
    {
        // Unfortunately, we cannot distinguish between "no items found because parent does have a child" and "no items found because the parent does not exist".
        // Maybe there is a more clever way to check whether the parent exists, but for now we have to execute an additional query.
        // Note that if we query for the root node, we do not need to check whether the parent exists, because the root node always exists.
        const bool parent_exists = this->CheckIfItemExists(parent.value());
        if (!parent_exists)
        {
            ostringstream string_stream;
            string_stream << "The parent with pk=" << parent.value() << " does not exist.";
            throw non_existing_item_exception(string_stream.str(), parent.value());
        }
    }
}

std::shared_ptr<IDbStatement> DocumentMetadataReader::CreateStatementForRetrievingItem(imgdoc2::DocumentMetadataItemFlags flags)
{
    ostringstream string_stream;
    string_stream << "SELECT " <<
        this->GetDocument()->GetDataBaseConfigurationCommon()->GetColumnNameOfMetadataTableOrThrow(DatabaseConfigurationCommon::kMetadataTable_Column_Pk) << ", " <<
        this->GetDocument()->GetDataBaseConfigurationCommon()->GetColumnNameOfMetadataTableOrThrow(DatabaseConfigurationCommon::kMetadataTable_Column_Name) << ", " <<
        this->GetDocument()->GetDataBaseConfigurationCommon()->GetColumnNameOfMetadataTableOrThrow(DatabaseConfigurationCommon::kMetadataTable_Column_TypeDiscriminator) << ", " <<
        this->GetDocument()->GetDataBaseConfigurationCommon()->GetColumnNameOfMetadataTableOrThrow(DatabaseConfigurationCommon::kMetadataTable_Column_ValueDouble) << ", " <<
        this->GetDocument()->GetDataBaseConfigurationCommon()->GetColumnNameOfMetadataTableOrThrow(DatabaseConfigurationCommon::kMetadataTable_Column_ValueInteger) << ", " <<
        this->GetDocument()->GetDataBaseConfigurationCommon()->GetColumnNameOfMetadataTableOrThrow(DatabaseConfigurationCommon::kMetadataTable_Column_ValueString) << " " <<
        "FROM [" << this->GetDocument()->GetDataBaseConfigurationCommon()->GetTableNameForMetadataTableOrThrow() << "] WHERE " <<
        "[" << this->GetDocument()->GetDataBaseConfigurationCommon()->GetColumnNameOfMetadataTableOrThrow(DatabaseConfigurationCommon::kMetadataTable_Column_Pk) << "]=?1;";
    auto statement = this->GetDocument()->GetDatabase_connection()->PrepareStatement(string_stream.str());
    return statement;
}

std::shared_ptr<IDbStatement> DocumentMetadataReader::CreateStatementForEnumerateAllItemsWithAncestorAndDataBind(bool recursive, bool include_path, std::optional<imgdoc2::dbIndex> parent)
{
    /* If we want to create "full-path" (include_path=true) for the items, we use a query like this:

    WITH RECURSIVE
      [cte](Pk,Name,AncestorId,TypeDiscriminator,ValueDouble,ValueInteger,ValueString,Path) AS(
        SELECT
               [Pk],
               [Name],
               [AncestorId],
               [TypeDiscriminator],
               [ValueDouble],
               [ValueInteger],
               [ValueString],
               [Name] As Path
        FROM   [METADATA]
        WHERE  [AncestorId] IS NULL
        UNION ALL
        SELECT
               [c].[Pk],
               [c].[Name],
               [c].[AncestorId],
               [c].[TypeDiscriminator],
               [c].[ValueDouble],
               [c].[ValueInteger],
               [c].[ValueString],
               [cte].Path || '/' ||c.Name
        FROM   [METADATA] [c]
               JOIN [cte] ON [c].[AncestorId] = [cte].[Pk]
      )
    SELECT
           [Pk],
           [Name],
           [TypeDiscriminator],
           [ValueDouble],
           [ValueInteger],
           [ValueString],
           [Path]
    FROM   [cte];

    If the path is not required, we can use a simpler query:

    WITH RECURSIVE
      [cte] AS(
        SELECT
               [Pk],
               [Name],
               [AncestorId],
               [TypeDiscriminator],
               [ValueDouble],
               [ValueInteger],
               [ValueString],
        FROM   [METADATA]
        WHERE  [AncestorId] IS NULL
        UNION ALL
        SELECT
               [c].[Pk],
               [c].[Name],
               [c].[AncestorId],
               [c].[TypeDiscriminator],
               [c].[ValueDouble],
               [c].[ValueInteger],
               [c].[ValueString],
        FROM   [METADATA] [c]
               JOIN [cte] ON [c].[AncestorId] = [cte].[Pk]
      )
    SELECT
           [Pk],
           [Name],
           [TypeDiscriminator],
           [ValueDouble],
           [ValueInteger],
           [ValueString],
    FROM   [cte];

     */
    const bool parent_has_value = parent.has_value();
    ostringstream string_stream;

    const auto metadata_table_name = this->GetDocument()->GetDataBaseConfigurationCommon()->GetTableNameForMetadataTableOrThrow();
    const auto column_name_pk = this->GetDocument()->GetDataBaseConfigurationCommon()->GetColumnNameOfMetadataTableOrThrow(DatabaseConfigurationCommon::kMetadataTable_Column_Pk);
    const auto column_name_name = this->GetDocument()->GetDataBaseConfigurationCommon()->GetColumnNameOfMetadataTableOrThrow(DatabaseConfigurationCommon::kMetadataTable_Column_Name);
    const auto column_name_ancestor_id = this->GetDocument()->GetDataBaseConfigurationCommon()->GetColumnNameOfMetadataTableOrThrow(DatabaseConfigurationCommon::kMetadataTable_Column_AncestorId);
    const auto column_name_type_discriminator = this->GetDocument()->GetDataBaseConfigurationCommon()->GetColumnNameOfMetadataTableOrThrow(DatabaseConfigurationCommon::kMetadataTable_Column_TypeDiscriminator);
    const auto column_name_value_double = this->GetDocument()->GetDataBaseConfigurationCommon()->GetColumnNameOfMetadataTableOrThrow(DatabaseConfigurationCommon::kMetadataTable_Column_ValueDouble);
    const auto column_name_value_integer = this->GetDocument()->GetDataBaseConfigurationCommon()->GetColumnNameOfMetadataTableOrThrow(DatabaseConfigurationCommon::kMetadataTable_Column_ValueInteger);
    const auto column_name_value_string = this->GetDocument()->GetDataBaseConfigurationCommon()->GetColumnNameOfMetadataTableOrThrow(DatabaseConfigurationCommon::kMetadataTable_Column_ValueString);

    if (recursive)
    {
        if (include_path)
        {
            string_stream << "WITH RECURSIVE [cte](" <<
                column_name_pk << "," << column_name_name << "," << column_name_ancestor_id << "," << column_name_type_discriminator << "," << column_name_value_double << "," << column_name_value_integer << "," << column_name_value_string << ",Path) AS(" <<
                "SELECT [" << column_name_pk << "],[" << column_name_name << "],[" << column_name_ancestor_id << "],[" << column_name_type_discriminator << "],[" << column_name_value_double << "],[" << column_name_value_integer << 
                "],[" << column_name_value_string << "],[" << column_name_name << "] As Path " <<
                "FROM [" << metadata_table_name << "] ";
            if (parent_has_value)
            {
                string_stream << "WHERE " << column_name_ancestor_id << "=?1 ";
            }
            else
            {
                string_stream << "WHERE " << column_name_ancestor_id << " IS NULL ";
            }

            string_stream << "UNION ALL " <<
                "SELECT [c].[" << column_name_pk << "],[c].[" << column_name_name << "],[c].[" << column_name_ancestor_id << "],[c].[" << column_name_type_discriminator << "],[c].[" << column_name_value_double << "],[c].[" << column_name_value_integer << "],[c].[" << 
                column_name_value_string << "],[cte].Path || '" << DocumentMetadataBase::kPathDelimiter_ << "' ||c." << column_name_name << " " <<
                "FROM [" << metadata_table_name << "] [c] " <<
                "JOIN [cte] ON [c].[" << column_name_ancestor_id << "] = [cte].[" << column_name_pk << "]) " <<
                "SELECT [" << column_name_pk << "],[" << column_name_name << "],[" << column_name_type_discriminator << "],[" << column_name_value_double << "],[" << column_name_value_integer << "],[" << column_name_value_string << "],[Path] " <<
                "FROM [cte];";
        }
        else
        {
            string_stream <<
                "WITH RECURSIVE cte AS(" <<
                "SELECT " << column_name_pk << "," << column_name_name << "," << column_name_ancestor_id << "," << column_name_type_discriminator << "," << column_name_value_double << "," << column_name_value_integer << "," << column_name_value_string << " " <<
                "FROM [" << metadata_table_name << "] ";

            if (parent_has_value)
            {
                string_stream << "WHERE " << column_name_ancestor_id << "=?1 ";
            }
            else
            {
                string_stream << "WHERE " << column_name_ancestor_id << " IS NULL ";
            }

            string_stream <<
                "UNION ALL " <<
                "SELECT c." << column_name_pk << ",c." << column_name_name << ",c." << column_name_ancestor_id << ",c." << column_name_type_discriminator << ",c." << column_name_value_double << ",c." << column_name_value_integer << ",c." << column_name_value_string << " " <<
                "FROM [" << metadata_table_name << "] c " <<
                "JOIN cte ON c." << column_name_ancestor_id << "=cte." << column_name_pk << " " <<
                ") " <<
                "SELECT " << column_name_pk << "," << column_name_name << "," << column_name_type_discriminator << "," << column_name_value_double << "," << column_name_value_integer << "," << column_name_value_string << " FROM cte;";
        }
    }
    else
    {
        if (include_path)
        {
            string_stream << "WITH RECURSIVE [cte](" <<
                column_name_pk << "," << column_name_name << "," << column_name_ancestor_id << "," << column_name_type_discriminator << "," << column_name_value_double << "," << column_name_value_integer << "," << column_name_value_string << ",Path) AS(" <<
                "SELECT [" << column_name_pk << "],[" << column_name_name << "],[" << column_name_ancestor_id << "],[" << column_name_type_discriminator << "],[" << column_name_value_double << "],[" << column_name_value_integer << "],[" << 
                column_name_value_string << "],[" << column_name_name << "] As Path " <<
                "FROM [" << metadata_table_name << "] ";
            if (parent_has_value)
            {
                string_stream << "WHERE " << column_name_ancestor_id << "=?1 ";
            }
            else
            {
                string_stream << "WHERE " << column_name_ancestor_id << " IS NULL ";
            }

            string_stream << "UNION ALL " <<
                "SELECT [c].[" << column_name_pk << "],[c].[" << column_name_name << "],[c].[" << column_name_ancestor_id << "],[c].[" << column_name_type_discriminator << "],[c].[" << column_name_value_double << "],[c].[" << column_name_value_integer << "],[c].[" << 
                column_name_value_string << "],[cte].Path || '" << DocumentMetadataBase::kPathDelimiter_ << "' ||c." << column_name_name << " " <<
                "FROM [" << metadata_table_name << "] [c] " <<
                "JOIN [cte] ON [c].[" << column_name_ancestor_id << "] = [cte].[" << column_name_pk << "]) " <<
                "SELECT [" << column_name_pk << "],[" << column_name_name << "],[" << column_name_type_discriminator << "],[" << column_name_value_double << "],[" << column_name_value_integer << "],[" << column_name_value_string << "],[Path] " <<
                "FROM [cte] ";
            if (parent_has_value)
            {
                string_stream << "WHERE " << column_name_ancestor_id << "=?1;";
            }
            else
            {
                string_stream << "WHERE " << column_name_ancestor_id << " IS NULL;";
            }
        }
        else
        {
            string_stream <<
                "SELECT " << column_name_pk << "," << column_name_name << "," << column_name_type_discriminator << "," << column_name_value_double << "," << column_name_value_integer << "," << column_name_value_string << " FROM [" << metadata_table_name << "] ";

            if (parent_has_value)
            {
                string_stream << "WHERE " << column_name_ancestor_id << "=?1 ";
            }
            else
            {
                string_stream << "WHERE " << column_name_ancestor_id << " IS NULL ";
            }
        }
    }

    auto statement = this->GetDocument()->GetDatabase_connection()->PrepareStatement(string_stream.str());
    if (parent_has_value)
    {
        statement->BindInt64(1, parent.value());
    }

    return statement;
}

imgdoc2::DocumentMetadataItem DocumentMetadataReader::RetrieveDocumentMetadataItemFromStatement(const std::shared_ptr<IDbStatement>& statement, imgdoc2::DocumentMetadataItemFlags flags, const string& path_to_prepend)
{
    DocumentMetadataItem item;
    item.flags = flags;
    if ((flags & DocumentMetadataItemFlags::kPrimaryKeyValid) == DocumentMetadataItemFlags::kPrimaryKeyValid)
    {
        item.primary_key = statement->GetResultInt64(0);
    }

    if ((flags & DocumentMetadataItemFlags::kNameValid) == DocumentMetadataItemFlags::kNameValid)
    {
        item.name = statement->GetResultString(1);
    }

    if ((flags & DocumentMetadataItemFlags::kDocumentMetadataTypeAndValueValid) == DocumentMetadataItemFlags::kDocumentMetadataTypeAndValueValid)
    {
        const auto database_item_type_value = statement->GetResultInt32(2);
        switch (static_cast<DatabaseDataTypeValue>(database_item_type_value))
        {
            case DatabaseDataTypeValue::null:
                item.value = std::monostate();
                item.type = DocumentMetadataType::kNull;
                break;
            case DatabaseDataTypeValue::int32:
                item.value = IDocumentMetadataWrite::metadata_item_variant(statement->GetResultInt32(4));
                item.type = DocumentMetadataType::kInt32;
                break;
            case DatabaseDataTypeValue::doublefloat:
                item.value = IDocumentMetadataWrite::metadata_item_variant(statement->GetResultDouble(3));
                item.type = DocumentMetadataType::kDouble;
                break;
            case DatabaseDataTypeValue::utf8string:
                item.value = IDocumentMetadataWrite::metadata_item_variant(statement->GetResultString(5));
                item.type = DocumentMetadataType::kText;
                break;
            case DatabaseDataTypeValue::json:
                item.value = IDocumentMetadataWrite::metadata_item_variant(statement->GetResultString(5));
                item.type = DocumentMetadataType::kJson;
                break;
            default:
                throw runtime_error("DocumentMetadataReader::GetItem: Unknown data type");
        }
    }

    if ((flags & DocumentMetadataItemFlags::kCompletePath) == DocumentMetadataItemFlags::kCompletePath)
    {
        item.complete_path = path_to_prepend + statement->GetResultString(6);
    }

    return item;
}

bool DocumentMetadataReader::GetPathForNode(imgdoc2::dbIndex node_id, std::string& path)
{
    /*
    Here we construct a query that will return the path for a given node. The query is constructed as follows:

    WITH RECURSIVE item_path(Pk, Name, AncestorId, path) AS(
        -- Base case: select items with no parent (top-level items)
        SELECT
            Pk,
            Name,
            AncestorId,
            Name AS path
        FROM METADATA
        WHERE AncestorId IS NULL

        UNION ALL

        -- Recursive case: join items with their parent items in the item_path
        SELECT
            i.Pk,
            i.Name,
            i.AncestorId,
            ip.path || '/' || i.Name AS path
        FROM METADATA i
        JOIN item_path ip ON i.AncestorId = ip.Pk
    )

    -- Choose the item for which you want to find the path
    SELECT path
    FROM item_path
    WHERE Pk = :the-item-id-here:;
    */
    ostringstream string_stream;

    const auto metadata_table_name = this->GetDocument()->GetDataBaseConfigurationCommon()->GetTableNameForMetadataTableOrThrow();
    const auto column_name_pk = this->GetDocument()->GetDataBaseConfigurationCommon()->GetColumnNameOfMetadataTableOrThrow(DatabaseConfigurationCommon::kMetadataTable_Column_Pk);
    const auto column_name_name = this->GetDocument()->GetDataBaseConfigurationCommon()->GetColumnNameOfMetadataTableOrThrow(DatabaseConfigurationCommon::kMetadataTable_Column_Name);
    const auto column_name_ancestor_id = this->GetDocument()->GetDataBaseConfigurationCommon()->GetColumnNameOfMetadataTableOrThrow(DatabaseConfigurationCommon::kMetadataTable_Column_AncestorId);

    string_stream << "WITH RECURSIVE item_path (" << column_name_pk << "," << column_name_name << "," << column_name_ancestor_id << ",path) AS( " <<
        "SELECT " << column_name_pk << "," << column_name_name << "," << column_name_ancestor_id << "," << column_name_name << " AS path " <<
        "FROM " << metadata_table_name << " WHERE " << column_name_ancestor_id << " IS NULL " <<
        "UNION ALL " <<
        "SELECT i." << column_name_pk << ",i." << column_name_name << ",i." << column_name_ancestor_id << ",ip.path || '" << DocumentMetadataBase::kPathDelimiter_ << "' || i." << column_name_name << " AS path " <<
        "FROM " << metadata_table_name << " i " <<
        "JOIN item_path ip ON i." << column_name_ancestor_id << " = ip." << column_name_pk << ") " <<
        "SELECT path FROM item_path WHERE " << column_name_pk << "=?1;";

    const auto statement = this->GetDocument()->GetDatabase_connection()->PrepareStatement(string_stream.str());
    statement->BindInt64(1, node_id);

    if (!this->GetDocument()->GetDatabase_connection()->StepStatement(statement.get()))
    {
        return false;
    }

    path = statement->GetResultString(0);
    return true;
}

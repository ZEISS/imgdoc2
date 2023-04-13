// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include "documentMetadataWriter.h"
#include <vector>
#include <string>
#include "gsl/util"

using namespace std;
using namespace imgdoc2;

imgdoc2::dbIndex DocumentMetadataWriter::UpdateOrCreateItem(
                  std::optional<imgdoc2::dbIndex> parent,
                  bool create_node_if_not_exists,
                  const string& name,
                  DocumentMetadataType type,
                  const IDocumentMetadata::metadata_item_variant& value)
{
    this->CheckNodeNameAndThrowIfInvalid(name);
    const DatabaseDataTypeValue item_type = DocumentMetadataBase::DetermineDatabaseDataTypeValueOrThrow(type, value);

    if (parent.has_value() && !this->CheckIfItemExists(parent.value()))
    {
        ostringstream string_stream;
        string_stream << "The parent with pk=" << parent.value() << " does not exist.";
        throw non_existing_item_exception(string_stream.str(), parent.value());
    }

    auto statement = this->CreateStatementForUpdateOrCreateItemAndBindData(create_node_if_not_exists, parent, name, item_type, value);
    this->GetDocument()->GetDatabase_connection()->Execute(statement.get());
    statement.reset();

    // Ok this worked the new item was inserted or updated. Now we need to get the id of the item.
    // TODO(Jbl): It is of course a bit unfortunate that we do another lookup here. However, I didn't find a
    //             a way around this (for all the cases we need to support).
    const auto select_statement = this->CreateQueryForNameAndAncestorIdStatement(name, parent);

    // we are expecting exactly one result, or zero in case of "not found"
    if (!this->GetDocument()->GetDatabase_connection()->StepStatement(select_statement.get()))
    {
        throw std::logic_error("Could not find the item we just inserted or updated");
    }

    const dbIndex pk_of_updated_or_created_item = select_statement->GetResultInt64(0);

    return pk_of_updated_or_created_item;
}

imgdoc2::dbIndex DocumentMetadataWriter::UpdateOrCreateItemForPath(
            bool create_path_if_not_exists,
            bool create_node_if_not_exists,
            const std::string& path,
            imgdoc2::DocumentMetadataType type,
            const IDocumentMetadata::metadata_item_variant& value)
{
    const auto path_parts = this->SplitPath(path);
    auto pk_of_nodes_on_path = this->GetNodeIdsForPathParts(path_parts);

    // If the size of the "pks of nodes on path" is smaller than the size of the path parts, then we 
    // need to create the missing nodes. Or - only if the caller requested it - we need to create the missing nodes.
    if (pk_of_nodes_on_path.size() < path_parts.size() - 1)
    {
        if (!create_path_if_not_exists)
        {
            // TODO(JBl): find a better exception type
            throw std::invalid_argument("The path does not exist and the caller did not request to create it.");
        }

        this->CreateMissingNodesOnPath(path_parts, pk_of_nodes_on_path);
    }

    return this->UpdateOrCreateItem(
        pk_of_nodes_on_path.empty() ? optional<dbIndex>(nullopt) : optional<dbIndex>(pk_of_nodes_on_path.back()),
        create_node_if_not_exists,
        string{ path_parts.back() },
        type,
        value);
}

uint64_t DocumentMetadataWriter::DeleteItem(
            std::optional<imgdoc2::dbIndex> primary_key,
            bool recursively)
{
    const auto statement = this->CreateStatementForDeleteItemAndBindData(recursively, primary_key);

    int64_t number_of_modified_rows = 0;

    // In the special case "primary_key=nullopt=root" and recursively=false we do not want to delete anything,
    //  and in this case "CreateStatementForDeleteItemAndBindData" returns 0. So, in this corner stone case,
    //  we want to do nothing and return 0.
    if (statement)
    {
        this->GetDocument()->GetDatabase_connection()->Execute(statement.get(), &number_of_modified_rows);
    }

    return gsl::narrow_cast<uint64_t>(number_of_modified_rows);
}

uint64_t DocumentMetadataWriter::DeleteItemForPath(
           const std::string& path,
           bool recursively)
{
    optional<imgdoc2::dbIndex> idx;
    const bool success = this->TryMapPathAndGetTerminalNode(path, &idx);
    if (success)
    {
        return this->DeleteItem(idx, recursively);
    }

    // TODO(Jbl): find a better exception type
    throw runtime_error("DocumentMetadataReader::DeleteItemForPath");
}

std::shared_ptr<IDbStatement> DocumentMetadataWriter::CreateStatementForUpdateOrCreateItemAndBindData(bool create_node_if_not_exists, std::optional<imgdoc2::dbIndex> parent, const std::string& name,
                DatabaseDataTypeValue database_data_type_value,
                const IDocumentMetadata::metadata_item_variant& value)
{
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

    // CAUTION: it seems if we want to check for a NULL, we cannot simply use the "=" operator, but need to use the "IS" operator.
    //          This is because the "=" operator does not work for NULL values, and it means that we cannot use data-binding, we have to modify
    //          the query string itself. This is not a problem, but it is something to keep in mind. 
    // TODO(Jbl): maybe there is a better way to do this?
    if (create_node_if_not_exists == false)
    {
        string_stream << "UPDATE [" << metadata_table_name << "] SET " <<
            "[" << column_name_type_discriminator << "] = ?3, " <<
            "[" << column_name_value_double << "] = ?4, " <<
            "[" << column_name_value_integer << "] = ?5, " <<
            "[" << column_name_value_string << "] = ?6 " <<
            "WHERE [" << column_name_name << "] = ?1 AND ";
        if (parent_has_value)
        {
            string_stream << "[" << column_name_ancestor_id << "] = ?2";
        }
        else
        {
            string_stream << "[" << column_name_ancestor_id << "] IS NULL";
        }
    }
    else
    {
        string_stream << "INSERT INTO [" << metadata_table_name << "] (" <<
            "[" << column_name_name << "]," <<
            "[" << column_name_ancestor_id << "]," <<
            "[" << column_name_type_discriminator << "]," <<
            "[" << column_name_value_double << "]," <<
            "[" << column_name_value_integer << "]," <<
            "[" << column_name_value_string << "]) " <<
            "VALUES(" << "?1, ?2, ?3, ?4, ?5, ?6" << ") " <<
            // There is a constraint on the table that ensures that the combination of name and ancestor id is unique.
            // So, if the insert fails because of a constraint violation, we update the existing row.
            "ON CONFLICT([" << column_name_name << "], [" << column_name_ancestor_id << "]) DO UPDATE " <<
            "SET [" << column_name_type_discriminator << "] = ?3, " <<
            "[" << column_name_value_double << "] = ?4, " <<
            "[" << column_name_value_integer << "] = ?5, " <<
            "[" << column_name_value_string << "] = ?6 " <<
            "WHERE [" << column_name_name << "] = ?1 AND ";
        if (parent_has_value)
        {
            string_stream << "[" << column_name_ancestor_id << "] = ?2";
        }
        else
        {
            string_stream << "[" << column_name_ancestor_id << "] IS NULL";
        }
    }

    auto statement = this->GetDocument()->GetDatabase_connection()->PrepareStatement(string_stream.str());

    int binding_index = 1;
    statement->BindString(binding_index++, name);
    if (parent_has_value)
    {
        statement->BindInt64(binding_index++, parent.value());
    }
    else
    {
        ++binding_index;
    }

    this->BindTypeDiscriminatorAndData(statement, binding_index, database_data_type_value, value); // this will bind 3 values

    return statement;
}

std::shared_ptr<IDbStatement> DocumentMetadataWriter::CreateQueryForNameAndAncestorIdStatement(const std::string& name, std::optional<imgdoc2::dbIndex> parent)
{
    const auto metadata_table_name = this->GetDocument()->GetDataBaseConfigurationCommon()->GetTableNameForMetadataTableOrThrow();
    const auto column_name_pk = this->GetDocument()->GetDataBaseConfigurationCommon()->GetColumnNameOfMetadataTableOrThrow(DatabaseConfigurationCommon::kMetadataTable_Column_Pk);
    const auto column_name_name = this->GetDocument()->GetDataBaseConfigurationCommon()->GetColumnNameOfMetadataTableOrThrow(DatabaseConfigurationCommon::kMetadataTable_Column_Name);
    const auto column_name_ancestor_id = this->GetDocument()->GetDataBaseConfigurationCommon()->GetColumnNameOfMetadataTableOrThrow(DatabaseConfigurationCommon::kMetadataTable_Column_AncestorId);

    const bool parent_has_value = parent.has_value();
    ostringstream string_stream;
    string_stream << "SELECT [" << column_name_pk << "] FROM [" << metadata_table_name << "] WHERE [" << column_name_name << "]=?1 AND ";
    if (parent_has_value)
    {
        string_stream << "[" << column_name_ancestor_id << "] = ?2";
    }
    else
    {
        string_stream << "[" << column_name_ancestor_id << "] IS NULL";
    }

    string_stream << ";";

    auto statement = this->GetDocument()->GetDatabase_connection()->PrepareStatement(string_stream.str());
    statement->BindString(1, name);
    if (parent_has_value)
    {
        statement->BindInt64(2, parent.value());
    }

    return statement;
}

/*
 with recursive paths(id, name, path, level) as (
    select Pk, Name, Name, 0 from METADATA where AncestorId is null AND Name='Node1'
    union
    select METADATA.Pk, METADATA.name, paths.path || '/' || METADATA.name, level+1
    from METADATA join paths where METADATA.AncestorId = paths.id AND
         ((level = 0 AND METADATA.Name="Node1_1") OR
          (level = 1 AND METADATA.Name="Node1_1_2"))
)
select id, path, level from paths
 */

void DocumentMetadataWriter::CreateMissingNodesOnPath(const std::vector<std::string_view>& path_parts, std::vector<imgdoc2::dbIndex>& pks_existing)
{
    const size_t count_existing_pks = pks_existing.size();
    for (size_t i = count_existing_pks; i < path_parts.size() - 1; ++i)
    {
        const auto new_node = this->UpdateOrCreateItem(
            i > 0 ? std::optional<imgdoc2::dbIndex>(pks_existing[i - 1]) : std::optional<imgdoc2::dbIndex>(),
            true,
            string{ path_parts[i] },
            DocumentMetadataType::kNull,
            std::monostate());
        pks_existing.push_back(new_node);
    }
}

std::shared_ptr<IDbStatement> DocumentMetadataWriter::CreateStatementForDeleteItemAndBindData(bool recursively, std::optional<imgdoc2::dbIndex> parent)
{
    ostringstream string_stream;

    const auto metadata_table_name = this->GetDocument()->GetDataBaseConfigurationCommon()->GetTableNameForMetadataTableOrThrow();
    const auto column_name_pk = this->GetDocument()->GetDataBaseConfigurationCommon()->GetColumnNameOfMetadataTableOrThrow(DatabaseConfigurationCommon::kMetadataTable_Column_Pk);
    const auto column_name_name = this->GetDocument()->GetDataBaseConfigurationCommon()->GetColumnNameOfMetadataTableOrThrow(DatabaseConfigurationCommon::kMetadataTable_Column_Name);
    const auto column_name_ancestor_id = this->GetDocument()->GetDataBaseConfigurationCommon()->GetColumnNameOfMetadataTableOrThrow(DatabaseConfigurationCommon::kMetadataTable_Column_AncestorId);

    if (parent.has_value())
    {
        if (!recursively)
        {
            string_stream << "DELETE FROM [" << metadata_table_name << "] WHERE " <<
                "[" << column_name_pk << "]=?1 AND NOT EXISTS(" <<
                "SELECT 1 FROM [" << metadata_table_name << "] WHERE " << "[" << column_name_ancestor_id << "]=?1" << ");";
        }
        else
        {
            // To delete all items below a given node recursively we use a CTE (Common Table Expression) to get all child nodes
            // of the given node and then delete all items with a PK that is in the result set of the CTE.
            // The WITH RECURSIVE clause defines a recursive CTE called children that selects all child nodes of the given node 
            // recursively. The first SELECT statement selects all direct child nodes of the given node, while the second SELECT 
            // statement selects all child nodes of the child nodes recursively by joining with the children CTE.
            // After the children CTE is defined, a separate DELETE statement is executed that deletes all rows in the hierarchy 
            // table that have an id that matches any of the id values returned by the children CTE, and we also delete the
            // specified node itself.
            string_stream << "WITH RECURSIVE children(id) AS (" <<
                "SELECT [" << column_name_pk << "] FROM [" << metadata_table_name << "] WHERE " << "[" << column_name_ancestor_id << "]=?1" <<
                "UNION ALL " <<
                "SELECT [" << metadata_table_name << "].[" << column_name_pk << "] FROM [" << metadata_table_name << "] JOIN children ON [" << metadata_table_name << "].[" << column_name_ancestor_id << "]=children.id" <<
                ") " <<
                "DELETE FROM [" << metadata_table_name << "] WHERE " << "[" << "Pk" << "] IN (SELECT id FROM children) OR " << "[" << column_name_pk << "]=?1;";
        }


        auto statement = this->GetDocument()->GetDatabase_connection()->PrepareStatement(string_stream.str());
        statement->BindInt64(1, parent.value());
        return statement;
    }
    else
    {
        // This means that we want to delete the "root" node. It is not possible to delete the "root" itself, but everything in it.
        // Following this logic, only "recursively=true" is allowed or makes sense.
        if (recursively)
        {
            string_stream << "WITH RECURSIVE children(id) AS (" <<
                "SELECT [" << column_name_pk << "] FROM [" << metadata_table_name << "] WHERE " << "[" << column_name_ancestor_id << "] IS NULL " <<
                "UNION ALL " <<
                "SELECT [" << metadata_table_name << "].[" << column_name_pk << "] FROM [" << metadata_table_name << "] JOIN children ON [" << metadata_table_name << "].[" << column_name_ancestor_id << "]=children.id" <<
                ") " <<
                "DELETE FROM [" << metadata_table_name << "] WHERE " << "[" << column_name_pk << "] IN (SELECT id FROM children) OR " << "[" << column_name_ancestor_id << "] IS NULL;";

            return this->GetDocument()->GetDatabase_connection()->PrepareStatement(string_stream.str());
        }
        else
        {
            return nullptr;
        }
    }
}

void DocumentMetadataWriter::CheckNodeNameAndThrowIfInvalid(const std::string& name)
{
    // the string must not be empty and it must not contain a slash
    // TODO(JBL): is the check for '/' sufficient (for UTF8-strings)?
    if (name.empty() || name.find('/') != std::string::npos)
    {
        throw imgdoc2::invalid_argument_exception("The 'name' must not be empty and it must not contain a slash");
    }
}

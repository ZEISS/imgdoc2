#include "documentMetadataWriter.h"

using namespace std;
using namespace imgdoc2;

//static int DetermineDiscriminator(const imgdoc2::IDocumentMetadata::metadata_item_variant& value)
//{
//    if (std::holds_alternative<std::string>(value))
//    {
//        return 1;
//    }
//    else if (std::holds_alternative<int>(value))
//    {
//        return 2;
//    }
//    else if (std::holds_alternative<double>(value))
//    {
//        return 3;
//    }
//    //else if (std::holds_alternative<json>(value))
//    //{
//    //    return 4;
//    //}
//    else
//    {
//        throw std::invalid_argument("Unknown metadata item type");
//    }
//}

imgdoc2::dbIndex DocumentMetadataWriter::UpdateOrCreateItem(
                  std::optional<imgdoc2::dbIndex> parent,
                  bool create_node_if_not_exists,
                  const string& name,
                  DocumentMetadataType type,
                  const IDocumentMetadata::metadata_item_variant& value)
{
    this->CheckNodeNameAndThrowIfInvalid(name);
    const DatabaseDataTypeValue item_type = DocumentMetadataBase::DetermineDatabaseDataTypeValueOrThrow(type, value);
    auto statement = this->CreateStatementForUpdateOrCreateItemAndBindData(create_node_if_not_exists, parent, name, item_type, value);
    this->document_->GetDatabase_connection()->Execute(statement.get());
    statement.reset();

    // Ok this worked the new item was inserted or updated. Now we need to get the id of the item.
    // TODO(Jbl): It is of course a bit unfortunate that we do another lookup here. However, I didn't find a
    //             a way around this (for all the cases we need to support).
    const auto select_statement = this->CreateQueryForNameAndAncestorIdStatement(name, parent);

    // we are expecting exactly one result, or zero in case of "not found"
    if (!this->document_->GetDatabase_connection()->StepStatement(select_statement.get()))
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
    if (pk_of_nodes_on_path.size() < path_parts.size()-1)
    {
       if (!create_path_if_not_exists)
       {
           // TODO(JBl): find a better exception type
           throw std::invalid_argument("The path does not exist and the caller did not request to create it.");
       }

       this->CreateMissingNodesOnPath(path_parts, pk_of_nodes_on_path);
    }

    return this->UpdateOrCreateItem(
        pk_of_nodes_on_path.back(), 
        create_node_if_not_exists, 
        string{ path_parts.back() },
        type, 
        value);
}

bool DocumentMetadataWriter::DeleteItem(
            std::optional<imgdoc2::dbIndex> parent,
            bool recursively)
{
    throw std::logic_error("The method or operation is not implemented.");
}
bool DocumentMetadataWriter::DeleteItemForPath(
           const std::string& path,
           bool recursively)
{
    throw std::logic_error("The method or operation is not implemented.");
}

std::shared_ptr<IDbStatement> DocumentMetadataWriter::CreateStatementForUpdateOrCreateItemAndBindData(bool create_node_if_not_exists, std::optional<imgdoc2::dbIndex> parent, const std::string& name,
                DatabaseDataTypeValue database_data_type_value,
                const IDocumentMetadata::metadata_item_variant& value)
{
    const bool parent_has_value = parent.has_value();
    ostringstream string_stream;

    // CAUTION: it seems if we want to check for a NULL, we cannot simply use the "=" operator, but need to use the "IS" operator.
    //          This is because the "=" operator does not work for NULL values, and it means that we cannot use data-binding, we have to modify
    //          the query string itself. This is not a problem, but it is something to keep in mind. 
    //          TODO(Jbl): maybe there is a better way to do this?
    if (create_node_if_not_exists == false)
    {
        string_stream << "UPDATE [" << "METADATA" << "] SET " <<
            "[" << "TypeDiscriminator" << "] = ?3, " <<
            "[" << "ValueDouble" << "] = ?4, " <<
            "[" << "ValueInteger" << "] = ?5, " <<
            "[" << "ValueString" << "] = ?6 " <<
            "WHERE [" << "Name" << "] = ?1 AND ";
        if (parent_has_value)
        {
            string_stream << "[" << "AncestorId" << "] = ?2";
        }
        else
        {
            string_stream << "[" << "AncestorId" << "] IS NULL";
        }
    }
    else
    {
        string_stream << "INSERT" << " INTO [" << "METADATA" << "] (" <<
            "[" << "Name" << "]," <<
            "[" << "AncestorId" << "]," <<
            "[" << "TypeDiscriminator" << "]," <<
            "[" << "ValueDouble" << "]," <<
            "[" << "ValueInteger" << "]," <<
            "[" << "ValueString" << "]) " <<
            "VALUES(" << "?1, ?2, ?3, ?4, ?5, ?6" << ") " <<
            // There is a constraint on the table that ensures that the combination of name and ancestor id is unique.
            // So, if the insert fails because of a constraint violation, we update the existing row.
            "ON CONFLICT([" << "Name" << "], [" << "AncestorId" << "]) DO UPDATE " <<
            "SET [" << "TypeDiscriminator" << "] = ?3, " <<
            "[" << "ValueDouble" << "] = ?4, " <<
            "[" << "ValueInteger" << "] = ?5, " <<
            "[" << "ValueString" << "] = ?6 " <<
            "WHERE [" << "Name" << "] = ?1 AND ";
        if (parent_has_value)
        {
            string_stream << "[" << "AncestorId" << "] = ?2";
        }
        else
        {
            string_stream << "[" << "AncestorId" << "] IS NULL";
        }
    }

    auto statement = this->document_->GetDatabase_connection()->PrepareStatement(string_stream.str());

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
    const bool parent_has_value = parent.has_value();
    ostringstream string_stream;
    string_stream << "SELECT [" << "Pk" << "] FROM [" << "METADATA" << "] WHERE [" << "Name" << "]=?1 AND ";
    if (parent_has_value)
    {
        string_stream << "[" << "AncestorId" << "] = ?2";
    }
    else
    {
        string_stream << "[" << "AncestorId" << "] IS NULL";
    }

    string_stream << ";";

    auto statement = this->document_->GetDatabase_connection()->PrepareStatement(string_stream.str());
    statement->BindString(1, name);
    if (parent_has_value)
    {
        statement->BindInt64(2, parent.value());
    }

    return statement;
}

/*
imgdoc2::dbIndex DocumentMetadataWriter::AddItem(std::optional<imgdoc2::dbIndex> parent_id, std::string name, imgdoc2::DocumentMetadataType type, const imgdoc2::IDocumentMetadata::metadata_item_variant& value)
{
    ostringstream string_stream;
    string_stream << "INSERT INTO [" << "METADATA" << "] (" <<
        "[" << "Name" << "]," <<
        "[" << "AncestorId" << "]," <<
        "[" << "TypeDiscriminator" << "]," <<
        "[" << "ValueDouble" << "]," <<
        "[" << "ValueInteger" << "]," <<
        "[" << "ValueString" << "]," <<
        "[" << "ValueJson" << "]) " <<
        "VALUES(" << "?1, ?2, ?3, ?4, ?5, ?6, ?7" << "); ";

    int discriminator = DetermineDiscriminator(value);

    const auto statement = this->document_->GetDatabase_connection()->PrepareStatement(string_stream.str());
    int binding_index = 1;
    statement->BindString(binding_index++, name);
    if (parent_id.has_value())
    {
        statement->BindInt64(binding_index++, parent_id.value());
    }
    else
    {
        statement->BindNull(binding_index++);
    }

    statement->BindInt32(binding_index++, discriminator);
    if (discriminator == 3)
    {
        statement->BindDouble(binding_index++, std::get<double>(value));
    }
    else
    {
        statement->BindNull(binding_index++);
    }

    if (discriminator == 2)
    {
        statement->BindInt32(binding_index++, std::get<int>(value));
    }
    else
    {
        statement->BindNull(binding_index++);
    }

    if (discriminator == 1)
    {
        statement->BindString(binding_index++, std::get<std::string>(value));
    }
    else
    {
        statement->BindNull(binding_index++);
    }

    statement->BindNull(binding_index++);

    auto id = this->document_->GetDatabase_connection()->ExecuteAndGetLastRowId(statement.get());
    return id;
}*/

//DocumentMetadataWriter::DataType DocumentMetadataWriter::DetermineDataType(imgdoc2::DocumentMetadataType type, const imgdoc2::IDocumentMetadata::metadata_item_variant& value)
//{
//    if (!std::holds_alternative<monostate>(value))
//    {
//        return DataType::null;
//    }
//
//    switch (type)
//    {
//        case DocumentMetadataType::Text:
//            // in this case the value must contain a string
//            if (!std::holds_alternative<std::string>(value))
//            {
//                throw std::invalid_argument("The value must be a string");
//            }
//
//            return DataType::utf8string;
//        case DocumentMetadataType::Int32:
//            // in this case the value must contain an integer
//            if (!std::holds_alternative<int>(value))
//            {
//                throw std::invalid_argument("The value must be an integer");
//            }
//
//            return DataType::int32;
//        case DocumentMetadataType::Double:
//            // in this case the value must contain a double
//            if (!std::holds_alternative<double>(value))
//            {
//                throw std::invalid_argument("The value must be a double");
//            }
//
//            return DataType::doublefloat;
//        case DocumentMetadataType::Json:
//            // in this case the value must contain a string
//            if (!std::holds_alternative<string>(value))
//            {
//                throw std::invalid_argument("The value must be a string");
//            }
//
//            return DataType::json;
//        case DocumentMetadataType::Default:
//            if (!std::holds_alternative<std::string>(value))
//            {
//                return DataType::utf8string;
//            }
//            else if (!std::holds_alternative<int>(value))
//            {
//                return DataType::int32;
//            }
//            else if (!std::holds_alternative<double>(value))
//            {
//                return DataType::doublefloat;
//            }
//            else
//            {
//                throw std::invalid_argument("Unknown metadata item type");
//            }
//    }
//}


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
    for (size_t i = count_existing_pks; i < path_parts.size()-1; ++i)
    {
        const auto new_node = this->UpdateOrCreateItem(
            i > 0 ? std::optional<imgdoc2::dbIndex>(pks_existing[i - 1]) : std::optional<imgdoc2::dbIndex>(),
            true,
            string{path_parts[i]},
            DocumentMetadataType::Null,
            std::monostate());
        pks_existing.push_back(new_node);
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

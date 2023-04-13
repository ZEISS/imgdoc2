// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include "documentMetadataBase.h"
#include <string>
#include "exceptions.h"
#include <gsl/narrow>

using namespace std;
using namespace imgdoc2;

DocumentMetadataBase::DatabaseDataTypeValue DocumentMetadataBase::DetermineDatabaseDataTypeValueOrThrow(imgdoc2::DocumentMetadataType type, const imgdoc2::IDocumentMetadata::metadata_item_variant& value)
{
    const DatabaseDataTypeValue result = DetermineDatabaseDataTypeValue(type, value);
    if (result == DatabaseDataTypeValue::invalid)
    {
        throw imgdoc2::invalid_argument_exception("The value is invalid");
    }

    return result;
}

/*static*/DocumentMetadataBase::DatabaseDataTypeValue DocumentMetadataBase::DetermineDatabaseDataTypeValue(imgdoc2::DocumentMetadataType type, const imgdoc2::IDocumentMetadata::metadata_item_variant& value)
{
    if (std::holds_alternative<monostate>(value))
    {
        return DatabaseDataTypeValue::null;
    }

    switch (type)
    {
        case DocumentMetadataType::kNull:
            // in this the value does not matter
            return DatabaseDataTypeValue::null;
        case DocumentMetadataType::kText:
            // in this case the value must contain a string
            if (!std::holds_alternative<std::string>(value))
            {
                throw invalid_argument_exception("The value must be a string");
            }

            return DatabaseDataTypeValue::utf8string;
        case DocumentMetadataType::kInt32:
            // in this case the value must contain an integer
            if (!std::holds_alternative<int>(value))
            {
                throw invalid_argument_exception("The value must be an integer");
            }

            return DatabaseDataTypeValue::int32;
        case DocumentMetadataType::kDouble:
            // in this case the value must contain a double
            if (!std::holds_alternative<double>(value))
            {
                throw invalid_argument_exception("The value must be a double");
            }

            return DatabaseDataTypeValue::doublefloat;
        case DocumentMetadataType::kJson:
            // in this case the value must contain a string
            if (!std::holds_alternative<string>(value))
            {
                throw invalid_argument_exception("The value must be a string");
            }

            return DatabaseDataTypeValue::json;
        case DocumentMetadataType::kDefault:
            if (std::holds_alternative<std::string>(value))
            {
                return DatabaseDataTypeValue::utf8string;
            }
            else if (std::holds_alternative<int>(value))
            {
                return DatabaseDataTypeValue::int32;
            }
            else if (std::holds_alternative<double>(value))
            {
                return DatabaseDataTypeValue::doublefloat;
            }
            else
            {
                throw invalid_argument_exception("Unknown metadata item type");
            }
        case DocumentMetadataType::kInvalid:
            throw invalid_argument_exception("The metadata type is invalid");
    }

    return DatabaseDataTypeValue::invalid;
}

int DocumentMetadataBase::BindTypeDiscriminatorAndData(
    const std::shared_ptr<IDbStatement>& database_statement,
    int binding_index,
    DatabaseDataTypeValue type,
    const imgdoc2::IDocumentMetadata::metadata_item_variant& value)
{
    database_statement->BindInt32(binding_index++, gsl::narrow_cast<int>(type));
    if (std::holds_alternative<double>(value))
    {
        database_statement->BindDouble(binding_index++, std::get<double>(value));
    }
    else
    {
        database_statement->BindNull(binding_index++);
    }

    if (std::holds_alternative<int32_t>(value))
    {
        database_statement->BindInt32(binding_index++, std::get<int>(value));
    }
    else
    {
        database_statement->BindNull(binding_index++);
    }

    if (std::holds_alternative<string>(value))
    {
        database_statement->BindString(binding_index++, std::get<string>(value));
    }
    else
    {
        database_statement->BindNull(binding_index++);
    }

    return binding_index;
}

/*static*/std::vector<std::string_view> DocumentMetadataBase::SplitPath(const std::string_view& path)
{
    std::vector<std::string_view> tokens;
    std::size_t start = 0, end;
    while ((end = path.find(DocumentMetadataBase::kPathDelimiter_, start)) != std::string_view::npos)
    {
        if (end == start)
        {
            throw invalid_path_exception("path must not contain zero-length fragments");
        }

        tokens.push_back(path.substr(start, end - start));
        start = end + 1;
    }

    if (start == path.size())
    {
        throw invalid_path_exception("path must not end with a delimiter");
    }

    tokens.push_back(path.substr(start));
    return tokens;
}

std::shared_ptr<IDbStatement> DocumentMetadataBase::CreateQueryForNodeIdsForPath(const std::vector<std::string_view>& path_parts)
{
    ostringstream string_stream;
    const auto metadata_table_name = this->GetDocument()->GetDataBaseConfigurationCommon()->GetTableNameForMetadataTableOrThrow();
    const auto column_name_pk = this->GetDocument()->GetDataBaseConfigurationCommon()->GetColumnNameOfMetadataTableOrThrow(DatabaseConfigurationCommon::kMetadataTable_Column_Pk);
    const auto column_name_name = this->GetDocument()->GetDataBaseConfigurationCommon()->GetColumnNameOfMetadataTableOrThrow(DatabaseConfigurationCommon::kMetadataTable_Column_Name);
    const auto column_name_ancestor_id = this->GetDocument()->GetDataBaseConfigurationCommon()->GetColumnNameOfMetadataTableOrThrow(DatabaseConfigurationCommon::kMetadataTable_Column_AncestorId);

    if (path_parts.size() > 1)
    {
        string_stream << "WITH RECURSIVE paths(id, name, level) AS( " <<
            "SELECT " << column_name_pk << "," << column_name_name << ",1 FROM [" << metadata_table_name << "] WHERE " << column_name_ancestor_id << " IS NULL AND " << column_name_name << "=? " <<
            "UNION " <<
            "SELECT " << metadata_table_name << "." << column_name_pk << ", " << metadata_table_name << "." << column_name_name << ",  level + 1 " <<
            "FROM [" << metadata_table_name << "] JOIN paths WHERE " << metadata_table_name << "." << column_name_ancestor_id << "=paths.id AND " <<
            "CASE level ";

        for (size_t i = 1; i < path_parts.size(); i++)
        {
            string_stream << "WHEN " << i << " THEN " << metadata_table_name << "." << column_name_name << "=? ";
        }

        string_stream << "END) " <<
            "SELECT id FROM paths;";
    }
    else if (path_parts.size() == 1)
    {
        string_stream << "SELECT " << column_name_pk << " FROM " << metadata_table_name << " WHERE " << column_name_ancestor_id << " IS NULL AND " << column_name_name << "=?;";
    }
    else
    {
        throw invalid_argument_exception("The path must contain at least one part");
    }

    auto statement = this->document_->GetDatabase_connection()->PrepareStatement(string_stream.str());
    return statement;
}

std::vector<imgdoc2::dbIndex> DocumentMetadataBase::GetNodeIdsForPath(const std::string& path, size_t* count_of_parts_in_path)
{
    // an empty string is legal (and means "the root"), ie. we return an empty vector
    if (path.empty())
    {
        if (count_of_parts_in_path != nullptr)
        {
            *count_of_parts_in_path = 0;
        }

        return {};
    }

    // the path must NOT start with a slash
    if (path[0] == DocumentMetadataBase::kPathDelimiter_)
    {
        throw invalid_path_exception("The path must not start with a slash");
    }

    const std::vector<std::string_view> tokens = DocumentMetadataBase::SplitPath(path);
    if (count_of_parts_in_path != nullptr)
    {
        *count_of_parts_in_path = tokens.size();
    }

    return this->GetNodeIdsForPathParts(tokens);
}

std::vector<imgdoc2::dbIndex> DocumentMetadataBase::GetNodeIdsForPathParts(const std::vector<std::string_view>& parts)
{
    const auto statement = this->CreateQueryForNodeIdsForPath(parts);

    // TODO(JBl) : The binding currently is making a copy of the string. This is not necessary, we could use a "STATIC" binding
    //              if we ensure that the string is not deleted before the statement is executed.
    for (size_t i = 0; i < parts.size(); i++)
    {
        statement->BindStringView(gsl::narrow<int>(i + 1), parts[i]);
    }

    std::vector<imgdoc2::dbIndex> result;
    result.reserve(parts.size());
    while (this->document_->GetDatabase_connection()->StepStatement(statement.get()))
    {
        const imgdoc2::dbIndex index = statement->GetResultInt64(0);
        result.push_back(index);
    }

    return result;
}

bool DocumentMetadataBase::TryMapPathAndGetTerminalNode(const std::string& path, std::optional<imgdoc2::dbIndex>* terminal_node_id)
{
    size_t count_of_parts_in_path;
    const auto node_ids = this->GetNodeIdsForPath(path, &count_of_parts_in_path);
    if (count_of_parts_in_path == 0)
    {
        // this is a "special case", the path is empty, which means the "root"
        if (terminal_node_id != nullptr)
        {
            *terminal_node_id = std::nullopt;
        }

        return true;
    }

    if (node_ids.size() == count_of_parts_in_path)
    {
        if (terminal_node_id != nullptr)
        {
            *terminal_node_id = node_ids.back();
        }

        return true;
    }

    return false;
}

bool DocumentMetadataBase::CheckIfItemExists(imgdoc2::dbIndex primary_key)
{
    ostringstream string_stream;
    string_stream << "SELECT EXISTS(SELECT 1 FROM [" << this->GetDocument()->GetDataBaseConfigurationCommon()->GetTableNameForMetadataTableOrThrow() << "] " << 
        "WHERE [" << this->GetDocument()->GetDataBaseConfigurationCommon()->GetColumnNameOfMetadataTableOrThrow(DatabaseConfigurationCommon::kMetadataTable_Column_Pk) << "]=?1)";

    const auto statement = this->GetDocument()->GetDatabase_connection()->PrepareStatement(string_stream.str());
    statement->BindInt64(1, primary_key);

    if (!this->GetDocument()->GetDatabase_connection()->StepStatement(statement.get()))
    {
        throw internal_error_exception("DocumentMetadataReader::CheckIfItemExists: Could not execute statement.");
    }

    const int64_t result = statement->GetResultInt64(0);
    return result == 1;
}

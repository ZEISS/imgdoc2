#include "documentMetadataBase.h"
#include "exceptions.h"
#include "gsl/narrow"

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
        case DocumentMetadataType::Text:
            // in this case the value must contain a string
            if (!std::holds_alternative<std::string>(value))
            {
                throw invalid_argument_exception("The value must be a string");
            }

            return DatabaseDataTypeValue::utf8string;
        case DocumentMetadataType::Int32:
            // in this case the value must contain an integer
            if (!std::holds_alternative<int>(value))
            {
                throw invalid_argument_exception("The value must be an integer");
            }

            return DatabaseDataTypeValue::int32;
        case DocumentMetadataType::Double:
            // in this case the value must contain a double
            if (!std::holds_alternative<double>(value))
            {
                throw invalid_argument_exception("The value must be a double");
            }

            return DatabaseDataTypeValue::doublefloat;
        case DocumentMetadataType::Json:
            // in this case the value must contain a string
            if (!std::holds_alternative<string>(value))
            {
                throw invalid_argument_exception("The value must be a string");
            }

            return DatabaseDataTypeValue::json;
        case DocumentMetadataType::Default:
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
    }

    return DatabaseDataTypeValue::invalid;
}

//void DocumentMetadataBase::BindAncestorId(const std::shared_ptr<IDbStatement>& database_statement, int binding_index, const std::optional<imgdoc2::dbIndex>& parent)
//{
//    if (parent.has_value())
//    {
//        database_statement->BindInt64(binding_index, parent.value());
//    }
//    else
//    {
//        database_statement->BindNull(binding_index);
//    }
//}

int DocumentMetadataBase::BindTypeDiscriminatorAndData(
    const std::shared_ptr<IDbStatement>& database_statement,
    int binding_index,
    DatabaseDataTypeValue type,
    const imgdoc2::IDocumentMetadata::metadata_item_variant& value)
{
    database_statement->BindInt32(binding_index++, (int)type);
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

static std::vector<std::string_view> tokenize(std::string_view str, char deliminator)
{
    std::vector<std::string_view> tokens;
    std::size_t start = 0, end = 0;
    while ((end = str.find(deliminator, start)) != std::string_view::npos)
    {
        tokens.push_back(str.substr(start, end - start));
        start = end + 1;
    }

    tokens.push_back(str.substr(start));
    return tokens;
}

std::vector<std::string_view> DocumentMetadataBase::SplitPath(const std::string_view& path)
{
    std::vector<std::string_view> tokens;
    std::size_t start = 0, end = 0;
    while ((end = path.find('/', start)) != std::string_view::npos)
    {
        tokens.push_back(path.substr(start, end - start));
        start = end + 1;
    }

    tokens.push_back(path.substr(start));
    return tokens;
}

std::shared_ptr<IDbStatement> DocumentMetadataBase::CreateQueryForNodeIdsForPath(const std::vector<std::string_view>& path_parts)
{
    ostringstream string_stream;

    if (path_parts.size() > 1)
    {
        string_stream << "WITH RECURSIVE paths(id, name, level) as ( " <<
            "SELECT Pk, Name,  1 from METADATA where AncestorId IS NULL AND Name=? " <<
            "UNION " <<
            "SELECT METADATA.Pk, METADATA.name,  level + 1 " <<
            "FROM METADATA JOIN paths WHERE METADATA.AncestorId = paths.id AND " <<
            "CASE level ";

        for (size_t i = 1; i < path_parts.size(); i++)
        {
            string_stream << "WHEN " << i << " THEN METADATA.Name=? ";
        }

        string_stream << "END) " <<
            "SELECT id FROM paths;";
    }
    else if (path_parts.size() == 1)
    {
        string_stream << "SELECT Pk FROM METADATA WHERE AncestorId IS NULL AND Name=?;";
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
    if (path[0] == '/')
    {
        // TODO(Jbl): find more appropriate exception
        throw invalid_argument_exception("The path must not start with a slash");
    }

    const std::vector<std::string_view> tokens = tokenize(path, '/');
    if (count_of_parts_in_path != nullptr)
    {
        *count_of_parts_in_path = tokens.size();
    }

    return this->GetNodeIdsForPathParts(tokens);
    /*const auto statement = this->CreateQueryForNodeIdsForPath(tokens);

    // TODO(JBl) : The binding currently is making a copy of the string. This is not necessary, we could use a "STATIC" binding
    //              if we ensure that the string is not deleted before the statement is executed.
    for (size_t i = 0; i < tokens.size(); i++)
    {
        statement->BindStringView(gsl::narrow<int>(i + 1), tokens[i]);
    }

    std::vector<imgdoc2::dbIndex> result;
    result.reserve(tokens.size());
    while (this->document_->GetDatabase_connection()->StepStatement(statement.get()))
    {
        const imgdoc2::dbIndex index = statement->GetResultInt64(0);
        result.push_back(index);
    }

    return result;*/
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

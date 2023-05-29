#include "MyCoolDB.h"

std::vector<std::string> MyCoolDB::GetTableNames() const {
    std::vector<std::string> table_names;
    table_names.reserve(tables_.size());
    for (auto& [table_name, _] : tables_) {
        table_names.push_back(table_name);
    }

    return table_names;
}

void MyCoolDB::Load(const std::filesystem::path& path) {
    std::ifstream file(path);

    char ch;
    std::string query;
    while (file.get(ch)) {
        if (ch == ';') {
            Request(query);
            query = "";
            continue;
        } else if (ch == '\n') {
            continue;
        }

        query += ch;
    }

    file.close();
}

void MyCoolDB::Save(const std::filesystem::path& path) {

    std::ofstream file(path);

    for (auto& [name, table] : tables_) {
        file << "CREATE TABLE " + name + " (\n";

        for (auto& [_, column] : table->columns_) {
            file <<  "    " + column->GetStrStructure() + ",\n";
        }

        for (auto& [column_name, column] : table->columns_) {
            if (!column->GetStrForeignKey().empty()) {
                file << "    FOREIGN KEY (" + name + ") REFERENCES " + column->GetStrForeignKey() + ",\n";
            }
        }

        file << ");\n\n";
    }

    for (auto& [name, table] : tables_) {
        std::string cur = "INSERT INTO " + name + "(";

        for (auto& [column_name, column] : table->columns_) {
            cur += column_name + ", ";
        }

        cur += ") VALUES (";

        for (size_t i = 0 ; i < table->Count(); ++i) {
            file << cur;

            for (auto& [_, column] : table->columns_) {
                file << column->GetStrData(i) + ", ";
            }

            file << ");\n";
        }

        file << '\n';
    }

    file.close();
}

void MyCoolDB::Request(const std::string& request) {
    RequestParser sql(request);

    if (sql.Type() == RequestParser::RequestType::Create && tables_.contains(sql.GetTableName())) {
        throw MyBDException("Table already exists");
    }

    if (sql.Type() != RequestParser::RequestType::Create && !tables_.contains(sql.GetTableName())) {
        throw MyBDException("No such table");
    }

    switch (sql.Type()) {
        case RequestParser::RequestType::Create: {
            tables_[sql.GetTableName()] = std::make_unique<Table>(sql, tables_);
            break;
        }

        case RequestParser::RequestType::Drop:
            tables_[sql.GetTableName()].reset();
            tables_.erase(sql.GetTableName());
            break;

        case RequestParser::RequestType::Update:
            tables_[sql.GetTableName()]->UpdateRowsValues(sql);
            break;

        case RequestParser::RequestType::Insert:
            tables_[sql.GetTableName()]->AddRow(sql);
            break;

        case RequestParser::RequestType::Delete:
            tables_[sql.GetTableName()]->DeleteRow(sql);
            break;

        default:
            return;
    }
}

Result MyCoolDB::RequestQuery(const std::string& request) {
    RequestParser tmp(request);

    switch (tmp.Type()) {
        case RequestParser::RequestType::Select:
            return Result(*tables_[tmp.GetTableName()], tmp);
        case RequestParser::RequestType::Join:
            return JoinTables(tmp);
        default:
            return {};
    }
}



Result MyCoolDB::JoinTables(RequestParser& sql) {
    if (sql.Type() != RequestParser::RequestType::Join) {
        return {};
    }

    Result first_table = RequestQuery("SELECT * FROM " + sql.GetTableName());
    Result second_table = RequestQuery("SELECT * FROM " + sql.GetData()["table join"]);

    return Result::JoinTables(first_table, second_table, sql);
}



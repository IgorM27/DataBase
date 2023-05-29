#include "Table.h"


Table::Table(const RequestParser& request, const std::map<std::string, std::unique_ptr<Table>>& tables) {
    if (request.type_ != RequestParser::RequestType::Create) {
        return;
    }

    for (auto& [column, tmp] : request.GetData()) {
        std::vector<std::string> param = split(tmp,' ');
        AddColumn(column, GetType(param[0]));
        if (std::find(param.begin(), param.end(), "PRIMARY") != param.end()) {
            columns_[column]->SetPrimaryKeyFlag(true);
        }
    }
    for (auto& [column, tmp] : request.GetData()) {
        if (column[0] == 'F' && column[1] == ' ') {
            CreateForeignKey(column.substr(2), tmp, tables);
            continue;
        }
    }
}

size_t Table::Count() const {
    return columns_.begin()->second->Size();
}


void Table::AddRow(const RequestParser& sql) {
    if (sql.type_ != RequestParser::RequestType::Insert) {
        return;
    }
    auto row = sql.GetData();
    for (const auto& [tmp, column] : columns_) {
        if (row.contains(column->GetName())) {
            column->AddValue(row[column->GetName()]);
        } else {
            column->AddDefault();
        }
    }
}

void Table::UpdateRowsValues(const RequestParser& request) {
    if (request.type_ != RequestParser::RequestType::Update) {
        return;
    }
    std::vector<RequestParser::Condition> conditions = request.GetConditions();
    for (size_t i = 0; i < Count(); ++i) {
        if (CheckConditions(conditions, i)) {
            for (auto& [column, value]: request.columns_values) {
                if (!columns_.contains(column)) {
                    throw MyBDException("Incorrect update request");
                }
                columns_[column]->SetValue(value, i);
            }
        }
    }
}

void Table::DeleteRow(const RequestParser& request) {
    if (request.type_ != RequestParser::RequestType::Delete) {
        return;
    }
    std::vector<RequestParser::Condition> conditions = request.GetConditions();
    for (long long i = Count() - 1; i >= 0; --i) {
        if (CheckConditions(conditions, i)) {
            for (auto& [name, column]: columns_) {
                column->DeleteValue(i);
            }
        }
    }
}


Table::Table(Table& table, const RequestParser& request) {
    if (request.Type() != RequestParser::RequestType::Select && request.Type() != RequestParser::RequestType::Join) {
        return;
    }
    std::unordered_map<std::string, std::string> row = request.GetData();
    for (const auto& [_, column] : table.columns_) {
        if (row.contains(column->GetName()) || row.contains("*")) {
            AddColumn(column->GetName(), column->Type());
        }
    }
    std::vector<RequestParser::Condition> conditions = request.GetConditions();
    for (size_t i = 0; i < table.Count(); ++i) {
        if (table.CheckConditions(conditions, i)) {
            for (auto& [name, column]: columns_) {
                column->CopyValueFrom(table.columns_[name].get(), i);
            }
        }
    }
}

std::vector<std::string> Table::split(const std::string& tmp, char ch) {
    std::vector<std::string> result;
    std::stringstream stream(tmp);
    std::string string;
    while (std::getline(stream, string, ch)) {
        result.push_back(string);
    }
    return result;
}

bool Table::CheckConditions(const std::vector<RequestParser::Condition>& conditions, size_t ind_row) {
    if (conditions.empty()) {
        return true;
    }
    size_t index = 0;
    while (index < conditions.size() && conditions[index].GetCondition() != RequestParser::Condition::Type::WHERE) {
        index++;
    }
    if (index >= conditions.size()) {
        return true;
    }
    bool result = CheckOneCondition(conditions[index], ind_row);
    index++;
    for (; index < conditions.size(); ++index) {
        bool tmp = CheckOneCondition(conditions[index], ind_row);
            if(conditions[index].GetCondition() == RequestParser::Condition::AND){
                result = result && tmp;
            } else if (conditions[index].GetCondition() == RequestParser::Condition::OR){
                result = result || tmp;
            } else if (conditions[index].GetCondition() == RequestParser::Condition::ON){
                throw MyBDException("Incorrect operand condition");
            }
    }
    return result;
}

bool Table::CheckOneCondition(const RequestParser::Condition& condition, size_t ind_row) {
    if (columns_.contains(condition.GetLhs()) && columns_.contains(condition.GetRhs())) {
        return columns_[condition.GetRhs()]->Compare(*columns_[condition.GetLhs()], condition, ind_row);
    }
    if (columns_.contains(condition.GetLhs())) {
        return columns_[condition.GetLhs()]->Compare(condition.GetRhs(), condition, ind_row);
    }
    if (columns_.contains(condition.GetRhs())) {
        return columns_[condition.GetRhs()]->Compare(condition.GetLhs(), condition, ind_row);
    }
    return true;
}

void Table::CreateForeignKey(const std::string& column, const std::string& foreign, const std::map<std::string, std::unique_ptr<Table>>& other_tables) {
    std::vector<std::string> parameters = split(foreign, ' ');
    columns_[column].get()->SetForeignKey(parameters[0],other_tables.at(parameters[0])->columns_.at(parameters[1]).get());
}

void Table::AddColumn(const std::string& name, const Column::VariablesType& type) {
    columns_[name] = std::make_unique<Column>(name, type);
}

std::vector<std::string> Table::GetColumnNames() const {
    std::vector<std::string> names;
    for (auto& [tmp_1, tmp_2] : columns_) {
        names.push_back(tmp_1);
    }
    return names;
}

Column::VariablesType Table::GetType(const std::string& type) {
    if (type == "INT") {
        return Column::VariablesType::Int;
    } else if (type == "VARCHAR") {
        return  Column::VariablesType::Varchar;
    } else if (type == "BOOL") {
        return Column::VariablesType::Bool;
    } else if (type == "DOUBLE") {
        return Column::VariablesType::Double;
    }
    return Column::VariablesType::Float;
}


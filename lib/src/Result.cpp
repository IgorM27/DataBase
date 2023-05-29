#include "Result.h"


bool Result::Next() {
    return ++current_ < Count();
}
void Result::Reset() {
    current_ = 0;
}
//std::ostream& operator<<(std::ostream& out, Result& result_set) {
//    std::vector<std::string> column_names = result_set.GetColumnNames();
//    std::ranges::reverse_copy(column_names, std::ostream_iterator<std::string>(out, " | "));
//    out << '\n';
//    for (size_t i = 0; i < result_set.Count(); ++i) {
//        for (auto j = column_names.cend() - 1; j >= column_names.cbegin(); --j) {
//            out << result_set.columns_[*j]->GetStrData(i) << " | ";
//        }
//        out << '\n';
//    }
//    return out;
//}

Result Result::JoinTables(Table& table1, Table& table2, RequestParser& request) {
    std::unordered_map<std::string, std::string> rows = request.GetData();
    std::string left_table_name = request.GetTableName() + '.';
    std::string right_table_name = rows["table join"] + '.';
    Table joined_table;
    for (auto& [name, column] : table1.columns_) {
        std::string table_column_name = left_table_name + name;
        if (rows.contains(table_column_name) || rows.contains("*")) {
            joined_table.AddColumn(table_column_name, column->Type());
        }
    }
    for (auto& [name, column] : table2.columns_) {
        std::string table_column_name = right_table_name + name;
        if (rows.contains(table_column_name) || rows.contains("*")) {
            joined_table.AddColumn(table_column_name, column->Type());
        }
    }
    Table* left_table = &table1;
    Table* right_table = &table2;
    if (rows["join variant"] == "RIGHT") {
        std::swap(left_table, right_table);
        std::swap(left_table_name, right_table_name);
    }
    bool Inner = rows["join variant"] == "INNER";
    std::vector<std::pair<Column*, Column*>> ON_conditions;
    ON_conditions.reserve(request.GetConditions().size());
    for (auto& condition : request.GetConditions()) {
        if (condition.GetCondition() == RequestParser::Condition::Type::WHERE) {
            break;
        }

        std::string left_column = split(condition.GetLhs(), '.')[1];
        std::string right_column = split(condition.GetRhs(), '.')[1];

        if (left_table->columns_.contains(left_column) && right_table->columns_.contains(right_column)) {
            ON_conditions.emplace_back(left_table->columns_[left_column].get(),
                                       right_table->columns_[right_column].get());
        } else if (right_table->columns_.contains(left_column) && left_table->columns_.contains(right_column)) {
            ON_conditions.emplace_back(left_table->columns_[right_column].get(),
                                       right_table->columns_[left_column].get());
        } else {
            throw MyBDException("Incorrect JOIN condition");
        }
    }
    RequestParser::Condition equals_obj{};
    equals_obj.SetOperation("=");
    for (size_t i = 0; i < left_table->Count(); ++i) {
        bool joined_flag = false;
        for (auto& [column1, column2] : ON_conditions) {
            for (size_t j = 0; j < right_table->Count(); ++j) {
                bool equals = true;
                for (auto& [lhs, rhs] : ON_conditions) {
                    if (!lhs->Compare(*rhs, equals_obj, i, j)) {
                        equals = false;
                        break;
                    }
                }
                if (equals) {
                    joined_flag = true;
                    for (auto& [name, column]: left_table->columns_) {
                        if (joined_table.columns_.contains(left_table_name + name)) {
                            joined_table.columns_[left_table_name + name]->CopyValueFrom(column.get(), i);
                        }
                    }
                    for (auto& [name, column]: right_table->columns_) {
                        if (joined_table.columns_.contains(right_table_name + name)) {
                            joined_table.columns_[right_table_name + name]->CopyValueFrom(column.get(), j);
                        }
                    }
                }
            }
        }
        if (!joined_flag && !Inner) {
            for (auto& [name, column]: left_table->columns_) {
                if (joined_table.columns_.contains(left_table_name + name)) {
                    joined_table.columns_[left_table_name + name]->CopyValueFrom(column.get(), i);
                }
            }
            for (auto& [name, _]: right_table->columns_) {
                joined_table.columns_[right_table_name + name]->AddValue("NULL");
            }
        }
    }
    return Result(joined_table, request);
}

Element Result::Get(const std::string& columnName) {
    if (current_ == SIZE_MAX) {
        ++current_;
    }
    if (current_ >= Count()) {
        return {};
    }
    for (const auto& [name, column] : columns_) {
        if (name == columnName) {
            return column.get()->GetValueFromIndex(current_);
        }
    }
    return {};
}

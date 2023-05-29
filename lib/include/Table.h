#pragma once

#include "RequestParser.h"
#include "Column.h"

class Table {
protected:
    std::unordered_map<std::string, std::unique_ptr<Column>> columns_;
    Table() = default;
    Table(Table& table, const RequestParser& sql);
    static std::vector<std::string> split(const std::string& tmp, char ch);

private:
    bool CheckConditions(const std::vector<RequestParser::Condition>& conditions, size_t ind_row);
    bool CheckOneCondition(const RequestParser::Condition& condition, size_t ind_row);
    void CreateForeignKey(const std::string& column, const std::string& foreign, const std::map<std::string, std::unique_ptr<Table>>& other_tables);
    void AddColumn(const std::string& name, const Column::VariablesType& type);
    Column::VariablesType GetType(const std::string& type);

public:
    explicit Table(const RequestParser& request, const std::map<std::string, std::unique_ptr<Table>>& tables);

    [[nodiscard]] size_t Count() const;
    void AddRow(const RequestParser& sql);
    void UpdateRowsValues(const RequestParser& request);
    void DeleteRow(const RequestParser& request);
    [[nodiscard]] std::vector<std::string> GetColumnNames() const;

    friend class Result;
    friend class MyCoolDB;
};

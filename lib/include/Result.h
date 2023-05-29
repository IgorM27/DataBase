#pragma once

#include "Table.h"

class Result : public Table {
public:
    Result() = default;
    explicit Result(Table& table, const RequestParser& sql)
        : Table(table, sql)
    {}
    static Result JoinTables(Table& table1, Table& table2, RequestParser& request);
    bool Next();
    void Reset();
    Element Get(const std::string& columnName);
//    friend std::ostream& operator<<(std::ostream& os, Result& result_set);

private:
    size_t current_ = SIZE_MAX;
};

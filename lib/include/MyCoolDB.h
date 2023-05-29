#pragma once

#include "Table.h"
#include "RequestParser.h"
#include "Result.h"

#include <memory>
#include <filesystem>
#include <fstream>

class MyCoolDB {
public:
    void Load(const std::filesystem::path& path);
    void Save(const std::filesystem::path& path);

    void Request(const std::string& request);
    Result RequestQuery(const std::string& request);

    [[nodiscard]] std::vector<std::string> GetTableNames() const;

private:

    Result JoinTables(RequestParser& sql);
    std::map<std::string, std::unique_ptr<Table>> tables_;

    friend class Table;
};

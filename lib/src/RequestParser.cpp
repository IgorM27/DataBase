#include "RequestParser.h"


RequestParser::RequestParser(const std::string& req) {
    SetType(req);
    if (type_ == RequestType::None) {
        throw MyBDException("Bad request");
    }
}

RequestParser::RequestType RequestParser::Type() const {
    return type_;
}

std::unordered_map<std::string, std::string> RequestParser::GetData() const {
    return columns_values;
}

std::vector<RequestParser::Condition> RequestParser::GetConditions() const {
    return conditions;
}

std::string RequestParser::GetTableName() const {
    return table;
}


void RequestParser::SetCondition(const std::string& tmp)  {
    if (tmp.empty()){
        return;
    }

    std::regex regex(R"((WHERE|AND|OR|ON)\s+([^\s=]+)\s*([(?:IS)<>!=]+)\s*((?:NOT NULL|NULL)|(?:'[^']*')|(?:[^,\s]+)+))");
    std::smatch matches;

    auto start= tmp.cbegin();
    while (std::regex_search(start, tmp.cend(), matches, regex)) {
        if (matches.size() >= 5) {

            Condition tmp_condition;
            tmp_condition.SetCondition(matches[1].str());
            tmp_condition.SetLhs(matches[2].str());
            tmp_condition.SetOperation(matches[3].str());
            tmp_condition.SetRhs(matches[4].str());

            conditions.push_back(tmp_condition);
        } else {
            throw MyBDException("Incorrect condition");
        }

        start = matches.suffix().first;
    }

    if (conditions.empty()) {
        throw MyBDException("Empty condition");
    }
}

void RequestParser::SetColumnsValues(const std::string& tmp, std::vector<std::string>& vector) {
    std::regex format(R"(('[^']*')|([^,\s]+))");

    std::sregex_iterator end;
    for (std::sregex_iterator begin(tmp.begin(), tmp.end(), format); begin != end; ++begin) {
        vector.push_back(begin->str());
    }
}

bool RequestParser::CheckSelect(const std::string& req) {
    std::regex regex(R"(SELECT\s+(.+)\s+FROM\s+([^\s]+)\s*(?:(WHERE\s+[^;]+))?[\s;]*)");
    std::smatch tmp;

    if (std::regex_match(req, tmp, regex) && tmp.size() >= 3) {
        std::string columns_str = tmp[1].str();
        if (columns_str == "*") {
            columns_values["*"] = "";
        } else {
            std::vector<std::string> columns;
            SetColumnsValues(columns_str, columns);

            for (auto& column : columns) {
                columns_values[column] = "";
            }
        }
        table = tmp[2].str();
        if (tmp.size() >= 4) {
            SetCondition(tmp[3].str());
        }
        return true;
    }
    return false;
}

bool RequestParser::CheckDelete(const std::string& req)  {
    std::regex regex(R"(DELETE\s+FROM\s+([^\s]+)\s*(?:(WHERE\s+[^;]+))?;*)");
    std::smatch tmp;

    if (std::regex_match(req, tmp, regex) && tmp.size() >= 2) {
        table = tmp[1].str();
        if (tmp.size() >= 3) {
            SetCondition(tmp[2].str());
        }
        return true;
    }
    return false;
}

bool RequestParser::CheckInsert(const std::string &req) {
    std::regex regex(R"(INSERT\s+INTO\s+([^\s]+)\s*\(([^)]+)\)\s*VALUES\s*\(([^)]+)\);?)");
    std::smatch tmp;

    if (std::regex_match(req, tmp, regex) && tmp.size() >= 4) {
        table = tmp[1].str();
        std::vector<std::string> columns;
        std::vector<std::string> values;
        SetColumnsValues(tmp[2].str(), columns);
        SetColumnsValues(tmp[3].str(), values);
        if (columns.size() != values.size()) {
            throw MyBDException("Incorrect insert req");
        }
        for (size_t i = 0; i < columns.size(); ++i) {
            columns_values[columns[i]] = values[i];
        }
        return true;
    }
    return false;

}

bool RequestParser::CheckUpdate(const std::string &req)  {
    std::regex regex(R"(UPDATE\s+([^\s]+)\s+SET\s+([^(?:WHERE)]+)\s*(WHERE\s+[^;]*)?[\s;]*)");
    std::smatch matches;

    if (std::regex_match(req, matches, regex) && matches.size() >= 3) {
        table = matches[1].str();
        std::string columns_str = matches[2].str();

        std::regex columns_regex("[^\\s=,]+|'[^']*'");
        std::sregex_iterator end;

        std::string column;
        for (std::sregex_iterator begin(columns_str.begin(), columns_str.end(), columns_regex); begin != end; ++begin) {
            std::string element_str = begin->str();
            if (column.empty()) {
                column = element_str;
            } else {
                columns_values[column] = element_str;
                column = "";
            }
        }
        if (matches.size() >= 4) {
            SetCondition(matches[3].str());
        }
        return true;
    }
    return false;
}

bool RequestParser::CheckCreate(const std::string &req) {
    std::regex regex(R"(CREATE\s+TABLE\s+([^\s]+)\s+\(([\sA-Za-z0-9-_,\(\)]*)\)[\s;]*)");
    std::smatch tmp;

    if (std::regex_match(req, tmp, regex) && tmp.size() >= 2) {
        table = tmp[1].str();

        std::string struct_ = tmp[2].str();
        std::regex struct_regex(R"(([^\s,\(]+)\s+([^\s,]+(?:\s+(?:PRIMARY KEY))*))");

        std::string::const_iterator start(struct_.cbegin());
        while (std::regex_search(start, struct_.cend(), tmp, struct_regex)) {
            if (tmp.size() >= 3 && tmp[2].str() != "KEY" && tmp[2].str() != "REFERENCES") {
                columns_values[tmp[1].str()] = tmp[2].str();
            }
            start = tmp.suffix().first;
        }

        if (columns_values.empty()) {
            return false;
        }

        std::regex foreign_key_regex(R"(FOREIGN\s+KEY\s+\(([^\s]+)\)\s+REFERENCES\s+([^\s\)]+)\(([^\s,]+)\))");

        start = struct_.cbegin();
        while (std::regex_search(start, struct_.cend(), tmp, foreign_key_regex)) {
            if (tmp.size() >= 4) {
                columns_values["F " + tmp[1].str()] = tmp[2].str() + " " + tmp[3].str();
                if (!columns_values.contains(tmp[1].str())) {
                    return false;
                }
            }
            start = tmp.suffix().first;
        }
        return true;
    }
    return false;
}

bool RequestParser::CheckDrop(const std::string &req) {
    std::regex regex(R"(DROP\s+TABLE\s+([^\s;]+)[\s;]*)");
    std::smatch tmp;

    if (std::regex_match(req, tmp, regex) && tmp.size() >= 2) {
        table = tmp[1].str();
        return true;
    }
    return false;
}

bool RequestParser::CheckJoin(const std::string& req) {
    std::regex regex(R"(SELECT\s+(.+)\s+FROM\s+([^\s]+)\s+(LEFT|RIGHT|INNER)?\s+JOIN\s+([^\s]+)\s+(ON\s+[^(?:WHERE)]+)\s*(?:(WHERE\s+[^;]+))?[\s;]*)");
    std::smatch tmp;

    if (std::regex_match(req, tmp, regex) && tmp.size() >= 6) {
        std::string columns_str = tmp[1].str();
        if (columns_str == "*") {
            columns_values["*"] = "";
        } else {
            std::vector<std::string> columns;
            SetColumnsValues(columns_str, columns);

            for (auto& column : columns) {
                columns_values[column] = "";
            }
        }

        table = tmp[2].str();
        columns_values["join variant"]  = tmp[3].str();
        columns_values["table join"] = tmp[4].str();

        SetCondition(tmp[5].str());

        if (tmp.size() >= 7) {
            SetCondition(tmp[6].str());
        }
        return true;
    }
    return false;
}

void RequestParser::SetType(const std::string& req) {
    if (CheckSelect(req)) {
        type_ = Select;
    } else if (CheckInsert(req)) {
        type_ = Insert;
    } else if (CheckDelete(req)) {
        type_ = Delete;
    } else if (CheckUpdate(req)) {
        type_ = Update;
    } else if (CheckCreate(req)) {
        type_ = Create;
    } else if (CheckDrop(req)) {
        type_ = Drop;
    } else if (CheckJoin(req)) {
        type_ = Join;
    }
}

void RequestParser::Condition::SetCondition(const std::string& tmp) {
    if (tmp == "AND") {
        condition_ = AND;
    } else if (tmp == "OR") {
        condition_ = OR;
    } else if (tmp == "WHERE") {
        condition_ = WHERE;
    }  else if (tmp == "ON") {
        condition_ = ON;
    } else {
        throw MyBDException("Incorrect tmp");
    }
}

void RequestParser::Condition::SetLhs(const std::string& lhs) {
    lhs_ = lhs;
}

void RequestParser::Condition::SetOperation(const std::string& tmp) {
    if (tmp == "=" || tmp == "IS") {
        operation_ = Equals;
    } else if (tmp == "!=") {
        operation_ = NotEquals;
    } else if (tmp == ">") {
        operation_ = More;
    } else if (tmp == ">=") {
        operation_ = MoreOrEquals;
    } else if (tmp == "<") {
        operation_ = Lower;
    } else if (tmp == "<=") {
        operation_ = LowerOrEquals;
    }
};

void RequestParser::Condition::SetRhs(const std::string& rhs) {
    if (rhs == "NULL" || rhs == "NOT NULL") {
        if (operation_ != Equals && operation_ != IsNull && operation_ != IsNotNull) {
            throw MyBDException("Bad condition");
        }

        if(rhs == "NULL"){
            operation_ = IsNull;
        } else {
            operation_ = IsNotNull;
        }
    }
    rhs_ = rhs;
}

RequestParser::Condition::Type RequestParser::Condition::GetCondition() const {
    return condition_;
}

std::string RequestParser::Condition::GetLhs() const {
    return lhs_;
}

RequestParser::Condition::Operator RequestParser::Condition::GetOperation() const {
    return operation_;
}

std::string RequestParser::Condition::GetRhs() const {
    return rhs_;
}

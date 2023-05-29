#pragma once

#include "MyBDException.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <regex>

class RequestParser {
public:
    explicit RequestParser(const std::string& req);
    struct Condition {

        enum Type {
            AND, OR, WHERE, ON
        };
        enum Operator {
            Equals, NotEquals, More, MoreOrEquals, Lower, LowerOrEquals, IsNull, IsNotNull
        };
    private:
        Type condition_ = ON;
        std::string lhs_;
        Operator operation_ = Equals;
        std::string rhs_;
    public:
        template<typename T>
        bool Compare(const T& lhs, const T& rhs) const {
            bool result = false;
            if (operation_ == Equals){
                result= lhs == rhs;
            }
            if (operation_ == NotEquals){
                result = lhs != rhs;
            }
            if (operation_ == NotEquals){
                result = lhs != rhs;
            }
            if (operation_ == More){
                result = lhs > rhs;
            }
            if (operation_ == Lower){
                result = lhs < rhs;
            }
            if (operation_ == MoreOrEquals){
                result = (lhs > rhs || lhs == rhs);
            }
            if (operation_ == LowerOrEquals){
                result = (lhs < rhs || lhs == rhs);
            }
            if (operation_ == IsNotNull){
                result = true;
            }
            return result;
        }

        void SetCondition(const std::string& tmp);
        void SetLhs(const std::string& lhs);
        void SetOperation(const std::string& tmp);
        void SetRhs (const std::string& rhs);

        [[nodiscard]] Type GetCondition() const;
        [[nodiscard]] std::string GetLhs() const;
        [[nodiscard]] Operator GetOperation() const;
        [[nodiscard]] std::string GetRhs() const;
    };

    enum RequestType {
        Select, Create, Drop, Insert, Update, Delete, Join, None
    };

    [[nodiscard]] RequestType Type() const;
    [[nodiscard]] std::unordered_map<std::string, std::string> GetData() const;
    [[nodiscard]] std::vector<Condition> GetConditions() const;
    [[nodiscard]] std::string GetTableName() const;

    friend class Table;

private:
    void SetCondition(const std::string& tmp);
    static void SetColumnsValues(const std::string& tmp, std::vector<std::string>& vector);
    void SetType(const std::string& req);

    bool CheckSelect(const std::string& req);
    bool CheckDelete(const std::string& req);
    bool CheckInsert(const std::string& req);
    bool CheckUpdate(const std::string& req);
    bool CheckCreate(const std::string& req);
    bool CheckDrop(const std::string& req);
    bool CheckJoin(const std::string& req);

    RequestType type_ = RequestType::None;
    std::string table;
    std::unordered_map<std::string, std::string> columns_values;
    std::vector<Condition> conditions;
};

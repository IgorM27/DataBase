#pragma once

#include "RequestParser.h"
#include "Element.h"

#include <string>

class Column {
public:
    enum VariablesType {
        Int, Varchar, Bool, Double, Float
    };

protected:
    std::string name_column;
    VariablesType type_;
    std::vector<Element> values;
    std::pair<std::string, Column*> foreign_key = {"", nullptr};
    Element default_ = Element();
    bool primary_key = false;

public:

    explicit Column(const std::string& name, VariablesType type){
        name_column = name;
        type_ = type;
    }

    ~Column() = default;

    [[nodiscard]] const std::string& GetName() const;
    [[nodiscard]] std::string GetStrData(size_t ind) const;
    [[nodiscard]] std::string GetStrStructure() const;

    [[nodiscard]] size_t Size() const;
    [[nodiscard]] VariablesType Type() const;

    void AddValue(const std::string& value);
    void AddDefault();
    void SetValue(const std::string& value, size_t ind);
    void Check(const std::string& value);
    Element GetValueFromIndex(size_t ind);



    void DeleteValue(size_t ind);
    void CopyValueFrom(Column* tmp, size_t ind);

    bool Compare(const Column& other, const RequestParser::Condition& operation, size_t ind_1, size_t ind_2 = SIZE_MAX);
    bool Compare(const std::string& other, const RequestParser::Condition& operation, size_t ind);


    void SetForeignKey(const std::string& table, Column* tmp);
    [[nodiscard]] std::string GetStrForeignKey() const;
    void SetPrimaryKeyFlag(bool value);
    [[nodiscard]] bool CheckForeignKey(const Element& value) const;
    [[nodiscard]] bool CheckPrimaryKey(const std::string & value) const;
    void CheckAvailable(const std::string& value) const;

    friend class ResultSet;
};

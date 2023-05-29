#include "Column.h"

const std::string& Column::GetName() const {
    return name_column;
}

Column::VariablesType Column::Type() const {
    return type_;
}

void Column::SetPrimaryKeyFlag(bool value) {
    primary_key = value;
}

std::string Column::GetStrStructure() const {
    std::string tmp = name_column;
    if(type_ == Int){
        tmp += " INT";
    }
    if(type_ == Bool){
        tmp += " BOOL";
    }
    if(type_ == Double){
        tmp += " DOUBLE";
    }
    if(type_ == Float){
        tmp += " FLOAT";
    }
    if (type_ == Varchar){
        tmp += " VARCHAR";
    }
    if (primary_key) {
        tmp += " PRIMARY KEY";
    }
    return tmp;
}

std::string Column::GetStrData(size_t ind) const {
    if (values[ind].IsNull()){
        return "NULL";
    }
    return values[ind].Value();
}

size_t Column::Size() const {
    return values.size();
}

void Column::AddValue(const std::string& value) {
    if (value == "NULL"){
        values.emplace_back(default_);
    } else {
        Check(value);
        values.emplace_back(value);
    }
}

void Column::AddDefault() {
    values.emplace_back(default_);
}

void Column::SetValue(const std::string& value, size_t ind) {
    Check(value);
    values[ind].SetValue(value);
}

void Column::DeleteValue(size_t ind) {
    values.erase(values.begin() + ind);
}

void Column::CopyValueFrom(Column* tmp, size_t ind) {
    values.push_back(tmp->values[ind]);
}

bool Column::Compare(const Column& other, const RequestParser::Condition& operation, size_t ind_1, size_t ind_2) {
    if (ind_2 == SIZE_MAX) {
        ind_2 = ind_1;
    }
    if (type_ == Int){
        return operation.Compare(values[ind_1].GetIntValue(), other.values[ind_2].GetIntValue());
    }
    if (type_ == Double){
        return operation.Compare(values[ind_1].GetDoubleValue(), other.values[ind_2].GetDoubleValue());
    }
    if (type_ == Bool){
        return operation.Compare(values[ind_1].GetBoolValue(), other.values[ind_2].GetBoolValue());
    }
    if (type_ == Float){
        return operation.Compare(values[ind_1].GetFloatValue(), other.values[ind_2].GetFloatValue());
    }
    if (type_ == Varchar){
        return operation.Compare(values[ind_1].GetStringValue(), other.values[ind_2].GetStringValue());
    }
    return false;
}

void Column::Check(const std::string& value) {
    CheckAvailable(value);
    if (type_ == Varchar){
        if (!(value.front() == '\'' && value.back() == '\'')){
            throw MyBDException("Incorrect varchar type");
        }
    }
    if (type_ == Bool){
        if(value != "true" && value != "false" && value != "0" && value != "1"){
            throw MyBDException("Incorrect bool type");
        }
    }

}

bool Column::Compare(const std::string& other, const RequestParser::Condition& operation, size_t ind) {
    Element tmp = values[ind];
    if (operation.GetOperation() == RequestParser::Condition::IsNotNull) {
        return !tmp.IsNull();
    }
    if (operation.GetOperation() == RequestParser::Condition::IsNull) {
        return tmp.IsNull();
    }
    if (tmp.IsNull()) {
        return false;
    }
    Element new_tmp(other);
    if (type_ == Int){
        return operation.Compare(tmp.GetIntValue(),new_tmp.GetIntValue());
    }
    if (type_ == Double){
        return operation.Compare(tmp.GetDoubleValue(),new_tmp.GetDoubleValue());
    }
    if (type_ == Bool){
        return operation.Compare(tmp.GetBoolValue(),new_tmp.GetBoolValue());
    }
    if (type_ == Float){
        return operation.Compare(tmp.GetFloatValue(),new_tmp.GetFloatValue());
    }
    if (type_ == Varchar){
        return operation.Compare(tmp.GetStringValue(),new_tmp.GetStringValue());
    }
    return false;
}

void Column::SetForeignKey(const std::string& table, Column* tmp) {
    foreign_key = {table, tmp};
}

std::string Column::GetStrForeignKey() const {
    if (foreign_key.second == nullptr) {
        return "";
    }
    return foreign_key.first + '(' + foreign_key.second->name_column + ')';
}

bool Column::CheckForeignKey(const Element& value) const {
    if (foreign_key.second == nullptr) {
        return true;
    }
    auto tmp = std::find_if(foreign_key.second->values.begin(), foreign_key.second->values.end(),[value](const Element& tmp) {return tmp == value;});
    if ( tmp == foreign_key.second->values.end()){
        return false;
    }
    return true;
}

bool Column::CheckPrimaryKey(const std::string& value) const {
    if (!primary_key) {
        return true;
    }
    auto tmp = std::find_if(values.begin(), values.end(),[value](const Element& tmp) {return value == tmp.Value();});
    if (tmp != values.end()){
        return false;
    }
    return true;
}

void Column::CheckAvailable(const std::string & value) const {
    if (!CheckForeignKey(Element(value)) || !CheckPrimaryKey(value)) {
        if(!CheckForeignKey(Element(value))){
            throw MyBDException("Incorrect foreign key");
        } else {
            throw MyBDException("Incorrect primary key");
        }
    }
}

Element Column::GetValueFromIndex(size_t ind) {
    return values[ind];
}





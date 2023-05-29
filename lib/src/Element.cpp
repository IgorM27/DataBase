#include "Element.h"

std::string Element::Value() const {
    return value_;
}

bool Element::IsNull() const {
    return null_;
}

bool Element::operator==(const Element& other) const {
    return value_==other.value_ && null_==other.null_;
}

std::ostream& operator<<(std::ostream& tmp, const Element& element) {
    if (element.null_) {
        tmp << "NULL";
        return tmp;
    }
    tmp << element.value_;
    return tmp;
}

void Element::SetValue(const std::string& value) {
    if (value=="NULL"){
        null_ = true;
        return;
    } else {
        value_ = value;
        null_ = false;
    }
}

int Element::GetIntValue() const {
    return std::strtol(value_.c_str(), nullptr, 10);
}

float Element::GetFloatValue() const {
    return std::strtof(value_.c_str(), nullptr);
}

double Element::GetDoubleValue() const {
    return std::strtod(value_.c_str(), nullptr);
}

bool Element::GetBoolValue() const {
    if (value_ == "false" || value_ == "0") {
        return false;
    }
    if (value_ == "true" || value_ == "1") {
        return true;
    }
    throw MyBDException("Bad boolean value");
}

std::string Element::GetStringValue() const {
    return value_.substr(1,value_.size() -2);
}

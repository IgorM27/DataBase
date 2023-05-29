#pragma once

#include <ostream>
#include "MyBDException.h"

class Element {
private:
    std::string value_;
    bool null_ = true;

public:
    Element()= default;

    explicit Element(const std::string& value){
        value_ = value;
        null_ = false;
    }

    [[nodiscard]] std::string  Value() const;
    [[nodiscard]] bool IsNull() const;

    bool operator==(const Element& other) const;

    friend std::ostream& operator<<(std::ostream& tmp, const Element& element);

    void SetValue(const std::string& value);

    [[nodiscard]] int GetIntValue() const;
    [[nodiscard]] float GetFloatValue() const;
    [[nodiscard]] double GetDoubleValue() const;
    [[nodiscard]] bool GetBoolValue() const;
    [[nodiscard]] std::string GetStringValue() const;
};

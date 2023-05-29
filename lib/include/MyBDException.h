#pragma once

#include <exception>
#include <string>

class MyBDException : public std::exception {
private:
    std::string text;
public:
    explicit MyBDException(const std::string& tmp){
        text = tmp;
    }
    [[nodiscard]] const char* what() const noexcept override {
        return text.c_str();
    }
};

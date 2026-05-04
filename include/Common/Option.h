#pragma once
#include <stdexcept>

template<typename T>
class Option {
private:
    T value;
    bool has_value;
public:
    Option() : value(T()), has_value(false) {}
    Option(const T& val) : value(val), has_value(true) {}
    static Option<T> None() { return Option<T>(); }
    bool IsSome() const { return has_value; }
    T GetValue() const {
        if (!has_value) throw std::logic_error("Option has no value");
        return value;
    }
};
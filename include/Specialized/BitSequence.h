#pragma once
#include "../Sequences/ArraySequence.h"
#include <stdexcept>
#include <iostream>
#include <functional>

struct Bit {
    bool value;
    Bit(bool v = false) : value(v) {}
    operator bool() const { return value; }
    bool operator==(const Bit& other) const { return value == other.value; }
    bool operator!=(const Bit& other) const { return value != other.value; }
};

class BitSequence : public MutableArraySequence<Bit> {
public:
    using MutableArraySequence<Bit>::MutableArraySequence;
    BitSequence() : MutableArraySequence<Bit>() {}
    BitSequence(const BitSequence& other) : MutableArraySequence<Bit>(other) {}

    Sequence<Bit>* GetSubsequence(int startIndex, int endIndex) const override {
        if (startIndex < 0 || endIndex >= this->GetLength() || startIndex > endIndex)
            throw std::out_of_range("IndexOutOfRange");
        BitSequence* sub = new BitSequence();
        for (int i = startIndex; i <= endIndex; ++i) sub->Append(this->Get(i));
        return sub;
    }

    Sequence<Bit>* Concat(Sequence<Bit>* list) override {
        BitSequence* target = new BitSequence(*this);
        for (int i = 0; i < list->GetLength(); ++i) target->Append(list->Get(i));
        return target;
    }

    BitSequence* operator&(const BitSequence& other) const {
        BitSequence* res = new BitSequence();
        int len = this->GetLength() < other.GetLength() ? this->GetLength() : other.GetLength();
        for (int i = 0; i < len; ++i) res->Append(Bit(this->Get(i).value && other.Get(i).value));
        return res;
    }

    BitSequence* operator|(const BitSequence& other) const {
        BitSequence* res = new BitSequence();
        int len = this->GetLength() < other.GetLength() ? this->GetLength() : other.GetLength();
        for (int i = 0; i < len; ++i) res->Append(Bit(this->Get(i).value || other.Get(i).value));
        return res;
    }

    BitSequence* operator^(const BitSequence& other) const {
        BitSequence* res = new BitSequence();
        int len = this->GetLength() < other.GetLength() ? this->GetLength() : other.GetLength();
        for (int i = 0; i < len; ++i) res->Append(Bit(this->Get(i).value != other.Get(i).value));
        return res;
    }

    BitSequence* operator!() const {
        BitSequence* res = new BitSequence();
        for (int i = 0; i < this->GetLength(); ++i) res->Append(Bit(!this->Get(i).value));
        return res;
    }

    void Print(const BitSequence& other, std::function<bool(bool, bool)> op) const {
        int len = this->GetLength() < other.GetLength() ? this->GetLength() : other.GetLength();
        for (int i = 0; i < len; ++i) std::cout << op(this->Get(i).value, other.Get(i).value);
    }
};
#pragma once
#include "../Common/IEnumerator.h"
#include <stdexcept>

template<typename T>
class Sequence {
public:
    virtual ~Sequence() = default;
    virtual T GetFirst() const = 0;
    virtual T GetLast() const = 0;
    virtual T Get(int index) const = 0;
    virtual int GetLength() const = 0;
    virtual Sequence<T>* Append(const T& item) = 0;
    virtual Sequence<T>* Prepend(const T& item) = 0;
    virtual Sequence<T>* InsertAt(const T& item, int index) = 0;
    virtual Sequence<T>* Concat(Sequence<T>* list) = 0;
    virtual Sequence<T>* GetSubsequence(int startIndex, int endIndex) const = 0;
    virtual IEnumerator<T>* GetEnumerator() = 0;
    virtual Sequence<T>* Clone() const = 0;
    virtual const T operator[](int index) const { return Get(index); }
    virtual T& operator[](int index) { throw std::runtime_error("Not supported"); }
};
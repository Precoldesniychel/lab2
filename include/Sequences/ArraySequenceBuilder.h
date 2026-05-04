#pragma once
#include "Sequence.h"
#include "ArraySequence.h"   // for MutableArraySequence
#include <stdexcept>

template<typename T>
class ArraySequenceBuilder {
private:
    T* buffer;
    size_t capacity;
    size_t count;
    
    void Resize(size_t newCapacity) {
        T* newBuffer = new T[newCapacity];
        for (size_t i = 0; i < count; ++i) {
            newBuffer[i] = buffer[i];
        }
        delete[] buffer;
        buffer = newBuffer;
        capacity = newCapacity;
    }

public:
    ArraySequenceBuilder(size_t initialCapacity = 16) 
        : buffer(new T[initialCapacity]), capacity(initialCapacity), count(0) {}
    
    ~ArraySequenceBuilder() { 
        delete[] buffer; 
    }
    
    ArraySequenceBuilder(const ArraySequenceBuilder&) = delete;
    ArraySequenceBuilder& operator=(const ArraySequenceBuilder&) = delete;

    ArraySequenceBuilder<T>& Add(const T& item) {
        if (count == capacity) {
            Resize(capacity * 2);
        }
        buffer[count++] = item;
        return *this;
    }
    
    ArraySequenceBuilder<T>& AddRange(Sequence<T>* seq) {
        if (!seq) throw std::invalid_argument("Null sequence");
        size_t len = seq->GetLength();
        if (count + len > capacity) {
            Resize(count + len);
        }
        for (size_t i = 0; i < len; ++i) {
            buffer[count++] = seq->Get(i);
        }
        return *this;
    }
    
    Sequence<T>* Build() {
        Sequence<T>* result = new MutableArraySequence<T>(buffer, count);
        count = 0;
        return result;
    }
};
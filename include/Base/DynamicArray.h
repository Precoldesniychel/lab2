#pragma once
#include <stdexcept>

template<class T> class DynamicArray {
private:
    T *data;
    int count;

public:
    DynamicArray(T *items, int count) {
        if (count < 0) throw std::out_of_range("IndexOutOfRange");
        this->count = count;
        this->data = new T[count];
        for (int i = 0; i < count; ++i) {
            this->data[i] = items[i];
        }
    }

    DynamicArray(int size) {
        if (size < 0) throw std::out_of_range("IndexOutOfRange");
        this->count = size;
        this->data = new T[size]();
    }

    DynamicArray(const DynamicArray<T> &dynamicArray) {
        this->count = dynamicArray.count;
        this->data = new T[this->count];
        for (int i = 0; i < this->count; ++i) {
            this->data[i] = dynamicArray.data[i];
        }
    }

    ~DynamicArray() {
        delete[] data;
    }

    T& Get(int index) {
        if (index < 0 || index >= this->count) {
            throw std::out_of_range("IndexOutOfRange");
        }
        return this->data[index];
    }

    const T& Get(int index) const {
        if (index < 0 || index >= this->count) {
            throw std::out_of_range("IndexOutOfRange");
        }
        return this->data[index];
    }

    int GetSize() const {
        return this->count;
    }

    int size() const { return GetSize(); }

    void Set(int index, T value) {
        if (index < 0 || index >= this->count) {
            throw std::out_of_range("IndexOutOfRange");
        }
        this->data[index] = value;
    }

    void set(const T &value, int index) {
        Set(index, value);
    }

    void Resize(int newSize) {
        if (newSize < 0) throw std::out_of_range("IndexOutOfRange");
        T *newData = new T[newSize]();
        int elementsToCopy = (newSize < this->count) ? newSize : this->count;
        for (int i = 0; i < elementsToCopy; ++i) {
            newData[i] = this->data[i];
        }
        delete[] this->data;
        this->data = newData;
        this->count = newSize;
    }
};
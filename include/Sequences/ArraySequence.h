#pragma once
#include "Sequence.h"
#include "../Base/DynamicArray.h"
#include <stdexcept>

template<class T>
class ArraySequence : public Sequence<T> {
private:
    class ArrayEnumerator : public IEnumerator<T> {
    private:
        const ArraySequence<T>* seq;
        int currentIndex;
    public:
        ArrayEnumerator(const ArraySequence<T>* s) : seq(s), currentIndex(-1) {}
        bool MoveNext() override {
            if (currentIndex + 1 < seq->GetLength()) { currentIndex++; return true; }
            return false;
        }
        T GetCurrent() const override {
            if (currentIndex < 0 || currentIndex >= seq->GetLength()) throw std::out_of_range("IndexOutOfRange");
            return seq->Get(currentIndex);
        }
        void Reset() override { currentIndex = -1; }
    };
protected:
    DynamicArray<T>* items;
    ArraySequence(DynamicArray<T>* data) : items(data) {}
    virtual Sequence<T>* instance() = 0;
    virtual Sequence<T>* CreateEmpty() const = 0;
public:
    ArraySequence() : items(new DynamicArray<T>(0)) {}
    ArraySequence(T* data, int count) : items(new DynamicArray<T>(data, count)) {}
    ArraySequence(const DynamicArray<T>& data) : items(new DynamicArray<T>(data)) {}
    virtual ~ArraySequence() { delete items; }

    T GetFirst() const override {
        if (this->GetLength() == 0) throw std::out_of_range("GetFirst: Sequence is empty");
        return items->Get(0);
    }
    T GetLast() const override {
        int length = this->GetLength();
        if (length == 0) throw std::out_of_range("GetLast: Sequence is empty");
        return items->Get(length - 1);
    }
    T Get(int index) const override {
        if (index < 0 || index >= this->GetLength()) throw std::out_of_range("IndexOutOfRange");
        return items->Get(index);
    }
    int GetLength() const override { return items->GetSize(); }
    const T operator[](int index) const override {
        if (index < 0 || index >= this->GetLength()) throw std::out_of_range("operator[] const: Index out of range");
        return items->Get(index);
    }
    T& operator[](int index) override {
        if (index < 0 || index >= this->GetLength()) throw std::out_of_range("operator[]: Index out of range");
        return items->Get(index);
    }
    Sequence<T>* Append(const T& item) override {
        Sequence<T>* target = this->instance();
        auto* arraySeq = static_cast<ArraySequence<T>*>(target);
        int oldSize = arraySeq->items->GetSize();
        arraySeq->items->Resize(oldSize + 1);
        arraySeq->items->Set(oldSize, item);
        return target;
    }
    Sequence<T>* Prepend(const T& item) override { return InsertAt(item, 0); }
    Sequence<T>* InsertAt(const T& item, int index) override {
        Sequence<T>* target = this->instance();
        auto* arraySeq = static_cast<ArraySequence<T>*>(target);
        int oldSize = arraySeq->items->GetSize();
        if (index < 0 || index > oldSize) throw std::out_of_range("IndexOutOfRange");
        arraySeq->items->Resize(oldSize + 1);
        for (int i = oldSize; i > index; --i) arraySeq->items->Set(i, arraySeq->items->Get(i - 1));
        arraySeq->items->Set(index, item);
        return target;
    }
    Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override {
        if (startIndex < 0 || endIndex >= this->GetLength() || startIndex > endIndex) throw std::out_of_range("IndexOutOfRange");
        Sequence<T>* result = this->CreateEmpty();
        for (int i = startIndex; i <= endIndex; ++i) result->Append(this->Get(i));
        return result;
    }
    Sequence<T>* Concat(Sequence<T>* list) override {
        Sequence<T>* target = this->instance();
        auto* arrayTarget = static_cast<ArraySequence<T>*>(target);
        int oldSize = arrayTarget->GetLength();
        int additionalSize = list->GetLength();
        arrayTarget->items->Resize(oldSize + additionalSize);
        for (int i = 0; i < additionalSize; ++i) arrayTarget->items->Set(oldSize + i, list->Get(i));
        return target;
    }
    IEnumerator<T>* GetEnumerator() override { return new ArrayEnumerator(this); }
    Sequence<T>* Clone() const override {
        Sequence<T>* copy = this->CreateEmpty();
        for(int i = 0; i < this->GetLength(); ++i) copy->Append(this->Get(i));
        return copy;
    }
};

template<class T>
class MutableArraySequence : public ArraySequence<T> {
public:
    using ArraySequence<T>::ArraySequence;
    Sequence<T>* CreateEmpty() const override { return new MutableArraySequence<T>(); }
protected:
    Sequence<T>* instance() override { return this; }
};

template<class T>
class ImmutableArraySequence : public ArraySequence<T> {
public:
    using ArraySequence<T>::ArraySequence;
    Sequence<T>* CreateEmpty() const override { return new ImmutableArraySequence<T>(); }

    T& operator[](int index) override {
        throw std::runtime_error("Immutable sequence does not support non-const operator[]");
    }
protected:
    Sequence<T>* instance() override {
        DynamicArray<T>* newArray = new DynamicArray<T>(*(this->items));
        return new ImmutableArraySequence<T>(newArray);
    }
};
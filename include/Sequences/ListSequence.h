#pragma once
#include "Sequence.h"
#include "../Base/LinkedList.h"
#include <stdexcept>

template<class T>
class ListSequence : public Sequence<T> {
private:
    class ListEnumerator : public IEnumerator<T> {
    private:
        const LinkedList<T> *list;
        int currentIndex;
    public:
        ListEnumerator(const LinkedList<T> *l) : list(l), currentIndex(-1) {}
        bool MoveNext() override {
            if (currentIndex + 1 < list->GetLength()) { currentIndex++; return true; }
            return false;
        }
        T GetCurrent() const override {
            if (currentIndex < 0 || currentIndex >= list->GetLength()) throw std::out_of_range("IndexOutOfRange");
            return list->Get(currentIndex);
        }
        void Reset() override { currentIndex = -1; }
    };
protected:
    LinkedList<T> *items;
    ListSequence(LinkedList<T> *list) : items(list) {}
    virtual Sequence<T>* instance() = 0;
    virtual Sequence<T>* CreateEmpty() const = 0;
public:
    ListSequence() : items(new LinkedList<T>()) {}
    ListSequence(T *data, int count) : items(new LinkedList<T>(data, count)) {}
    ListSequence(const LinkedList<T> &list) : items(new LinkedList<T>(list)) {}
    virtual ~ListSequence() { delete items; }

    T GetFirst() const override {
        if (GetLength() == 0) throw std::out_of_range("Sequence is empty");
        return items->GetFirst();
    }
    T GetLast() const override {
        if (GetLength() == 0) throw std::out_of_range("Sequence is empty");
        return items->GetLast();
    }
    T Get(int index) const override {
        if (index < 0 || index >= GetLength()) throw std::out_of_range("IndexOutOfRange");
        return items->Get(index);
    }
    int GetLength() const override { return items->GetLength(); }
    const T operator[](int index) const override { return items->Get(index); }
    T &operator[](int index) override { return items->GetReference(index); }
    IEnumerator<T>* GetEnumerator() override { return new ListEnumerator(this->items); }

    Sequence<T> *Append(const T &item) override {
        Sequence<T> *target = this->instance();
        static_cast<ListSequence<T> *>(target)->items->Append(item);
        return target;
    }
    Sequence<T> *Prepend(const T &item) override {
        Sequence<T> *target = this->instance();
        static_cast<ListSequence<T> *>(target)->items->Prepend(item);
        return target;
    }
    Sequence<T> *Concat(Sequence<T> *list) override {
        Sequence<T> *target = this->instance();
        for (int i = 0; i < list->GetLength(); ++i) target->Append(list->Get(i));
        return target;
    }
    Sequence<T> *InsertAt(const T &item, int index) override {
        if (index < 0 || index > GetLength()) throw std::out_of_range("IndexOutOfRange");
        Sequence<T> *target = this->instance();
        static_cast<ListSequence<T> *>(target)->items->InsertAt(item, index);
        return target;
    }
    Sequence<T>* Clone() const override {
        Sequence<T>* res = this->CreateEmpty();
        return res->Concat(const_cast<ListSequence<T>*>(this));
    }
    Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override {
        if (startIndex < 0 || endIndex >= GetLength() || startIndex > endIndex) throw std::out_of_range("IndexOutOfRange");
        Sequence<T>* result = this->CreateEmpty();
        for (int i = startIndex; i <= endIndex; i++) result->Append(this->Get(i));
        return result;
    }
};

template<class T>
class MutableListSequence : public ListSequence<T> {
public:
    using ListSequence<T>::ListSequence;
    Sequence<T> *CreateEmpty() const override { return new MutableListSequence<T>(); }
protected:
    Sequence<T> *instance() override { return this; }
};

template<typename T>
class ImmutableListSequence : public ListSequence<T> {
public:
    ImmutableListSequence() : ListSequence<T>() {}

    ImmutableListSequence(LinkedList<T>* list) : ListSequence<T>(list) {}

    ImmutableListSequence(T* data, int count) : ListSequence<T>(data, count) {}

    ImmutableListSequence(Sequence<T>* seq) : ListSequence<T>(seq) {}

    ImmutableListSequence(const ImmutableListSequence<T>& other)
        : ListSequence<T>(new LinkedList<T>(*(other.items))) {}

    ~ImmutableListSequence() = default;

    Sequence<T>* Append(const T& item) override {
        LinkedList<T>* newList = new LinkedList<T>(*(this->items));
        newList->Append(item);
        return new ImmutableListSequence<T>(newList);
    }

    Sequence<T>* Prepend(const T& item) override {
        LinkedList<T>* newList = new LinkedList<T>(*(this->items));
        newList->Prepend(item);
        return new ImmutableListSequence<T>(newList);
    }

    Sequence<T>* InsertAt(const T& item, int index) override {
        if (index > this->items->GetLength())
            throw std::out_of_range("IndexOutOfRange");
        LinkedList<T>* newList = new LinkedList<T>(*(this->items));
        newList->InsertAt(item, index);
        return new ImmutableListSequence<T>(newList);
    }

    void SetAt(const T& item, int index) {
        throw std::runtime_error("ImmutableSequenceModification");
    }

    Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override {
        if (startIndex > endIndex || endIndex >= this->items->GetLength())
            throw std::out_of_range("IndexOutOfRange");
        LinkedList<T>* sub = new LinkedList<T>();
        for (size_t i = startIndex; i <= endIndex; ++i) {
            sub->Append(this->items->Get(i));
        }
        return new ImmutableListSequence<T>(sub);
    }

    Sequence<T>* GetSubsequence(Sequence<int>* indices) {
        LinkedList<T>* sub = new LinkedList<T>();
        for (size_t i = 0; i < indices->GetLength(); ++i) {
            sub->Append(this->items->Get(indices->Get(i)));
        }
        return new ImmutableListSequence<T>(sub);
    }
    Sequence<T> *CreateEmpty() const override { return new ImmutableListSequence<T>(); }
    Sequence<T>* instance() override {
        return new ImmutableListSequence<T>(new LinkedList<T>(*(this->items)));
    }

    // Запрет изменения элемента через неконстантный operator[]
    T& operator[](int index) override {
        throw std::runtime_error("Immutable sequence does not support non-const operator[]");
    }
};
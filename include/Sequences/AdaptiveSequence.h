#pragma once
#include "Sequence.h"
#include <stdexcept>

template<typename T>
class AdaptiveSequence : public Sequence<T> {
private:
    Sequence<T>* arraySeq;
    Sequence<T>* listSeq;
    Sequence<T>* active;
    size_t threshold;
    size_t insertCount;
    
    void Evaluate() {
        if (active == listSeq && arraySeq->GetLength() < threshold && insertCount < threshold / 2) {
            SwitchTo(arraySeq);
        } else if (active == arraySeq && arraySeq->GetLength() >= threshold && insertCount >= threshold / 2) {
            SwitchTo(listSeq);
        }
        insertCount = 0;
    }
    
    void SwitchTo(Sequence<T>* target) {
        Sequence<T>* old = active;
        active = target->instance();
        delete old;
    }

public:
    AdaptiveSequence(size_t threshold = 64) 
        : threshold(threshold), insertCount(0) {
        arraySeq = new MutableArraySequence<T>();
        listSeq = new MutableListSequence<T>();
        active = arraySeq;
    }
    
    AdaptiveSequence(Sequence<T>* seq, size_t threshold = 64) 
        : threshold(threshold), insertCount(0) {
        if (!seq) throw std::invalid_argument("Null sequence");
        arraySeq = new MutableArraySequence<T>(*seq);
        listSeq = new MutableListSequence<T>(*seq);
        active = seq->GetLength() < threshold ? arraySeq : listSeq;
    }
    
    ~AdaptiveSequence() {
        delete arraySeq;
        delete listSeq;
        if (active != arraySeq && active != listSeq) delete active;
    }
    
    Sequence<T>* Append(const T& item) override {
        Sequence<T>* res = active->Append(item);
        delete active;
        active = res;
        ++insertCount;
        Evaluate();
        return this;
    }
    
    Sequence<T>* Prepend(const T& item) override {
        Sequence<T>* res = active->Prepend(item);
        delete active;
        active = res;
        ++insertCount;
        Evaluate();
        return this;
    }
    
    Sequence<T>* InsertAt(const T& item, size_t index) override {
        Sequence<T>* res = active->InsertAt(item, index);
        delete active;
        active = res;
        ++insertCount;
        Evaluate();
        return this;
    }
    
    void SetAt(const T& item, size_t index) override { 
        active->SetAt(item, index); 
    }
    
    Sequence<T>* GetSubsequence(size_t startIndex, size_t endIndex) override { 
        return active->GetSubsequence(startIndex, endIndex); 
    }
    
    Sequence<T>* GetSubsequence(Sequence<size_t>* indices) override { 
        return active->GetSubsequence(indices); 
    }
    
    size_t GetLength() const override { 
        return active->GetLength(); 
    }
    
    T GetFirst() const override { 
        return active->GetFirst(); 
    }
    
    T GetLast() const override { 
        return active->GetLast(); 
    }
    
    IEnumerator<T>* GetEnumerator() override { 
        return active->GetEnumerator(); 
    }
    
    Sequence<T>* instance() override { 
        return new AdaptiveSequence<T>(active, threshold); 
    }
};
#pragma once
#include "../Common/IEnumerator.h"
#include <stdexcept>

template<typename T>
class SequenceEnumerator : public IEnumerator<T> {
private:
    Sequence<T>* sequence;
    size_t currentIndex;
    bool started;
public:
    SequenceEnumerator(Sequence<T>* seq) : sequence(seq), currentIndex(0), started(false) {}
    ~SequenceEnumerator() = default;
    bool MoveNext() override {
        if (!started) {
            started = true;
            return currentIndex < sequence->GetLength();
        }
        ++currentIndex;
        return currentIndex < sequence->GetLength();
    }
    T GetCurrent() const override {
        if (!started || currentIndex >= sequence->GetLength()) throw std::out_of_range("EnumeratorEnd");
        return sequence->Get(currentIndex);
    }
    void Reset() override {
        currentIndex = 0;
        started = false;
    }
};
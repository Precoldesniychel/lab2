#pragma once

template<typename T>
class IEnumerator {
public:
    virtual ~IEnumerator() = default;
    virtual bool MoveNext() = 0;
    virtual T GetCurrent() const = 0;
    virtual void Reset() = 0;
};
#pragma once
#include "Sequence.h"
#include "ArraySequence.h"
#include "../Common/Pair.h"
#include "../Common/Option.h"
#include <stdexcept>

class SequenceFunctions {
private:
    // Вспомогательная сортировка (quick sort) для массива
    template<typename T>
    static void QuickSort(T* arr, int left, int right) {
        if (left >= right) return;
        T pivot = arr[(left + right) / 2];
        int i = left, j = right;
        while (i <= j) {
            while (arr[i] < pivot) i++;
            while (arr[j] > pivot) j--;
            if (i <= j) {
                T tmp = arr[i];
                arr[i] = arr[j];
                arr[j] = tmp;
                i++;
                j--;
            }
        }
        QuickSort(arr, left, j);
        QuickSort(arr, i, right);
    }

    template<typename T>
    static void SortArray(T* arr, int size) {
        QuickSort(arr, 0, size - 1);
    }

public:
    template<class T>
    struct MinMaxAvg {
        T min;
        T max;
        double avg;
    };

    template<typename T, typename Func>
    static auto Map(Sequence<T>* seq, Func func) -> Sequence<decltype(func(std::declval<T>()))>* {
        using U = decltype(func(std::declval<T>()));
        if (!seq) throw std::invalid_argument("Null sequence");
        Sequence<U>* result = new MutableArraySequence<U>();
        for (int i = 0; i < seq->GetLength(); ++i) {
            result->Append(func(seq->Get(i)));
        }
        return result;
    }

    template<typename T, typename Func>
    static Sequence<T>* Where(Sequence<T>* seq, Func pred) {
        if (!seq) throw std::invalid_argument("Null sequence");
        Sequence<T>* result = new MutableArraySequence<T>();
        for (int i = 0; i < seq->GetLength(); ++i) {
            if (pred(seq->Get(i))) {
                result->Append(seq->Get(i));
            }
        }
        return result;
    }

    template<typename T, typename U, typename Func>
    static U Reduce(Sequence<T>* seq, U acc, Func func) {
        if (!seq) throw std::invalid_argument("Null sequence");
        for (int i = 0; i < seq->GetLength(); ++i) {
            acc = func(acc, seq->Get(i));
        }
        return acc;
    }

    template<typename T, typename U>
    static Sequence<Pair<T, U>>* Zip(Sequence<T>* s1, Sequence<U>* s2) {
        if (!s1 || !s2) throw std::invalid_argument("Null sequence");
        int len = s1->GetLength() < s2->GetLength() ? s1->GetLength() : s2->GetLength();
        Sequence<Pair<T, U>>* result = new MutableArraySequence<Pair<T, U>>();
        for (int i = 0; i < len; ++i) {
            result->Append(Pair<T, U>(s1->Get(i), s2->Get(i)));
        }
        return result;
    }

    template<typename T, typename U>
    static Pair<Sequence<T>*, Sequence<U>*> Unzip(Sequence<Pair<T, U>>* seq) {
        if (!seq) throw std::invalid_argument("Null sequence");
        Sequence<T>* s1 = new MutableArraySequence<T>();
        Sequence<U>* s2 = new MutableArraySequence<U>();
        for (int i = 0; i < seq->GetLength(); ++i) {
            Pair<T, U> p = seq->Get(i);
            s1->Append(p.first);
            s2->Append(p.second);
        }
        return Pair<Sequence<T>*, Sequence<U>*>(s1, s2);
    }

    template<typename T, typename Func>
    static Sequence<T>* FlatMap(Sequence<T>* seq, Func func) {
        if (!seq) throw std::invalid_argument("Null sequence");
        Sequence<T>* result = new MutableArraySequence<T>();
        for (int i = 0; i < seq->GetLength(); ++i) {
            Sequence<T>* mapped = func(seq->Get(i));
            for (int j = 0; j < mapped->GetLength(); ++j) {
                result->Append(mapped->Get(j));
            }
            delete mapped;
        }
        return result;
    }

    template<typename T>
    static Sequence<T>* Skip(Sequence<T>* seq, int count) {
        if (!seq) throw std::invalid_argument("Null sequence");
        Sequence<T>* result = new MutableArraySequence<T>();
        for (int i = count; i < seq->GetLength(); ++i) {
            result->Append(seq->Get(i));
        }
        return result;
    }

    template<typename T, typename Func>
    static Sequence<Sequence<T>*>* Split(Sequence<T>* seq, Func delimiter) {
        if (!seq) throw std::invalid_argument("Null sequence");
        Sequence<Sequence<T>*>* result = new MutableArraySequence<Sequence<T>*>();
        Sequence<T>* current = new MutableArraySequence<T>();
        for (int i = 0; i < seq->GetLength(); ++i) {
            if (delimiter(seq->Get(i))) {
                result->Append(current);
                current = new MutableArraySequence<T>();
            } else {
                current->Append(seq->Get(i));
            }
        }
        if (current->GetLength() > 0) {
            result->Append(current);
        } else {
            delete current;
        }
        return result;
    }

    template<typename T>
    static Sequence<T>* Slice(Sequence<T>* seq, int start, int count, Sequence<T>* replacement = nullptr) {
        if (!seq) throw std::invalid_argument("Null sequence");
        int len = seq->GetLength();
        int actualStart = start < 0 ? len + start : start;
        if (actualStart < 0 || actualStart > len) throw std::out_of_range("Invalid start index");

        int actualCount = count < 0 ? 0 : count;
        if (actualStart + actualCount > len) actualCount = len - actualStart;

        Sequence<T>* result = new MutableArraySequence<T>();
        for (int i = 0; i < actualStart; ++i) result->Append(seq->Get(i));
        if (replacement) {
            for (int i = 0; i < replacement->GetLength(); ++i) {
                result->Append(replacement->Get(i));
            }
        }
        int skipEnd = actualStart + actualCount;
        for (int i = skipEnd; i < len; ++i) result->Append(seq->Get(i));

        return result;
    }

    // Try‑семантика
    template<typename T>
    static Option<T> TryFirst(Sequence<T>* seq) {
        if (!seq || seq->GetLength() == 0) return Option<T>::None();
        return Option<T>(seq->Get(0));
    }

    template<typename T, typename Pred>
    static Option<T> TryFind(Sequence<T>* seq, Pred pred) {
        if (!seq) return Option<T>::None();
        for (int i = 0; i < seq->GetLength(); ++i) {
            T val = seq->Get(i);
            if (pred(val)) return Option<T>(val);
        }
        return Option<T>::None();
    }

    // Статистические функции
    template<typename T>
    static MinMaxAvg<T> GetMinMaxAvg(Sequence<T>* seq) {
        if (!seq || seq->GetLength() == 0) throw std::invalid_argument("Empty sequence");
        T min = seq->Get(0);
        T max = seq->Get(0);
        double sum = 0;
        for (int i = 0; i < seq->GetLength(); ++i) {
            T val = seq->Get(i);
            if (val < min) min = val;
            if (val > max) max = val;
            sum += static_cast<double>(val);
        }
        return {min, max, sum / seq->GetLength()};
    }

    template<typename T>
    static double GetMedian(Sequence<T>* seq) {
        int len = seq->GetLength();
        if (!seq || len == 0) throw std::invalid_argument("Empty sequence");

        T* tempArr = new T[len];
        for (int i = 0; i < len; ++i) tempArr[i] = seq->Get(i);
        SortArray(tempArr, len);

        double median = (len % 2 == 0)
            ? (static_cast<double>(tempArr[len / 2 - 1] + tempArr[len / 2]) / 2.0)
            : static_cast<double>(tempArr[len / 2]);

        delete[] tempArr;
        return median;
    }

    template<typename T>
    static int CountInversions(Sequence<T>* seq) {
        if (!seq) throw std::invalid_argument("Null sequence");
        int count = 0;
        for (int i = 0; i < seq->GetLength(); ++i) {
            for (int j = i + 1; j < seq->GetLength(); ++j) {
                if (seq->Get(i) > seq->Get(j)) count++;
            }
        }
        return count;
    }

    template<typename T>
    static Sequence<Sequence<T>*>* GetPrefixes(Sequence<T>* seq) {
        if (!seq) throw std::invalid_argument("Null sequence");
        Sequence<Sequence<T>*>* result = new MutableArraySequence<Sequence<T>*>();
        for (int i = 1; i <= seq->GetLength(); ++i) {
            result->Append(seq->GetSubsequence(0, i - 1));
        }
        return result;
    }

    template<typename T>
    static Sequence<Sequence<T>*>* GetSuffixes(Sequence<T>* seq) {
        if (!seq) throw std::invalid_argument("Null sequence");
        Sequence<Sequence<T>*>* result = new MutableArraySequence<Sequence<T>*>();
        for (int i = 0; i < seq->GetLength(); ++i) {
            result->Append(seq->GetSubsequence(i, seq->GetLength() - 1));
        }
        return result;
    }

    template<typename T>
    static Sequence<T>* GetMovingAverage(Sequence<T>* seq, int window) {
        if (!seq || window <= 0) throw std::invalid_argument("Invalid parameters");
        Sequence<T>* result = new MutableArraySequence<T>();
        for (int i = 0; i <= seq->GetLength() - window; ++i) {
            double sum = 0;
            for (int j = 0; j < window; ++j) {
                sum += static_cast<double>(seq->Get(i + j));
            }
            result->Append(static_cast<T>(sum / window));
        }
        return result;
    }

    template<typename T>
    static Sequence<T>* GetMirrorSum(Sequence<T>* seq) {
        if (!seq) throw std::invalid_argument("Null sequence");
        Sequence<T>* result = new MutableArraySequence<T>();
        int len = seq->GetLength();
        for (int i = 0; i < len; ++i) {
            result->Append(seq->Get(i) + seq->Get(len - 1 - i));
        }
        return result;
    }
};
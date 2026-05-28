#pragma once
#include "Sequence.h"
#include "ArraySequence.h"
#include "../Common/Pair.h"
#include "../Common/Option.h"
#include <stdexcept>

class SequenceFunctions {
private:
    template<typename T>
    static long long Merge(T *arr, T *temp, int left, int mid, int right) {
        int i = left;
        int j = mid + 1;
        int k = left;
        long long count = 0;
        while (i <= mid && j <= right) {
            if (arr[i] <= arr[j]) {
                temp[k++] = arr[i++];
            } else {
                temp[k++] = arr[j++];
                count += (mid - i + 1);
            }
        }
        while (i <= mid) temp[k++] = arr[i++];
        while (j <= right) temp[k++] = arr[j++];
        for (i = left; i <= right; i++) arr[i] = temp[i];
        return count;
    }

    template<typename T>
    static long long MergeSortAndCount(T *arr, T *temp, int left, int right) {
        long long count = 0;
        if (left < right) {
            int mid = left + (right - left) / 2;
            count += MergeSortAndCount(arr, temp, left, mid);
            count += MergeSortAndCount(arr, temp, mid + 1, right);
            count += Merge(arr, temp, left, mid, right);
        }
        return count;
    }

    template<typename T>
    static void QuickSort(T *arr, int left, int right) {
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
    static void SortArray(T *arr, int size) {
        QuickSort(arr, 0, size - 1);
    }

    template<typename T>
    static void CleanEnumerator(IEnumerator<T> *e) {
        if (e) {
            delete e;
        }
    }

public:
    template<class T>
    struct MinMaxAvg {
        T min;
        T max;
        double avg;
    };

    template<typename T, typename Func>
    static auto Map(Sequence<T> *seq, Func func) -> Sequence<decltype(func(std::declval<T>()))> * {
        using U = decltype(func(std::declval<T>()));
        Sequence<U> *result = new MutableArraySequence<U>();
        auto *enumerator = seq->GetEnumerator();
        while (enumerator->MoveNext()) {
            result->Append(func(enumerator->GetCurrent()));
        }
        delete enumerator;
        return result;
    }

    template<typename T, typename Func>
    static Sequence<T> *Where(Sequence<T> *seq, Func pred) {
        Sequence<T> *result = new MutableArraySequence<T>();
        auto *enumerator = seq->GetEnumerator();
        while (enumerator->MoveNext()) {
            T val = enumerator->GetCurrent();
            if (pred(val)) result->Append(val);
        }
        delete enumerator;
        return result;
    }

    template<typename T, typename U, typename Func>
    static U Reduce(Sequence<T> *seq, U acc, Func func) {
        if (!seq) throw std::invalid_argument("Null sequence");
        auto *enumerator = seq->GetEnumerator();
        while (enumerator->MoveNext()) {
            acc = func(acc, enumerator->GetCurrent());
        }
        delete enumerator;
        return acc;
    }

    template<typename T, typename U>
    static Sequence<Pair<T, U> > *Zip(Sequence<T> *s1, Sequence<U> *s2) {
        if (!s1 || !s2) throw std::invalid_argument("Null sequence");
        Sequence<Pair<T, U> > *result = new MutableArraySequence<Pair<T, U> >();

        auto *it1 = s1->GetEnumerator();
        auto *it2 = s2->GetEnumerator();

        while (it1->MoveNext() && it2->MoveNext()) {
            result->Append(Pair<T, U>(it1->GetCurrent(), it2->GetCurrent()));
        }

        delete it1;
        delete it2;
        return result;
    }

    template<typename T, typename U>
    static Pair<Sequence<T> *, Sequence<U> *> Unzip(Sequence<Pair<T, U> > *seq) {
        if (!seq) throw std::invalid_argument("Null sequence");
        Sequence<T> *s1 = new MutableArraySequence<T>();
        Sequence<U> *s2 = new MutableArraySequence<U>();

        auto *it = seq->GetEnumerator();
        while (it->MoveNext()) {
            Pair<T, U> p = it->GetCurrent();
            s1->Append(p.first);
            s2->Append(p.second);
        }
        delete it;
        return Pair<Sequence<T> *, Sequence<U> *>(s1, s2);
    }

    template<typename T, typename Func>
    static Sequence<T> *FlatMap(Sequence<T> *seq, Func func) {
        if (!seq) throw std::invalid_argument("Null sequence");
        Sequence<T> *result = new MutableArraySequence<T>();

        auto *it = seq->GetEnumerator();
        while (it->MoveNext()) {
            Sequence<T> *mapped = func(it->GetCurrent());
            auto *innerIt = mapped->GetEnumerator();
            while (innerIt->MoveNext()) {
                result->Append(innerIt->GetCurrent());
            }
            delete innerIt;
            delete mapped;
        }
        delete it;
        return result;
    }

    template<typename T>
    static Sequence<T> *Skip(Sequence<T> *seq, int count) {
        if (!seq) throw std::invalid_argument("Null sequence");
        Sequence<T> *result = new MutableArraySequence<T>();
        for (int i = count; i < seq->GetLength(); ++i) {
            result->Append(seq->Get(i));
        }
        return result;
    }

    template<typename T, typename Func>
    static Sequence<Sequence<T> *> *Split(Sequence<T> *seq, Func delimiter) {
        if (!seq) throw std::invalid_argument("Null sequence");
        Sequence<Sequence<T> *> *result = new MutableArraySequence<Sequence<T> *>();
        Sequence<T> *current = new MutableArraySequence<T>();
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
    static Sequence<T> *Slice(Sequence<T> *seq, int start, int count, Sequence<T> *replacement = nullptr) {
        if (!seq) throw std::invalid_argument("Null sequence");
        int len = seq->GetLength();
        int actualStart = start < 0 ? len + start : start;
        if (actualStart < 0 || actualStart > len) throw std::out_of_range("Invalid start index");

        Sequence<T> *result = new MutableArraySequence<T>();
        auto *it = seq->GetEnumerator();

        int currentIndex = 0;
        while (currentIndex < actualStart && it->MoveNext()) {
            result->Append(it->GetCurrent());
            currentIndex++;
        }

        int toSkip = count < 0 ? 0 : count;
        while (toSkip > 0 && it->MoveNext()) {
            toSkip--;
            currentIndex++;
        }

        if (replacement) {
            auto *repIt = replacement->GetEnumerator();
            while (repIt->MoveNext()) {
                result->Append(repIt->GetCurrent());
            }
            delete repIt;
        }

        while (it->MoveNext()) {
            result->Append(it->GetCurrent());
        }

        delete it;
        return result;
    }


    template<typename T>
    static Option<T> TryFirst(Sequence<T> *seq) {
        if (!seq || seq->GetLength() == 0) return Option<T>::None();
        return Option<T>(seq->Get(0));
    }

    template<typename T, typename Pred>
    static Option<T> TryFind(Sequence<T> *seq, Pred pred) {
        if (!seq) return Option<T>::None();
        for (int i = 0; i < seq->GetLength(); ++i) {
            T val = seq->Get(i);
            if (pred(val)) return Option<T>(val);
        }
        return Option<T>::None();
    }


    template<typename T>
    static MinMaxAvg<T> GetMinMaxAvg(Sequence<T> *seq) {
        if (!seq || seq->GetLength() == 0) throw std::invalid_argument("Empty sequence");

        auto *it = seq->GetEnumerator();
        it->MoveNext();
        T firstVal = it->GetCurrent();
        T min = firstVal;
        T max = firstVal;
        double sum = static_cast<double>(firstVal);

        while (it->MoveNext()) {
            T val = it->GetCurrent();
            if (val < min) min = val;
            if (val > max) max = val;
            sum += static_cast<double>(val);
        }

        delete it;
        return {min, max, sum / seq->GetLength()};
    }

    template<typename T>
    static double GetMedian(Sequence<T> *seq) {
        int len = seq->GetLength();
        if (!seq || len == 0) throw std::invalid_argument("Empty sequence");

        T *tempArr = new T[len];
        auto *it = seq->GetEnumerator();
        int i = 0;
        while (it->MoveNext()) {
            tempArr[i++] = it->GetCurrent();
        }
        delete it;

        SortArray(tempArr, len);
        double median = (len % 2 == 0)
                            ? (static_cast<double>(tempArr[len / 2 - 1] + tempArr[len / 2]) / 2.0)
                            : static_cast<double>(tempArr[len / 2]);

        delete[] tempArr;
        return median;
    }

    template<typename T>
    static long long CountInversions(Sequence<T> *seq) {
        int n = seq->GetLength();
        if (n < 2) return 0;
        T *arr = new T[n];
        for (int i = 0; i < n; i++) arr[i] = seq->Get(i);
        T *temp = new T[n];
        long long result = MergeSortAndCount(arr, temp, 0, n - 1);
        delete[] arr;
        delete[] temp;
        return result;
    }

    template<typename T>
    static Sequence<Sequence<T> *> *GetPrefixes(Sequence<T> *seq) {
        if (!seq) throw std::invalid_argument("Null sequence");
        Sequence<Sequence<T> *> *result = new MutableArraySequence<Sequence<T> *>();
        for (int i = 1; i <= seq->GetLength(); ++i) {
            result->Append(seq->GetSubsequence(0, i - 1));
        }
        return result;
    }

    template<typename T>
    static Sequence<Sequence<T> *> *GetSuffixes(Sequence<T> *seq) {
        if (!seq) throw std::invalid_argument("Null sequence");
        Sequence<Sequence<T> *> *result = new MutableArraySequence<Sequence<T> *>();
        for (int i = 0; i < seq->GetLength(); ++i) {
            result->Append(seq->GetSubsequence(i, seq->GetLength() - 1));
        }
        return result;
    }

    template<typename T>
    static Sequence<T> *GetMovingAverage(Sequence<T> *seq, int window) {
        if (!seq || window <= 0) throw std::invalid_argument("Invalid parameters");
        int len = seq->GetLength();
        if (window > len) return new MutableArraySequence<T>();

        Sequence<T> *result = new MutableArraySequence<T>();
        IEnumerator<T> *it = seq->GetEnumerator();

        double currentSum = 0;
        T *windowBuffer = new T[window];
        int bufferIndex = 0;

        for (int i = 0; i < window; ++i) {
            if (it->MoveNext()) {
                T val = it->GetCurrent();
                windowBuffer[i] = val;
                currentSum += static_cast<double>(val);
            }
        }

        result->Append(static_cast<T>(currentSum / window));

        while (it->MoveNext()) {
            T newVal = it->GetCurrent();
            T oldVal = windowBuffer[bufferIndex];
            currentSum = currentSum - static_cast<double>(oldVal) + static_cast<double>(newVal);
            windowBuffer[bufferIndex] = newVal;
            bufferIndex = (bufferIndex + 1) % window;
            result->Append(static_cast<T>(currentSum / window));
        }

        delete[] windowBuffer;
        delete it;
        return result;
    }

    template<typename T>
    static Sequence<T> *GetMirrorSum(Sequence<T> *seq) {
        if (!seq) throw std::invalid_argument("Null sequence");
        int len = seq->GetLength();
        T *temp = new T[len];
        auto *it = seq->GetEnumerator();
        int idx = 0;
        while (it->MoveNext()) temp[idx++] = it->GetCurrent();
        delete it;
        Sequence<T> *result = new MutableArraySequence<T>();
        for (int i = 0; i < len; ++i) {
            result->Append(temp[i] + temp[len - 1 - i]);
        }
        delete[] temp;
        return result;
    }

    template<typename T>
    static bool ContainsSubsequence(Sequence<T>* main, Sequence<T>* sub) {
        if (!main || !sub) return false;
        int n = main->GetLength();
        int m = sub->GetLength();
        if (m == 0) return true;
        if (m > n) return false;

        for (int i = 0; i <= n - m; ++i) {
            bool found = true;
            for (int j = 0; j < m; ++j) {
                if (main->Get(i + j) != sub->Get(j)) {
                    found = false;
                    break;
                }
            }
            if (found) return true;
        }
        return false;
    }

    template<typename T, typename Func>
    static Pair<Sequence<T>*, Sequence<T>*> SplitBy(Sequence<T>* seq, Func predicate) {
        if (!seq) throw std::invalid_argument("Null sequence");

        Sequence<T>* trueSeq = new MutableArraySequence<T>();
        Sequence<T>* falseSeq = new MutableArraySequence<T>();

        auto* enumerator = seq->GetEnumerator();
        while (enumerator->MoveNext()) {
            T val = enumerator->GetCurrent();
            if (predicate(val)) trueSeq->Append(val);
            else falseSeq->Append(val);
        }
        delete enumerator;

        return Pair<Sequence<T>*, Sequence<T>*>(trueSeq, falseSeq);
    }
};

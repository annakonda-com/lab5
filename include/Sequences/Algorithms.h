#pragma once
#include "Sequence.h"
#include "ArraySequence.h"

template<typename T>
Sequence<T>* SortSequence(const Sequence<T>* seq) {
    int n = seq->GetLength();
    DynamicArray<T> arr(n);
    for (int i = 0; i < n; ++i) arr.Set(i, seq->Get(i));

    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            if (arr.Get(j) > arr.Get(j + 1)) {
                T temp = arr.Get(j);
                arr.Set(j, arr.Get(j + 1));
                arr.Set(j + 1, temp);
            }
        }
    }

    MutableArraySequence<T>* result = new MutableArraySequence<T>();
    for (int i = 0; i < n; ++i) result->Append(arr.Get(i));
    return result;
}

template<typename T>
Sequence<T>* MergeSequences(const Sequence<T>* a, const Sequence<T>* b) {
    int i = 0, j = 0;
    int lenA = a->GetLength();
    int lenB = b->GetLength();
    MutableArraySequence<T>* merged = new MutableArraySequence<T>();

    while (i < lenA && j < lenB) {
        if (a->Get(i) < b->Get(j))
            merged->Append(a->Get(i++));
        else
            merged->Append(b->Get(j++));
    }
    while (i < lenA) merged->Append(a->Get(i++));
    while (j < lenB) merged->Append(b->Get(j++));
    return merged;
}
#pragma once
#include "../../Sequences/Sequence.h"

class MapReduceEngine {
public:
    template <typename T, typename Res, typename Func>
    static void Map(const Sequence<T>* source, Sequence<Res>* destination, Func func) {
        if (!source || !destination) return;
        for (int i = 0; i < source->GetLength(); ++i) {
            destination->Append(func(source->Get(i)));
        }
    }

    template <typename T, typename Func>
    static T Reduce(const Sequence<T>* source, Func func, T init) {
        if (!source) return init;
        T accumulator = init;
        for (int i = 0; i < source->GetLength(); ++i) {
            accumulator = func(accumulator, source->Get(i));
        }
        return accumulator;
    }
};
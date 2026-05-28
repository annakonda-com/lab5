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
        return this->InternalInsert(item, this->GetLength());
    }
    Sequence<T>* Prepend(const T& item) override { return InsertAt(item, 0); }
    Sequence<T>* InsertAt(const T& item, int index) override {
        int oldLen = GetLength();
        if (index < 0 || index > oldLen) throw std::out_of_range("Index");
        return InternalInsert(item, index);
    }
    Sequence<T>* GetSubsequence(int start, int end) const override {
        if (start < 0 || end >= GetLength() || start > end) throw std::out_of_range("Index");

        Sequence<T>* result = this->CreateEmpty();
        auto* arrRes = static_cast<ArraySequence<T>*>(result);
        int subLen = end - start + 1;

        arrRes->items->Resize(subLen);
        for (int i = 0; i < subLen; ++i) {
            arrRes->items->Set(i, this->items->Get(start + i));
        }
        return result;
    }

    Sequence<T>* Concat(Sequence<T>* list) override {
        Sequence<T>* result = this->CreateEmpty();
        auto* arrRes = static_cast<ArraySequence<T>*>(result);
        int n1 = this->GetLength();
        int n2 = list->GetLength();

        arrRes->items->Resize(n1 + n2);
        for (int i = 0; i < n1; ++i) arrRes->items->Set(i, this->items->Get(i));
        for (int i = 0; i < n2; ++i) arrRes->items->Set(n1 + i, list->Get(i));

        return result;
    }
    IEnumerator<T>* GetEnumerator() override { return new ArrayEnumerator(this); }
    Sequence<T>* Clone() const override {
        Sequence<T>* copy = this->CreateEmpty();
        for(int i = 0; i < this->GetLength(); ++i) copy->Append(this->Get(i));
        return copy;
    }
    virtual Sequence<T>* InternalInsert(const T& item, int index) = 0;
};

template<class T>
class MutableArraySequence : public ArraySequence<T> {
public:
    using ArraySequence<T>::ArraySequence;
    Sequence<T>* CreateEmpty() const override { return new MutableArraySequence<T>(); }
protected:
    Sequence<T>* InternalInsert(const T& item, int index) override {
        int oldLen = this->items->GetSize();
        this->items->Resize(oldLen + 1);
        for (int i = oldLen; i > index; --i) {
            this->items->Get(i) = std::move(this->items->Get(i-1));
        }
        this->items->Get(index) = item;
        return this;
    }
    Sequence<T>* RemoveAt(int index) override {
        if (index < 0 || index >= this->items->GetSize()) throw std::out_of_range("IndexOutOfRange");
        for (int i = index; i < this->items->GetSize() - 1; ++i) {
            this->items->Get(i) = std::move(this->items->Get(i+1));
        }
        this->items->Resize(this->items->GetSize() - 1);
        return this;
    }
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
        DynamicArray<T>* newArray = new DynamicArray(std::move(*this->items));
        return new ImmutableArraySequence(newArray);
    }

    Sequence<T>* InternalInsert(const T& item, int index) override {
        int oldLen = this->GetLength();
        DynamicArray<T>* newData = new DynamicArray<T>(*this->items);
        newData->Resize(oldLen + 1);
        for (int i = oldLen; i > index; --i) {
            newData->Get(i) = std::move(newData->Get(i - 1));
        }
        newData->Get(index) = item;
        return new ImmutableArraySequence<T>(newData);
    }

    Sequence<T>* RemoveAt(int index) override {
        if (index < 0 || index >= this->items->GetSize()) throw std::out_of_range("IndexOutOfRange");
        int oldLen = this->GetLength();
        DynamicArray<T>* newData = new DynamicArray<T>(oldLen - 1);
        for (int i = 0; i < index; ++i) (*newData).Get(i) = this->items->Get(i);
        for (int i = index; i < oldLen - 1; ++i) (*newData).Get(i) = this->items->Get(i+1);
        return new ImmutableArraySequence<T>(newData);
    }
};
#pragma once
#include <stdexcept>

template<class T> class DynamicArray {
private:
    T *data;
    int count;
    int capacity;

    void Reallocate(int newCapacity) {
        T* newData = new T[newCapacity];
        for (int i = 0; i < count; ++i) {
            newData[i] = std::move(data[i]);
        }
        delete[] data;
        data = newData;
        capacity = newCapacity;
    }

public:
    DynamicArray& operator=(DynamicArray&& other) noexcept {
        if (this != &other) {
            delete[] data;
            data = other.data;
            count = other.count;
            capacity = other.capacity;
            other.data = nullptr;
            other.count = 0;
            other.capacity = 0;
        }
        return *this;
    }

    DynamicArray() : data(nullptr), count(0), capacity(0) {}

    DynamicArray(int size) : data(new T[size]), count(size), capacity(size) {
        for (int i = 0; i < size; ++i) {
            data[i] = T();
        }
    }

    DynamicArray(const DynamicArray& other)
        : data(new T[other.capacity]), count(other.count), capacity(other.capacity) {
        for (int i = 0; i < count; ++i) {
            data[i] = other.data[i];
        }
    }

    DynamicArray(DynamicArray&& other) noexcept
        : data(other.data), count(other.count), capacity(other.capacity) {
        other.data = nullptr;
        other.count = 0;
        other.capacity = 0;
    }

    DynamicArray(T* items, int count) : data(new T[count]), count(count), capacity(count) {
        for (int i = 0; i < count; ++i) {
            data[i] = items[i];
        }
    }

    DynamicArray& operator=(const DynamicArray& other) {
        if (this != &other) {
            delete[] data;
            capacity = other.capacity;
            count = other.count;
            data = new T[capacity];
            for (int i = 0; i < count; ++i) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }

    ~DynamicArray() {
        delete[] data;
    }

    T& Get(int index) {
        if (index < 0 || index >= this->count) {
            throw std::out_of_range("IndexOutOfRange");
        }
        return this->data[index];
    }

    const T& Get(int index) const {
        if (index < 0 || index >= this->count) {
            throw std::out_of_range("IndexOutOfRange");
        }
        return this->data[index];
    }

    int GetSize() const { return this->count; }

    void Set(int index, const T& value) {
        if (index < 0 || index >= count) throw std::out_of_range("Index");
        data[index] = value;
    }

    void Resize(int newSize) {
        if (newSize > capacity) {
            int newCap = (capacity == 0) ? 8 : capacity * 2;
            if (newSize > newCap) newCap = newSize;
            Reallocate(newCap);
        }
        count = newSize;
    }

};
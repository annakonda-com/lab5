#pragma once
#include <stdexcept>

template<class T>
class LinkedList {
public:
    struct Node {
        T data;
        Node *next;
        Node *prev;

        Node(const T &item) : data(item), next(nullptr), prev(nullptr) {}
    };

    Node *head;
    Node *tail;
    int count;

    LinkedList() : head(nullptr), tail(nullptr), count(0) {}

    LinkedList(T *items, int count) : LinkedList() {
        if (count < 0) throw std::out_of_range("IndexOutOfRange");
        for (int i = 0; i < count; ++i) {
            Append(items[i]);
        }
    }

    LinkedList(const LinkedList<T> &list) : LinkedList() {
        Node *current = list.head;
        while (current) {
            Append(current->data);
            current = current->next;
        }
    }

    LinkedList(LinkedList&& other) noexcept
        : head(other.head), tail(other.tail), count(other.count) {
        other.head = nullptr;
        other.tail = nullptr;
        other.count = 0;
    }

    ~LinkedList() {
        Node *current = head;
        while (current) {
            Node *next = current->next;
            delete current;
            current = next;
        }
    }


    LinkedList& operator=(LinkedList&& other) noexcept {
        if (this != &other) {
            Node* current = head;
            while (current) {
                Node* next = current->next;
                delete current;
                current = next;
            }
            head = other.head;
            tail = other.tail;
            count = other.count;
            other.head = nullptr;
            other.tail = nullptr;
            other.count = 0;
        }
        return *this;
    }

    T GetFirst() const {
        if (count == 0) throw std::out_of_range("IndexOutOfRange");
        return head->data;
    }

    T GetLast() const {
        if (count == 0) throw std::out_of_range("IndexOutOfRange");
        return tail->data;
    }

    T Get(int index) const {
        if (index < 0 || index >= count) throw std::out_of_range("IndexOutOfRange");
        return GetNodeAt(index)->data;
    }

    T &GetReference(int index) {
        if (index < 0 || index >= count) throw std::out_of_range("IndexOutOfRange");
        return GetNodeAt(index)->data;
    }

    const Node *GetFirstNode() const { return head; }
    Node *GetFirstNode() { return head; }

    int GetLength() const { return count; }

    void Append(const T &item) {
        Node *newNode = new Node(item);
        if (count == 0) {
            head = tail = newNode;
        } else {
            newNode->prev = tail;
            tail->next = newNode;
            tail = newNode;
        }
        count++;
    }

    void Prepend(const T &item) {
        Node *newNode = new Node(item);
        if (count == 0) {
            head = tail = newNode;
        } else {
            newNode->next = head;
            head->prev = newNode;
            head = newNode;
        }
        count++;
    }

    void InsertAt(const T &item, int index) {
        if (index < 0 || index > count) throw std::out_of_range("IndexOutOfRange");

        if (index == 0) {
            Prepend(item);
        } else if (index == count) {
            Append(item);
        } else {
            Node *current = GetNodeAt(index);
            Node *newNode = new Node(item);

            newNode->prev = current->prev;
            newNode->next = current;
            current->prev->next = newNode;
            current->prev = newNode;
            count++;
        }
    }

    void RemoveAt(int index) {
        if (index < 0 || index >= count) throw std::out_of_range("IndexOutOfRange");

        Node* current = GetNodeAt(index);

        if (current->prev) current->prev->next = current->next;
        if (current->next) current->next->prev = current->prev;
        if (current == head) head = current->next;
        if (current == tail) tail = current->prev;

        delete current;
        count--;
    }

    LinkedList<T> *GetSubList(int startIndex, int endIndex) const {
        if (startIndex < 0 || startIndex >= count || endIndex < 0 || endIndex >= count || startIndex > endIndex) {
            throw std::out_of_range("IndexOutOfRange");
        }
        LinkedList<T> *subList = new LinkedList<T>();
        Node *current = GetNodeAt(startIndex);
        for (int i = startIndex; i <= endIndex; ++i) {
            subList->Append(current->data);
            current = current->next;
        }
        return subList;
    }

    LinkedList<T> *Concat(const LinkedList<T> &list) const {
        LinkedList<T> *result = new LinkedList<T>(*this);
        Node *current = list.head;
        while (current) {
            result->Append(current->data);
            current = current->next;
        }
        return result;
    }

private:
    Node* GetNodeAt(int index) const {
        if (index < count / 2) {
            Node* current = head;
            for (int i = 0; i < index; ++i) current = current->next;
            return current;
        } else {
            Node* current = tail;
            for (int i = count - 1; i > index; --i) current = current->prev;
            return current;
        }
    }
};
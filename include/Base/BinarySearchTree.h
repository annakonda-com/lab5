#pragma once
#include <functional>

template <typename TKey, typename TValue>
class BSTNode {
public:
    TKey key;
    TValue value;
    BSTNode *left, *right;

    BSTNode(TKey k, TValue v)
        : key(k), value(v), left(nullptr), right(nullptr) {}
};

template <typename TKey, typename TValue>
class BinarySearchTree {
private:
    BSTNode<TKey, TValue>* root;

    void Destroy(BSTNode<TKey, TValue>* node) {
        if (node) {
            Destroy(node->left);
            Destroy(node->right);
            delete node;
        }
    }

    // Вспомогательный метод для вставки или поиска
    TValue& InternalGetOrInsert(BSTNode<TKey, TValue>*& node, TKey key, TValue defaultValue) {
        if (!node) {
            node = new BSTNode<TKey, TValue>(key, defaultValue);
            return node->value;
        }
        if (key < node->key) return InternalGetOrInsert(node->left, key, defaultValue);
        if (key > node->key) return InternalGetOrInsert(node->right, key, defaultValue);
        return node->value; // Ключ найден, возвращаем ссылку на существующее значение
    }

    void InOrder(BSTNode<TKey, TValue>* node, std::function<void(const TKey&, TValue&)> func) {
        if (!node) return;
        InOrder(node->left, func);
        func(node->key, node->value);
        InOrder(node->right, func);
    }

    BSTNode<TKey, TValue>* InternalFind(BSTNode<TKey, TValue>* node, const TKey& key) const {
        if (!node) return nullptr;
        if (key < node->key) return InternalFind(node->left, key);
        if (key > node->key) return InternalFind(node->right, key);
        return node;
    }

public:
    BinarySearchTree() : root(nullptr) {}
    ~BinarySearchTree() { Destroy(root); }

    // Возвращает ссылку на значение. Если ключа нет — создает узел с defaultValue.
    TValue& GetOrInsert(TKey key, TValue defaultValue) {
        return InternalGetOrInsert(root, key, defaultValue);
    }

    BSTNode<TKey, TValue>* Find(const TKey& key) const {
        return InternalFind(root, key);
    }

    void Traverse(std::function<void(const TKey&, TValue&)> func) {
        InOrder(root, func);
    }
};
#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <functional>


using namespace std;
using namespace chrono;

// ============================================================
// UTILITIES: CSV Parser
// ============================================================
inline vector<string> parseCSVLine(const string& line) {
    vector<string> result;
    string cell;
    bool inQuotes = false;
    for (size_t i = 0; i < line.size(); i++) {
        char c = line[i];
        if (c == '"') {
            if (inQuotes && i + 1 < line.size() && line[i+1] == '"') {
                cell += '"';
                i++; // Skip next quote
            } else {
                inQuotes = !inQuotes;
            }
        } else if (c == ',' && !inQuotes) {
            result.push_back(cell);
            cell.clear();
        } else {
            cell += c;
        }
    }
    result.push_back(cell);
    return result;
}

// ============================================================
// UTILITIES: Memory Estimation Helpers
// ============================================================
template<typename ValueType>
inline size_t getValueMemoryUsage(const ValueType& val) {
    return sizeof(val);
}

inline size_t getValueMemoryUsage(const string& val) {
    return sizeof(val) + val.capacity();
}

// ============================================================

// DOMAIN: Version Node (digunakan oleh semua struktur)
// ============================================================
struct Version {
    int versionNumber;
    string date;
    string editor;
    string content;
    Version* next; // untuk LinkedList

    Version(int v, string d, string e, string c)
        : versionNumber(v), date(d), editor(e), content(c), next(nullptr) {}

    size_t getMemoryUsageBytes() const {
        return sizeof(*this) + date.capacity() + editor.capacity() + content.capacity();
    }
};

// ============================================================
// DOMAIN: Document
// ============================================================
struct Document {
    string docId;
    string docName;
    int totalVersions;

    Document() : totalVersions(0) {}
    Document(string id, string name) : docId(id), docName(name), totalVersions(0) {}
};

// ============================================================
// STRUKTUR 1: LinkedList untuk menyimpan riwayat versi
// Head = versi terlama, Tail = versi terbaru
// ============================================================
class VersionLinkedList {
public:
    Version* head;
    Version* tail;
    int size;

    VersionLinkedList() : head(nullptr), tail(nullptr), size(0) {}

    ~VersionLinkedList() {
        Version* curr = head;
        while (curr) {
            Version* next = curr->next;
            delete curr;
            curr = next;
        }
    }

    // O(1) - tambah versi baru di akhir
    void addVersion(int vNum, string date, string editor, string content) {
        Version* newNode = new Version(vNum, date, editor, content);
        if (!tail) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        size++;
    }

    // O(1) - hapus versi terbaru (rollback)
    void rollback() {
        if (!head) return;
        if (head == tail) {
            delete head;
            head = tail = nullptr;
            size--;
            return;
        }
        Version* curr = head;
        while (curr->next != tail) curr = curr->next;
        delete tail;
        tail = curr;
        tail->next = nullptr;
        size--;
    }

    // O(n) - akses versi ke-k
    Version* getVersion(int k) {
        Version* curr = head;
        int idx = 1;
        while (curr && idx < k) {
            curr = curr->next;
            idx++;
        }
        return curr;
    }

    // O(n) - tampilkan semua versi
    void printAll() {
        Version* curr = head;
        while (curr) {
            cout << "  [v" << curr->versionNumber << "] "
                 << curr->date << " | " << curr->editor
                 << " | " << curr->content.substr(0, 40) << "...\n";
            curr = curr->next;
        }
    }

    size_t getMemoryUsageBytes() const {
        size_t total = sizeof(*this);
        Version* curr = head;
        while (curr) {
            total += curr->getMemoryUsageBytes();
            curr = curr->next;
        }
        return total;
    }
};

// ============================================================
// STRUKTUR 2: Stack (array-based) untuk menyimpan riwayat versi
// Top = versi terbaru
// ============================================================
class VersionStack {
public:
    vector<Version*> stack;

    VersionStack() {}

    ~VersionStack() {
        for (auto v : stack) delete v;
    }

    // O(1) amortized
    void addVersion(int vNum, string date, string editor, string content) {
        stack.push_back(new Version(vNum, date, editor, content));
    }

    // O(1) - pop top
    void rollback() {
        if (stack.empty()) return;
        delete stack.back();
        stack.pop_back();
    }

    // O(1) - akses versi ke-k
    Version* getVersion(int k) {
        if (k < 1 || k > (int)stack.size()) return nullptr;
        return stack[k - 1];
    }

    int size() { return (int)stack.size(); }

    void printAll() {
        for (auto v : stack) {
            cout << "  [v" << v->versionNumber << "] "
                 << v->date << " | " << v->editor
                 << " | " << v->content.substr(0, 40) << "...\n";
        }
    }

    size_t getMemoryUsageBytes() const {
        size_t total = sizeof(*this);
        total += stack.capacity() * sizeof(Version*);
        for (auto v : stack) {
            if (v) {
                total += v->getMemoryUsageBytes();
            }
        }
        return total;
    }
};

// ============================================================
// STRUKTUR 3: Hash Table dengan Separate Chaining
// Untuk menyimpan dokumen berdasarkan ID
// ============================================================
const int TABLE_SIZE = 1024;

template<typename T>
struct HashNode {
    string key;
    T value;
    HashNode* next;
    HashNode(string k, T v) : key(k), value(v), next(nullptr) {}

    size_t getMemoryUsageBytes() const {
        return sizeof(*this) + key.capacity() + getValueMemoryUsage(value);
    }
};

template<typename T>
class HashMap {
private:
    HashNode<T>* table[TABLE_SIZE];

    int hash(const string& key) {
        size_t h = 0;
        for (char c : key) h = h * 31 + c;
        return (int)(h % TABLE_SIZE);
    }

public:
    int totalInserts = 0;
    int collisions = 0;

    HashMap() {
        for (int i = 0; i < TABLE_SIZE; i++) table[i] = nullptr;
    }

    ~HashMap() {
        for (int i = 0; i < TABLE_SIZE; i++) {
            HashNode<T>* curr = table[i];
            while (curr) {
                HashNode<T>* next = curr->next;
                delete curr;
                curr = next;
            }
        }
    }

    // O(1) average
    void insert(const string& key, T value) {
        int idx = hash(key);
        if (table[idx]) collisions++;
        HashNode<T>* newNode = new HashNode<T>(key, value);
        newNode->next = table[idx];
        table[idx] = newNode;
        totalInserts++;
    }

    // O(1) average
    T* search(const string& key) {
        int idx = hash(key);
        HashNode<T>* curr = table[idx];
        while (curr) {
            if (curr->key == key) return &curr->value;
            curr = curr->next;
        }
        return nullptr;
    }

    // O(1) average
    bool remove(const string& key) {
        int idx = hash(key);
        HashNode<T>* curr = table[idx];
        HashNode<T>* prev = nullptr;
        while (curr) {
            if (curr->key == key) {
                if (prev) prev->next = curr->next;
                else table[idx] = curr->next;
                delete curr;
                return true;
            }
            prev = curr;
            curr = curr->next;
        }
        return false;
    }

    // Ambil semua keys (untuk listing)
    vector<string> getAllKeys() {
        vector<string> keys;
        for (int i = 0; i < TABLE_SIZE; i++) {
            HashNode<T>* curr = table[i];
            while (curr) {
                keys.push_back(curr->key);
                curr = curr->next;
            }
        }
        return keys;
    }

    size_t getMemoryUsageBytes() const {
        size_t total = sizeof(*this);
        for (int i = 0; i < TABLE_SIZE; i++) {
            HashNode<T>* curr = table[i];
            while (curr) {
                total += curr->getMemoryUsageBytes();
                curr = curr->next;
            }
        }
        return total;
    }
};

// ============================================================
// STRUKTUR 4: Binary Search Tree (BST)
// ============================================================
template<typename K, typename V>
struct BSTNode {
    K key;
    V value;
    BSTNode* left;
    BSTNode* right;

    BSTNode(K k, V v) : key(k), value(v), left(nullptr), right(nullptr) {}

    size_t getMemoryUsageBytes() const {
        size_t total = sizeof(*this);
        total += getValueMemoryUsage(key);
        total += getValueMemoryUsage(value);
        if (left) total += left->getMemoryUsageBytes();
        if (right) total += right->getMemoryUsageBytes();
        return total;
    }
};

template<typename K, typename V>
class BST {
private:
    BSTNode<K, V>* root;

    void destroy(BSTNode<K, V>* node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

    BSTNode<K, V>* insertNode(BSTNode<K, V>* node, const K& key, const V& value) {
        if (!node) {
            return new BSTNode<K, V>(key, value);
        }
        if (key < node->key) {
            node->left = insertNode(node->left, key, value);
        } else if (key > node->key) {
            node->right = insertNode(node->right, key, value);
        } else {
            node->value = value;
        }
        return node;
    }

    BSTNode<K, V>* searchNode(BSTNode<K, V>* node, const K& key) const {
        if (!node || node->key == key) {
            return node;
        }
        if (key < node->key) {
            return searchNode(node->left, key);
        }
        return searchNode(node->right, key);
    }

    BSTNode<K, V>* findMin(BSTNode<K, V>* node) const {
        while (node && node->left) node = node->left;
        return node;
    }

    BSTNode<K, V>* removeNode(BSTNode<K, V>* node, const K& key, bool& success) {
        if (!node) {
            success = false;
            return nullptr;
        }
        if (key < node->key) {
            node->left = removeNode(node->left, key, success);
        } else if (key > node->key) {
            node->right = removeNode(node->right, key, success);
        } else {
            success = true;
            if (!node->left) {
                BSTNode<K, V>* temp = node->right;
                delete node;
                return temp;
            } else if (!node->right) {
                BSTNode<K, V>* temp = node->left;
                delete node;
                return temp;
            }
            BSTNode<K, V>* temp = findMin(node->right);
            node->key = temp->key;
            node->value = temp->value;
            node->right = removeNode(node->right, temp->key, success);
        }
        return node;
    }

    void collectKeys(BSTNode<K, V>* node, vector<K>& keys) const {
        if (!node) return;
        collectKeys(node->left, keys);
        keys.push_back(node->key);
        collectKeys(node->right, keys);
    }

public:
    BST() : root(nullptr) {}

    ~BST() {
        destroy(root);
    }

    void insert(const K& key, const V& value) {
        root = insertNode(root, key, value);
    }

    V* search(const K& key) const {
        BSTNode<K, V>* node = searchNode(root, key);
        if (!node) return nullptr;
        return &node->value;
    }

    bool remove(const K& key) {
        bool success = false;
        root = removeNode(root, key, success);
        return success;
    }

    vector<K> getAllKeys() const {
        vector<K> keys;
        collectKeys(root, keys);
        return keys;
    }

    size_t getMemoryUsageBytes() const {
        size_t total = sizeof(*this);
        if (root) {
            total += root->getMemoryUsageBytes();
        }
        return total;
    }
};

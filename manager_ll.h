#pragma once
#include "structures.h"

// ============================================================
// SYSTEM 1: HashMap + LinkedList
// ============================================================
struct DocEntryLL {
    Document doc;
    VersionLinkedList* versions;

    DocEntryLL() : versions(nullptr) {}
    DocEntryLL(Document d) : doc(d), versions(new VersionLinkedList()) {}

    size_t getMemoryUsageBytes() const {
        size_t total = sizeof(*this);
        total += doc.docId.capacity() + doc.docName.capacity();
        if (versions) {
            total += versions->getMemoryUsageBytes();
        }
        return total;
    }
};

inline size_t getValueMemoryUsage(const DocEntryLL& val) {
    return val.getMemoryUsageBytes();
}

class DocManagerLinkedList {
public:
    HashMap<DocEntryLL> docMap;     // key: docId
    HashMap<string> nameToId;       // key: docName -> docId (untuk search by name)

    // O(1) average
    bool insertDocument(string id, string name, string date, string editor, string content) {
        if (docMap.search(id)) return false; // sudah ada
        DocEntryLL entry(Document(id, name));
        entry.versions->addVersion(1, date, editor, content);
        entry.doc.totalVersions = 1;
        docMap.insert(id, entry);
        nameToId.insert(name, id);
        return true;
    }

    // O(1) average
    bool addVersion(string id, string date, string editor, string content) {
        DocEntryLL* entry = docMap.search(id);
        if (!entry) return false;
        entry->doc.totalVersions++;
        entry->versions->addVersion(entry->doc.totalVersions, date, editor, content);
        return true;
    }

    // O(1) by ID, O(n) by name
    DocEntryLL* searchById(const string& id) {
        return docMap.search(id);
    }

    DocEntryLL* searchByName(const string& name) {
        string* id = nameToId.search(name);
        if (!id) return nullptr;
        return docMap.search(*id);
    }

    // O(1) rollback
    bool rollback(const string& id) {
        DocEntryLL* entry = docMap.search(id);
        if (!entry || entry->versions->size <= 1) return false;
        entry->versions->rollback();
        entry->doc.totalVersions--;
        return true;
    }

    // O(n) list semua
    void listAll() {
        auto keys = docMap.getAllKeys();
        cout << "\n=== Daftar Dokumen (LinkedList) ===\n";
        for (auto& k : keys) {
            DocEntryLL* e = docMap.search(k);
            if (e) {
                cout << "[" << e->doc.docId << "] " << e->doc.docName
                     << " - " << e->doc.totalVersions << " versi\n";
            }
        }
    }

    // Save ke file
    void saveToFile(const string& filename) {
        ofstream f(filename);
        auto keys = docMap.getAllKeys();
        for (auto& k : keys) {
            DocEntryLL* e = docMap.search(k);
            if (!e) continue;
            Version* v = e->versions->head;
            while (v) {
                // Format: docId|docName|vNum|date|editor|content
                f << e->doc.docId << "|"
                  << e->doc.docName << "|"
                  << v->versionNumber << "|"
                  << v->date << "|"
                  << v->editor << "|"
                  << v->content << "\n";
                v = v->next;
            }
        }
        f.close();
    }

    // Load dari file
    void loadFromFile(const string& filename) {
        ifstream f(filename);
        if (!f.is_open()) return;
        string line;
        while (getline(f, line)) {
            stringstream ss(line);
            string id, name, vNum, date, editor, content;
            getline(ss, id, '|');
            getline(ss, name, '|');
            getline(ss, vNum, '|');
            getline(ss, date, '|');
            getline(ss, editor, '|');
            getline(ss, content, '|');

            DocEntryLL* existing = docMap.search(id);
            if (!existing) {
                insertDocument(id, name, date, editor, content);
            } else {
                addVersion(id, date, editor, content);
            }
        }
        f.close();
    }

    // Import dari CSV
    int importFromCSV(const string& filename, int maxRows = -1) {
        ifstream f(filename);
        if (!f.is_open()) {
            return -1;
        }
        string line;
        // Skip header
        if (!getline(f, line)) return 0;

        int count = 0;
        while (getline(f, line)) {
            if (maxRows > 0 && count >= maxRows) break;
            
            // Bersihkan carriage return (\r) jika ada
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            if (line.empty()) continue;

            vector<string> cols = parseCSVLine(line);
            if (cols.size() < 7) continue;

            string id = cols[1];        // StockCode
            string name = cols[2];      // Description
            string date = cols[4];      // InvoiceDate
            string editor = cols[6];    // Customer ID
            string invoice = cols[0];   // Invoice
            string qty = cols[3];       // Quantity
            string price = cols[5];     // Price (UnitPrice)

            if (id.empty()) continue;
            if (name.empty()) name = "Unnamed Document";
            if (editor.empty()) editor = "Unknown";

            // Format Konten Dokumen: InvoiceNo+Quantity+UnitPrice
            string content = "Invoice: " + invoice + ", Qty: " + qty + ", Price: " + price;

            if (!insertDocument(id, name, date, editor, content)) {
                addVersion(id, date, editor, content);
            }
            count++;
        }
        f.close();
        return count;
    }

    size_t getMemoryUsageBytes() const {
        size_t total = sizeof(*this);
        total += docMap.getMemoryUsageBytes();
        total += nameToId.getMemoryUsageBytes();
        return total;
    }
};

class DocManagerBSTLinkedList {
public:
    BST<string, DocEntryLL> docMap;     // key: docId
    BST<string, string> nameToId;       // key: docName -> docId (untuk search by name)

    // O(log N) average
    bool insertDocument(string id, string name, string date, string editor, string content) {
        if (docMap.search(id)) return false; // sudah ada
        DocEntryLL entry(Document(id, name));
        entry.versions->addVersion(1, date, editor, content);
        entry.doc.totalVersions = 1;
        docMap.insert(id, entry);
        nameToId.insert(name, id);
        return true;
    }

    // O(log N) average
    bool addVersion(string id, string date, string editor, string content) {
        DocEntryLL* entry = docMap.search(id);
        if (!entry) return false;
        entry->doc.totalVersions++;
        entry->versions->addVersion(entry->doc.totalVersions, date, editor, content);
        return true;
    }

    // O(log N) by ID, O(log N) by name
    DocEntryLL* searchById(const string& id) {
        return docMap.search(id);
    }

    DocEntryLL* searchByName(const string& name) {
        string* id = nameToId.search(name);
        if (!id) return nullptr;
        return docMap.search(*id);
    }

    // O(log N) average rollback
    bool rollback(const string& id) {
        DocEntryLL* entry = docMap.search(id);
        if (!entry || entry->versions->size <= 1) return false;
        entry->versions->rollback();
        entry->doc.totalVersions--;
        return true;
    }

    // O(N) list semua
    void listAll() {
        auto keys = docMap.getAllKeys();
        cout << "\n=== Daftar Dokumen (BST + LinkedList) ===\n";
        for (auto& k : keys) {
            DocEntryLL* e = docMap.search(k);
            if (e) {
                cout << "[" << e->doc.docId << "] " << e->doc.docName
                     << " - " << e->doc.totalVersions << " versi\n";
            }
        }
    }

    // Save ke file
    void saveToFile(const string& filename) {
        ofstream f(filename);
        auto keys = docMap.getAllKeys();
        for (auto& k : keys) {
            DocEntryLL* e = docMap.search(k);
            if (!e) continue;
            Version* v = e->versions->head;
            while (v) {
                f << e->doc.docId << "|"
                  << e->doc.docName << "|"
                  << v->versionNumber << "|"
                  << v->date << "|"
                  << v->editor << "|"
                  << v->content << "\n";
                v = v->next;
            }
        }
        f.close();
    }

    // Load dari file
    void loadFromFile(const string& filename) {
        ifstream f(filename);
        if (!f.is_open()) return;
        string line;
        while (getline(f, line)) {
            stringstream ss(line);
            string id, name, vNum, date, editor, content;
            getline(ss, id, '|');
            getline(ss, name, '|');
            getline(ss, vNum, '|');
            getline(ss, date, '|');
            getline(ss, editor, '|');
            getline(ss, content, '|');

            DocEntryLL* existing = docMap.search(id);
            if (!existing) {
                insertDocument(id, name, date, editor, content);
            } else {
                addVersion(id, date, editor, content);
            }
        }
        f.close();
    }

    // Import dari CSV
    int importFromCSV(const string& filename, int maxRows = -1) {
        ifstream f(filename);
        if (!f.is_open()) {
            return -1;
        }
        string line;
        // Skip header
        if (!getline(f, line)) return 0;

        int count = 0;
        while (getline(f, line)) {
            if (maxRows > 0 && count >= maxRows) break;
            
            // Bersihkan carriage return (\r) jika ada
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            if (line.empty()) continue;

            vector<string> cols = parseCSVLine(line);
            if (cols.size() < 7) continue;

            string id = cols[1];        // StockCode
            string name = cols[2];      // Description
            string date = cols[4];      // InvoiceDate
            string editor = cols[6];    // Customer ID
            string invoice = cols[0];   // Invoice
            string qty = cols[3];       // Quantity
            string price = cols[5];     // Price (UnitPrice)

            if (id.empty()) continue;
            if (name.empty()) name = "Unnamed Document";
            if (editor.empty()) editor = "Unknown";

            // Format Konten Dokumen: InvoiceNo+Quantity+UnitPrice
            string content = "Invoice: " + invoice + ", Qty: " + qty + ", Price: " + price;

            if (!insertDocument(id, name, date, editor, content)) {
                addVersion(id, date, editor, content);
            }
            count++;
        }
        f.close();
        return count;
    }

    size_t getMemoryUsageBytes() const {
        size_t total = sizeof(*this);
        total += docMap.getMemoryUsageBytes();
        total += nameToId.getMemoryUsageBytes();
        return total;
    }
};


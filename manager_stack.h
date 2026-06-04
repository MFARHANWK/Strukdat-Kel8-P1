#pragma once
#include "structures.h"

// ============================================================
// SYSTEM 2: HashMap + Stack (vector-based)
// ============================================================
struct DocEntryStack {
    Document doc;
    VersionStack* versions;

    DocEntryStack() : versions(nullptr) {}
    DocEntryStack(Document d) : doc(d), versions(new VersionStack()) {}

    size_t getMemoryUsageBytes() const {
        size_t total = sizeof(*this);
        total += doc.docId.capacity() + doc.docName.capacity();
        if (versions) {
            total += versions->getMemoryUsageBytes();
        }
        return total;
    }
};

inline size_t getValueMemoryUsage(const DocEntryStack& val) {
    return val.getMemoryUsageBytes();
}

class DocManagerStack {
public:
    HashMap<DocEntryStack> docMap;
    HashMap<string> nameToId;

    bool insertDocument(string id, string name, string date, string editor, string content) {
        if (docMap.search(id)) return false;
        DocEntryStack entry(Document(id, name));
        entry.versions->addVersion(1, date, editor, content);
        entry.doc.totalVersions = 1;
        docMap.insert(id, entry);
        nameToId.insert(name, id);
        return true;
    }

    bool addVersion(string id, string date, string editor, string content) {
        DocEntryStack* entry = docMap.search(id);
        if (!entry) return false;
        entry->doc.totalVersions++;
        entry->versions->addVersion(entry->doc.totalVersions, date, editor, content);
        return true;
    }

    DocEntryStack* searchById(const string& id) {
        return docMap.search(id);
    }

    DocEntryStack* searchByName(const string& name) {
        string* id = nameToId.search(name);
        if (!id) return nullptr;
        return docMap.search(*id);
    }

    // O(1) rollback - keunggulan Stack
    bool rollback(const string& id) {
        DocEntryStack* entry = docMap.search(id);
        if (!entry || entry->versions->size() <= 1) return false;
        entry->versions->rollback();
        entry->doc.totalVersions--;
        return true;
    }

    void listAll() {
        auto keys = docMap.getAllKeys();
        cout << "\n=== Daftar Dokumen (Stack) ===\n";
        for (auto& k : keys) {
            DocEntryStack* e = docMap.search(k);
            if (e) {
                cout << "[" << e->doc.docId << "] " << e->doc.docName
                     << " - " << e->doc.totalVersions << " versi\n";
            }
        }
    }

    void saveToFile(const string& filename) {
        ofstream f(filename);
        auto keys = docMap.getAllKeys();
        for (auto& k : keys) {
            DocEntryStack* e = docMap.search(k);
            if (!e) continue;
            for (auto v : e->versions->stack) {
                f << e->doc.docId << "|"
                  << e->doc.docName << "|"
                  << v->versionNumber << "|"
                  << v->date << "|"
                  << v->editor << "|"
                  << v->content << "\n";
            }
        }
        f.close();
    }

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

            DocEntryStack* existing = docMap.search(id);
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

class DocManagerBSTStack {
public:
    BST<string, DocEntryStack> docMap;
    BST<string, string> nameToId;

    bool insertDocument(string id, string name, string date, string editor, string content) {
        if (docMap.search(id)) return false;
        DocEntryStack entry(Document(id, name));
        entry.versions->addVersion(1, date, editor, content);
        entry.doc.totalVersions = 1;
        docMap.insert(id, entry);
        nameToId.insert(name, id);
        return true;
    }

    bool addVersion(string id, string date, string editor, string content) {
        DocEntryStack* entry = docMap.search(id);
        if (!entry) return false;
        entry->doc.totalVersions++;
        entry->versions->addVersion(entry->doc.totalVersions, date, editor, content);
        return true;
    }

    DocEntryStack* searchById(const string& id) {
        return docMap.search(id);
    }

    DocEntryStack* searchByName(const string& name) {
        string* id = nameToId.search(name);
        if (!id) return nullptr;
        return docMap.search(*id);
    }

    // O(log N) rollback
    bool rollback(const string& id) {
        DocEntryStack* entry = docMap.search(id);
        if (!entry || entry->versions->size() <= 1) return false;
        entry->versions->rollback();
        entry->doc.totalVersions--;
        return true;
    }

    void listAll() {
        auto keys = docMap.getAllKeys();
        cout << "\n=== Daftar Dokumen (BST + Stack) ===\n";
        for (auto& k : keys) {
            DocEntryStack* e = docMap.search(k);
            if (e) {
                cout << "[" << e->doc.docId << "] " << e->doc.docName
                     << " - " << e->doc.totalVersions << " versi\n";
            }
        }
    }

    void saveToFile(const string& filename) {
        ofstream f(filename);
        auto keys = docMap.getAllKeys();
        for (auto& k : keys) {
            DocEntryStack* e = docMap.search(k);
            if (!e) continue;
            for (auto v : e->versions->stack) {
                f << e->doc.docId << "|"
                  << e->doc.docName << "|"
                  << v->versionNumber << "|"
                  << v->date << "|"
                  << v->editor << "|"
                  << v->content << "\n";
            }
        }
        f.close();
    }

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

            DocEntryStack* existing = docMap.search(id);
            if (!existing) {
                insertDocument(id, name, date, editor, content);
            } else {
                addVersion(id, date, editor, content);
            }
        }
        f.close();
    }

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


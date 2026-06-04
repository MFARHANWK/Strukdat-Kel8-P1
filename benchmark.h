#pragma once
#include "manager_ll.h"
#include "manager_stack.h"
#include <random>
#include <algorithm>

// ============================================================
// OS MEMORY MONITORING UTILITY
// ============================================================
#ifdef __APPLE__
#include <mach/mach.h>
inline size_t getOSMemoryUsage() {
    struct mach_task_basic_info info;
    mach_msg_type_number_t infoCount = MACH_TASK_BASIC_INFO_COUNT;
    if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &infoCount) != KERN_SUCCESS) {
        return 0;
    }
    return (size_t)info.resident_size;
}
#elif defined(_WIN32)
#include <windows.h>
#include <psapi.h>
inline size_t getOSMemoryUsage() {
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize;
    }
    return 0;
}
#else
#include <unistd.h>
#include <fstream>
inline size_t getOSMemoryUsage() {
    std::ifstream file("/proc/self/statm");
    if (file.is_open()) {
        size_t pages = 0;
        file >> pages; // Virtual size pages
        file >> pages; // Resident set size pages
        return pages * sysconf(_SC_PAGESIZE);
    }
    return 0;
}
#endif

// ============================================================
// BENCHMARK UTILITIES
// ============================================================

// Timer helper
struct Timer {
    high_resolution_clock::time_point start;
    void reset() { start = high_resolution_clock::now(); }
    double elapsedMs() {
        auto end = high_resolution_clock::now();
        return duration<double, milli>(end - start).count();
    }
};

// Generate random string
string randStr(int len, mt19937& rng) {
    static const string chars = "abcdefghijklmnopqrstuvwxyz0123456789";
    string s;
    for (int i = 0; i < len; i++)
        s += chars[rng() % chars.size()];
    return s;
}

// Generate data sintetis mirip dataset UCI
struct SyntheticRecord {
    string docId;
    string docName;
    string date;
    string editor;
    string content;
};

vector<SyntheticRecord> generateDataset(int n, mt19937& rng) {
    vector<SyntheticRecord> data;
    // Buat ~n/5 dokumen unik dengan rata-rata 5 versi
    int numDocs = max(1, n / 5);
    vector<string> docIds, docNames;

    for (int i = 0; i < numDocs; i++) {
        docIds.push_back("DOC" + to_string(1000 + i));
        docNames.push_back("Document_" + randStr(6, rng));
    }

    for (int i = 0; i < n; i++) {
        SyntheticRecord r;
        r.docId   = docIds[i % numDocs];
        r.docName = docNames[i % numDocs];
        // Format tanggal: YYYY-MM-DD
        int year  = 2009 + (rng() % 3);
        int month = 1 + (rng() % 12);
        int day   = 1 + (rng() % 28);
        r.date    = to_string(year) + "-"
                  + (month < 10 ? "0" : "") + to_string(month) + "-"
                  + (day < 10 ? "0" : "") + to_string(day);
        r.editor  = "C" + to_string(10000 + rng() % 5000);
        r.content = "Invoice content " + randStr(20, rng) + " qty=" + to_string(rng() % 100);
        data.push_back(r);
    }
    return data;
}

inline vector<SyntheticRecord> loadDatasetFromCSV(const string& filename, int n) {
    vector<SyntheticRecord> data;
    ifstream f(filename);
    if (!f.is_open()) {
        return data;
    }
    string line;
    if (!getline(f, line)) return data; // skip header
    
    int count = 0;
    while (getline(f, line) && count < n) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty()) continue;
        vector<string> cols = parseCSVLine(line);
        if (cols.size() < 7) continue;

        SyntheticRecord r;
        r.docId = cols[1];
        r.docName = cols[2];
        r.date = cols[4];
        r.editor = cols[6];
        string invoice = cols[0];
        string qty = cols[3];
        string price = cols[5];

        if (r.docId.empty()) continue;
        if (r.docName.empty()) r.docName = "Unnamed Document";
        if (r.editor.empty()) r.editor = "Unknown";
        r.content = "Invoice: " + invoice + ", Qty: " + qty + ", Price: " + price;

        data.push_back(r);
        count++;
    }
    f.close();
    return data;
}

inline vector<SyntheticRecord> getBenchmarkDataset(int n, mt19937& rng, const string& csvFilename = "Untitled spreadsheet - Year 2009-2010.csv") {
    auto data = loadDatasetFromCSV(csvFilename, n);
    if ((int)data.size() < n) {
        cout << "[Benchmark] Menggunakan data sintetis (fallback) karena file CSV tidak ditemukan atau baris kurang dari " << n << ".\n";
        return generateDataset(n, rng);
    }
    cout << "[Benchmark] Berhasil memuat " << data.size() << " baris dari CSV asli (" << csvFilename << ").\n";
    return data;
}

// ============================================================
// EXPERIMENT RUNNER
// ============================================================
struct BenchResult {
    string operation;
    int dataSize;
    double llHashMapTimeMs;
    double stackHashMapTimeMs;
    double llBstTimeMs;
    double stackBstTimeMs;
};

vector<BenchResult> runBenchmark(const vector<int>& sizes) {
    vector<BenchResult> results;
    mt19937 rng(42); // seed tetap agar reproducible
    Timer t;

    for (int N : sizes) {
        cout << "\n[Benchmark] N = " << N << " records...\n";
        auto dataset = getBenchmarkDataset(N, rng);

        // -------- INSERT --------
        {
            DocManagerLinkedList mllHashMap;
            DocManagerStack      mstHashMap;
            DocManagerBSTLinkedList mllBST;
            DocManagerBSTStack      mstBST;

            // LL HashMap
            t.reset();
            for (auto& r : dataset) {
                if (!mllHashMap.searchById(r.docId))
                    mllHashMap.insertDocument(r.docId, r.docName, r.date, r.editor, r.content);
                else
                    mllHashMap.addVersion(r.docId, r.date, r.editor, r.content);
            }
            double llHm = t.elapsedMs();

            // Stack HashMap
            t.reset();
            for (auto& r : dataset) {
                if (!mstHashMap.searchById(r.docId))
                    mstHashMap.insertDocument(r.docId, r.docName, r.date, r.editor, r.content);
                else
                    mstHashMap.addVersion(r.docId, r.date, r.editor, r.content);
            }
            double stHm = t.elapsedMs();

            // LL BST
            t.reset();
            for (auto& r : dataset) {
                if (!mllBST.searchById(r.docId))
                    mllBST.insertDocument(r.docId, r.docName, r.date, r.editor, r.content);
                else
                    mllBST.addVersion(r.docId, r.date, r.editor, r.content);
            }
            double llBst = t.elapsedMs();

            // Stack BST
            t.reset();
            for (auto& r : dataset) {
                if (!mstBST.searchById(r.docId))
                    mstBST.insertDocument(r.docId, r.docName, r.date, r.editor, r.content);
                else
                    mstBST.addVersion(r.docId, r.date, r.editor, r.content);
            }
            double stBst = t.elapsedMs();

            results.push_back({"INSERT", N, llHm, stHm, llBst, stBst});
            cout << "  INSERT  -> LL_HM: " << fixed << setprecision(3) << llHm
                 << " ms | Stack_HM: " << stHm << " ms | LL_BST: " << llBst << " ms | Stack_BST: " << stBst << " ms\n";

            // -------- SEARCH --------
            {
                string targetId = dataset[N/2].docId;
                int reps = 100000;
                long long dummyCheck = 0;

                t.reset();
                for (int i = 0; i < reps; i++) {
                    if (mllHashMap.searchById(targetId)) {
                        dummyCheck++;
                    }
                }
                double llHmS = t.elapsedMs() / reps;

                t.reset();
                for (int i = 0; i < reps; i++) {
                    if (mstHashMap.searchById(targetId)) {
                        dummyCheck++;
                    }
                }
                double stHmS = t.elapsedMs() / reps;

                t.reset();
                for (int i = 0; i < reps; i++) {
                    if (mllBST.searchById(targetId)) {
                        dummyCheck++;
                    }
                }
                double llBstS = t.elapsedMs() / reps;

                t.reset();
                for (int i = 0; i < reps; i++) {
                    if (mstBST.searchById(targetId)) {
                        dummyCheck++;
                    }
                }
                double stBstS = t.elapsedMs() / reps;

                // Force compiler to keep the loops
                if (dummyCheck == 999999999) {
                    cout << "Prevent optimization: " << dummyCheck << "\n";
                }

                results.push_back({"SEARCH", N, llHmS, stHmS, llBstS, stBstS});
                cout << "  SEARCH  -> LL_HM: " << fixed << setprecision(6) << llHmS << " ms | Stack_HM: " << stHmS << " ms | LL_BST: " << llBstS << " ms | Stack_BST: " << stBstS << " ms\n";
            }

            // -------- ROLLBACK --------
            {
                string targetId = dataset[0].docId;
                int reps = min(100, N/5);

                t.reset();
                for (int i = 0; i < reps; i++) mllHashMap.rollback(targetId);
                double llHmR = t.elapsedMs() / max(1, reps);

                t.reset();
                for (int i = 0; i < reps; i++) mstHashMap.rollback(targetId);
                double stHmR = t.elapsedMs() / max(1, reps);

                t.reset();
                for (int i = 0; i < reps; i++) mllBST.rollback(targetId);
                double llBstR = t.elapsedMs() / max(1, reps);

                t.reset();
                for (int i = 0; i < reps; i++) mstBST.rollback(targetId);
                double stBstR = t.elapsedMs() / max(1, reps);

                results.push_back({"ROLLBACK", N, llHmR, stHmR, llBstR, stBstR});
                cout << "  ROLLBACK-> LL_HM: " << llHmR << " ms | Stack_HM: " << stHmR << " ms | LL_BST: " << llBstR << " ms | Stack_BST: " << stBstR << " ms\n";
            }

            // -------- DELETE (remove doc) --------
            {
                string targetId = dataset[1].docId;
                int reps = 10;

                t.reset();
                for (int i = 0; i < reps; i++) mllHashMap.docMap.remove(targetId);
                double llHmD = t.elapsedMs() / reps;

                t.reset();
                for (int i = 0; i < reps; i++) mstHashMap.docMap.remove(targetId);
                double stHmD = t.elapsedMs() / reps;

                t.reset();
                for (int i = 0; i < reps; i++) mllBST.docMap.remove(targetId);
                double llBstD = t.elapsedMs() / reps;

                t.reset();
                for (int i = 0; i < reps; i++) mstBST.docMap.remove(targetId);
                double stBstD = t.elapsedMs() / reps;

                results.push_back({"DELETE", N, llHmD, stHmD, llBstD, stBstD});
                cout << "  DELETE  -> LL_HM: " << llHmD << " ms | Stack_HM: " << stHmD << " ms | LL_BST: " << llBstD << " ms | Stack_BST: " << stBstD << " ms\n";
            }
        }
    }
    return results;
}

// Simpan hasil benchmark ke CSV
void saveBenchmarkCSV(const vector<BenchResult>& results, const string& filename) {
    ofstream f(filename);
    f << "Operation,DataSize,LL_HashMap_ms,Stack_HashMap_ms,LL_BST_ms,Stack_BST_ms\n";
    for (auto& r : results) {
        f << r.operation << ","
          << r.dataSize << ","
          << fixed << setprecision(6) << r.llHashMapTimeMs << ","
          << r.stackHashMapTimeMs << ","
          << r.llBstTimeMs << ","
          << r.stackBstTimeMs << "\n";
    }
    f.close();
    cout << "\n[Benchmark] Hasil disimpan ke: " << filename << "\n";
}

// ============================================================
// MEMORY BENCHMARK UTILITIES
// ============================================================
struct MemBenchResult {
    int dataSize;
    double llHashMapTheoreticalKb;
    double stackHashMapTheoreticalKb;
    double llBstTheoreticalKb;
    double stackBstTheoreticalKb;
    double llHashMapOsRssKb;
    double stackHashMapOsRssKb;
    double llBstOsRssKb;
    double stackBstOsRssKb;
};

inline vector<MemBenchResult> runMemoryBenchmark(const vector<int>& sizes) {
    vector<MemBenchResult> results;
    mt19937 rng(42);

    for (int N : sizes) {
        cout << "\n[Memory Benchmark] N = " << N << " records...\n";
        auto dataset = getBenchmarkDataset(N, rng);

        MemBenchResult res;
        res.dataSize = N;

        // --- Benchmark LL HashMap Memory ---
        {
            size_t beforeOS = getOSMemoryUsage();
            DocManagerLinkedList* m = new DocManagerLinkedList();
            for (auto& r : dataset) {
                if (!m->searchById(r.docId))
                    m->insertDocument(r.docId, r.docName, r.date, r.editor, r.content);
                else
                    m->addVersion(r.docId, r.date, r.editor, r.content);
            }
            size_t afterOS = getOSMemoryUsage();
            res.llHashMapTheoreticalKb = (double)m->getMemoryUsageBytes() / 1024.0;
            res.llHashMapOsRssKb = (afterOS > beforeOS) ? (double)(afterOS - beforeOS) / 1024.0 : 0.0;
            delete m;
        }

        // --- Benchmark Stack HashMap Memory ---
        {
            size_t beforeOS = getOSMemoryUsage();
            DocManagerStack* m = new DocManagerStack();
            for (auto& r : dataset) {
                if (!m->searchById(r.docId))
                    m->insertDocument(r.docId, r.docName, r.date, r.editor, r.content);
                else
                    m->addVersion(r.docId, r.date, r.editor, r.content);
            }
            size_t afterOS = getOSMemoryUsage();
            res.stackHashMapTheoreticalKb = (double)m->getMemoryUsageBytes() / 1024.0;
            res.stackHashMapOsRssKb = (afterOS > beforeOS) ? (double)(afterOS - beforeOS) / 1024.0 : 0.0;
            delete m;
        }

        // --- Benchmark LL BST Memory ---
        {
            size_t beforeOS = getOSMemoryUsage();
            DocManagerBSTLinkedList* m = new DocManagerBSTLinkedList();
            for (auto& r : dataset) {
                if (!m->searchById(r.docId))
                    m->insertDocument(r.docId, r.docName, r.date, r.editor, r.content);
                else
                    m->addVersion(r.docId, r.date, r.editor, r.content);
            }
            size_t afterOS = getOSMemoryUsage();
            res.llBstTheoreticalKb = (double)m->getMemoryUsageBytes() / 1024.0;
            res.llBstOsRssKb = (afterOS > beforeOS) ? (double)(afterOS - beforeOS) / 1024.0 : 0.0;
            delete m;
        }

        // --- Benchmark Stack BST Memory ---
        {
            size_t beforeOS = getOSMemoryUsage();
            DocManagerBSTStack* m = new DocManagerBSTStack();
            for (auto& r : dataset) {
                if (!m->searchById(r.docId))
                    m->insertDocument(r.docId, r.docName, r.date, r.editor, r.content);
                else
                    m->addVersion(r.docId, r.date, r.editor, r.content);
            }
            size_t afterOS = getOSMemoryUsage();
            res.stackBstTheoreticalKb = (double)m->getMemoryUsageBytes() / 1024.0;
            res.stackBstOsRssKb = (afterOS > beforeOS) ? (double)(afterOS - beforeOS) / 1024.0 : 0.0;
            delete m;
        }

        results.push_back(res);
        cout << "  THEORETICAL -> LL_HM: " << fixed << setprecision(2) << res.llHashMapTheoreticalKb 
             << " KB | Stack_HM: " << res.stackHashMapTheoreticalKb 
             << " KB | LL_BST: " << res.llBstTheoreticalKb 
             << " KB | Stack_BST: " << res.stackBstTheoreticalKb << " KB\n";
    }
    return results;
}

inline void saveMemoryBenchmarkCSV(const vector<MemBenchResult>& results, const string& filename) {
    ofstream f(filename);
    f << "DataSize,LL_HashMap_Theoretical_KB,Stack_HashMap_Theoretical_KB,LL_BST_Theoretical_KB,Stack_BST_Theoretical_KB,LL_HashMap_OS_RSS_KB,Stack_HashMap_OS_RSS_KB,LL_BST_OS_RSS_KB,Stack_BST_OS_RSS_KB\n";
    for (auto& r : results) {
        f << r.dataSize << ","
          << fixed << setprecision(2) << r.llHashMapTheoreticalKb << ","
          << r.stackHashMapTheoreticalKb << ","
          << r.llBstTheoreticalKb << ","
          << r.stackBstTheoreticalKb << ","
          << r.llHashMapOsRssKb << ","
          << r.stackHashMapOsRssKb << ","
          << r.llBstOsRssKb << ","
          << r.stackBstOsRssKb << "\n";
    }
    f.close();
    cout << "\n[Memory Benchmark] Hasil disimpan ke: " << filename << "\n";
}

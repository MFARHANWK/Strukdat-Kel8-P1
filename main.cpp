#include "manager_ll.h"
#include "manager_stack.h"
#include "benchmark.h"

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void printHeader() {
    cout << "╔══════════════════════════════════════════════════════╗\n";
    cout << "║     SISTEM MANAJEMEN VERSI DOKUMEN SEDERHANA         ║\n";
    cout << "║     Struktur Data - IPB University 2026              ║\n";
    cout << "╚══════════════════════════════════════════════════════╝\n";
}

void menuCLI(string argv0) {
    DocManagerLinkedList sysLL;
    DocManagerStack      sysStack;
    DocManagerBSTLinkedList sysBSTLL;
    DocManagerBSTStack      sysBSTStack;

    // Load data dari file jika ada
    sysLL.loadFromFile("data_ll.txt");
    sysStack.loadFromFile("data_stack.txt");
    sysBSTLL.loadFromFile("data_bst_ll.txt");
    sysBSTStack.loadFromFile("data_bst_stack.txt");

    int choice;
    while (true) {
        printHeader();
        cout << "\n[MENU UTAMA]\n";
        cout << "1. Insert Dokumen Baru\n";
        cout << "2. Tambah Versi Dokumen\n";
        cout << "3. Search Dokumen (by ID)\n";
        cout << "4. Search Dokumen (by Nama)\n";
        cout << "5. Lihat Riwayat Versi\n";
        cout << "6. Akses Versi Tertentu\n";
        cout << "7. Rollback ke Versi Sebelumnya\n";
        cout << "8. List Semua Dokumen\n";
        cout << "9. Simpan Data ke File\n";
        cout << "10. Jalankan Benchmark Performa\n";
        cout << "11. Load Data dari CSV (Dataset)\n";
        cout << "12. Jalankan Benchmark Memori\n";
        cout << "0. Keluar\n";
        cout << "\nPilihan: ";
        cin >> choice;
        cin.ignore();

        if (choice == 0) {
            sysLL.saveToFile("data_ll.txt");
            sysStack.saveToFile("data_stack.txt");
            sysBSTLL.saveToFile("data_bst_ll.txt");
            sysBSTStack.saveToFile("data_bst_stack.txt");
            cout << "Data disimpan. Sampai jumpa!\n";
            break;
        }

        if (choice == 1) {
            cout << "\n--- Insert Dokumen Baru ---\n";
            string id, name, date, editor, content;
            cout << "ID Dokumen  : "; getline(cin, id);
            cout << "Nama Dokumen: "; getline(cin, name);
            cout << "Tanggal     : "; getline(cin, date);
            cout << "Editor      : "; getline(cin, editor);
            cout << "Konten      : "; getline(cin, content);

            bool r1 = sysLL.insertDocument(id, name, date, editor, content);
            bool r2 = sysStack.insertDocument(id, name, date, editor, content);
            bool r3 = sysBSTLL.insertDocument(id, name, date, editor, content);
            bool r4 = sysBSTStack.insertDocument(id, name, date, editor, content);
            if (r1 && r2 && r3 && r4)
                cout << "[OK] Dokumen berhasil ditambahkan.\n";
            else
                cout << "[GAGAL] ID dokumen sudah ada.\n";
        }

        else if (choice == 2) {
            cout << "\n--- Tambah Versi Dokumen ---\n";
            string id, date, editor, content;
            cout << "ID Dokumen: "; getline(cin, id);
            cout << "Tanggal   : "; getline(cin, date);
            cout << "Editor    : "; getline(cin, editor);
            cout << "Konten    : "; getline(cin, content);

            bool r1 = sysLL.addVersion(id, date, editor, content);
            bool r2 = sysStack.addVersion(id, date, editor, content);
            bool r3 = sysBSTLL.addVersion(id, date, editor, content);
            bool r4 = sysBSTStack.addVersion(id, date, editor, content);
            if (r1 && r2 && r3 && r4) cout << "[OK] Versi baru ditambahkan.\n";
            else                      cout << "[GAGAL] Dokumen tidak ditemukan.\n";
        }

        else if (choice == 3) {
            cout << "\n--- Search by ID ---\n";
            string id;
            cout << "ID Dokumen: "; getline(cin, id);
            auto* e = sysBSTLL.searchById(id);
            if (e) {
                cout << "Ditemukan: [" << e->doc.docId << "] " << e->doc.docName
                     << " | " << e->doc.totalVersions << " versi\n";
            } else {
                cout << "Dokumen tidak ditemukan.\n";
            }
        }

        else if (choice == 4) {
            cout << "\n--- Search by Nama ---\n";
            string name;
            cout << "Nama Dokumen: "; getline(cin, name);
            auto* e = sysBSTLL.searchByName(name);
            if (e) {
                cout << "Ditemukan: [" << e->doc.docId << "] " << e->doc.docName
                     << " | " << e->doc.totalVersions << " versi\n";
            } else {
                cout << "Dokumen tidak ditemukan.\n";
            }
        }

        else if (choice == 5) {
            cout << "\n--- Riwayat Versi ---\n";
            string id;
            cout << "ID Dokumen: "; getline(cin, id);
            auto* e = sysBSTLL.searchById(id);
            if (e) {
                cout << "Versi dokumen [" << id << "]:\n";
                e->versions->printAll();
            } else {
                cout << "Dokumen tidak ditemukan.\n";
            }
        }

        else if (choice == 6) {
            cout << "\n--- Akses Versi Tertentu ---\n";
            string id;
            int vNum;
            cout << "ID Dokumen: "; getline(cin, id);
            cout << "Nomor Versi: "; cin >> vNum; cin.ignore();

            auto* e = sysBSTLL.searchById(id);
            if (e) {
                Version* v = e->versions->getVersion(vNum);
                if (v) {
                    cout << "Versi " << v->versionNumber << ":\n"
                         << "  Tanggal : " << v->date << "\n"
                         << "  Editor  : " << v->editor << "\n"
                         << "  Konten  : " << v->content << "\n";
                } else {
                    cout << "Versi tidak ditemukan.\n";
                }
            } else {
                cout << "Dokumen tidak ditemukan.\n";
            }
        }

        else if (choice == 7) {
            cout << "\n--- Rollback ---\n";
            string id;
            cout << "ID Dokumen: "; getline(cin, id);

            bool r1 = sysLL.rollback(id);
            bool r2 = sysStack.rollback(id);
            bool r3 = sysBSTLL.rollback(id);
            bool r4 = sysBSTStack.rollback(id);
            if (r1 && r2 && r3 && r4) cout << "[OK] Rollback berhasil. Versi terakhir dihapus.\n";
            else                      cout << "[GAGAL] Tidak bisa rollback (dokumen tidak ada atau hanya 1 versi).\n";
        }

        else if (choice == 8) {
            sysBSTLL.listAll();
        }

        else if (choice == 9) {
            sysLL.saveToFile("data_ll.txt");
            sysStack.saveToFile("data_stack.txt");
            sysBSTLL.saveToFile("data_bst_ll.txt");
            sysBSTStack.saveToFile("data_bst_stack.txt");
            cout << "[OK] Data berhasil disimpan.\n";
        }

        else if (choice == 10) {
            cout << "\n--- BENCHMARK PERFORMA ---\n";
            cout << "Menguji dengan N = 100, 1000, 5000, 10000, 50000 records...\n\n";

            vector<int> sizes = {100, 1000, 5000, 10000, 50000};
            auto results = runBenchmark(sizes);
            saveBenchmarkCSV(results, "benchmark_results.csv");

            cout << "\n[Selesai] Cek file benchmark_results.csv untuk data lengkap.\n";
        }

        else if (choice == 11) {
            cout << "\n--- LOAD DATA DARI CSV ---\n";
            string filename;
            int maxRows = 0;
            cout << "Nama file CSV [default: Untitled spreadsheet - Year 2009-2010.csv]: ";
            getline(cin, filename);
            if (filename.empty()) {
                filename = "Untitled spreadsheet - Year 2009-2010.csv";
            }
            cout << "Jumlah baris yang ingin di-load (0 untuk semua): ";
            cin >> maxRows;
            cin.ignore();

            if (maxRows < 0) {
                cout << "[GAGAL] Jumlah baris tidak boleh negatif.\n";
            } else {
                if (maxRows == 0) maxRows = -1; // load semua

                cout << "Sedang memuat data dari " << filename << "...\n";
                auto startT = high_resolution_clock::now();
                int countLL = sysLL.importFromCSV(filename, maxRows);
                int countStack = sysStack.importFromCSV(filename, maxRows);
                int countBSTLL = sysBSTLL.importFromCSV(filename, maxRows);
                int countBSTStack = sysBSTStack.importFromCSV(filename, maxRows);
                auto endT = high_resolution_clock::now();
                double elapsed = duration<double, milli>(endT - startT).count();

                if (countLL < 0 || countStack < 0 || countBSTLL < 0 || countBSTStack < 0) {
                    cout << "[GAGAL] Tidak dapat membuka atau membaca file CSV.\n";
                } else {
                    cout << "[OK] Berhasil memuat " << countLL << " baris transaksi ritel.\n";
                    cout << "Waktu pemrosesan: " << fixed << setprecision(2) << elapsed << " ms\n";
                }
            }
        }

        else if (choice == 12) {
            cout << "\n--- BENCHMARK MEMORI ---\n";
            cout << "Menguji dengan N = 100, 1000, 5000, 10000, 50000 records...\n\n";

            vector<int> sizes = {100, 1000, 5000, 10000, 50000};
            auto results = runMemoryBenchmark(sizes, argv0);
            saveMemoryBenchmarkCSV(results, "benchmark_memory.csv");

            cout << "\n[Selesai] Cek file benchmark_memory.csv untuk data lengkap.\n";
        }

        else {
            cout << "Pilihan tidak valid.\n";
        }

        cout << "\nTekan Enter untuk lanjut...";
        cin.ignore();
        cin.get();
    }
}

// ============================================================
// MAIN
// ============================================================
int main(int argc, char* argv[]) {
    if (argc > 3 && string(argv[1]) == "--bench-mem-single") {
        string type = argv[2];
        int N = stoi(argv[3]);
        runSingleMemoryBenchmark(type, N);
        return 0;
    }

    string argv0 = "./doc_version_system";
    if (argc > 0) argv0 = argv[0];

    if (argc > 1 && string(argv[1]) == "--benchmark") {
        // Mode benchmark otomatis (tanpa CLI)
        cout << "=== MODE BENCHMARK OTOMATIS ===\n";
        vector<int> sizes = {100, 1000, 5000, 10000, 50000, 100000};
        
        cout << "\n--- RUNNING PERFORMANCE BENCHMARK ---\n";
        auto results = runBenchmark(sizes);
        saveBenchmarkCSV(results, "benchmark_results.csv");

        cout << "\n--- RUNNING MEMORY BENCHMARK ---\n";
        auto memResults = runMemoryBenchmark(sizes, argv0);
        saveMemoryBenchmarkCSV(memResults, "benchmark_memory.csv");
        return 0;
    }

    menuCLI(argv0);
    return 0;
}

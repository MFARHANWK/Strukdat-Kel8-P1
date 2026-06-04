# Best Practices: Sistem Manajemen Versi Dokumen Sederhana
*Struktur Data (Strukdat) — IPB University*

Dokumen ini berisi panduan dan praktik terbaik (*best practices*) untuk merancang, mengimplementasikan, dan menganalisis performa program **Sistem Manajemen Versi Dokumen Sederhana**. Panduan ini disusun untuk membantu mahasiswa mencapai target progres Minggu ke-7 dan finalisasi Minggu ke-14 dengan kode yang bersih, efisien, bebas dari *memory leak*, dan siap dibenchmark secara objektif.

---

## 1. Arsitektur & Pemilihan Struktur Data

Sistem ini dirancang untuk membandingkan performa dua atau lebih struktur data untuk mengelola riwayat versi dokumen secara linear.

### A. Indeks Utama: Hash Map (Pencarian Dokumen)
Pencarian dokumen berdasarkan `docId` atau `docName` harus berjalan dalam waktu konstan ($\mathcal{O}(1)$ average case).
*   **Best Practice**: Gunakan **Hash Table** kustom dengan **Separate Chaining** (resolusi tabrakan dengan linked list pada bucket).
*   **Fungsi Hash**: Gunakan algoritma polinomial rolling hash (misalnya pengali konstanta $31$) untuk mengubah string ID/Nama menjadi indeks tabel hash.
*   **Double Indexing**: Sediakan dua tabel hash terpisah:
    1.  `docMap`: Memetakan `docId` $\rightarrow$ `DocEntry` (menyimpan metadata dokumen dan riwayat versi).
    2.  `nameToId`: Memetakan `docName` $\rightarrow$ `docId` untuk mempercepat pencarian berdasarkan nama tanpa penelusuran linear.

### B. Penyimpanan Versi: LinkedList vs. Stack (Vector-based)
Dua struktur utama yang dibandingkan untuk melacak riwayat versi:

| Dimensi Perbandingan | Singly Linked List (`VersionLinkedList`) | Stack berbasis Array (`VersionStack` / `std::vector`) |
| :--- | :--- | :--- |
| **Tambah Versi Baru** | $\mathcal{O}(1)$ dengan menyimpan pointer `tail`. | $\mathcal{O}(1)$ *amortized* (`push_back()`). |
| **Rollback Versi** | $\mathcal{O}(N)$ karena harus mencari simpul sebelum `tail`. | $\mathcal{O}(1)$ dengan melakukan `pop_back()`. |
| **Akses Versi ke-$k$** | $\mathcal{O}(N)$ karena harus menelusuri simpul dari `head`. | $\mathcal{O}(1)$ dengan pengaksesan indeks acak `vector[k-1]`. |
| **Penggunaan Memori** | Overhead pointer `next` di setiap simpul. Alokasi dinamis berulang. | Efisien secara lokal (contiguous). Kadang menyisakan kapasitas tak terpakai akibat *doubling capacity*. |

*   **Rekomendasi Optimal**:
    *   Jika menggunakan **Linked List**, pertimbangkan menggunakan **Doubly Linked List** agar operasi *rollback* (hapus versi terakhir) dapat dioptimalkan menjadi $\mathcal{O}(1)$ dengan memanfaatkan pointer `prev` dari `tail`.
    *   Jika menggunakan **Singly Linked List**, pastikan untuk selalu mengupdate pointer `tail` saat insert agar operasi penambahan versi tetap berjalan dalam $\mathcal{O}(1)$, bukan $\mathcal{O}(N)$.

---

## 2. Struktur Kode & Domain Model yang Bersih

Pemisahan tanggung jawab (*separation of concerns*) sangat penting untuk menjaga kualitas kode dan memudahkan pengujian. Pisahkan definisi struktur data dasar dari logika manajer sistem.

### A. Model Domain (`structures.h`)
Mendefinisikan entitas data murni tanpa logika aplikasi.
```cpp
// Representasi simpul versi dokumen
struct Version {
    int versionNumber;
    string date;
    string editor;
    string content;
    Version* next; // Digunakan jika diimplementasikan sebagai Linked List

    Version(int v, string d, string e, string c)
        : versionNumber(v), date(d), editor(e), content(c), next(nullptr) {}
};

// Metadata dokumen
struct Document {
    string docId;
    string docName;
    int totalVersions;

    Document() : totalVersions(0) {}
    Document(string id, string name) : docId(id), docName(name), totalVersions(0) {}
};
```

### B. Kelas Manajer (`manager_ll.h` & `manager_stack.h`)
Kelas ini mengkoordinasikan Hash Map dan riwayat versi dokumen. Hindari pencampuran logika UI (CLI) di dalam kelas ini agar mudah diuji secara otomatis pada benchmark.
*   **Kembalikan tipe data boolean atau pointer** untuk menunjukkan status sukses/gagal suatu operasi (misalnya `bool addVersion(...)`), bukan langsung melakukan cetak `cout` ke layar di dalam fungsi inti.

---

## 3. Manajemen Memori C++ (Bebas Leak)

Karena program ini mengalokasikan data versi secara dinamis (`new Version`), penanganan memori secara manual sangatlah krusial untuk mencegah kebocoran memori (*memory leak*).

*   **Aturan Emas Destruktor (RAII)**: Setiap kelas pembungkus struktur data wajib membebaskan memori objek yang didelete pada destruktornya.
    ```cpp
    // Destruktor LinkedList wajib membersihkan semua simpul berantai
    ~VersionLinkedList() {
        Version* curr = head;
        while (curr) {
            Version* next = curr->next;
            delete curr;
            curr = next;
        }
    }

    // Destruktor Stack wajib menghapus semua pointer Version yang disimpan di vector
    ~VersionStack() {
        for (auto v : stack) {
            delete v;
        }
    }
    ```
*   **Rollback / Hapus Versi**: Saat melakukan operasi rollback, pastikan node versi yang dibuang benar-benar di-`delete` dari heap memori, bukan hanya memutus pointernya.
*   **Hindari Copying yang Tidak Perlu**: Gunakan referensi konstanta (`const string&`) saat melewatkan parameter string yang besar (seperti konten dokumen) ke dalam fungsi untuk menghemat alokasi memori sementara.

---

## 4. Pengujian dan Benchmark Performa (Target Akhir)

Untuk menghasilkan grafik analisis yang valid pada Minggu ke-14, implementasi benchmark harus dilakukan secara terstandarisasi.

### A. Metodologi Benchmark
1.  **Gunakan Timer Presisi Tinggi**: Manfaatkan pustaka `<chrono>` bawaan C++ (khususnya `high_resolution_clock`).
    ```cpp
    struct Timer {
        high_resolution_clock::time_point start;
        void reset() { start = high_resolution_clock::now(); }
        double elapsedMs() {
            auto end = high_resolution_clock::now();
            return duration<double, milli>(end - start).count();
        }
    };
    ```
2.  **Dataset Dummy Sintetis**: Buat pembuat data acak (`generateDataset`) dengan seed tetap (misalnya menggunakan `std::mt19937` dengan seed `42`) agar hasil pengujian bersifat *reproducible* (dapat diulang dengan hasil yang konsisten).
3.  **Uji dengan Berbagai Skala ($N$)**: Lakukan pengujian performa dari skala kecil hingga besar: $N = 100, 1000, 5000, 10000, 50000, 100000$.

### B. Output CSV untuk Visualisasi
Simpan hasil pengujian benchmark langsung ke file CSV dari program C++ Anda agar mudah diolah menjadi grafik menggunakan Python (Matplotlib/Seaborn) atau Microsoft Excel.
```cpp
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
}
```

---

## 5. Roadmap Pencapaian Target Progres

### 🎯 Minggu ke-7 (Progress Target)
Fokus pada fungsionalitas dasar dan pembuktian konsep awal:
*   [ ] **Spesifikasi & Skenario**: Dokumentasikan bagaimana alur kerja insert, search, dan rollback dalam file [DESCRIPTION.md](file:///Users/triwahyurubianto/Documents/Kuliah/Ilkomerz/Semester%204/Strukdat/Projek%20Strukdat/DESCRIPTION.md).
*   [ ] **Implementasi Struktur ke-1**: Selesaikan implementasi *LinkedList* lengkap dengan penambahan versi dan *rollback*.
*   [ ] **Dataset Awal**: Buat fungsi pembuat data dummy sederhana untuk pengujian fungsional.
*   [ ] **Verifikasi CLI/Unit Test**: Pastikan operasi pencarian dan rollback mengembalikan data yang benar.

### 🎯 Minggu ke-14 (Final Target)
Fokus pada optimasi, perbandingan komparatif, dan antarmuka demo:
*   [ ] **Implementasi Struktur ke-2**: Tuntaskan implementasi *Stack/Vector* dan pastikan perilakunya sama dengan sistem *LinkedList*.
*   [ ] **Automasi Benchmark**: Buat opsi kompilasi khusus (seperti flag `--benchmark`) untuk menjalankan pengujian performa tanpa interaksi pengguna.
*   [ ] **Analisis & Grafik**: Impor CSV hasil benchmark ke Python/Excel untuk membuat grafik perbandingan waktu eksekusi dan penggunaan memori.
*   [ ] **Demo Interaktif**: Sediakan menu CLI atau GUI sederhana yang intuitif bagi pengguna untuk mencoba simulasi manajemen versi dokumen secara langsung.

---

## 6. Anti-Patterns yang Harus Dihindari

*   ❌ **Pencarian Linear pada Dokumen**: Melakukan pencarian dokumen dengan melintasi seluruh koleksi dokumen satu per satu ($\mathcal{O}(N)$). Gunakan Hash Map untuk menjamin pencarian berkecepatan $\mathcal{O}(1)$.
*   ❌ **Memory Leak saat Rollback**: Menghapus versi dari riwayat namun lupa membebaskan memori simpul lama dengan perintah `delete`. Hal ini dapat menyebabkan program kehabisan memori pada dataset skala besar.
*   ❌ **Double-free Crash**: Melakukan penghapusan pointer versi yang sama di dua tempat berbeda tanpa menyetel pointer tersebut menjadi `nullptr` terlebih dahulu.
*   ❌ **Kompilasi Tanpa Optimasi**: Melakukan benchmark pada mode Debug. Selalu gunakan flag optimasi compiler `-O2` saat melakukan benchmark (`g++ -O2 main.cpp ...`) agar hasil waktu mencerminkan performa rilis yang sebenarnya.

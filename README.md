# Sistem Manajemen Versi Dokumen (Document Version Control System)
*Proyek Praktikum Struktur Data — Ilkomerz IPB University 2026*

Aplikasi Sistem Manajemen Versi Dokumen (Version Control sederhana) berbasis C++ yang dirancang untuk membandingkan kinerja efisiensi waktu eksekusi dan konsumsi memori dari empat kombinasi struktur data utama:
1. **LL_HashMap**: Singly Linked List (Riwayat Versi) + Hash Map (Indeks Pencarian)
2. **Stack_HashMap**: Array-based Stack (Riwayat Versi) + Hash Map (Indeks Pencarian)
3. **LL_BST**: Singly Linked List (Riwayat Versi) + Binary Search Tree (Indeks Pencarian)
4. **Stack_BST**: Array-based Stack (Riwayat Versi) + Binary Search Tree (Indeks Pencarian)

Sistem ini diintegrasikan dengan dataset transaksi ritel nyata skala besar (`Untitled spreadsheet - Year 2009-2010.csv` dengan ~525 ribu baris) serta dilengkapi modul benchmark performa (Waktu Eksekusi) dan memori (Theoretical Heap & OS RSS).

---

## 🚀 Fitur Utama

- **Manajemen Versi Dokumen**:
  - **Insert Dokumen**: Menambahkan dokumen baru beserta versi awal (v1).
  - **Tambah Versi**: Menumpuk versi baru pada dokumen yang sudah ada (v1 $\rightarrow$ v2 $\rightarrow$ v3).
  - **Rollback Versi**: Menghapus versi terbaru dan mengembalikan dokumen ke versi sebelumnya.
- **Indeks Pencarian Ganda**:
  - **Hash Map (Separate Chaining)**: Pencarian ID dan Nama berbiaya rata-rata $\mathcal{O}(1)$.
  - **Binary Search Tree (BST)**: Pencarian ID dan Nama berbiaya rata-rata $\mathcal{O}(\log N)$, dengan traversal data terurut alfabetis secara alami.
- **Integrasi Dataset Ritel Riil**:
  - Penguraian CSV secara dinamis:
    - *StockCode* $\rightarrow$ ID Dokumen (`docId`)
    - *Description* $\rightarrow$ Nama Dokumen (`docName`)
    - *InvoiceDate* $\rightarrow$ Tanggal Versi (`date`)
    - *Customer ID* $\rightarrow$ Penulis (`editor`)
    - *InvoiceNo + Qty + UnitPrice* $\rightarrow$ Konten Dokumen (`content`)
- **Modul Benchmark Otomatis**:
  - Membandingkan durasi operasi **INSERT**, **SEARCH**, **ROLLBACK**, dan **DELETE** untuk ukuran data $N = 100$ hingga $N = 100.000$ records.
  - Mengukur penggunaan memori teoretis (Heap) dan memori fisik ril tingkat sistem (OS RSS).
- **Serialisasi File**: Menyimpan dan memuat keadaan data ke dalam berkas teks terpisah (`data_ll.txt`, `data_stack.txt`, `data_bst_ll.txt`, `data_bst_stack.txt`).

---

## 📂 Struktur Repositori

- **[structures.h](file:///Users/triwahyurubianto/Documents/Kuliah/Ilkomerz/Semester%204/Strukdat/Projek%20Strukdat/structures.h)**: Implementasi mandiri tipe data dasar kustom (`Version`, `Document`, `VersionLinkedList`, `VersionStack`, `HashMap`, dan `BST`).
- **[manager_ll.h](file:///Users/triwahyurubianto/Documents/Kuliah/Ilkomerz/Semester%204/Strukdat/Projek%20Strukdat/manager_ll.h)**: Implementasi manajer dokumen berbasis LinkedList (`DocManagerLinkedList` & `DocManagerBSTLinkedList`).
- **[manager_stack.h](file:///Users/triwahyurubianto/Documents/Kuliah/Ilkomerz/Semester%204/Strukdat/Projek%20Strukdat/manager_stack.h)**: Implementasi manajer dokumen berbasis Stack (`DocManagerStack` & `DocManagerBSTStack`).
- **[benchmark.h](file:///Users/triwahyurubianto/Documents/Kuliah/Ilkomerz/Semester%204/Strukdat/Projek%20Strukdat/benchmark.h)**: Pengukur performa waktu dan memori (OS RSS & Heap).
- **[main.cpp](file:///Users/triwahyurubianto/Documents/Kuliah/Ilkomerz/Semester%204/Strukdat/Projek%20Strukdat/main.cpp)**: Entrypoint CLI interaktif dan mode benchmark otomatis.
- **[Makefile](file:///Users/triwahyurubianto/Documents/Kuliah/Ilkomerz/Semester%204/Strukdat/Projek%20Strukdat/Makefile)**: Otomatisasi proses build dan clean program.
- **[DESCRIPTION.md](file:///Users/triwahyurubianto/Documents/Kuliah/Ilkomerz/Semester%204/Strukdat/Projek%20Strukdat/DESCRIPTION.md)**: Analisis mendalam arsitektur sistem dan komparasi teoretis struktur data.
- **[STRUCTURE.md](file:///Users/triwahyurubianto/Documents/Kuliah/Ilkomerz/Semester%204/Strukdat/Projek%20Strukdat/STRUCTURE.md)**: Penjelasan rinci baris kode dan cara kerja seluruh fungsi program.

---

## 🛠️ Cara Mengompilasi & Menjalankan

### Persyaratan Sistem
- Compiler C++ dengan dukungan minimal standar **C++11** atau **C++17** (e.g. GCC atau Clang).
- Utilitas `make` untuk proses otomasi.

### Kompilasi Program
Jalankan perintah berikut di direktori proyek:
```bash
make clean && make
```
Ini akan menghapus sisa file kompilasi sebelumnya dan membangun executable `doc_version_system` dengan optimasi `-O2`.

### Menjalankan CLI Interaktif
Jalankan program utama secara manual:
```bash
./doc_version_system
```
Anda akan diarahkan ke antarmuka CLI berbasis menu teks untuk menguji setiap fungsi (insert, add version, rollback, load CSV, save files, dll.).

### Menjalankan Benchmark Otomatis
Untuk menjalankan pengujian performa secara otomatis pada keempat struktur data tanpa menu interaktif, jalankan:
```bash
make benchmark
```
Setelah proses selesai, hasil pengujian akan ditulis pada berkas:
- **`benchmark_results.csv`**: Kinerja waktu durasi eksekusi (INSERT, SEARCH, ROLLBACK, DELETE).
- **`benchmark_memory.csv`**: Kinerja alokasi memori (Theoretical Heap & OS RSS).

---

## 📊 Rangkuman Hasil Analisis ($N = 100.000$ records)

### Kinerja Waktu INSERT (Waktu Pemrosesan)
- **Hash Map** (`LL_HashMap` / `Stack_HashMap`): **~37 - 40 ms** (Sangat Cepat — $\mathcal{O}(1)$ average).
- **BST** (`LL_BST` / `Stack_BST`): **~4800 ms** (Lebih Lambat — $\mathcal{O}(\log N)$ average dengan overhead pembandingan leksikografis kunci string).

### Konsumsi Memori Teoretis Heap
- **BST**: Sedikit lebih besar pada volume data besar karena overhead pointer ganda (`left` dan `right` total 16 byte per simpul pada sistem 64-bit), dibandingkan Hash Map dengan separate chaining yang hanya menggunakan satu pointer chain (`next` 8 byte per simpul).
- **Stack (Vector-based)**: Menggunakan alokasi heap sedikit lebih tinggi daripada Singly Linked List karena faktor pelipatgandaan kapasitas vector (vector capacity doubling), namun memiliki performa waktu operasi rollback dan akses versi secara konstan ($\mathcal{O}(1)$).

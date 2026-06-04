# Penjelasan Struktur Data & Penjelasan Baris Kode Fungsi Program
*Sistem Manajemen Versi Dokumen Sederhana — IPB University*

Berkas ini memberikan dokumentasi lengkap mengenai setiap fungsi, struktur kelas, dan logika program yang diimplementasikan di seluruh berkas proyek. Setiap fungsi dijelaskan berdasarkan **kegunaan** (tujuan dibuatnya fungsi) dan **cara kerja** (bagaimana logika kodenya dieksekusi).

---

## 1. Daftar Berkas Proyek

Proyek ini terbagi menjadi berkas-berkas modular sebagai berikut:
- **[structures.h](file:///Users/triwahyurubianto/Documents/Kuliah/Ilkomerz/Semester%204/Strukdat/Projek%20Strukdat/structures.h)**: Defini data domain (`Version`, `Document`) dan struktur penyimpanan dasar (`VersionLinkedList`, `VersionStack`, `HashMap`).
- **[manager_ll.h](file:///Users/triwahyurubianto/Documents/Kuliah/Ilkomerz/Semester%204/Strukdat/Projek%20Strukdat/manager_ll.h)**: Logika pengelolaan versi dokumen berbasis **Singly Linked List**.
- **[manager_stack.h](file:///Users/triwahyurubianto/Documents/Kuliah/Ilkomerz/Semester%204/Strukdat/Projek%20Strukdat/manager_stack.h)**: Logika pengelolaan versi dokumen berbasis **Stack/Vector**.
- **[benchmark.h](file:///Users/triwahyurubianto/Documents/Kuliah/Ilkomerz/Semester%204/Strukdat/Projek%20Strukdat/benchmark.h)**: Pengujian kecepatan durasi eksekusi dan konsumsi memori program.
- **[main.cpp](file:///Users/triwahyurubianto/Documents/Kuliah/Ilkomerz/Semester%204/Strukdat/Projek%20Strukdat/main.cpp)**: Titik masuk utama program (CLI Interaktif & Benchmark Otomatis).
- **[Makefile](file:///Users/triwahyurubianto/Documents/Kuliah/Ilkomerz/Semester%204/Strukdat/Projek%20Strukdat/Makefile)**: Otomatisasi kompilasi dan pengujian benchmark.

---

## 2. Dokumentasi Fungsi pada [structures.h](file:///Users/triwahyurubianto/Documents/Kuliah/Ilkomerz/Semester%204/Strukdat/Projek%20Strukdat/structures.h)

### A. Utilitas Umum

#### 1. `parseCSVLine(const string& line)`
- **Kegunaan**: Memecah sebaris teks CSV menjadi elemen-elemen kolom string secara presisi.
- **Cara Kerja**: Karakter diiterasi satu per satu. Karakter tanda kutip ganda (`"`) digunakan sebagai penanda untuk mengabaikan tanda koma pemisah kolom di dalam teks kutip (quoted field). Karakter selain koma pembatas dimasukkan ke penampung string sel sementara. Setiap kali menemukan tanda koma di luar tanda kutip, string sel tersebut ditambahkan ke vektor kolom hasil penguraian.

#### 2. `getValueMemoryUsage(const ValueType& val)` (Template Generic)
- **Kegunaan**: Menaksir konsumsi memori dari tipe data generik.
- **Cara Kerja**: Mengembalikan hasil operasi operator `sizeof(val)`. Fungsi ini dioverload secara khusus untuk beberapa tipe data tertentu (`std::string` dan entri manajer dokumen) agar dapat mengukur konsumsi memori dinamis di heap.

#### 3. `getValueMemoryUsage(const string& val)` (Overload Khusus)
- **Kegunaan**: Menaksir konsumsi memori string dinamis di heap.
- **Cara Kerja**: Mengembalikan penjumlahan dari ukuran statis string (`sizeof(val)`) dengan kapasitas alokasi buffer internal string (`val.capacity()`).

---

### B. Struktur `Version` dan `Document`

#### 4. `Version::Version(...)` (Constructor)
- **Kegunaan**: Menginisialisasi objek representasi versi tunggal dari dokumen.
- **Cara Kerja**: Memetakan parameter masukan `vNum` (nomor versi), `date` (tanggal), `editor` (penulis/pengedit), dan `content` (konten versi) ke atribut internal struct. Pointer linked list `next` diatur mula-mula ke `nullptr`.

#### 5. `Version::getMemoryUsageBytes()`
- **Kegunaan**: Menghitung memori dinamis yang dipakai oleh satu objek versi dokumen.
- **Cara Kerja**: Mengembalikan total byte dari `sizeof(*this)` (ukuran struct) + kapasitas buffer alokasi string `date`, `editor`, dan `content` di heap.

#### 6. `Document::Document(...)` (Constructor)
- **Kegunaan**: Menginisialisasi objek metadata dasar dokumen.
- **Cara Kerja**: Konstruktor default menyetel `totalVersions` ke 0. Konstruktor berparameter memetakan `id` (StockCode) ke `docId` dan `name` (Description) ke `docName` serta mereset jumlah versi ke 0.

---

### C. Kelas `VersionLinkedList`

#### 7. `VersionLinkedList::VersionLinkedList()` (Constructor)
- **Kegunaan**: Membuat objek penampung versi berbasis linked list yang kosong.
- **Cara Kerja**: Pointer awal (`head`), pointer akhir (`tail`), dan ukuran list (`size`) diatur secara eksplisit ke `nullptr` dan `0`.

#### 8. `VersionLinkedList::~VersionLinkedList()` (Destructor)
- **Kegunaan**: Membersihkan memori heap yang dipesan linked list untuk mencegah kebocoran memori.
- **Cara Kerja**: List ditelusuri secara berantai dari `head`. Pointer ke node selanjutnya disimpan sementara, kemudian node saat ini dihapus menggunakan perintah `delete` hingga seluruh node terbebaskan.

#### 9. `VersionLinkedList::addVersion(...)`
- **Kegunaan**: Menyisipkan node versi dokumen baru di bagian ekor list.
- **Cara Kerja**: Node `Version` baru dialokasikan di heap. Jika list kosong (`tail` bernilai `nullptr`), setel `head` dan `tail` menunjuk ke node baru tersebut. Jika tidak, setel pointer `next` dari `tail` saat ini ke node baru, kemudian geser pointer `tail` utama ke node baru tersebut. Jumlah ukuran `size` dinaikkan 1. Kompleksitas operasi: $\mathcal{O}(1)$.

#### 10. `VersionLinkedList::rollback()`
- **Kegunaan**: Menghapus versi dokumen terbaru (paling akhir).
- **Cara Kerja**: Jika list kosong, langsung kembali. Jika list hanya berisi satu node (`head == tail`), hapus node tersebut, setel `head` dan `tail` ke `nullptr`, lalu kurangi `size`. Jika list memiliki lebih dari satu node, lakukan loop dari `head` untuk mencari node sebelum `tail`. Setelah ditemukan, hapus node `tail` lama, atur pointer `next` dari node sebelum `tail` tersebut ke `nullptr`, dan setel node tersebut sebagai `tail` yang baru. Kompleksitas operasi: $\mathcal{O}(N)$.

#### 11. `VersionLinkedList::getVersion(int k)`
- **Kegunaan**: Mengakses pointer node versi dokumen ke-$k$.
- **Cara Kerja**: Penelusuran berantai dimulai dari `head` ke pointer `next` sebanyak $k-1$ kali. Mengembalikan pointer target jika indeks berada dalam rentang valid, atau `nullptr` jika di luar batas list. Kompleksitas operasi: $\mathcal{O}(N)$.

#### 12. `VersionLinkedList::printAll()`
- **Kegunaan**: Menampilkan riwayat seluruh versi dokumen ke layar CLI.
- **Cara Kerja**: Melakukan iterasi dari `head` ke `tail`. Di setiap iterasi, mencetak nomor versi, tanggal update, editor, dan cuplikan konten dokumen (40 karakter pertama).

#### 13. `VersionLinkedList::getMemoryUsageBytes()`
- **Kegunaan**: Menghitung total konsumsi memori seluruh isi linked list riwayat versi.
- **Cara Kerja**: Mengembalikan ukuran struktur dasar list (`sizeof(*this)`) ditambah dengan memori dinamis dari masing-masing node `Version` hasil iterasi dari `head` ke `tail`.

---

### D. Kelas `VersionStack`

#### 14. `VersionStack::VersionStack()` (Constructor)
- **Kegunaan**: Membuat objek penampung stack kosong.
- **Cara Kerja**: Menginisialisasi internal vector dari standard library `std::vector<Version*>` sebagai basis penyimpanan stack.

#### 15. `VersionStack::~VersionStack()` (Destructor)
- **Kegunaan**: Membebaskan memori dinamis seluruh node versi di heap.
- **Cara Kerja**: Seluruh pointer `Version*` yang disimpan di dalam vector dihapus satu per satu menggunakan loop dan perintah `delete`.

#### 16. `VersionStack::addVersion(...)`
- **Kegunaan**: Memasukkan versi baru ke posisi paling atas (top) stack.
- **Cara Kerja**: Mengalokasikan objek `Version` baru di heap, lalu menambahkannya ke ujung belakang vector menggunakan metode `push_back()`. Kompleksitas operasi: $\mathcal{O}(1)$ amortized.

#### 17. `VersionStack::rollback()`
- **Kegunaan**: Menghapus versi paling baru di atas stack.
- **Cara Kerja**: Jika stack kosong, fungsi langsung kembali. Jika tidak, hapus objek versi di ujung belakang vector dengan perintah `delete stack.back()` lalu potong ukuran vector dengan memanggil `stack.pop_back()`. Kompleksitas operasi: $\mathcal{O}(1)$.

#### 18. `VersionStack::getVersion(int k)`
- **Kegunaan**: Mengakses objek versi ke-$k$.
- **Cara Kerja**: Memanfaatkan keunggulan penyimpanan memori kontigu pada vector untuk langsung melakukan pengalamatan indeks array `stack[k - 1]` tanpa iterasi penelusuran. Kompleksitas operasi: $\mathcal{O}(1)$.

#### 19. `VersionStack::size()`
- **Kegunaan**: Mengembalikan jumlah total versi yang ada di dalam stack.
- **Cara Kerja**: Mengembalikan hasil fungsi `stack.size()` dari internal vector.

#### 20. `VersionStack::printAll()`
- **Kegunaan**: Menampilkan riwayat seluruh versi di stack ke layar.
- **Cara Kerja**: Melakukan iterasi berurutan dari elemen pertama hingga terakhir pada vector dan menampilkan informasi masing-masing versi secara ringkas.

#### 21. `VersionStack::getMemoryUsageBytes()`
- **Kegunaan**: Menghitung total konsumsi memori stack versi.
- **Cara Kerja**: Mengembalikan penjumlahan dari ukuran kelas (`sizeof(*this)`) + memori alokasi kapasitas internal vector (`stack.capacity() * sizeof(Version*)`) + memori dinamis dari setiap node `Version` di dalam stack.

---

### E. Kelas `HashMap`

#### 22. `HashNode::HashNode(...)` (Constructor)
- **Kegunaan**: Membuat simpul bucket baru dalam separate chaining.
- **Cara Kerja**: Mengisi kunci `key`, nilai `value`, dan menginisialisasi pointer rantai tabrakan `next` ke `nullptr`.

#### 23. `HashNode::getMemoryUsageBytes()`
- **Kegunaan**: Menghitung penggunaan memori satu simpul bucket hash.
- **Cara Kerja**: Mengembalikan total byte dari `sizeof(*this)` + kapasitas string `key` + ukuran memori dari `value` (ditaksir lewat template helper `getValueMemoryUsage`).

#### 24. `HashMap::HashMap()` (Constructor)
- **Kegunaan**: Membuat objek tabel hash kosong.
- **Cara Kerja**: Mengisi seluruh sel pointer array `table` dari indeks $0$ sampai `TABLE_SIZE - 1` dengan `nullptr`.

#### 25. `HashMap::~HashMap()` (Destructor)
- **Kegunaan**: Mengosongkan map dan membebaskan memori seluruh bucket separate chaining.
- **Cara Kerja**: Iterasi dilakukan pada setiap bucket array. Di setiap bucket, linked list rantai tabrakan ditelusuri dan dibebaskan satu per satu menggunakan loop `delete`.

#### 26. `HashMap::hash(const string& key)` (Private)
- **Kegunaan**: Menghitung nilai hash numerik dari kunci string untuk menentukan indeks bucket.
- **Cara Kerja**: Menggunakan algoritma hash polinomial di mana variabel penampung hash dikalikan dengan konstanta prima $31$ dan ditambahkan dengan nilai ASCII karakter string secara iteratif, kemudian hasilnya dimodulo dengan `TABLE_SIZE` (1024).

#### 27. `HashMap::insert(const string& key, T value)`
- **Kegunaan**: Memasukkan data baru ke dalam tabel hash.
- **Cara Kerja**: Hitung indeks menggunakan `hash(key)`. Jika pointer bucket pada indeks tersebut tidak `nullptr`, naikkan nilai counter `collisions`. Alokasikan `HashNode` baru di heap, setel pointer `next` dari node baru tersebut ke pointer awal bucket saat ini, lalu setel pointer awal bucket ke node baru tersebut (head insertion). Kompleksitas operasi: $\mathcal{O}(1)$.

#### 28. `HashMap::search(const string& key)`
- **Kegunaan**: Mencari nilai data yang diasosiasikan dengan kunci tertentu.
- **Cara Kerja**: Dapatkan indeks bucket lewat `hash(key)`. Telusuri rantai linked list di indeks tersebut karakter per karakter untuk mencocokkan `key`. Jika ditemukan, kembalikan pointer alamat memori dari `value` tersebut. Jika tidak ditemukan hingga akhir list, kembalikan `nullptr`. Kompleksitas operasi: $\mathcal{O}(1)$ average.

#### 29. `HashMap::remove(const string& key)`
- **Kegunaan**: Menghapus pasangan data key-value dari tabel hash.
- **Cara Kerja**: Dapatkan indeks bucket target. Lakukan penelusuran rantai linked list dengan melacak pointer node saat ini dan node sebelumnya. Jika ditemukan node dengan kunci yang cocok, ubah pointer link pada node sebelumnya untuk melompati node target (atau setel bucket awal jika node target berada di depan), hapus node target dengan `delete`, dan kembalikan `true`. Kembalikan `false` jika tidak ditemukan. Kompleksitas operasi: $\mathcal{O}(1)$ average.

#### 30. `HashMap::getAllKeys()`
- **Kegunaan**: Mengumpulkan seluruh kunci string yang terdaftar di tabel hash.
- **Cara Kerja**: Melakukan iterasi dari indeks bucket $0$ hingga $1023$. Di setiap bucket, telusuri seluruh node rantai tabrakan dan masukkan kunci stringnya ke dalam vector hasil.

#### 31. `HashMap::getMemoryUsageBytes()`
- **Kegunaan**: Menghitung konsumsi memori internal dan eksternal dinamis dari tabel hash.
- **Cara Kerja**: Menjumlahkan ukuran statis tabel hash (`sizeof(*this)`) dengan total konsumsi memori dari seluruh `HashNode` di dalam bucket chaining.

---

### F. Struktur `BSTNode` dan Kelas `BST`

#### 32. `BSTNode::BSTNode(...)` (Constructor)
- **Kegunaan**: Menginisialisasi simpul (node) baru pada pohon biner BST.
- **Cara Kerja**: Memetakan kunci `key` dan nilai `value` ke atribut internal node, serta mengatur pointer anak kiri (`left`) dan anak kanan (`right`) ke `nullptr`.

#### 33. `BSTNode::getMemoryUsageBytes()`
- **Kegunaan**: Menghitung total memori dinamis yang digunakan oleh simpul ini dan seluruh sub-pohon di bawahnya.
- **Cara Kerja**: Menjumlahkan ukuran statis node (`sizeof(*this)`) dengan ukuran memori kunci `key` (lewat `getValueMemoryUsage`), ukuran memori nilai `value` (lewat `getValueMemoryUsage`), dan secara rekursif memanggil `getMemoryUsageBytes()` untuk anak kiri (`left`) dan anak kanan (`right`) jika tidak bernilai `nullptr`.

#### 34. `BST::BST()` (Constructor)
- **Kegunaan**: Membuat objek pohon pencarian biner (BST) baru yang kosong.
- **Cara Kerja**: Mengatur pointer akar utama (`root`) ke `nullptr`.

#### 35. `BST::~BST()` (Destructor)
- **Kegunaan**: Membersihkan seluruh memori dinamis yang dialokasikan pohon di heap.
- **Cara Kerja**: Memanggil helper privat `destroy(root)` untuk melakukan penghapusan seluruh node secara rekursif menggunakan post-order traversal.

#### 36. `BST::destroy(BSTNode<K, V>* node)` (Private Helper)
- **Kegunaan**: Membebaskan memori dari sub-pohon yang berakar pada `node`.
- **Cara Kerja**: Menggunakan rekursi: hapus sub-pohon kiri, hapus sub-pohon kanan, lalu lakukan operasi `delete node` pada node itu sendiri.

#### 37. `BST::insertNode(...)` (Private Helper)
- **Kegunaan**: Menyisipkan pasangan key-value baru ke dalam sub-pohon secara rekursif.
- **Cara Kerja**: Jika sub-pohon kosong (`node == nullptr`), buat dan kembalikan `BSTNode` baru. Jika kunci baru lebih kecil dari kunci node saat ini, sisipkan secara rekursif ke sub-pohon kiri. Jika kunci lebih besar, sisipkan ke sub-pohon kanan. Jika kunci sama, perbarui nilai node tersebut dengan nilai baru. Kembalikan pointer `node` saat ini setelah pembaruan pointer anak.

#### 38. `BST::insert(const K& key, const V& value)`
- **Kegunaan**: Memasukkan data baru ke dalam pohon pencarian biner.
- **Cara Kerja**: Memanggil helper privat `insertNode` dengan argumen `root` utama pohon, kunci `key`, dan nilai `value`, lalu menyimpan kembali pointer root baru yang dikembalikan. Kompleksitas operasi: $\mathcal{O}(\log N)$ rata-rata.

#### 39. `BST::searchNode(...)` (Private Helper)
- **Kegunaan**: Mencari simpul dengan kunci tertentu secara rekursif.
- **Cara Kerja**: Jika sub-pohon kosong atau kunci node cocok dengan kunci pencarian, kembalikan pointer node saat ini. Jika kunci pencarian lebih kecil, lakukan pencarian secara rekursif pada sub-pohon kiri. Jika lebih besar, lakukan pencarian rekursif pada sub-pohon kanan.

#### 40. `BST::search(const K& key)`
- **Kegunaan**: Mencari nilai data yang diasosiasikan dengan kunci tertentu dalam pohon.
- **Cara Kerja**: Memanggil helper privat `searchNode` dengan kunci target. Jika ditemukan node yang tidak `nullptr`, kembalikan alamat memori dari nilai data (`&node->value`). Jika tidak ditemukan, kembalikan `nullptr`. Kompleksitas operasi: $\mathcal{O}(\log N)$ rata-rata.

#### 41. `BST::findMin(BSTNode<K, V>* node)` (Private Helper)
- **Kegunaan**: Menemukan simpul dengan kunci terkecil (in-order successor) dalam sub-pohon.
- **Cara Kerja**: Melakukan iterasi ke anak kiri (`left`) secara terus-menerus hingga tidak ada anak kiri lagi, lalu mengembalikan node paling kiri tersebut.

#### 42. `BST::removeNode(...)` (Private Helper)
- **Kegunaan**: Menghapus simpul dengan kunci tertentu dari sub-pohon secara rekursif.
- **Cara Kerja**:
  - Jika sub-pohon kosong, setel indikator keberhasilan `success` ke `false` dan kembalikan `nullptr`.
  - Jika kunci target lebih kecil dari kunci node, panggil rekursif pada sub-pohon kiri.
  - Jika kunci target lebih besar, panggil rekursif pada sub-pohon kanan.
  - Jika kunci cocok, lakukan penghapusan berdasarkan kasus anak:
    - Jika node tidak memiliki anak kiri, simpan pointer anak kanan, hapus node saat ini, lalu kembalikan pointer anak kanan tersebut.
    - Jika node tidak memiliki anak kanan, simpan pointer anak kiri, hapus node saat ini, lalu kembalikan pointer anak kiri tersebut.
    - Jika node memiliki dua anak, cari successor in-order terkecilnya (`findMin` dari sub-pohon kanan), salin kunci dan nilainya ke node saat ini, kemudian hapus successor tersebut secara rekursif dari sub-pohon kanan.
  Kembalikan pointer node saat ini. Setel `success` ke `true` jika terjadi kecocokan kunci.

#### 43. `BST::remove(const K& key)`
- **Kegunaan**: Menghapus pasangan key-value berdasarkan kunci dari pohon.
- **Cara Kerja**: Memanggil helper privat `removeNode` dengan `root` utama, kunci target, dan variabel penampung status sukses `success`. Mengembalikan status sukses tersebut. Kompleksitas operasi: $\mathcal{O}(\log N)$ rata-rata.

#### 44. `BST::collectKeys(...)` (Private Helper)
- **Kegunaan**: Mengumpulkan seluruh kunci di pohon secara terurut leksikografis menggunakan in-order traversal.
- **Cara Kerja**: Jika sub-pohon tidak kosong, lakukan rekursi pada anak kiri (`left`), masukkan kunci node saat ini ke dalam vektor hasil, lalu lakukan rekursi pada anak kanan (`right`).

#### 45. `BST::getAllKeys()`
- **Kegunaan**: Mengembalikan semua kunci di pohon dalam bentuk vektor terurut secara alfabetis/leksikografis.
- **Cara Kerja**: Menginisialisasi vektor kosong, memanggil helper privat `collectKeys` dari `root`, lalu mengembalikan vektor tersebut. Kompleksitas operasi: $\mathcal{O}(N)$.

#### 46. `BST::getMemoryUsageBytes()`
- **Kegunaan**: Menghitung total konsumsi memori seluruh pohon BST.
- **Cara Kerja**: Mengembalikan ukuran objek BST (`sizeof(*this)`) ditambah dengan memori seluruh node secara rekursif lewat pemanggilan `root->getMemoryUsageBytes()` jika root tidak kosong.

---

## 3. Dokumentasi Fungsi pada [manager_ll.h](file:///Users/triwahyurubianto/Documents/Kuliah/Ilkomerz/Semester%204/Strukdat/Projek%20Strukdat/manager_ll.h)

#### 32. `DocEntryLL::DocEntryLL(...)` (Constructors)
- **Kegunaan**: Membuat objek entri dokumen baru untuk dikelola LinkedList.
- **Cara Kerja**: Konstruktor default menyetel pointer `versions` ke `nullptr`. Konstruktor berparameter mengisi objek metadata `Document` dan mengalokasikan objek linked list versi `VersionLinkedList` baru di heap.

#### 33. `DocEntryLL::getMemoryUsageBytes()`
- **Kegunaan**: Menghitung memori dinamis yang terpakai oleh entri dokumen linked list.
- **Cara Kerja**: Mengembalikan ukuran `sizeof(*this)` + kapasitas alokasi string ID dokumen & Nama dokumen + total memori dinamis dari objek `versions`.

#### 34. `getValueMemoryUsage(const DocEntryLL& val)` (Overload Khusus)
- **Kegunaan**: Menghubungkan fungsi helper penaksir memori dengan objek `DocEntryLL`.
- **Cara Kerja**: Memanggil dan mengembalikan hasil dari `val.getMemoryUsageBytes()`.

#### 35. `DocManagerLinkedList::insertDocument(...)`
- **Kegunaan**: Mendaftarkan dokumen baru ke dalam sistem.
- **Cara Kerja**: Periksa apakah ID dokumen sudah ada di `docMap`. Jika ada, kembalikan `false`. Jika tidak, buat objek `DocEntryLL` baru, daftarkan data versi awal (v1) ke dalamnya, masukkan entri dokumen ke `docMap` (berdasarkan ID), dan daftarkan relasi nama dokumen ke ID dokumen di `nameToId`. Kembalikan `true`.

#### 36. `DocManagerLinkedList::addVersion(...)`
- **Kegunaan**: Menambahkan versi baru pada dokumen yang sudah ada.
- **Cara Kerja**: Cari dokumen target di `docMap` berdasarkan ID. Jika tidak ada, kembalikan `false`. Jika ada, naikkan atribut `totalVersions` dokumen, lalu panggil metode `addVersion` pada linked list versi dokumen tersebut dengan isi data versi baru. Kembalikan `true`.

#### 37. `DocManagerLinkedList::searchById(...)`
- **Kegunaan**: Mengambil pointer entri dokumen berdasarkan ID.
- **Cara Kerja**: Memanggil dan mengembalikan hasil pencarian kunci ID pada `docMap`.

#### 38. `DocManagerLinkedList::searchByName(...)`
- **Kegunaan**: Mengambil pointer entri dokumen berdasarkan Nama dokumen.
- **Cara Kerja**: Cari ID dokumen di `nameToId` menggunakan parameter nama. Jika ID ditemukan, cari dan kembalikan pointer entri dokumen di `docMap` menggunakan ID tersebut. Jika tidak ditemukan, kembalikan `nullptr`.

#### 39. `DocManagerLinkedList::rollback(...)`
- **Kegunaan**: Menghapus versi terbaru dokumen terdaftar.
- **Cara Kerja**: Cari dokumen di `docMap`. Jika dokumen ditemukan dan memiliki jumlah versi lebih dari 1, panggil metode `rollback()` pada linked list versinya dan kurangi counter `totalVersions` pada dokumen tersebut. Kembalikan `true`. Jika gagal, kembalikan `false`.

#### 40. `DocManagerLinkedList::listAll()`
- **Kegunaan**: Menampilkan seluruh daftar dokumen yang dikelola ke layar.
- **Cara Kerja**: Ambil semua kunci ID dari `docMap`, cari entri masing-masing dokumen secara iteratif, lalu tampilkan ID, nama dokumen, dan total versi ke layar.

#### 41. `DocManagerLinkedList::saveToFile(...)`
- **Kegunaan**: Melakukan serialisasi data dokumen dan riwayat versinya ke file teks.
- **Cara Kerja**: Membuka file output teks. Iterasi seluruh kunci ID dokumen dari `docMap`, telusuri secara linear seluruh versi dokumen dari tertua ke terbaru, dan tulis informasi tersebut baris demi baris menggunakan separator pipa (`docId|docName|vNum|date|editor|content`).

#### 42. `DocManagerLinkedList::loadFromFile(...)`
- **Kegunaan**: Membaca kembali data hasil serialisasi dari file teks ke memori program.
- **Cara Kerja**: Membuka file input teks. Baris demi baris dibaca, lalu dipecah menggunakan `stringstream` berdasarkan pembatas pipa (`|`). Jika dokumen belum pernah dimasukkan ke memori (diperiksa via `docMap`), panggil `insertDocument()`. Jika sudah ada, panggil `addVersion()` untuk merekonstruksi riwayat versi.

#### 43. `DocManagerLinkedList::importFromCSV(...)`
- **Kegunaan**: Mengimpor data transaksi ritel skala besar dari file CSV ke manajer dokumen.
- **Cara Kerja**: Membuka file CSV. Melewati baris pertama (header). Baris-baris berikutnya dibaca satu per satu hingga mencapai batasan `maxRows` (jika diatur). Setiap baris diurai menggunakan helper `parseCSVLine` untuk mendapatkan data kolom. Kolom dipetakan sesuai aturan: StockCode $\rightarrow$ ID Dokumen, Description $\rightarrow$ Nama Dokumen, InvoiceDate $\rightarrow$ Tanggal Versi, Customer ID $\rightarrow$ Editor, dan gabungan InvoiceNo+Quantity+UnitPrice menjadi Konten. Jika dokumen belum terdaftar, panggil `insertDocument()`, jika sudah ada panggil `addVersion()`.

#### 44. `DocManagerLinkedList::getMemoryUsageBytes()`
- **Kegunaan**: Menghitung total konsumsi memori seluruh manajer dokumen berbasis LinkedList.
- **Cara Kerja**: Mengembalikan ukuran objek manajer (`sizeof(*this)`) ditambah dengan total penggunaan memori dinamis dari peta indeks `docMap` dan `nameToId`.

#### 44a. `DocManagerBSTLinkedList` Methods
(`insertDocument`, `addVersion`, `searchById`, `searchByName`, `rollback`, `listAll`, `saveToFile`, `loadFromFile`, `importFromCSV`, `getMemoryUsageBytes`)
- **Kegunaan & Cara Kerja**: Alur logika, parameter input, dan kegunaan fungsi-fungsi ini identik dengan metode-metode pada kelas `DocManagerLinkedList`. Perbedaan utamanya terletak pada penggunaan struktur data `BST` sebagai pengganti `HashMap` untuk memetakan ID dokumen ke entri dokumen (`docMap`) dan nama dokumen ke ID dokumen (`nameToId`). Operasi pencarian, penyisipan, dan penghapusan bernilai $\mathcal{O}(\log N)$ rata-rata (alih-alih $\mathcal{O}(1)$ rata-rata pada Hash Map). Selain itu, fungsi `listAll()` menampilkan daftar dokumen terurut secara alfabetis berdasarkan ID-nya berkat in-order traversal dari BST.

---

## 4. Dokumentasi Fungsi pada [manager_stack.h](file:///Users/triwahyurubianto/Documents/Kuliah/Ilkomerz/Semester%204/Strukdat/Projek%20Strukdat/manager_stack.h)

#### 45. `DocEntryStack::DocEntryStack(...)` (Constructors)
- **Kegunaan**: Membuat objek entri dokumen baru untuk dikelola Stack/Vector.
- **Cara Kerja**: Sama dengan `DocEntryLL`, namun mengalokasikan objek stack versi `VersionStack` baru di heap.

#### 46. `DocEntryStack::getMemoryUsageBytes()`
- **Kegunaan**: Menghitung memori dinamis yang terpakai oleh entri dokumen stack.
- **Cara Kerja**: Mengembalikan ukuran `sizeof(*this)` + kapasitas alokasi string ID dokumen & Nama dokumen + total memori dinamis dari objek stack versi `versions`.

#### 47. `getValueMemoryUsage(const DocEntryStack& val)` (Overload Khusus)
- **Kegunaan**: Menghubungkan fungsi helper penaksir memori dengan objek `DocEntryStack`.
- **Cara Kerja**: Memanggil dan mengembalikan hasil dari `val.getMemoryUsageBytes()`.

#### 48. `DocManagerStack` Methods
(`insertDocument`, `addVersion`, `searchById`, `searchByName`, `rollback`, `listAll`, `saveToFile`, `loadFromFile`, `importFromCSV`, `getMemoryUsageBytes`)
- **Kegunaan & Cara Kerja**: Alur logika, parameter input, dan kegunaan fungsi-fungsi ini identik dengan metode-metode pada kelas `DocManagerLinkedList`. Perbedaan utamanya terletak pada operasi rollback versi (`rollback()`) yang memanggil metode rollback milik `VersionStack` (operasi vector `pop_back()` berbiaya konstan $\mathcal{O}(1)$) dan traversal penyimpanan data ke file yang mengiterasi element vector secara kontigu alih-alih linked list.

#### 48a. `DocManagerBSTStack` Methods
(`insertDocument`, `addVersion`, `searchById`, `searchByName`, `rollback`, `listAll`, `saveToFile`, `loadFromFile`, `importFromCSV`, `getMemoryUsageBytes`)
- **Kegunaan & Cara Kerja**: Alur logika, parameter input, dan kegunaan fungsi-fungsi ini identik dengan metode-metode pada kelas `DocManagerStack`. Perbedaan utamanya terletak pada penggunaan `BST` sebagai pengganti `HashMap` untuk mengindeks dokumen. Operasi pencarian, penyisipan, dan penghapusan bernilai $\mathcal{O}(\log N)$ rata-rata. Fungsi `listAll()` menampilkan daftar dokumen terurut secara alfabetis berdasarkan ID-nya berkat traversal in-order dari BST.

---

## 5. Dokumentasi Fungsi pada [benchmark.h](file:///Users/triwahyurubianto/Documents/Kuliah/Ilkomerz/Semester%204/Strukdat/Projek%20Strukdat/benchmark.h)

#### 49. `Timer::reset()`
- **Kegunaan**: Menandai waktu mulai (timestamp awal) untuk penghitungan durasi operasi.
- **Cara Kerja**: Mengisi variabel `start` dengan waktu saat ini menggunakan clock resolusi tinggi dari standard library `<chrono>`.

#### 50. `Timer::elapsedMs()`
- **Kegunaan**: Mendapatkan total waktu berjalan dalam satuan milidetik.
- **Cara Kerja**: Mengambil waktu saat ini, menghitung selisih durasi terhadap waktu `start` dalam bentuk pecahan milidetik, lalu mengembalikannya sebagai tipe data `double`.

#### 51. `randStr(int len, mt19937& rng)`
- **Kegunaan**: Membuat string acak dengan panjang tertentu untuk data simulasi.
- **Cara Kerja**: Melakukan iterasi sebanyak `len` kali untuk memilih karakter secara acak dari kumpulan karakter alfanumerik berdasarkan generator bilangan acak `rng`.

#### 52. `generateDataset(int n, mt19937& rng)`
- **Kegunaan**: Membuat dataset transaksi sintetis untuk disimulasikan sebagai log input benchmark.
- **Cara Kerja**: Menentukan jumlah dokumen unik ($N/5$). Membuat daftar ID dokumen acak dan nama dokumen acak. Lalu, melakukan perulangan sebanyak $N$ kali untuk membangkitkan data tanggal, editor, dan konten acak, dan menyimpannya ke dalam vector `SyntheticRecord`.

#### 53. `runBenchmark(const vector<int>& sizes)`
- **Kegunaan**: Menjalankan rangkaian benchmark performa waktu eksekusi untuk membandingkan HashMap vs BST dan LinkedList vs Stack.
- **Cara Kerja**: Program mengiterasi setiap ukuran data $N$ yang ditentukan. Di setiap ukuran data, program menginisialisasi keempat manajer dokumen (`DocManagerLinkedList`, `DocManagerStack`, `DocManagerBSTLinkedList`, `DocManagerBSTStack`), membangkitkan dataset sintetis, lalu mengukur durasi waktu pengujian untuk operasi penambahan versi baru (**INSERT**), pencarian data (**SEARCH**), pembatalan versi (**ROLLBACK**), dan penghapusan dokumen (**DELETE**) pada masing-masing sistem menggunakan objek `Timer`. Hasil pengujian disimpan ke dalam penampung hasil benchmark.

#### 54. `saveBenchmarkCSV(...)`
- **Kegunaan**: Menyimpan laporan hasil benchmark performa waktu keempat sistem ke berkas CSV.
- **Cara Kerja**: Membuka berkas target `benchmark_results.csv`, menulis header kolom data (`Operation,DataSize,LL_HashMap_ms,Stack_HashMap_ms,LL_BST_ms,Stack_BST_ms`), lalu secara berurutan menuliskan metrik hasil benchmark per baris data dan menutup berkas.

#### 55. `getOSMemoryUsage()`
- **Kegunaan**: Menghitung total alokasi memori fisik ril (Resident Set Size) dari proses saat ini.
- **Cara Kerja**: Menggunakan kode terkompilasi bersyarat (`#ifdef`):
  - Pada macOS: Memanggil API Mach kernel `task_info` untuk mengambil struktur data `mach_task_basic_info`.
  - Pada Windows: Menggunakan API PSAPI `GetProcessMemoryInfo` untuk membaca `WorkingSetSize`.
  - Pada Linux: Membaca dan memparsing data penggunaan halaman memori dari file `/proc/self/statm` dan mengalikannya dengan ukuran halaman sistem (`sysconf`).

#### 56. `runMemoryBenchmark(const vector<int>& sizes)`
- **Kegunaan**: Menjalankan rangkaian benchmark konsumsi memori keempat sistem (HashMap vs BST dan LinkedList vs Stack).
- **Cara Kerja**: Mengiterasi setiap ukuran data $N$. Untuk masing-masing dari keempat struktur data, program mengukur memori awal OS sebelum alokasi, mengalokasikan manajer secara dinamis di heap, memuat dataset transaksi ritel ke manajer, mengukur memori akhir OS, menghitung delta OS (RSS), mengambil estimasi memori heap teoretis dari manajer (menggunakan fungsi rekursif `getMemoryUsageBytes()`), dan kemudian segera menghapus manajer untuk membebaskan heap sebelum beralih ke struktur berikutnya.

#### 57. `saveMemoryBenchmarkCSV(...)`
- **Kegunaan**: Mengekspor laporan hasil benchmark memori keempat kombinasi sistem ke berkas CSV `benchmark_memory.csv`.
- **Cara Kerja**: Membuka berkas CSV target, menuliskan header baris, lalu menuliskan hasil ukuran memori teoretis dan OS RSS untuk masing-masing ukuran data $N$ dalam satuan KB untuk keempat sistem, dan menutup berkas.

---

## 6. Dokumentasi Fungsi pada [main.cpp](file:///Users/triwahyurubianto/Documents/Kuliah/Ilkomerz/Semester%204/Strukdat/Projek%20Strukdat/main.cpp)

#### 58. `clearScreen()`
- **Kegunaan**: Membersihkan terminal dari teks keluaran sebelumnya untuk kerapian antarmuka.
- **Cara Kerja**: Menggunakan instruksi pra-prosesor `#ifdef` untuk mendeteksi sistem operasi Windows (`system("cls")`) atau sistem operasi lainnya seperti macOS/Linux (`system("clear")`).

#### 59. `printHeader()`
- **Kegunaan**: Menampilkan identitas program aplikasi pada layar CLI.
- **Cara Kerja**: Mencetak kotak bingkai teks dekoratif sistem menggunakan karakter blok unicode lewat aliran `std::cout`.

#### 60. `menuCLI()`
- **Kegunaan**: Menyediakan antarmuka interaktif CLI berbasis teks untuk berinteraksi dengan pengguna.
- **Cara Kerja**: Keempat objek manajer dokumen (`sysLL`, `sysStack`, `sysBSTLL`, `sysBSTStack`) dideklarasikan dan data yang tersimpan sebelumnya dari masing-masing file teks (`data_ll.txt`, `data_stack.txt`, `data_bst_ll.txt`, `data_bst_stack.txt`) dimuat otomatis. Perulangan `while(true)` dijalankan untuk menampilkan pilihan menu program dari 1 hingga 12 dan masukan 0 untuk keluar. Berdasarkan input pilihan pengguna, program meminta input tambahan yang sesuai (seperti ID dokumen, tanggal, dll.) dan memanggil metode manajer yang bersesuaian pada keempat manajer tersebut secara paralel atau terintegrasi. Saat memilih keluar (`0`), seluruh basis data di memori disimpan kembali ke file teks secara otomatis sebelum keluar.

#### 61. `main(int argc, char* argv[])`
- **Kegunaan**: Titik masuk utama program C++.
- **Cara Kerja**: Menganalisis parameter argumen baris perintah (`argv`). Jika dijalankan dengan flag `--benchmark` (misalnya saat dijalankan lewat perintah terminal `make benchmark`), program melewati antarmuka CLI interaktif dan langsung memicu eksekusi benchmark performa waktu dan memori untuk keempat kombinasi secara otomatis, menulis berkas CSV hasil benchmark, lalu langsung keluar. Jika tidak ada argumen, program akan meluncurkan `menuCLI()`.

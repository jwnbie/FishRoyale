# Fish Royale: A Technical Implementation of C++ Object-Oriented Programming

Dokumentasi ini menyajikan rincian teknis mengenai proyek Fish Royale, sebuah aplikasi permainan survival 2D berbasis Windows yang dikembangkan menggunakan bahasa pemrograman C++ dan library grafis WinBGIm. Fokus utama dari proyek ini adalah demonstrasi arsitektur perangkat lunak berbasis objek dalam simulasi interaksi entitas secara real-time.

---

## 1. Arsitektur Perangkat Lunak

Proyek ini menerapkan prinsip-prinsip Pemrograman Berorientasi Objek (OOP) untuk memastikan modularitas dan skalabilitas kode:

### A. Hirarki Kelas
* **Base Class (`Fish`)**: Mendefinisikan atribut fundamental seperti koordinat spasial, dimensi entitas, status persistensi, serta manajemen alokasi memori untuk aset grafis (bitmap).
* **Derived Classes (`Player`, `SmallFish`, `Shark`)**: Mengimplementasikan spesialisasi perilaku. Kelas `Player` mencakup logika kontrol input, sementara `SmallFish` dan `Shark` menggunakan algoritma pergerakan otomatis dan sistem respawn terprogram.

### B. Prinsip Desain
* **Polimorfisme**: Menggunakan fungsi virtual untuk metode `draw()` dan `update()`, memungkinkan eksekusi perilaku spesifik entitas dalam struktur loop yang terpadu.
* **Encapsulation**: Enkapsulasi data status entitas di dalam objek untuk menjaga integritas logika permainan.
* **Collision Detection**: Implementasi deteksi tabrakan menggunakan kalkulasi jarak Euclidean guna menentukan interaksi antar entitas secara akurat.

---

## 2. Fitur Teknis Utama

* **Double Buffering**: Penggunaan mekanisme `setactivepage` dan `setvisualpage` untuk eliminasi efek flickering pada rendering grafis.
* **Manajemen Aset**: Integrasi API Windows (`mmsystem.h`) untuk pemrosesan audio asinkron dan manipulasi bitmap menggunakan teknik masking (AND/XOR put) untuk transparansi sprite.
* **Sistem Koordinat Dinamis**: Logika pembatasan area gerak (clamping) untuk memastikan entitas tetap berada dalam batas viewport yang ditentukan.

---

## 3. Instruksi Instalasi dan Kompilasi

### Prasyarat Sistem
* Compiler: MinGW (GCC/G++) versi 32-bit (direkomendasikan untuk kompatibilitas WinBGIm).
* Library: Header `graphics.h` dan objek binary `libbgi.a` harus terintegrasi pada direktori compiler.

### Prosedur Kompilasi
Eksekusi perintah berikut melalui terminal untuk melakukan linking terhadap dependensi sistem Windows yang diperlukan:

```bash
g++ main.cpp -o FishRoyale -lbgi -lgdi32 -lcomdlg32 -luuid -loleaut32 -lole32 -lwinmm

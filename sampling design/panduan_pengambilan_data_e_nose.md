# 📋 Panduan Pengambilan Data e-Nose

> ⚠️ **ATURAN EMAS (GOLDEN RULES)**
> 1. **Jangan campur gas:** Selesaikan 1 jenis gas (misal: Alkohol) untuk beberapa kali ulangan, baru pindah ke gas lain (misal: Knalpot).
> 2. **Kondisikan Ruangan:** Lakukan semua sampling di dalam satu ruangan ber-AC yang suhunya stabil (karena e-Nose mensimulasikan kabin mobil ber-AC).
> 3. **Siapkan Kipas Angin:** Wajib menyiapkan 1 kipas angin meja / kipas kecil portabel untuk proses *purging* (pembersihan chamber).

---

## 🌡️ TAHAP 1: PEMANASAN SENSOR (PRE-HEATING)
*Dilakukan hanya 1 kali di awal hari sebelum mulai sampling.*

1. Masukkan board ESP32 beserta semua sensor ke dalam *chamber* akrilik.
2. Sambungkan kabel USB dari ESP32 ke Laptop.
3. Buka tutup *chamber* secara penuh (agar berisi udara ruangan AC murni).
4. Nyalakan sistem.
5. **TUNGGU MINIMAL 30 MENIT.** Jangan mengambil data apapun. Biarkan sensor menyala agar elemen pemanasnya (*heater*) mencapai suhu operasional maksimal dan voltase *baseline*-nya tidak naik-turun lagi.

---

## 🔄 TAHAP 2: SIKLUS PENGAMBILAN DATA (SAMPLING CYCLE)
*Satu siklus di bawah ini akan menghasilkan 1 file CSV. Lakukan siklus ini 3 hingga 5 kali (3-5 file) untuk SETIAP jenis gas.*

### A. Fase Baseline (Menit ke 0 - 1)
1. Siapkan bahan uji (misal: tisu beralkohol) tapi **JANGAN** dimasukkan dulu. Letakkan agak jauh dari *chamber*.
2. Pastikan *chamber* tertutup rapat berisi udara bersih.
3. Buka Terminal/CMD di laptop, ketik `python logger.py` lalu tekan **Enter**.
4. Biarkan sistem merekam udara bersih di dalam *chamber* selama **1 menit penuh**. *(Catatan: Ini sangat krusial agar Machine Learning tahu nilai awal sensor sebelum naik).*

### B. Fase Injeksi & Eksposur (Menit ke 1 - 4)
1. Tepat setelah 1 menit, buka tutup *chamber* secepat mungkin, masukkan bahan uji (tisu alkohol / piring durian / tiupkan asap knalpot), dan **TUTUP RAPAT** kembali. Usahakan proses buka-tutup ini di bawah 5 detik.
2. Amati layar laptop. Nilai voltase sensor akan meloncat naik.
3. Biarkan merekam selama **3 menit penuh**. Nilai tersebut perlahan akan mencapai puncak dan mendatar (*plateau*).

### C. Fase Penyimpanan (Menit ke 4)
1. Di layar CMD laptop, tekan **Ctrl + C** untuk menghentikan rekaman.
2. File CSV otomatis tersimpan di folder dataset (dengan nama contoh: `enose_data_20261006_160500.csv`).
3. **SANGAT PENTING:** Langsung *Rename* (Ganti Nama) file tersebut agar tidak pusing nantinya. Contoh format nama:
   - `01_Alkohol_Sample1.csv`
   - `02_Knalpot_Sample1.csv`
   - `03_Durian_Sample1.csv`

---

## 💨 TAHAP 3: PEMBERSIHAN CHAMBER (PURGING)
*Wajib dilakukan setelah menghentikan rekaman (Tahap 2C) dan sebelum memulai siklus selanjutnya (Tahap 2A).*

1. Buka tutup *chamber* lebar-lebar.
2. Keluarkan bahan uji dari dalam *chamber*, amankan/jauhkan dari lokasi.
3. Nyalakan kipas angin dan arahkan tiupannya langsung ke dalam *chamber* untuk mengusir sisa gas yang terperangkap.
4. Pantau voltase sensor (bisa dengan menjalankan `logger.py` sebentar atau lihat Serial Monitor). Pastikan angkanya sudah turun mendekati nilai *Baseline* di awal tadi.
5. Proses pembersihan ini biasanya butuh waktu **5 hingga 10 menit** (tergantung seberapa pekat gasnya).
6. Jika nilai sudah kembali normal, hentikan kipas angin.
7. Ulangi kembali ke **Tahap 2A** untuk membuat file `Sample2.csv`, `Sample3.csv`, dan seterusnya.

---

## 📊 URUTAN PENGAMBILAN GAS YANG DISARANKAN
*Agar sisa bau gas sebelumnya tidak merusak bau gas berikutnya, lakukan sampling dengan urutan dari "yang paling ringan" hingga "yang baunya paling menempel":*

1. **Udara Bersih/AC Normal:** Hanya tutup chamber kosong berisikan udara ruangan. Rekam 3 menit, ulangi 3x.
2. **Korek Api Gas (Butana):** Gas sangat ringan, sangat cepat hilang ditiup kipas.
3. **Alkohol / Hand Sanitizer:** Cepat menguap dan cepat hilang saat di-*purging*.
4. **Asap Knalpot / Obat Nyamuk:** Meninggalkan sedikit residu di dinding akrilik.
5. **Durian / Makanan Basi:** Lakukan ini paling **terakhir**! Karena aroma durian atau bawang putih sangat pekat dan baunya bisa menempel di dinding akrilik berhari-hari meskipun sudah dikipas.
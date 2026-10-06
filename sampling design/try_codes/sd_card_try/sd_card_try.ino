#include <SPI.h>
#include <SD.h>

// ==============================================================
// KONFIGURASI PIN SPI UNTUK SD CARD (Berdasarkan Schematic)
// ==============================================================
#define SD_CS    16
#define SD_MOSI  17
#define SD_SCK   18
#define SD_MISO  21

SPIClass spiSD(FSPI); // Menggunakan antarmuka FSPI hardware pada ESP32-S3

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("\n=======================================");
  Serial.println("       SD Card Modul Tester");
  Serial.println("=======================================");

  // Inisialisasi pin SPI custom
  spiSD.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);

  Serial.print("Mencari SD Card... ");
  
  // Mencoba inisialisasi SD Card
  if (!SD.begin(SD_CS, spiSD, 4000000)) {
    Serial.println("GAGAL!");
    Serial.println("-> Cek kembali kabel CS, MOSI, MISO, SCK, VCC, dan GND.");
    Serial.println("-> Pastikan SD Card sudah diformat FAT32.");
    return;
  }
  Serial.println("BERHASIL TERDETEKSI!");

  // Cek tipe SD Card
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("Tidak ada SD Card yang terpasang di modul.");
    return;
  }

  Serial.print("Tipe SD Card: ");
  if (cardType == CARD_MMC) Serial.println("MMC");
  else if (cardType == CARD_SD) Serial.println("SDSC");
  else if (cardType == CARD_SDHC) Serial.println("SDHC");
  else Serial.println("UNKNOWN");

  // Ukuran kartu
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("Kapasitas SD Card: %llu MB\n", cardSize);

  // --- Uji Coba Menulis File ---
  Serial.println("\n--- Menguji Fitur Tulis (Write) ---");
  File dataFile = SD.open("/test_enose.csv", FILE_WRITE);
  
  if (dataFile) {
    Serial.println("Menulis ke /test_enose.csv...");
    dataFile.println("Waktu,Status,Test");
    dataFile.println("1000,OK,Ini adalah baris tes pertama");
    dataFile.close();
    Serial.println("File berhasil ditutup dan disimpan.");
  } else {
    Serial.println("Gagal membuat/membuka file /test_enose.csv");
  }

  // --- Uji Coba Membaca File ---
  Serial.println("\n--- Menguji Fitur Baca (Read) ---");
  dataFile = SD.open("/test_enose.csv", FILE_READ);
  if (dataFile) {
    Serial.println("Isi dari /test_enose.csv:");
    while (dataFile.available()) {
      Serial.write(dataFile.read());
    }
    dataFile.close();
  } else {
    Serial.println("Gagal membaca file /test_enose.csv");
  }

  Serial.println("\nTesting Selesai! Jika muncul teks yang kamu tulis, berarti modul SD Card 100% aman.");
}

void loop() {
  // Tidak ada yang dilakukan di loop
}

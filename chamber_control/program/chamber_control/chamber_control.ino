#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

// oled screen config
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// PINS
const int pinGasIn = 4;
const int pinGasOut = 16;
const int pinSignalInputGas = 17;
const int pinSignalOutGas = 5;
const int pinBuzzer = 18;

// Override pin I2C agar tidak bentrok
const int I2C_SDA = 8;
const int I2C_SCL = 9;

// process timer (dalam milidetik, 10000 = 10 detik)
unsigned long durasiGasIn = 10000;
unsigned long durasiGasOut = 10000;

// state variables
bool gasInProcess = false;
bool gasOutProcess = false;
bool prevGasInProcess = false;
bool prevGasOutProcess = false;
unsigned long processTime = 0;
unsigned long lastDisplayUpdate = 0; // Untuk animasi OLED 50ms

void setup() {
  Serial.begin(115200);
  // PENTING UNTUK ESP32-S3: Tunggu USB CDC aktif sebelum print apa-apa
  delay(3000);
  Serial.println("\n\n--- Memulai Sistem ESP32-S3 ---");

  pinMode(pinGasIn, INPUT);
  pinMode(pinGasOut, INPUT);
  pinMode(pinSignalInputGas, OUTPUT);
  pinMode(pinSignalOutGas, OUTPUT);
  pinMode(pinBuzzer, OUTPUT);

  digitalWrite(pinSignalInputGas, LOW);
  digitalWrite(pinSignalOutGas, LOW);
  digitalWrite(pinBuzzer, LOW);

  Wire.begin(I2C_SDA, I2C_SCL);

  // display ini
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3c)) {
    // Jika OLED tidak terpasang, print berulang kali agar tidak terlewat di
    // Serial Monitor
    for (;;) {
      Serial.println(F("Gagal inisialisasi OLED! Cek koneksi kabel (atau "
                       "pasang ke board)."));
      delay(2000);
    }
  }

  // Wajib atur warna teks, jika tidak teks akan transparan/hitam
  display.setTextColor(SSD1306_WHITE);

  tampilkanStandby();

  Serial.println("OLED Berhasil. Sistem Siap!");
}

void loop() {
  unsigned long waktuSekarang = millis();

  // --- 1. CEK JIKA TIDAK ADA PROSES YANG BERJALAN (IDLE) ---
  if (!gasInProcess && !gasOutProcess) {

    // Baca tombol Gas In
    if (digitalRead(pinGasIn) == HIGH) {
      gasInProcess = true;
      processTime = waktuSekarang;
      digitalWrite(pinSignalInputGas, HIGH);

      Serial.println("Tombol Gas IN Ditekan! Memulai Proses...");
    }
    // Baca tombol Gas Out
    else if (digitalRead(pinGasOut) == HIGH) {
      gasOutProcess = true;
      processTime = waktuSekarang;
      digitalWrite(pinSignalOutGas, HIGH);

      Serial.println("Tombol Gas OUT Ditekan! Memulai Proses...");
    }
  }

  // --- 2. PROSES GAS IN BERJALAN ---
  if (gasInProcess) {
    unsigned long waktuBerjalan = waktuSekarang - processTime;

    // Jika waktu habis
    if (waktuBerjalan >= durasiGasIn) {
      digitalWrite(pinSignalInputGas, LOW); // Matikan Valve & Pompa
      gasInProcess = false;
      bunyiBuzzerSelesai();
    }
  }

  // --- 3. PROSES GAS OUT BERJALAN ---
  if (gasOutProcess) {
    unsigned long waktuBerjalan = waktuSekarang - processTime;

    // Jika waktu habis
    if (waktuBerjalan >= durasiGasOut) {
      digitalWrite(pinSignalOutGas, LOW); // Matikan Valve & Pompa
      gasOutProcess = false;
      bunyiBuzzerSelesai();
    }
  }

  // --- 4. UPDATE LAYAR OLED (UI/UX 60FPS) ---
  // Kita perbarui layar setiap 50ms agar animasi progress bar mulus
  if (waktuSekarang - lastDisplayUpdate >= 50) {
    lastDisplayUpdate = waktuSekarang;

    if (!gasInProcess && !gasOutProcess) {
      // Hanya gambar layar standby jika baru saja berubah dari aktif ke mati
      // (Mencegah flicker karena digambar terus-menerus)
      if (prevGasInProcess || prevGasOutProcess) {
        tampilkanStandby();
      }
    } else if (gasInProcess) {
      tampilkanAktif("GAS IN AKTIF", waktuSekarang - processTime, durasiGasIn);
    } else if (gasOutProcess) {
      tampilkanAktif("GAS OUT AKTIF", waktuSekarang - processTime,
                     durasiGasOut);
    }

    // Simpan riwayat status
    prevGasInProcess = gasInProcess;
    prevGasOutProcess = gasOutProcess;
  }
}

// Fungsi untuk membunyikan buzzer 3 kali
void bunyiBuzzerSelesai() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(pinBuzzer, HIGH);
    delay(100);
    digitalWrite(pinBuzzer, LOW);
    delay(100);
  }
}

// ================= FUNGSI DESAIN UI OLED =================

void tampilkanStandby() {
  display.clearDisplay();

  // Gambar bingkai luar
  display.drawRect(0, 0, 128, 32, SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(24, 12);
  display.print("SISTEM STANDBY");

  display.display();
}

void tampilkanAktif(String mode, unsigned long waktuBerjalan,
                    unsigned long durasiTotal) {
  display.clearDisplay();

  // Teks Mode di kiri atas
  display.setTextSize(1);
  display.setCursor(0, 2);
  display.print(mode);

  // Hitung sisa detik untuk teks di kanan atas
  long sisaWaktuMs = durasiTotal - waktuBerjalan;
  if (sisaWaktuMs < 0)
    sisaWaktuMs = 0;
  int sisaDetik = (sisaWaktuMs / 1000) + 1;
  if (sisaDetik > (durasiTotal / 1000))
    sisaDetik = (durasiTotal / 1000); // Capping display

  display.setCursor(105, 2);
  display.print(sisaDetik);
  display.print("s");

  // Gambar Bingkai Progress Bar (Kotak luar)
  display.drawRect(0, 16, 128, 14, SSD1306_WHITE);

  // Hitung lebar isi progress bar (Menyusut dari 124 pixel ke 0)
  int fillWidth = map(sisaWaktuMs, 0, durasiTotal, 0, 124);

  // Gambar isi progress bar
  display.fillRect(2, 18, fillWidth, 10, SSD1306_WHITE);

  display.display();
}

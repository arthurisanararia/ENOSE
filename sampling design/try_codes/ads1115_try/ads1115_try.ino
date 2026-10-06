#include <Wire.h>
#include <Adafruit_ADS1X15.h>

#define I2C_SDA 8
#define I2C_SCL 9

// ==============================================================
// KONFIGURASI JUMLAH ADS1115
// Ubah variabel ini sesuai dengan jumlah ADS yang sedang dipakai (1 hingga 4)
// ==============================================================
#define NUM_ADS 2

// Alamat I2C standar untuk ADS1115:
// Pin ADDR -> GND = 0x48
// Pin ADDR -> VDD = 0x49
// Pin ADDR -> SDA = 0x4A
// Pin ADDR -> SCL = 0x4B
const uint8_t ADS_ADDRESSES[4] = {0x48, 0x49, 0x4A, 0x4B};

// Membuat array objek untuk ADS1115 agar lebih mudah diakses menggunakan perulangan (loop)
Adafruit_ADS1115 ads[4];
bool ads_connected[4] = {false, false, false, false}; // Menyimpan status koneksi masing-masing ADS

void setup() {
  Serial.begin(115200);
  while (!Serial);
  
  Serial.println("\n=======================================");
  Serial.println("     ADS1115 Multiboard Tester");
  Serial.println("=======================================");

  // Inisialisasi I2C
  Wire.begin(I2C_SDA, I2C_SCL);

  // Inisialisasi dan cek koneksi setiap modul ADS1115
  for (int i = 0; i < NUM_ADS; i++) {
    Serial.print("Inisialisasi ADS1115 #");
    Serial.print(i + 1);
    Serial.print(" (Alamat 0x");
    Serial.print(ADS_ADDRESSES[i], HEX);
    Serial.print(") ... ");

    // ads.begin() melakukan inisiasi ke modul dan me-return true jika berhasil
    if (ads[i].begin(ADS_ADDRESSES[i], &Wire)) {
      Serial.println("BERHASIL");
      // Set Gain (opsional). GAIN_TWOTHIRDS membaca voltase +/- 6.144V
      // 1 bit = 0.1875mV
      ads[i].setGain(GAIN_TWOTHIRDS);
      ads_connected[i] = true;
    } else {
      Serial.println("GAGAL (Tidak terdeteksi)");
      ads_connected[i] = false;
    }
  }
}

void loop() {
  Serial.println("\n--- Hasil Pembacaan ADS1115 ---");
  
  for (int i = 0; i < NUM_ADS; i++) {
    // Hanya baca ADS1115 yang berhasil terhubung saat inisiasi
    if (ads_connected[i]) {
      Serial.print("[ADS #");
      Serial.print(i + 1);
      Serial.print(" | 0x");
      Serial.print(ADS_ADDRESSES[i], HEX);
      Serial.println("]");

      // Baca 4 channel (A0 sampai A3) di setiap modul ADS1115
      for (int ch = 0; ch < 4; ch++) {
        int16_t adc_val = ads[i].readADC_SingleEnded(ch);
        float voltage = ads[i].computeVolts(adc_val);
        
        Serial.print("  A");
        Serial.print(ch);
        Serial.print(": Raw = ");
        Serial.print(adc_val);
        Serial.print("\t Voltage = ");
        Serial.print(voltage, 4); // presisi 4 angka di belakang koma
        Serial.println(" V");
      }
    }
  }
  
  delay(2000); // jeda 2 detik tiap perulangan
}

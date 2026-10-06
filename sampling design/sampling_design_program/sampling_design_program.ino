#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <Adafruit_BME280.h>

// --- Konfigurasi Pin I2C ---
#define I2C_SDA 8
#define I2C_SCL 9

// --- Konfigurasi Alamat I2C ---
#define ADS1_ADDRESS 0x48
#define ADS2_ADDRESS 0x49

// --- Inisialisasi Objek ---
Adafruit_ADS1115 ads1;
Adafruit_ADS1115 ads2;
Adafruit_BME280 bme;

// --- Status Modul ---
bool ads1OK = false;
bool ads2OK = false;
bool bmeOK  = false;

// --- Konfigurasi Waktu Sampling ---
const unsigned long SAMPLING_INTERVAL = 1000; // Ambil data tiap 1000 ms (1 detik)
unsigned long lastSampleTime = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(100000); // 100kHz standard mode untuk I2C

  Serial.println("=========================================");
  Serial.println("  ENOSE SAMPLING PROGRAM (Serial CSV)");
  Serial.println("=========================================");
  Serial.println("Inisialisasi Sensor...");

  // Inisialisasi ADS1115 #1
  if (ads1.begin(ADS1_ADDRESS, &Wire)) {
    ads1.setGain(GAIN_TWOTHIRDS); // Range +/- 6.144V
    ads1OK = true;
    Serial.println("-> ADS1115 #1 (0x48) : OK");
  } else {
    Serial.println("-> ADS1115 #1 (0x48) : GAGAL TERDETEKSI!");
  }

  // Inisialisasi ADS1115 #2
  if (ads2.begin(ADS2_ADDRESS, &Wire)) {
    ads2.setGain(GAIN_TWOTHIRDS);
    ads2OK = true;
    Serial.println("-> ADS1115 #2 (0x49) : OK");
  } else {
    Serial.println("-> ADS1115 #2 (0x49) : GAGAL TERDETEKSI!");
  }

  // Inisialisasi BME280 (coba 0x76 lalu 0x77)
  if (bme.begin(0x76, &Wire)) {
    bmeOK = true;
    Serial.println("-> BME280 (0x76)     : OK");
  } else if (bme.begin(0x77, &Wire)) {
    bmeOK = true;
    Serial.println("-> BME280 (0x77)     : OK");
  } else {
    // Kalau gagal (misal belum terpasang), program tetap jalan
    Serial.println("-> BME280            : GAGAL TERDETEKSI (Bypass diaktifkan)");
  }

  Serial.println("=========================================");
  Serial.println("Menunggu stabilitas sensor (3 detik)...");
  delay(3000);

  // Mencetak header kolom untuk file CSV
  // Header ini akan dideteksi oleh script Python di laptop
  Serial.println("Time_ms,TGS2600_V,TGS2602_V,MQ4_V,TGS2611_V,TGS2620_V,TGS5042_V,MQ135_V,MQ2_V,Temp_C,Hum_%,Press_hPa");
}

void loop() {
  unsigned long currentMillis = millis();

  // Eksekusi rutin pembacaan setiap SAMPLING_INTERVAL terpenuhi
  if (currentMillis - lastSampleTime >= SAMPLING_INTERVAL) {
    lastSampleTime = currentMillis;

    // --- BACA ADS1115 #1 ---
    float tgs2600_v = 0.0, tgs2602_v = 0.0, mq4_v = 0.0, tgs2611_v = 0.0;
    if (ads1OK) {
      tgs2600_v = ads1.computeVolts(ads1.readADC_SingleEnded(0));
      tgs2602_v = ads1.computeVolts(ads1.readADC_SingleEnded(1));
      mq4_v     = ads1.computeVolts(ads1.readADC_SingleEnded(2));
      tgs2611_v = ads1.computeVolts(ads1.readADC_SingleEnded(3));
    }

    // --- BACA ADS1115 #2 ---
    float tgs2620_v = 0.0, tgs5042_v = 0.0, mq135_v = 0.0, mq2_v = 0.0;
    if (ads2OK) {
      tgs2620_v = ads2.computeVolts(ads2.readADC_SingleEnded(0));
      tgs5042_v = ads2.computeVolts(ads2.readADC_SingleEnded(1));
      mq135_v   = ads2.computeVolts(ads2.readADC_SingleEnded(2));
      mq2_v     = ads2.computeVolts(ads2.readADC_SingleEnded(3));
    }

    // --- BACA BME280 ---
    // Jika BME280 tidak terpasang, nilai tetap 0.0
    float temp = 0.0, hum = 0.0, press = 0.0;
    if (bmeOK) {
      temp  = bme.readTemperature();
      hum   = bme.readHumidity();
      press = bme.readPressure() / 100.0F; // Konversi Pascal ke hPa
    }

    // --- CETAK DATA KE SERIAL (Berformat CSV) ---
    // Jangan tambahkan teks lain di sini agar format CSV tidak rusak
    Serial.print(currentMillis); Serial.print(",");
    
    Serial.print(tgs2600_v, 4); Serial.print(",");
    Serial.print(tgs2602_v, 4); Serial.print(",");
    Serial.print(mq4_v, 4);     Serial.print(",");
    Serial.print(tgs2611_v, 4); Serial.print(",");
    
    Serial.print(tgs2620_v, 4); Serial.print(",");
    Serial.print(tgs5042_v, 4); Serial.print(",");
    Serial.print(mq135_v, 4);   Serial.print(",");
    Serial.print(mq2_v, 4);     Serial.print(",");
    
    Serial.print(temp, 2);  Serial.print(",");
    Serial.print(hum, 2);   Serial.print(",");
    Serial.print(press, 2);
    
    // Baris baru menandakan akhir baris data saat ini
    Serial.println(); 
  }
}
#include <Wire.h>

#define I2C_SDA 8
#define I2C_SCL 9

void setup() {
  Serial.begin(115200);
  while (!Serial); // Tunggu Serial Monitor terbuka (opsional)
  
  Serial.println("\n==================================");
  Serial.println(" I2C Scanner - ESP32-S3");
  Serial.println("==================================");
  
  // Memulai I2C dengan pin SDA dan SCL yang digunakan di board kamu
  Wire.begin(I2C_SDA, I2C_SCL);
}

void loop() {
  byte error, address;
  int nDevices;

  Serial.println("Scanning I2C Bus...");

  nDevices = 0;
  // Alamat I2C berkisar dari 1 hingga 127
  for (address = 1; address < 127; address++ ) {
    // Memulai transmisi ke alamat tertentu
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.print(address, HEX);
      Serial.println("  !");
      nDevices++;
    } 
    else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.println(address, HEX);
    }
  }
  
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  } else {
    Serial.println("Scan done\n");
  }

  // Jeda 5 detik sebelum scan lagi
  delay(5000);
}

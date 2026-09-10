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

// process timer (dalam milidetik, 10000 = 10 detik)
unsigned long durasiGasIn = 10000;
unsigned long durasiGasOut = 10000;

// state variables
bool gasInProcess = false;
bool gasOutProcess = false;
unsigned long processTime = 0;
unsigned long lastDisplayUpdate = 0; // Untuk mencegah OLED berkedip

void setup() {
  Serial.begin(115200);

  pinMode(pinGasIn, INPUT);
  pinMode(pinGasOut, INPUT);
  pinMode(pinSignalInputGas, OUTPUT);
  pinMode(pinSignalOutGas, OUTPUT);
  pinMode(pinBuzzer, OUTPUT);

  digitalWrite(pinSignalInputGas, LOW);
  digitalWrite(pinSignalOutGas, LOW);
  digitalWrite(pinBuzzer, LOW);

  // display ini
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3c)) {
    Serial.println(F("Failed OLED inisialitation"));
    for (;;)
      ;
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Sistem Siap!");
  display.display();
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
      
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("PROSES GAS IN...");
      display.display();
    } 
    // Baca tombol Gas Out
    else if (digitalRead(pinGasOut) == HIGH) {
      gasOutProcess = true;                 
      processTime = waktuSekarang;         
      digitalWrite(pinSignalOutGas, HIGH); 
      
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("PROSES GAS OUT...");
      display.display();
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
      
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Sistem Siap!");
      display.display();
    } else {
      // Jika waktu belum habis, tampilkan sisa waktu (update tiap 100ms agar OLED tidak flicker)
      if (waktuSekarang - lastDisplayUpdate > 100) {
        lastDisplayUpdate = waktuSekarang;
        int sisaDetik = (durasiGasIn - waktuBerjalan) / 1000;
        
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("PROSES GAS IN...");
        display.setCursor(0, 16);
        display.print("Sisa Waktu: ");
        display.print(sisaDetik);
        display.println(" dtk");
        display.display();
      }
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
      
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Sistem Siap!");
      display.display();
    } else {
      // Jika waktu belum habis, tampilkan sisa waktu
      if (waktuSekarang - lastDisplayUpdate > 100) {
        lastDisplayUpdate = waktuSekarang;
        int sisaDetik = (durasiGasOut - waktuBerjalan) / 1000;
        
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("PROSES GAS OUT...");
        display.setCursor(0, 16);
        display.print("Sisa Waktu: ");
        display.print(sisaDetik);
        display.println(" dtk");
        display.display();
      }
    }
  }
}

// Fungsi untuk membunyikan buzzer 3 kali
void bunyiBuzzerSelesai() {
  for(int i = 0; i < 3; i++) {
    digitalWrite(pinBuzzer, HIGH);
    delay(100); 
    digitalWrite(pinBuzzer, LOW);
    delay(100); 
  }
}

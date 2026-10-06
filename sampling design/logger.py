import serial
import time
import csv
import os

# ==============================================================
# KONFIGURASI LOGGER
# ==============================================================
# Ganti 'COM3' dengan port ESP32-S3 di laptopmu (misal: 'COM5' atau '/dev/ttyUSB0')
SERIAL_PORT = 'COM18'  
BAUD_RATE = 115200
OUTPUT_DIR = 'dataset' # Folder tempat menyimpan file CSV

# Membuat folder 'dataset' jika belum ada
if not os.path.exists(OUTPUT_DIR):
    os.makedirs(OUTPUT_DIR)

# Nama file dibuat unik berdasarkan tanggal dan jam pengambilan data
timestamp_str = time.strftime('%Y%m%d_%H%M%S')
filename = os.path.join(OUTPUT_DIR, f"enose_data_{timestamp_str}.csv")

print("=============================================")
print("      E-NOSE SERIAL TO CSV LOGGER")
print("=============================================")
print(f"[*] Mencoba terhubung ke {SERIAL_PORT} dengan baud rate {BAUD_RATE}...")

try:
    # Membuka koneksi serial
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    print(f"[*] BERHASIL terhubung ke {SERIAL_PORT}!")
    print(f"[*] Data akan disimpan di file: {filename}")
    print("[*] Menunggu ESP32 mengirim data header CSV...")
    print("[!] TEKAN CTRL+C UNTUK BERHENTI DAN MENYIMPAN FILE.\n")

    with open(filename, mode='a', newline='') as file:
        writer = csv.writer(file)
        header_written = False
        
        while True:
            # Membaca data baris per baris dari Serial
            if ser.in_waiting > 0:
                # Membaca baris dan membersihkan spasi/karakter kosong di awal/akhir
                raw_line = ser.readline()
                try:
                    line = raw_line.decode('utf-8').strip()
                except UnicodeDecodeError:
                    continue # Abaikan karakter aneh (noise serial)
                
                # Mengabaikan pesan boot/inisialisasi, mencari baris Header CSV dari ESP32
                if "Time_ms" in line and not header_written:
                    # Menambahkan kolom "Laptop_Time" di urutan paling depan
                    headers = ["Laptop_Time"] + line.split(',')
                    writer.writerow(headers)
                    file.flush() # Paksa simpan ke disk
                    header_written = True
                    print(f"[HEADER] {', '.join(headers)}")
                    continue
                
                # Memproses data angka setelah header tercetak
                if header_written:
                    data_elements = line.split(',')
                    # Memastikan baris data memiliki jumlah kolom yang tepat (12 kolom dari ESP32)
                    if len(data_elements) == 12:
                        current_time = time.strftime('%Y-%m-%d %H:%M:%S')
                        row = [current_time] + data_elements
                        writer.writerow(row)
                        file.flush()
                        
                        print(f"[{current_time}] Data tersimpan -> {line}")
                    elif len(line) > 0:
                        # Print pesan dari ESP32 yang bukan format CSV (seperti pesan inisialisasi tambahan)
                        print(f"[ESP32 Msg]: {line}")

except serial.SerialException:
    print(f"\n[ERROR] Tidak bisa membuka port {SERIAL_PORT}.")
    print("-> Pastikan kabel USB tersambung.")
    print("-> Pastikan nomor port COM sudah benar.")
    print("-> Pastikan Serial Monitor di Arduino IDE sudah DITUTUP!")
except KeyboardInterrupt:
    print("\n\n[INFO] Proses pengambilan data dihentikan oleh pengguna.")
finally:
    if 'ser' in locals() and ser.is_open:
        ser.close()
        print("[INFO] Port Serial ditutup. Data aman di dalam CSV.")

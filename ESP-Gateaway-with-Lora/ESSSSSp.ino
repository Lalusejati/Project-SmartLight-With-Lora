#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Konfigurasi pin LoRa
#define LORA_SCK   18
#define LORA_MISO  19
#define LORA_MOSI  23
#define LORA_CS     5
#define LORA_RST   14
#define LORA_DIO0  26

// LCD I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);

// WiFi config
const char* ssid = "vivo T1 Pro 5G";
const char* password = "987654321";

// Alamat server
const char* server = "192.168.223.90";

// Fungsi bantu parsing string
String getValue(String data, String key) {
  int idx = data.indexOf(key + ":");
  if (idx == -1) return "";
  int start = idx + key.length() + 1;
  int end = data.indexOf(",", start);
  if (end == -1) end = data.length();
  return data.substring(start, end);
}

void setup() {
  Serial.begin(9600);

  // Koneksi WiFi
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi Terhubung");

  // LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Gateway Mulai");

  // LoRa
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
  LoRa.setPins(LORA_CS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(433E6)) {
    Serial.println("❌ LoRa gagal dimulai!");
    lcd.setCursor(0, 1);
    lcd.print("LoRa ERROR");
    while (1);
  }

  Serial.println("✅ LoRa Siap Menerima");
  lcd.setCursor(0, 1);
  lcd.print("LoRa siap...");
  delay(2000);
  lcd.clear();
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String incoming = "";
    while (LoRa.available()) {
      incoming += (char)LoRa.read();
    }

    Serial.println("📥 Data Masuk: " + incoming);

    // Ambil nilai dari format: LDR:xxx,Suhu:xx,Kelembapan:xx,Jarak:xx
    String ldr = getValue(incoming, "LDR");
    String suhu = getValue(incoming, "Suhu");
    String kelembapan = getValue(incoming, "Kelembapan");
    String jarak = getValue(incoming, "Jarak");

    // Tampilkan ke Serial & LCD
    Serial.println("LDR: " + ldr + " | Suhu: " + suhu + "C | Lembab: " + kelembapan + "% | Jarak: " + jarak + "cm");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("L:" + ldr + " S:" + suhu + "C");
    lcd.setCursor(0, 1);
    lcd.print("H:" + kelembapan + "% J:" + jarak);

    delay(1000); // jeda tampilan

    // Kirim data ke server PHP
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      String url = "http://" + String(server) + "/SmartLight1/WebControl/kirimdata.php";
      url += "?ldr=" + ldr + "&distance=" + jarak + "&suhu=" + suhu + "&kelembapan=" + kelembapan;

      http.begin(url);
      int httpCode = http.GET();
      String response = http.getString();
      Serial.println("🌐 Respon Server: " + response);
      http.end();
    } else {
      Serial.println("❌ WiFi tidak tersedia!");
    }

    delay(3000); // jeda sebelum menerima paket LoRa berikutnya
  }
}

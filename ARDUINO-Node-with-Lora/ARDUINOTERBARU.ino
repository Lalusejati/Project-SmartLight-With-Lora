#include <SPI.h>
#include <LoRa.h>
#include <Servo.h>
#include "DHT.h"

// Konfigurasi pin LoRa
#define LORA_SS   10
#define LORA_RST   8
#define LORA_DIO0  9

// Sensor dan output
#define trigPin 4
#define echoPin 5
#define ledPin 6
#define ldrPin A0
#define DHTPIN 2
#define DHTTYPE DHT11

Servo myservo;
DHT dht(DHTPIN, DHTTYPE);

long duration;
int distance;
int ldrValue;
float suhu, kelembapan;

int calculateDistance() {
  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  return pulseIn(echoPin, HIGH) * 0.034 / 2;
}

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);
  myservo.attach(7);
  dht.begin();

  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa Gagal Dimulai");
    while (1);
  }
  Serial.println("✅ LoRa Siap Kirim");
}

void loop() {
  // Baca sensor
  ldrValue = analogRead(ldrPin);
  suhu = dht.readTemperature();
  kelembapan = dht.readHumidity();

  if (isnan(suhu) || isnan(kelembapan)) {
    Serial.println("❌ Gagal membaca sensor DHT11!");
    return;
  }

  Serial.println("📊 Sensor: LDR=" + String(ldrValue) + ", Suhu=" + String(suhu) + "°C, Kelembapan=" + String(kelembapan) + "%");

  // Sweeping servo (radar)
  for (int angle = 15; angle <= 180; angle++) {
    myservo.write(angle);
    delay(15);
    distance = calculateDistance();
    kontrolLampu();
    debugRadar(angle, distance);
  }

  for (int angle = 180; angle >= 15; angle--) {
    myservo.write(angle);
    delay(15);
    distance = calculateDistance();
    kontrolLampu();
    debugRadar(angle, distance);
  }

  // Kirim data ke Gateway via LoRa
  String data = "LDR:" + String(ldrValue) +
                ",Suhu:" + String(suhu) +
                ",Kelembapan:" + String(kelembapan) +
                ",Jarak:" + String(distance); // jarak terakhir dari radar sweep

  Serial.println("📡 Mengirim: " + data);
  LoRa.beginPacket();
  LoRa.print(data);
  LoRa.endPacket();

  delay(3000);
}

void kontrolLampu() {
  if ((distance < 15) || (ldrValue < 300) || (suhu < 22 && kelembapan > 85)) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
}

void debugRadar(int angle, int jarak) {
  Serial.print("Sudut: "); Serial.print(angle);
  Serial.print("°, Jarak: "); Serial.print(jarak); Serial.println(" cm");
}

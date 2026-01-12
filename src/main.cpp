#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <esp_sleep.h>

// Pin für den Taster (ESP32-lolin lite GPIO 4)
#define BUTTON_PIN 4

// Struktur muss EXAKT wie beim Empfänger sein!
typedef struct struct_message {
    int playerID; 
} struct_message;

struct_message meineDaten;

// Adresse der Zentrale (Broadcast oder spezifische MAC)
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// RTC-Speicher für schnellere Wiederverbindung
RTC_DATA_ATTR int bootCount = 0;

void setupDeepSleep() {
  // GPIO 4 als Wakeup-Quelle (LOW = gedrückt)
  esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_PIN, 0);
  
  Serial.println("Gehe in Deep Sleep...");
  Serial.flush();
  
  // Deep Sleep aktivieren
  esp_deep_sleep_start();
}

void setup() {
  Serial.begin(9600);
  
  bootCount++;
  Serial.printf("Boot #%d - Wakeup durch Tastendruck\n", bootCount);
  
  // Taster-Pin konfigurieren
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // WiFi minimal konfigurieren für ESP-NOW
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  
  // TX-Power reduzieren spart Strom (optional, falls Reichweite ausreicht)
  // esp_wifi_set_max_tx_power(8); // Werte: 8-84 (2-20dBm)

  // ESP-NOW initialisieren
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init fehlgeschlagen!");
    setupDeepSleep();
    return;
  }

  // Peer registrieren
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Fehler beim Hinzufügen des Peers!");
    setupDeepSleep();
    return;
  }

  // Spieler-ID (kannst du auch aus der MAC ableiten)
  meineDaten.playerID = 1; 

  // Nachricht senden
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&meineDaten, sizeof(meineDaten));
  
  if (result == ESP_OK) {
    Serial.println("✓ Buzzer-Signal gesendet!");
  } else {
    Serial.println("✗ Sende-Fehler!");
  }

  // Kurz warten um sicherzustellen, dass Nachricht raus ist
  delay(50);
  
  // Warten bis Taste losgelassen wird (verhindert Mehrfach-Trigger)
  while(digitalRead(BUTTON_PIN) == LOW) {
    delay(10);
  }
  
  Serial.println("Taste losgelassen - bereit für Sleep");
  delay(10);
  
  // Sofort zurück in Deep Sleep
  setupDeepSleep();
}

void loop() {
  // Loop wird nie erreicht, da wir direkt nach setup() schlafen gehen
}
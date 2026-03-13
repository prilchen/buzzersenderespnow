#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <esp_sleep.h>
#include "esp_bt.h"

// --- DEBUG MODUS ---
// 1 = Seriell an | 0 = Seriell aus (spart Strom!)
#define DEBUG 0

#if DEBUG == 1
  #define DEBUG_PRINTLN(x) Serial.println(x)
  #define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
  #define DEBUG_BEGIN(x) Serial.begin(x)
  #define DEBUG_FLUSH() Serial.flush()
#else
  #define DEBUG_PRINTLN(x)
  #define DEBUG_PRINTF(...)
  #define DEBUG_BEGIN(x)
  #define DEBUG_FLUSH()
#endif

// Pin für den Taster (RTC-fähig, z.B. GPIO 4)
#define BUTTON_PIN 4

typedef struct struct_message {
    int playerID;
} struct_message;

struct_message meineDaten;

// Adresse der Zentrale (Broadcast oder spezifische MAC)
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

RTC_DATA_ATTR int bootCount = 0;

void setupDeepSleep() {
    // Wakeup über Taster (LOW)
    esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_PIN, 0);

    DEBUG_PRINTLN("Gehe in Deep Sleep...");
    DEBUG_FLUSH();

    esp_deep_sleep_start();
}

void setup() {
    DEBUG_BEGIN(115200);

    bootCount++;
    DEBUG_PRINTF("Boot #%d - Wakeup durch Tastendruck\n", bootCount);

    // Taster mit internem Pullup (für echten Minimalverbrauch besser: externer Pullup)
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    // WLAN nur als Station
    WiFi.mode(WIFI_STA);

    // ESP-NOW initialisieren
    if (esp_now_init() != ESP_OK) {
        DEBUG_PRINTLN("ESP-NOW Init fehlgeschlagen!");
        setupDeepSleep();
        return;
    }

    // Peer registrieren (Broadcast)
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;      // aktueller WiFi-Kanal
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        DEBUG_PRINTLN("Fehler beim Hinzufügen des Peers!");
        esp_now_deinit();
        setupDeepSleep();
        return;
    }

    // Eigene Spieler-ID setzen
    meineDaten.playerID = 1;

    // Nachricht senden
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&meineDaten, sizeof(meineDaten));

    if (result == ESP_OK) {
        DEBUG_PRINTLN("✓ Buzzer-Signal gesendet!");
    } else {
        DEBUG_PRINTLN("✗ Sende-Fehler!");
    }

    // Kurz warten, damit das Paket sicher raus ist
    delay(20);

    // ESP-NOW & WiFi & Bluetooth deaktivieren, bevor wir schlafen
    esp_now_deinit();
    WiFi.mode(WIFI_OFF);
    btStop();

    // Warten bis Taste losgelassen wird (verhindert Mehrfach-Trigger)
    while (digitalRead(BUTTON_PIN) == LOW) {
        delay(5);
    }

    DEBUG_PRINTLN("Taste losgelassen - bereit für Sleep");
    delay(5);

    setupDeepSleep();
}

void loop() {}

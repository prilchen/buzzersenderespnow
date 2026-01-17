#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <esp_sleep.h>

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

// Pin für den Taster (ESP32-lolin lite GPIO 4)
#define BUTTON_PIN 4

typedef struct struct_message {
    int playerID; 
} struct_message;

struct_message meineDaten;

// Adresse der Zentrale (Broadcast oder spezifische MAC)
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

RTC_DATA_ATTR int bootCount = 0;

void setupDeepSleep() {
    esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_PIN, 0);
    DEBUG_PRINTLN("Gehe in Deep Sleep...");
    DEBUG_FLUSH();
    esp_deep_sleep_start();
}

void setup() {
    DEBUG_BEGIN(115200); // 115200 ist schneller fertig als 9600
    
    bootCount++;
    DEBUG_PRINTF("Boot #%d - Wakeup durch Tastendruck\n", bootCount);
    
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    
    // ESP-NOW initialisieren
    if (esp_now_init() != ESP_OK) {
        DEBUG_PRINTLN("ESP-NOW Init fehlgeschlagen!");
        setupDeepSleep();
        return;
    }

    // Peer registrieren
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        DEBUG_PRINTLN("Fehler beim Hinzufügen des Peers!");
        setupDeepSleep();
        return;
    }

    meineDaten.playerID = 1; 

    // Nachricht senden
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&meineDaten, sizeof(meineDaten));
    
    if (result == ESP_OK) {
        DEBUG_PRINTLN("✓ Buzzer-Signal gesendet!");
    } else {
        DEBUG_PRINTLN("✗ Sende-Fehler!");
    }

    // Kurz warten für Sendevorgang
    delay(50);
    
    // Warten bis Taste losgelassen wird (verhindert Mehrfach-Trigger)
    while(digitalRead(BUTTON_PIN) == LOW) {
        delay(10);
    }
    
    DEBUG_PRINTLN("Taste losgelassen - bereit für Sleep");
    delay(10);
    
    setupDeepSleep();
}

void loop() {}
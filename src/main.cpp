#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

// Pin für den Taster (ESP32-lolin lite GPIO 4)
#define BUTTON_PIN 4

// Struktur muss EXAKT wie beim Empfänger sein!
typedef struct struct_message {
    int playerID; 
} struct_message;

struct_message meineDaten;

// Adresse der Zentrale (Broadcast ist okay, aber Unicast wäre noch schneller)
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void setup() {
  Serial.begin(9600);
  
  // Interner Pullup – Taster schaltet gegen GND
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // WiFi im Station-Modus
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init fehlgeschlagen!");
    return;
  }

  // Peer registrieren
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Fehler beim Hinzufügen des Peers!");
    return;
  }

  // Identität dieses Buzzers (für den Fall, dass die MAC nicht bekannt ist)
  meineDaten.playerID = 1; 

  Serial.println("Buzzer bereit – Sende nur bei Tastendruck.");
}

void loop() {
  // Nur senden, wenn Taste gedrückt (LOW)
  if (digitalRead(BUTTON_PIN) == LOW) {
    
    // Sofort senden für minimale Latenz!
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &meineDaten, sizeof(meineDaten));
    
    if (result == ESP_OK) {
      Serial.println("Buzzer-Signal gesendet!");
    } else {
      Serial.println("Sende-Fehler!");
    }

    // "Debounce" und warten, bis Taste wieder losgelassen wird
    // Verhindert Dauerfeuer bei einem langen Druck
    delay(200); 
    while(digitalRead(BUTTON_PIN) == LOW) {
        delay(10); 
    }
  }
}
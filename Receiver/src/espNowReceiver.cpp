#include "espNowReceiver.h"

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

#include "leds.h"
#include "memory.h"
#include "configWebserver.h"
#include "constants.h"

long lastMessageReceived;

// Broadcast address, sends to all devices nearby
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message
{
  boolean program[maxAtemInputs];
  boolean preview[maxAtemInputs];
  boolean transition;
  boolean request;
} struct_message;

// Create a struct_message called recvData
struct_message recvData;

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len)
{
  lastMessageReceived = millis();
  memcpy(&recvData, incomingData, sizeof(recvData));
  Serial.println("Bytes received: " + String(len));
  Serial.println("Program: " + String(recvData.program[camId - 1]));
  Serial.println("Preview: " + String(recvData.preview[camId - 1]));
  Serial.println("Transition: " + String(recvData.transition));
  Serial.println("Request: " + String(recvData.request));

  if (recvData.request == true)
    return;

  digitalWrite(PROGRAM_LED, recvData.program[camId - 1] ? HIGH : LOW);

  digitalWrite(PREVIEW_LED, recvData.preview[camId - 1] ? HIGH : LOW);
}

void requestState()
{
  // send request
  recvData = {0, 0, false, true};
  esp_now_send(broadcastAddress, (uint8_t *)&recvData, sizeof(recvData));
}

void setupEspNow()
{
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);

  requestState();
  lastMessageReceived = millis();
}

void espNowLoop()
{
  if (millis() - lastMessageReceived > timeBetweenStateRequests)
  {
    lastMessageReceived += 3000;
    requestState();
    digitalWrite(STATUS_LED, LOW);
  } else {
    digitalWrite(STATUS_LED, HIGH);
  }
  delay(5);
}
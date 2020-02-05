#include <Arduino.h>
//////////////////espnow
#include <esp_now.h>
#include <stdint.h>
#include <string.h>
/////////////////////////
/////////////////////auto wifi
#include <AutoWifi.h>
IPAddress ip;

///////////////////////
/////////////////////screen
////#include "TFT_eSPI.h"
#include <SPI.h>
#include "WiFi.h"
// #include <Wire.h>
#include <Button2.h>
#include "esp_adc_cal.h"
//TFT_eSPI tft = TFT_eSPI(135, 240); // Invoke custom library

///////////////////////

////////////////////TONE
#include "Tone32.h"
#define BUZZER_PIN 16
#define BUZZER_CHANNEL 0
////////////////////////
#define FACTORYRESET_BUTTON 35

///////////////////////Screen Setup

////////////////////////////

////////////////////ESPNOW SETUP
// SENDER and RECEIVER are arbitrary with ESP-NOW, this is solely for this example
//#define SENDER
#ifndef SENDER
#define RECEIVER
#endif
/////////////////////////////////
//////////////////////////Loop timer
int period = 1000;
unsigned long time_now = 0;
//////////////////////////////////
static uint8_t broadcast_mac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

typedef struct __attribute__((packed)) esp_now_msg_t
{
  uint32_t address;
  uint32_t counter;
  // Can put lots of things here...
} esp_now_msg_t;

static void playTone(){
  Serial.println("play tone");
   // put your main code here, to run repeatedly:
  tone(BUZZER_PIN, NOTE_C4, 500, BUZZER_CHANNEL);
  noTone(BUZZER_PIN, BUZZER_CHANNEL);
  tone(BUZZER_PIN, NOTE_D4, 500, BUZZER_CHANNEL);
  noTone(BUZZER_PIN, BUZZER_CHANNEL);
  tone(BUZZER_PIN, NOTE_E4, 500, BUZZER_CHANNEL);
  noTone(BUZZER_PIN, BUZZER_CHANNEL);
  tone(BUZZER_PIN, NOTE_F4, 500, BUZZER_CHANNEL);
  noTone(BUZZER_PIN, BUZZER_CHANNEL);
  tone(BUZZER_PIN, NOTE_G4, 500, BUZZER_CHANNEL);
  noTone(BUZZER_PIN, BUZZER_CHANNEL);
  tone(BUZZER_PIN, NOTE_A4, 500, BUZZER_CHANNEL);
  noTone(BUZZER_PIN, BUZZER_CHANNEL);
  tone(BUZZER_PIN, NOTE_B4, 500, BUZZER_CHANNEL);
  noTone(BUZZER_PIN, BUZZER_CHANNEL);
}

static void handle_error(esp_err_t err)
{
  switch (err)
  {
  case ESP_ERR_ESPNOW_NOT_INIT:
    Serial.println("Not init");
    break;

  case ESP_ERR_ESPNOW_ARG:
    Serial.println("Argument invalid");
    break;

  case ESP_ERR_ESPNOW_INTERNAL:
    Serial.println("Internal error");
    break;

  case ESP_ERR_ESPNOW_NO_MEM:
    Serial.println("Out of memory");
    break;

  case ESP_ERR_ESPNOW_NOT_FOUND:
    Serial.println("Peer is not found");
    break;

  case ESP_ERR_ESPNOW_IF:
    Serial.println("Current WiFi interface doesn't match that of peer");
    break;

  default:
    break;
  }
}
static void send_msg(esp_now_msg_t *msg)
{
  // Pack
  uint16_t packet_size = sizeof(esp_now_msg_t);
  uint8_t msg_data[packet_size];
  memcpy(&msg_data[0], msg, sizeof(esp_now_msg_t));

  esp_err_t status = esp_now_send(broadcast_mac, msg_data, packet_size);
  if (ESP_OK != status)
  {
    Serial.println("Error sending message");
    handle_error(status);
  }
  Serial.println("Message sent!");
}
void message(){
          static uint32_t counter = 0;
          esp_now_msg_t msg;
          msg.address = 0;
          msg.counter = ++counter;
          send_msg(&msg);
          //digitalWrite(LED_PIN, !digitalRead(LED_PIN));
}
static void msg_recv_cb(const uint8_t *mac_addr, const uint8_t *data, int len)
{
  if (len == sizeof(esp_now_msg_t))
  {
    esp_now_msg_t msg;
    memcpy(&msg, data, len);

    Serial.print("Counter: ");
    Serial.println(msg.counter);
    //playTone();
    //digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        message();
        

  }
}

static void msg_send_cb(const uint8_t *mac, esp_now_send_status_t sendStatus)
{

  switch (sendStatus)
  {
  case ESP_NOW_SEND_SUCCESS:
    Serial.println("Send success");
    break;

  case ESP_NOW_SEND_FAIL:
    Serial.println("Send Failure");
    break;

  default:
    break;
  }
}


void lcdMessage(String msg)
{

  Serial.println(msg);
  // tft.fontHeight(2);
  // tft.setRotation(1);
  // //tft.fillScreen(TFT_BLACK);
  // tft.drawString(msg, tft.width()/4, tft.height() / 2, 4);  //string,start x,start y, font weight {1;2;4;6;7;8}
}
static void network_setup(void)
{
  //Puts ESP in STATION MODE
  //WiFi.mode(WIFI_STA);
  //WiFi.disconnect();

  if (esp_now_init() != 0)
  {
    Serial.println("esp_now_init()!=0");
    return;
  }

  esp_now_peer_info_t peer_info;
  //peer_info.channel = WIFI_CHANNEL;
  memcpy(peer_info.peer_addr, broadcast_mac, 6);
  peer_info.ifidx = ESP_IF_WIFI_STA;
  peer_info.encrypt = false;
  esp_err_t status = esp_now_add_peer(&peer_info);
  if (ESP_OK != status)
  {
    Serial.println("Could not add peer");
    handle_error(status);
  }

  // Set up callback
  status = esp_now_register_recv_cb(msg_recv_cb);
  if (ESP_OK != status)
  {
    Serial.println("Could not register callback");
    handle_error(status);
  }

  status = esp_now_register_send_cb(msg_send_cb);
  if (ESP_OK != status)
  {
    Serial.println("Could not register send callback");
    handle_error(status);
  }
}

void setupWifi()
{
  WiFi.mode(WIFI_STA);

  AutoWifi a;

#ifdef FACTORYRESET_BUTTON
  pinMode(FACTORYRESET_BUTTON, INPUT);
  if (!digitalRead(FACTORYRESET_BUTTON)) // 1 means not pressed
    a.resetProvisioning();
#endif

  if (!a.isProvisioned())
    lcdMessage("Setup wifi!");
  else
    lcdMessage(String("join ") + a.getSSID());

  a.startWifi();

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(F("."));
  }
  ip = WiFi.localIP();
  Serial.println(F("WiFi connected"));
  Serial.println(ip);
  lcdMessage(ip.toString());
}

void setup()
{
  // put your setup code here, to run once:
  lcdMessage("booting");

  Serial.begin(115200);
  while (!Serial)
  {
    ;
  }

  setupWifi();
  network_setup();
  Serial.println("Listening...");
  //tft.init();
}



void loop()
{
  if (millis() >= time_now + 200 + 1)
  {
    time_now += period;
    //Serial.println("core 1 loop on reciever");
        Serial.print("1.");
            //Serial.println(WiFi.channel());


  }

  // // put your main code here, to run repeatedly:
  // tone(BUZZER_PIN, NOTE_C4, 500, BUZZER_CHANNEL);
  // noTone(BUZZER_PIN, BUZZER_CHANNEL);
  // tone(BUZZER_PIN, NOTE_D4, 500, BUZZER_CHANNEL);
  // noTone(BUZZER_PIN, BUZZER_CHANNEL);
  // tone(BUZZER_PIN, NOTE_E4, 500, BUZZER_CHANNEL);
  // noTone(BUZZER_PIN, BUZZER_CHANNEL);
  // tone(BUZZER_PIN, NOTE_F4, 500, BUZZER_CHANNEL);
  // noTone(BUZZER_PIN, BUZZER_CHANNEL);
  // tone(BUZZER_PIN, NOTE_G4, 500, BUZZER_CHANNEL);
  // noTone(BUZZER_PIN, BUZZER_CHANNEL);
  // tone(BUZZER_PIN, NOTE_A4, 500, BUZZER_CHANNEL);
  // noTone(BUZZER_PIN, BUZZER_CHANNEL);
  // tone(BUZZER_PIN, NOTE_B4, 500, BUZZER_CHANNEL);
  // noTone(BUZZER_PIN, BUZZER_CHANNEL);
}
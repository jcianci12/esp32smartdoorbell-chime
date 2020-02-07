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
#include <WiFiClient.h>
#include <WebServer.h>

#include <HTTPClient.h>

WebServer server(80);

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
#define BUZZER_PIN 22
#define BUZZER_CHANNEL 0
////////////////////////
#define FACTORYRESET_BUTTON 35

///////////////////////Screen Setup

////////////////////////////


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

static void playTone()
{
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



void lcdMessage(String msg)
{

  Serial.println(msg);
  // tft.fontHeight(2);
  // tft.setRotation(1);
  // //tft.fillScreen(TFT_BLACK);
  // tft.drawString(msg, tft.width()/4, tft.height() / 2, 4);  //string,start x,start y, font weight {1;2;4;6;7;8}
}


void setupWifi()
{
  WiFi.disconnect(true); // disconnects STA Mode
  delay(1000);
  WiFi.softAPdisconnect(true); // disconnects AP Mode
  delay(1000);
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
String SendHTML(String message){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>LED Control</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #3498db;}\n";
  ptr +=".button-on:active {background-color: #2980b9;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>ESP32 Web Server</h1>\n";
  ptr +="<h3>Using Access Point(AP) Mode</h3>\n";
  
  {ptr +="<p>LED1 Status: ON</p><a class=\"button button-off\" href=\"/led1off\">OFF</a>\n";}

  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;

}

void sendReponse(void){
 playTone();
   //Serial.println("GPIO4 Status: OFF | GPIO5 Status: OFF");
  server.send(200, "text/html", SendHTML("Hit!")); 
}


void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

void setup()
{

#ifdef ENABLE_OLED
  hasDisplay = display.init();
  if (hasDisplay)
  {
    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_16);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
  }
#endif
  lcdMessage("booting");

  Serial.begin(115200);
  while (!Serial)
  {
    ;
  }

 
  IPAddress ip;



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
  server.on("/", HTTP_GET, sendReponse);
    server.onNotFound(handle_NotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop()
{
   server.handleClient();

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
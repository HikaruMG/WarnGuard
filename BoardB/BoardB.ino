/*  --Credit--

    Idol - Yoasobi (Buildup + chorus part) using Arduino UNO buzzer
    Made in Arduino IDE by Iwan_Aizakku

    RTC-Code From www.cybertice.com/article/677/

    Fur Elise
    Connect a piezo buzzer or speaker to pin 11 or select a new pin.
    More songs available at https://github.com/robsoncouto/arduino-songs
    - Robson Couto, 2019
*/

// Include 
#include <WiFi.h>
#include <PubSubClient.h>
#include "config.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>
#include "pitches.h"  //Contains notes as presets for buzzer

// Defines
#define I2CADDR 0x20
#define DS3231_I2C_ADDRESS 0x68
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C
#define TOPIC_PASSCODE TOPIC_PREFIX "/pass"
#define TOPIC_ST TOPIC_PREFIX "/start"
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
WiFiClient wifiClient;
PubSubClient mqtt(MQTT_BROKER, 1883, wifiClient);
uint32_t last_publish;


const int melody[] PROGMEM = {
  NOTE_E5, 16, NOTE_DS5, 16, //1
  NOTE_E5, 16, NOTE_DS5, 16, NOTE_E5, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_C5, 16,
  NOTE_A4, -8, NOTE_C4, 16, NOTE_E4, 16, NOTE_A4, 16,
  NOTE_B4, -8, NOTE_E4, 16, NOTE_GS4, 16, NOTE_B4, 16,
  NOTE_C5, 8,  REST, 16, NOTE_E4, 16, NOTE_E5, 16,  NOTE_DS5, 16,
  
  NOTE_E5, 16, NOTE_DS5, 16, NOTE_E5, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_C5, 16,//6
  NOTE_A4, -8, NOTE_C4, 16, NOTE_E4, 16, NOTE_A4, 16, 
  NOTE_B4, -8, NOTE_E4, 16, NOTE_C5, 16, NOTE_B4, 16, 
  NOTE_A4 , 4, REST, 8, //9 - 1st ending

  //repaets from 1 ending on 10
  NOTE_E5, 16, NOTE_DS5, 16, //1
  NOTE_E5, 16, NOTE_DS5, 16, NOTE_E5, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_C5, 16,
  NOTE_A4, -8, NOTE_C4, 16, NOTE_E4, 16, NOTE_A4, 16,
  NOTE_B4, -8, NOTE_E4, 16, NOTE_GS4, 16, NOTE_B4, 16,
  NOTE_C5, 8,  REST, 16, NOTE_E4, 16, NOTE_E5, 16,  NOTE_DS5, 16,
  
  NOTE_E5, 16, NOTE_DS5, 16, NOTE_E5, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_C5, 16,//6
  NOTE_A4, -8, NOTE_C4, 16, NOTE_E4, 16, NOTE_A4, 16, 
  NOTE_B4, -8, NOTE_E4, 16, NOTE_C5, 16, NOTE_B4, 16, 
  NOTE_A4, 8, REST, 16, NOTE_B4, 16, NOTE_C5, 16, NOTE_D5, 16, //10 - 2nd ending
  //continues from 11
  NOTE_E5, -8, NOTE_G4, 16, NOTE_F5, 16, NOTE_E5, 16, 
  NOTE_D5, -8, NOTE_F4, 16, NOTE_E5, 16, NOTE_D5, 16, //12
  
  NOTE_C5, -8, NOTE_E4, 16, NOTE_D5, 16, NOTE_C5, 16, //13
  NOTE_B4, 8, REST, 16, NOTE_E4, 16, NOTE_E5, 16, REST, 16,
  REST, 16, NOTE_E5, 16, NOTE_E6, 16, REST, 16, REST, 16, NOTE_DS5, 16,
  NOTE_E5, 16, REST, 16, REST, 16, NOTE_DS5, 16, NOTE_E5, 16, NOTE_DS5, 16,
  NOTE_E5, 16, NOTE_DS5, 16, NOTE_E5, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_C5, 16,
  NOTE_A4, 8, REST, 16, NOTE_C4, 16, NOTE_E4, 16, NOTE_A4, 16,
  
  NOTE_B4, 8, REST, 16, NOTE_E4, 16, NOTE_GS4, 16, NOTE_B4, 16, //19
  NOTE_C5, 8, REST, 16, NOTE_E4, 16, NOTE_E5, 16,  NOTE_DS5, 16,
  NOTE_E5, 16, NOTE_DS5, 16, NOTE_E5, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_C5, 16,
  NOTE_A4, 8, REST, 16, NOTE_C4, 16, NOTE_E4, 16, NOTE_A4, 16,
  NOTE_B4, 8, REST, 16, NOTE_E4, 16, NOTE_C5, 16, NOTE_B4, 16,
  NOTE_A4, 8, REST, 16, NOTE_B4, 16, NOTE_C5, 16, NOTE_D5, 16, //24 (1st ending)
  
  //repeats from 11
  NOTE_E5, -8, NOTE_G4, 16, NOTE_F5, 16, NOTE_E5, 16, 
  NOTE_D5, -8, NOTE_F4, 16, NOTE_E5, 16, NOTE_D5, 16, //12
  
  NOTE_C5, -8, NOTE_E4, 16, NOTE_D5, 16, NOTE_C5, 16, //13
  NOTE_B4, 8, REST, 16, NOTE_E4, 16, NOTE_E5, 16, REST, 16,
  REST, 16, NOTE_E5, 16, NOTE_E6, 16, REST, 16, REST, 16, NOTE_DS5, 16,
  NOTE_E5, 16, REST, 16, REST, 16, NOTE_DS5, 16, NOTE_E5, 16, NOTE_DS5, 16,
  NOTE_E5, 16, NOTE_DS5, 16, NOTE_E5, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_C5, 16,
  NOTE_A4, 8, REST, 16, NOTE_C4, 16, NOTE_E4, 16, NOTE_A4, 16,
  
  NOTE_B4, 8, REST, 16, NOTE_E4, 16, NOTE_GS4, 16, NOTE_B4, 16, //19
  NOTE_C5, 8, REST, 16, NOTE_E4, 16, NOTE_E5, 16,  NOTE_DS5, 16,
  NOTE_E5, 16, NOTE_DS5, 16, NOTE_E5, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_C5, 16,
  NOTE_A4, 8, REST, 16, NOTE_C4, 16, NOTE_E4, 16, NOTE_A4, 16,
  NOTE_B4, 8, REST, 16, NOTE_E4, 16, NOTE_C5, 16, NOTE_B4, 16,
  NOTE_A4, 8, REST, 16, NOTE_C5, 16, NOTE_C5, 16, NOTE_C5, 16, //25 - 2nd ending

  //continues from 26
  NOTE_C5 , 4, NOTE_F5, -16, NOTE_E5, 32, //26
  NOTE_E5, 8, NOTE_D5, 8, NOTE_AS5, -16, NOTE_A5, 32,
  NOTE_A5, 16, NOTE_G5, 16, NOTE_F5, 16, NOTE_E5, 16, NOTE_D5, 16, NOTE_C5, 16,
  NOTE_AS4, 8, NOTE_A4, 8, NOTE_A4, 32, NOTE_G4, 32, NOTE_A4, 32, NOTE_B4, 32,
  NOTE_C5 , 4, NOTE_D5, 16, NOTE_DS5, 16,
  NOTE_E5, -8, NOTE_E5, 16, NOTE_F5, 16, NOTE_A4, 16,
  NOTE_C5 , 4,  NOTE_D5, -16, NOTE_B4, 32,
  
  
  NOTE_C5, 32, NOTE_G5, 32, NOTE_G4, 32, NOTE_G5, 32, NOTE_A4, 32, NOTE_G5, 32, NOTE_B4, 32, NOTE_G5, 32, NOTE_C5, 32, NOTE_G5, 32, NOTE_D5, 32, NOTE_G5, 32, //33
  NOTE_E5, 32, NOTE_G5, 32, NOTE_C6, 32, NOTE_B5, 32, NOTE_A5, 32, NOTE_G5, 32, NOTE_F5, 32, NOTE_E5, 32, NOTE_D5, 32, NOTE_G5, 32, NOTE_F5, 32, NOTE_D5, 32,
  NOTE_C5, 32, NOTE_G5, 32, NOTE_G4, 32, NOTE_G5, 32, NOTE_A4, 32, NOTE_G5, 32, NOTE_B4, 32, NOTE_G5, 32, NOTE_C5, 32, NOTE_G5, 32, NOTE_D5, 32, NOTE_G5, 32,

  NOTE_E5, 32, NOTE_G5, 32, NOTE_C6, 32, NOTE_B5, 32, NOTE_A5, 32, NOTE_G5, 32, NOTE_F5, 32, NOTE_E5, 32, NOTE_D5, 32, NOTE_G5, 32, NOTE_F5, 32, NOTE_D5, 32, //36
  NOTE_E5, 32, NOTE_F5, 32, NOTE_E5, 32, NOTE_DS5, 32, NOTE_E5, 32, NOTE_B4, 32, NOTE_E5, 32, NOTE_DS5, 32, NOTE_E5, 32, NOTE_B4, 32, NOTE_E5, 32, NOTE_DS5, 32,
  NOTE_E5, -8, NOTE_B4, 16, NOTE_E5, 16, NOTE_DS5, 16,
  NOTE_E5, -8, NOTE_B4, 16, NOTE_E5, 16, REST, 16,

  REST, 16, NOTE_DS5, 16, NOTE_E5, 16, REST, 16, REST, 16, NOTE_DS5, 16, //40
  NOTE_E5, 16, NOTE_DS5, 16, NOTE_E5, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_C5, 16,
  NOTE_A4, 8, REST, 16, NOTE_C4, 16, NOTE_E4, 16, NOTE_A4, 16,
  NOTE_B4, 8, REST, 16, NOTE_E4, 16, NOTE_GS4, 16, NOTE_B4, 16,
  NOTE_C5, 8, REST, 16, NOTE_E4, 16, NOTE_E5, 16, NOTE_DS5, 16,
  NOTE_E5, 16, NOTE_DS5, 16, NOTE_E5, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_C5, 16,

  NOTE_A4, 8, REST, 16, NOTE_C4, 16, NOTE_E4, 16, NOTE_A4, 16, //46
  NOTE_B4, 8, REST, 16, NOTE_E4, 16, NOTE_C5, 16, NOTE_B4, 16,
  NOTE_A4, 8, REST, 16, NOTE_B4, 16, NOTE_C5, 16, NOTE_D5, 16,
  NOTE_E5, -8, NOTE_G4, 16, NOTE_F5, 16, NOTE_E5, 16,
  NOTE_D5, -8, NOTE_F4, 16, NOTE_E5, 16, NOTE_D5, 16,
  NOTE_C5, -8, NOTE_E4, 16, NOTE_D5, 16, NOTE_C5, 16,
  NOTE_B4, 8, REST, 16, NOTE_E4, 16, NOTE_E5, 16, REST, 16,
  REST, 16, NOTE_E5, 16, NOTE_E6, 16, REST, 16, REST, 16, NOTE_DS5, 16,

  NOTE_E5, 16, REST, 16, REST, 16, NOTE_DS5, 16, NOTE_E5, 16, NOTE_D5, 16, //54
  NOTE_E5, 16, NOTE_DS5, 16, NOTE_E5, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_C5, 16,
  NOTE_A4, 8, REST, 16, NOTE_C4, 16, NOTE_E4, 16, NOTE_A4, 16,
  NOTE_B4, 8, REST, 16, NOTE_E4, 16, NOTE_GS4, 16, NOTE_B4, 16,
  NOTE_C5, 8, REST, 16, NOTE_E4, 16, NOTE_E5, 16, NOTE_DS5, 16,
  NOTE_E5, 16, NOTE_DS5, 16, NOTE_E5, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_C5, 16,
  
  NOTE_A4, 8, REST, 16, NOTE_C4, 16, NOTE_E4, 16, NOTE_A4, 16, //60
  NOTE_B4, 8, REST, 16, NOTE_E4, 16, NOTE_C5, 16, NOTE_B4, 16,
  NOTE_A4, 8, REST, 16, REST, 16, REST, 8, 
  NOTE_CS5 , -4, 
  NOTE_D5 , 4, NOTE_E5, 16, NOTE_F5, 16,
  NOTE_F5 , 4, NOTE_F5, 8, 
  NOTE_E5 , -4,
  NOTE_D5 , 4, NOTE_C5, 16, NOTE_B4, 16,
  NOTE_A4 , 4, NOTE_A4, 8,
  NOTE_A4, 8, NOTE_C5, 8, NOTE_B4, 8,
  NOTE_A4 , -4,
  NOTE_CS5 , -4,

  NOTE_D5 , 4, NOTE_E5, 16, NOTE_F5, 16, //72
  NOTE_F5 , 4, NOTE_F5, 8,
  NOTE_F5 , -4,
  NOTE_DS5 , 4, NOTE_D5, 16, NOTE_C5, 16,
  NOTE_AS4 , 4, NOTE_A4, 8,
  NOTE_GS4 , 4, NOTE_G4, 8,
  NOTE_A4 , -4,
  NOTE_B4 , 4, REST, 8,
  NOTE_A3, -32, NOTE_C4, -32, NOTE_E4, -32, NOTE_A4, -32, NOTE_C5, -32, NOTE_E5, -32, NOTE_D5, -32, NOTE_C5, -32, NOTE_B4, -32,

  NOTE_A4, -32, NOTE_C5, -32, NOTE_E5, -32, NOTE_A5, -32, NOTE_C6, -32, NOTE_E6, -32, NOTE_D6, -32, NOTE_C6, -32, NOTE_B5, -32, //80
  NOTE_A4, -32, NOTE_C5, -32, NOTE_E5, -32, NOTE_A5, -32, NOTE_C6, -32, NOTE_E6, -32, NOTE_D6, -32, NOTE_C6, -32, NOTE_B5, -32,
  NOTE_AS5, -32, NOTE_A5, -32, NOTE_GS5, -32, NOTE_G5, -32, NOTE_FS5, -32, NOTE_F5, -32, NOTE_E5, -32, NOTE_DS5, -32, NOTE_D5, -32,

  NOTE_CS5, -32, NOTE_C5, -32, NOTE_B4, -32, NOTE_AS4, -32, NOTE_A4, -32, NOTE_GS4, -32, NOTE_G4, -32, NOTE_FS4, -32, NOTE_F4, -32, //84
  NOTE_E4, 16, NOTE_DS5, 16, NOTE_E5, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_C5, 16,
  NOTE_A4, -8, NOTE_C4, 16, NOTE_E4, 16, NOTE_A4, 16,
  NOTE_B4, -8, NOTE_E4, 16, NOTE_GS4, 16, NOTE_B4, 16,

  NOTE_C5, 8, REST, 16, NOTE_E4, 16, NOTE_E5, 16, NOTE_DS5, 16, //88
  NOTE_E5, 16, NOTE_DS5, 16, NOTE_E5, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_C5, 16, 
  NOTE_A4, -8, NOTE_C4, 16, NOTE_E4, 16, NOTE_A4, 16, 
  NOTE_B4, -8, NOTE_E4, 16, NOTE_C5, 16, NOTE_B4, 16, 
  NOTE_A4, -8, REST, -8,
  REST, -8, NOTE_G4, 16, NOTE_F5, 16, NOTE_E5, 16,
  NOTE_D5 , 4, REST, 8,
  REST, -8, NOTE_E4, 16, NOTE_D5, 16, NOTE_C5, 16,
  
  NOTE_B4, -8, NOTE_E4, 16, NOTE_E5, 8, //96
  NOTE_E5, 8, NOTE_E6, -8, NOTE_DS5, 16,
  NOTE_E5, 16, REST, 16, REST, 16, NOTE_DS5, 16, NOTE_E5, 16, NOTE_DS5, 16,
  NOTE_E5, 16, NOTE_DS5, 16, NOTE_E5, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_C5, 16,
  NOTE_A4, -8, NOTE_C4, 16, NOTE_E4, 16, NOTE_A4, 16,
  NOTE_B4, -8, NOTE_E4, 16, NOTE_GS4, 16, NOTE_B4, 16,

  NOTE_C5, 8, REST, 16, NOTE_E4, 16, NOTE_E5, 16, NOTE_DS5, 16, //102
  NOTE_E5, 16, NOTE_DS5, 16, NOTE_E5, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_C5, 16,
  NOTE_A4, -8, NOTE_C4, 16, NOTE_E4, 16, NOTE_A4, 16,
  NOTE_B4, -8, NOTE_E4, 16, NOTE_C5, 16, NOTE_B4, 16,
  NOTE_A4 , -4,
};

const int buzzer = 4; //buzzer is on pin 8
const int buttonsw1 = 1;
int buttonState = 0;
int buttonStart = 1;
int checkstart = 0;
int checkpush = 0;
int tempo = 80;
int notes = sizeof(melody) / sizeof(melody[0]) / 2;
int wholenote = (60000 * 4) / tempo;
int divider = 0, noteDuration = 0;
int a;
int b;
int c;
int d;


String password = "0000";

void setup() {
  Serial.begin(9600);
  Wire.begin(48, 47);
  connect_wifi();
  connect_mqtt();
  last_publish = 0;
  pinMode(buttonsw1, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
}

void loop() {
  digitalWrite(buzzer,LOW);
  mqtt.loop();
  introMessage();
}

void connect_wifi() {
  printf("WiFi MAC address is %s\n", WiFi.macAddress().c_str());
  printf("Connecting to WiFi %s.\n", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    printf(".");
    fflush(stdout);
    delay(500);
  }
  printf("\nWiFi connected.\n");
}

void connect_mqtt() {
  printf("Connecting to MQTT broker at %s.\n", MQTT_BROKER);
  if (!mqtt.connect("", MQTT_USER, MQTT_PASS)) {
    printf("Failed to connect to MQTT broker.\n");
    for (;;) {} // wait here forever
  }
  mqtt.setCallback(mqtt_callback);
  mqtt.subscribe(TOPIC_PASSCODE);
  mqtt.subscribe(TOPIC_ST);
  printf("MQTT broker connected.\n");
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  if (strcmp(topic, TOPIC_ST) == 0) {
    payload[length] = 0;
    int value = atoi((char*)payload); 
    if (value == 1) {
      Serial.println("Game Request to Start");
      ranpass();
      pubpass();
    }
    else{
      Serial.println("Stop the game");
    }
  }
}

//Idol-Yoasobii
void song1(){
  tone(buzzer, NOTE_GS4);
  delay(194);
  tone(buzzer, NOTE_DS5);
  delay(194);
  tone(buzzer, NOTE_CS5);
  delay(194);
  noTone(buzzer);
  delay(190);
  tone(buzzer, NOTE_CS5);
  delay(194);
  noTone(buzzer);
  delay(190);
  tone(buzzer, NOTE_CS5);
  delay(194);
  noTone(buzzer);
  delay(110);
  tone(buzzer, NOTE_CS5);
  delay(194);
  noTone(buzzer);
  delay(110);
  tone(buzzer, NOTE_CS5);
  delay(194);
  noTone(buzzer);
  delay(190);
  tone(buzzer, NOTE_B4);
  delay(194);
  tone(buzzer, NOTE_CS5);
  delay(194);
  tone(buzzer, NOTE_B4);
  delay(194);
  tone(buzzer, NOTE_CS5);
  delay(194);
  tone(buzzer, NOTE_B4);
  delay(194);
  tone(buzzer, NOTE_CS5);
  delay(194);


  // Mata suki ni saseru
  tone(buzzer, NOTE_E5);
  delay(194);
  tone(buzzer, NOTE_DS5);
  delay(194);
  noTone(buzzer);
  delay(360);
  tone(buzzer, NOTE_E5);
  delay(194);
  tone(buzzer, NOTE_DS5);
  delay(194);
  noTone(buzzer);
  delay(200);
  tone(buzzer, NOTE_GS5);
  delay(194);
  tone(buzzer, NOTE_G5);
  delay(194);
  noTone(buzzer);
  delay(194);
  tone(buzzer, NOTE_GS5);
  delay(194);
  tone(buzzer, NOTE_G5);
  delay(194);
  tone(buzzer, NOTE_GS5);
  delay(194);
  tone(buzzer, NOTE_AS5);
  delay(194);
  noTone(buzzer);
  delay(194);


  // Daremo ga me wo ubawareteiku
  tone(buzzer, NOTE_E4);
  delay(194);
  tone(buzzer, NOTE_G4);
  delay(194);
  tone(buzzer, NOTE_A4);
  delay(194);
  noTone(buzzer);
  delay(194);
  tone(buzzer, NOTE_E5);
  delay(194);
  noTone(buzzer);
  delay(194);
  tone(buzzer, NOTE_A4);
  delay(194);
  noTone(buzzer);
  delay(194);
  tone(buzzer, NOTE_G4);
  delay(194);
  tone(buzzer, NOTE_A4);
  delay(194);
  noTone(buzzer);
  delay(194);
  tone(buzzer, NOTE_E5);
  delay(194);
  tone(buzzer, NOTE_A4);
  delay(194);
  tone(buzzer, NOTE_G4);
  delay(194);
  tone(buzzer, NOTE_A4);
  delay(194);
  noTone(buzzer);
  delay(194);


  // Kimi wa kanpeki de kyuukyoku no aidoru
  tone(buzzer, NOTE_E4);
  delay(194);
  tone(buzzer, NOTE_G4);
  delay(194);
  tone(buzzer, NOTE_A4);
  delay(194);
  noTone(buzzer);
  delay(194);
  tone(buzzer, NOTE_C5);
  delay(194);
  noTone(buzzer);
  delay(194);
  tone(buzzer, NOTE_B4);
  delay(194);
  noTone(buzzer);
  delay(194);
  tone(buzzer, NOTE_G4);
  delay(194);
  tone(buzzer, NOTE_A4);
  delay(194);
  noTone(buzzer);
  delay(194);
  tone(buzzer, NOTE_B4);
  delay(194);
  tone(buzzer, NOTE_C5);
  delay(194);
  tone(buzzer, NOTE_D5);
  delay(194);
  tone(buzzer, NOTE_F5);
  delay(200);
  noTone(buzzer);
  delay(194);
  tone(buzzer, NOTE_E5);
  delay(194);
  noTone(buzzer);
  delay(194);


  // Konrinzai arawarenai
  tone(buzzer, NOTE_A4);
  delay(194);
  noTone(buzzer);
  delay(194);
  tone(buzzer, NOTE_E5);
  delay(194);
  noTone(buzzer);
  delay(194);
  tone(buzzer, NOTE_A4);
  delay(194);
  noTone(buzzer);
  delay(194);
  tone(buzzer, NOTE_G4);
  delay(194);
  tone(buzzer, NOTE_A4);
  delay(194);
  noTone(buzzer);
  delay(194);
  tone(buzzer, NOTE_E5);
  delay(194);
  tone(buzzer, NOTE_A4);
  delay(194);
  tone(buzzer, NOTE_G4);
  delay(194);
  tone(buzzer, NOTE_A4);
  delay(194);
  noTone(buzzer);
  delay(194);


  // Ichibanboshi no umarekawari
  tone(buzzer, NOTE_E4);
  delay(194);
  tone(buzzer, NOTE_G4);
  delay(194);
  tone(buzzer, NOTE_A4);
  delay(194);
  noTone(buzzer);
  delay(194);
  tone(buzzer, NOTE_C5);
  delay(194);
  noTone(buzzer);
  delay(194);
  tone(buzzer, NOTE_B4);
  delay(194);
  noTone(buzzer);
  delay(194);
  tone(buzzer, NOTE_G4);
  delay(194);
  tone(buzzer, NOTE_A4);
  delay(194);
  noTone(buzzer);
  delay(194);
  tone(buzzer, NOTE_A4);
  delay(194);
  noTone(buzzer);
  delay(194);
  tone(buzzer, NOTE_G4);
  delay(194);
  tone(buzzer, NOTE_A4);
  delay(194);
  noTone(buzzer);
  delay(194); 

  // Ah, sono egao de aishiteru de
  tone(buzzer, NOTE_C5);
  delay(194);
  noTone(buzzer);
  delay(194);
  tone(buzzer, NOTE_A4);
  delay(200);
  noTone(buzzer);
  delay(380);
  tone(buzzer, NOTE_G4);
  delay(194);
  tone(buzzer, NOTE_A4);
  delay(194);
  tone(buzzer, NOTE_C5);
  delay(194);
  tone(buzzer, NOTE_D5);
  delay(194);
  tone(buzzer, NOTE_E5);
  delay(194);
  tone(buzzer, NOTE_A4);
  delay(194);
  noTone(buzzer);
  delay(380);
  tone(buzzer, NOTE_G4);
  delay(194);
  tone(buzzer, NOTE_A4);
  delay(194);
  tone(buzzer, NOTE_C5);
  delay(194);
  tone(buzzer, NOTE_D5);
  delay(194);
  tone(buzzer, NOTE_E5);
  delay(194);
  tone(buzzer, NOTE_A4);
  delay(194);
  noTone(buzzer);
  delay(194);

  // Daremo kare mo toriko ni shite iku 
  tone(buzzer, NOTE_C5);
  delay(194);
  noTone(buzzer);
  delay(194);
  tone(buzzer, NOTE_D5);
  delay(194);
  tone(buzzer, NOTE_C5);
  delay(194);
  tone(buzzer, NOTE_D5);
  delay(194);
  tone(buzzer, NOTE_C5);
  delay(194);
  tone(buzzer, NOTE_D5);
  delay(194);
  tone(buzzer, NOTE_E5);
  delay(194);
  tone(buzzer, NOTE_A4);
  delay(194);
  tone(buzzer, NOTE_C5);
  delay(194);
  tone(buzzer, NOTE_G4);
  delay(194);
  tone(buzzer, NOTE_A4);
  delay(194);
  tone(buzzer, NOTE_E4);
  delay(194);
  tone(buzzer, NOTE_G4);
  delay(194);
  tone(buzzer, NOTE_A4);
  delay(200);
  noTone(buzzer);
  delay(350);

  // Sono hitomi ga
  tone(buzzer, NOTE_G4);
  delay(194);
  tone(buzzer, NOTE_A4);
  delay(194);
  tone(buzzer, NOTE_C5);
  delay(194);
  tone(buzzer, NOTE_D5);
  delay(194);
  tone(buzzer, NOTE_E5);
  delay(194);
  tone(buzzer, NOTE_A4);
  delay(220);
  noTone(buzzer);
  delay(350);

  // Sono kotoba ga uso demo
  // Sore wa kanzen na ai
  tone(buzzer, NOTE_G4);
  delay(80);
  noTone(buzzer);
  delay(30);
  tone(buzzer, NOTE_G4);
  delay(100);
  tone(buzzer, NOTE_A4);
  delay(200);
  tone(buzzer, NOTE_C5);
  delay(194);
  tone(buzzer, NOTE_D5);
  delay(194);
  tone(buzzer, NOTE_E5);
  delay(200);
  noTone(buzzer);
  delay(8);
  tone(buzzer, NOTE_D5);
  delay(194);
  tone(buzzer, NOTE_E5);
  delay(194);
  tone(buzzer, NOTE_A4);
  delay(194);
  tone(buzzer, NOTE_C5);
  delay(194);
  tone(buzzer, NOTE_G4);
  delay(194);
  tone(buzzer, NOTE_A4);
  delay(194);
  tone(buzzer, NOTE_E4);
  delay(194);
  tone(buzzer, NOTE_A4);
  delay(194);
  noTone(buzzer);
  delay(194);
  tone(buzzer, NOTE_A4);
  delay(194);
  noTone(buzzer);
  delay(194);
  tone(buzzer, NOTE_G4);
  delay(194);
  tone(buzzer, NOTE_A4);
  delay(220);
  noTone(buzzer);
}
//
void song2(){
  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {

    // calculates the duration of each note
    divider = pgm_read_word_near(melody+thisNote + 1);
    if (divider > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(buzzer, pgm_read_word_near(melody+thisNote), noteDuration * 0.9);

    // Wait for the specief duration before playing the next note.
    delay(noteDuration);

    // stop the waveform generation before the next note.
    noTone(buzzer);
  }
}
void introMessage(){
  display.clearDisplay();
  display.setTextSize(2);             // Draw 2X-scale text
  display.setTextColor(BLACK, WHITE);
  display.setCursor(10,5);
  display.println("Get Up!");
  

  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
  display.setCursor(5,45);
  display.println("Push A To Enter Passcode");

  display.display();
}

void startgame(){
  String payload("1");
  mqtt.publish(TOPIC_ST, payload.c_str());
}

void ranpass(){
  a = random(0,9);
  b = random(0,9);
  c = random(0,9);
  d = random(0,9);
  password = String(a)+String(b)+String(c)+String(d);
  Serial.println(password);
}

void pubpass(){
  String payload(password);
  mqtt.publish(TOPIC_PASSCODE, payload.c_str());
}


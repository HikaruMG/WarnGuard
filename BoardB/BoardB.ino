/*  --Credit--

    Idol - Yoasobi (Buildup + chorus part) using Arduino UNO buzzer
    Made in Arduino IDE by Iwan_Aizakku

    RTC-Code From www.cybertice.com/article/677/

    Never Gonna Give You Up - Rick Astley
    Score available at https://musescore.com/chlorondria_5/never-gonna-give-you-up_alto-sax
    - Arranged by Chlorondria

    Dino-Game
    Author:    Harsh Mittal
    Created:   10.07.2021
    Github: www.github.com/harshmittal2210
    Email: harshmittal2210@gmail.com
    (c) Copyright by Harsh Mittal.
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
#define I2CADDR 0x20 //กำหนดช่องสัญญาณ
#define DS3231_I2C_ADDRESS 0x68 //กำหนดช่องสัญญาณ
#define SCREEN_WIDTH 128 // ความกว้างจอ OLED
#define SCREEN_HEIGHT 64 // ความสูงจอ OLED
#define OLED_RESET     4 // กำหนดช่องสัญญาณ Reset OLED
#define SCREEN_ADDRESS 0x3C //กำหนดช่องสัญญาณ

//-------------MQTT---------------
#define TOPIC_PASSCODE TOPIC_PREFIX "/pass"
#define TOPIC_ST TOPIC_PREFIX "/start"
#define TOPIC_SP TOPIC_PREFIX "/speed"
#define TOPIC_SG TOPIC_PREFIX "/song"

//-------------Dino---------------
#define DINO_WIDTH 25 //กว้าง
#define DINO_HEIGHT 26 //สูง
#define DINO_INIT_X 10 // กำหนดจุดเริ่มต้น
#define DINO_INIT_Y 29 // กำหนดจุดเริ่มต้น
#define BASE_LINE_X 0
#define BASE_LINE_Y 54
#define BASE_LINE_X1 127
#define BASE_LINE_Y1 54
#define TREE1_WIDTH 11
#define TREE1_HEIGHT 23
#define TREE2_WIDTH 22
#define TREE2_HEIGHT 23
#define TREE_Y 35
#define JUMP_PIXEL 22

//-------------Pixel Design----------------
static const unsigned char PROGMEM dino1[]={
  // 'dino', 25x26px
  0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xfe, 0x00, 0x00, 0x06, 0xff, 0x00, 0x00, 0x0e, 0xff, 0x00, 
  0x00, 0x0f, 0xff, 0x00, 0x00, 0x0f, 0xff, 0x00, 0x00, 0x0f, 0xff, 0x00, 0x00, 0x0f, 0xc0, 0x00, 
  0x00, 0x0f, 0xfc, 0x00, 0x40, 0x0f, 0xc0, 0x00, 0x40, 0x1f, 0x80, 0x00, 0x40, 0x7f, 0x80, 0x00, 
  0x60, 0xff, 0xe0, 0x00, 0x71, 0xff, 0xa0, 0x00, 0x7f, 0xff, 0x80, 0x00, 0x7f, 0xff, 0x80, 0x00, 
  0x7f, 0xff, 0x80, 0x00, 0x3f, 0xff, 0x00, 0x00, 0x1f, 0xff, 0x00, 0x00, 0x0f, 0xfe, 0x00, 0x00, 
  0x03, 0xfc, 0x00, 0x00, 0x01, 0xdc, 0x00, 0x00, 0x01, 0x8c, 0x00, 0x00, 0x01, 0x8c, 0x00, 0x00, 
  0x01, 0x0c, 0x00, 0x00, 0x01, 0x8e, 0x00, 0x00
}; 

static const unsigned char PROGMEM tree1[]={
  // 'tree1', 11x23px
  0x1e, 0x00, 0x1f, 0x00, 0x1f, 0x40, 0x1f, 0xe0, 0x1f, 0xe0, 0xdf, 0xe0, 0xff, 0xe0, 0xff, 0xe0, 
  0xff, 0xe0, 0xff, 0xe0, 0xff, 0xe0, 0xff, 0xe0, 0xff, 0xc0, 0xff, 0x00, 0xff, 0x00, 0x7f, 0x00, 
  0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00
};

static const unsigned char PROGMEM tree2[]={
  // 'tree2', 22x23px
  0x1e, 0x01, 0xe0, 0x1f, 0x03, 0xe0, 0x1f, 0x4f, 0xe8, 0x1f, 0xff, 0xfc, 0x1f, 0xff, 0xfc, 0xdf, 
  0xff, 0xfc, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xfc, 0xff, 0xff, 
  0xfc, 0xff, 0xef, 0xfc, 0xff, 0x83, 0xfc, 0xff, 0x03, 0xfc, 0xff, 0x03, 0xf8, 0x7f, 0x03, 0xe0, 
  0x1f, 0x03, 0xe0, 0x1f, 0x03, 0xe0, 0x1f, 0x03, 0xe0, 0x1f, 0x03, 0xe0, 0x1f, 0x03, 0xe0, 0x1f, 
  0x03, 0xe0, 0x1f, 0x03, 0xe0
};

int melody[] = {
  /*NOTE_D5,-4, NOTE_E5,-4, NOTE_A4,4, //1
  NOTE_E5,-4, NOTE_FS5,-4, NOTE_A5,16, NOTE_G5,16, NOTE_FS5,8,
  NOTE_D5,-4, NOTE_E5,-4, NOTE_A4,2,
  NOTE_A4,16, NOTE_A4,16, NOTE_B4,16, NOTE_D5,8, NOTE_D5,16,
  NOTE_D5,-4, NOTE_E5,-4, NOTE_A4,4, //repeat from 1
  NOTE_E5,-4, NOTE_FS5,-4, NOTE_A5,16, NOTE_G5,16, NOTE_FS5,8,
  NOTE_D5,-4, NOTE_E5,-4, NOTE_A4,2,
  NOTE_A4,16, NOTE_A4,16, NOTE_B4,16, NOTE_D5,8, NOTE_D5,16,
  REST,4, NOTE_B4,8, NOTE_CS5,8, NOTE_D5,8, NOTE_D5,8, NOTE_E5,8, NOTE_CS5,-8,
  NOTE_B4,16, NOTE_A4,2, REST,4, 

  REST,8, NOTE_B4,8, NOTE_B4,8, NOTE_CS5,8, NOTE_D5,8, NOTE_B4,4, NOTE_A4,8, //7
  NOTE_A5,8, REST,8, NOTE_A5,8, NOTE_E5,-4, REST,4, 
  NOTE_B4,8, NOTE_B4,8, NOTE_CS5,8, NOTE_D5,8, NOTE_B4,8, NOTE_D5,8, NOTE_E5,8, REST,8,
  REST,8, NOTE_CS5,8, NOTE_B4,8, NOTE_A4,-4, REST,4,
  REST,8, NOTE_B4,8, NOTE_B4,8, NOTE_CS5,8, NOTE_D5,8, NOTE_B4,8, NOTE_A4,4,
  NOTE_E5,8, NOTE_E5,8, NOTE_E5,8, NOTE_FS5,8, NOTE_E5,4, REST,4,
   
  NOTE_D5,2, NOTE_E5,8, NOTE_FS5,8, NOTE_D5,8, //13
  NOTE_E5,8, NOTE_E5,8, NOTE_E5,8, NOTE_FS5,8, NOTE_E5,4, NOTE_A4,4,
  REST,2, NOTE_B4,8, NOTE_CS5,8, NOTE_D5,8, NOTE_B4,8,
  REST,8, NOTE_E5,8, NOTE_FS5,8, NOTE_E5,-4, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
  NOTE_FS5,-8, NOTE_FS5,-8, NOTE_E5,-4, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,

  NOTE_E5,-8, NOTE_E5,-8, NOTE_D5,-8, NOTE_CS5,16, NOTE_B4,-8, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16, //18
  NOTE_D5,4, NOTE_E5,8, NOTE_CS5,-8, NOTE_B4,16, NOTE_A4,8, NOTE_A4,8, NOTE_A4,8, 
  NOTE_E5,4, NOTE_D5,2, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
  NOTE_FS5,-8, NOTE_FS5,-8, NOTE_E5,-4, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
  NOTE_A5,4, NOTE_CS5,8, NOTE_D5,-8, NOTE_CS5,16, NOTE_B4,8, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,

  NOTE_D5,4, NOTE_E5,8, NOTE_CS5,-8, NOTE_B4,16, NOTE_A4,4, NOTE_A4,8,  //23
  NOTE_E5,4, NOTE_D5,2, REST,4,
  REST,8, NOTE_B4,8, NOTE_D5,8, NOTE_B4,8, NOTE_D5,8, NOTE_E5,4, REST,8,
  REST,8, NOTE_CS5,8, NOTE_B4,8, NOTE_A4,-4, REST,4,
  REST,8, NOTE_B4,8, NOTE_B4,8, NOTE_CS5,8, NOTE_D5,8, NOTE_B4,8, NOTE_A4,4,
  REST,8, NOTE_A5,8, NOTE_A5,8, NOTE_E5,8, NOTE_FS5,8, NOTE_E5,8, NOTE_D5,8,
  
  REST,8, NOTE_A4,8, NOTE_B4,8, NOTE_CS5,8, NOTE_D5,8, NOTE_B4,8, //29
  REST,8, NOTE_CS5,8, NOTE_B4,8, NOTE_A4,-4, REST,4,
  NOTE_B4,8, NOTE_B4,8, NOTE_CS5,8, NOTE_D5,8, NOTE_B4,8, NOTE_A4,4, REST,8,
  REST,8, NOTE_E5,8, NOTE_E5,8, NOTE_FS5,4, NOTE_E5,-4, 
  NOTE_D5,2, NOTE_D5,8, NOTE_E5,8, NOTE_FS5,8, NOTE_E5,4, 
  NOTE_E5,8, NOTE_E5,8, NOTE_FS5,8, NOTE_E5,8, NOTE_A4,8, NOTE_A4,4,

  REST,-4, NOTE_A4,8, NOTE_B4,8, NOTE_CS5,8, NOTE_D5,8, NOTE_B4,8, //35
  REST,8, NOTE_E5,8, NOTE_FS5,8, NOTE_E5,-4, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
  NOTE_FS5,-8, NOTE_FS5,-8, NOTE_E5,-4, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
  NOTE_E5,-8, NOTE_E5,-8, NOTE_D5,-8, NOTE_CS5,16, NOTE_B4,8, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
  NOTE_D5,4, NOTE_E5,8, NOTE_CS5,-8, NOTE_B4,16, NOTE_A4,4, NOTE_A4,8, */

   NOTE_E5,4, NOTE_D5,2, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16, //40
  NOTE_FS5,-8, NOTE_FS5,-8, NOTE_E5,-4, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
  NOTE_A5,4, NOTE_CS5,8, NOTE_D5,-8, NOTE_CS5,16, NOTE_B4,8, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
  NOTE_D5,4, NOTE_E5,8, NOTE_CS5,-8, NOTE_B4,16, NOTE_A4,4, NOTE_A4,8,  
  NOTE_E5,4, NOTE_D5,2, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
   
  NOTE_FS5,-8, NOTE_FS5,-8, NOTE_E5,-4, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16, //45
  NOTE_A5,4, NOTE_CS5,8, NOTE_D5,-8, NOTE_CS5,16, NOTE_B4,8, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
  NOTE_D5,4, NOTE_E5,8, NOTE_CS5,-8, NOTE_B4,16, NOTE_A4,4, NOTE_A4,8,  
  NOTE_E5,4, NOTE_D5,2, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
  NOTE_FS5,-8, NOTE_FS5,-8, NOTE_E5,-4, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16, //45
  
  NOTE_A5,4, NOTE_CS5,8, NOTE_D5,-8, NOTE_CS5,16, NOTE_B4,8, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
  NOTE_D5,4, NOTE_E5,8, NOTE_CS5,-8, NOTE_B4,16, NOTE_A4,4, NOTE_A4,8, 

  NOTE_E5,4, NOTE_D5,2, REST,4
};

//------------Declare-------------------
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
WiFiClient wifiClient;
PubSubClient mqtt(MQTT_BROKER, 1883, wifiClient);
uint32_t last_publish;
int a;
int b;
int c;
int d;
const int buzzer = 4; //buzzer is on pin 4
const int buttonsw1 = 1;
int buttonState = 0;
int buttonStart = 1;
int checkstart = 0;
int checkpush = 0;
int gamestart = 1;
int rscore = 0;
int score = 0;
String password = "0000"; //รหัสเริ่มต้น
int tempo = 114; //ความเร็วเพลง
int notes = sizeof(melody) / sizeof(melody[0]) / 2;
int wholenote = (60000 * 4) / tempo; //จังหวะหยุด
int divider = 0, noteDuration = 0;
int rled = 19;
int yled = 20;
int gled = 21;
int checksong = 0;

void setup() {
  pubstart(); //ส่ง Status Public
  gamestart = 1;
  pinMode(rled, OUTPUT);
  pinMode(yled, OUTPUT);
  pinMode(gled, OUTPUT);
  digitalWrite(rled,HIGH);
  digitalWrite(yled,LOW);
  digitalWrite(gled,LOW);
  Serial.begin(9600);
  Wire.begin(48, 47); //กำหนด SDL,SDA
  connect_wifi();
  connect_mqtt();
  last_publish = 0;
  pinMode(buttonsw1, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);

  //---------OLED Check------------
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  mainshow(); //หน้าเริ่มต้น
}

void loop() {
  mqtt.loop();
  digitalWrite(buzzer,LOW);
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
  mqtt.subscribe(TOPIC_SP);
  mqtt.subscribe(TOPIC_SG);
  printf("MQTT broker connected.\n");
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  if (strcmp(topic, TOPIC_ST) == 0) {
    payload[length] = 0;
    int value = atoi((char*)payload); 
    if (value == 1) {
      gamestart = 1;
      Serial.println("Game Request to Start");
      digitalWrite(gled,HIGH);
      digitalWrite(yled,LOW);
      digitalWrite(rled,LOW);
      rscore = random(3,7); //สุ่มคะแนนที่ต้องการ
      ranpass(); //สุ่ม passcode
      pubpass(); //Public Passcode ให้ Board A
      introMessage(); //หน้าเริ่มเกม
      while(gamestart){
        score=0;
        buttonStart = digitalRead(buttonsw1);
        if(buttonStart==0){
          checkstart = 1;
          if (gamestart == 1){
          play();
          }
        }
      }
    }
    else{
      Serial.println("Stop the game");
    }
  }
  if (strcmp(topic, TOPIC_SP) == 0) {
    payload[length] = 0;
    int value = atoi((char*)payload);
    ranpass(); //สุ่ม passcode
    pubpass(); //Public Passcode ให้ Board A
  }
  if (strcmp(topic, TOPIC_SG) == 0) {
    payload[length] = 0; 
    int value = atoi((char*)payload); 
    if (value == 1) {
      checksong = 1;
      digitalWrite(yled,HIGH);
    }
    else{
      checksong = 0;
      digitalWrite(yled,LOW);
    }
  }
}

void song1(){
  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {

    // calculates the duration of each note
    divider = melody[thisNote + 1];
    if (divider > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(buzzer, melody[thisNote], noteDuration * 0.9);

    // Wait for the specief duration before playing the next note.
    delay(noteDuration);

    // stop the waveform generation before the next note.
    noTone(buzzer);
  }
}
//
void introMessage(){
  display.clearDisplay();
  display.setTextSize(2);       
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10,5);
  display.println("Dino Game");
  

  display.setTextSize(1);
  
  display.setCursor(5,45);
  display.println("Enter 1 To Play ");

  display.display();
}

//---------------Dino Game---------------------
void moveDino(int16_t *y, int type=0){
  display.drawBitmap(DINO_INIT_X, *y, dino1, DINO_WIDTH, DINO_HEIGHT, SSD1306_WHITE);
}
void moveTree(int16_t *x, int type=0){
  if(type==0){
    display.drawBitmap(*x, TREE_Y, tree1, TREE1_WIDTH, TREE1_HEIGHT, SSD1306_WHITE);
  }
  else if(type==1){
    display.drawBitmap(*x, TREE_Y, tree2, TREE2_WIDTH, TREE2_HEIGHT, SSD1306_WHITE);
  }
  
}

// Game over display with score
void gameOver(int score=0){
  display.clearDisplay();

  display.setTextSize(2);             // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10,5);
  display.println("Game Over");
  

  display.setTextSize(1);

  display.setCursor(10,30);
  display.print("Score: ");
  display.print(score);
  score = 0;
  
  display.setCursor(1,45);
  display.println("Enter 1 To Play Again");
  display.display();
}

// Display score while running the game
void displayScore(int score){
  display.setTextSize(1);
  display.setCursor(64,10);
  display.print("Score: ");
  display.print(score);
}
void displayRScore(int rscore){
  display.setTextSize(1);
  display.setCursor(60,20);
  display.print("R-Score: ");
  display.print(rscore);
}
// Main play function
void play(){

  int16_t tree_x=127;
  int16_t tree1_x=195;
  int tree_type = random(0,2);
  int tree_type1 = random(0,2);

  int16_t dino_y = DINO_INIT_Y;
  int input_command;
  int jump=0;

  
  for(;;){
    display.clearDisplay();
    buttonState = digitalRead(buttonsw1);
    input_command = buttonState;

    if(input_command==0){
      Serial.println("Jump");
      if(jump==0) jump=1;
    }

    if(jump==1){
      dino_y--;
      if(dino_y== (DINO_INIT_Y-JUMP_PIXEL)){
        jump=2;
        score++;
      }
    }
    else if(jump==2){
      dino_y++;
      if(dino_y== DINO_INIT_Y){
        jump=0; 
      }
    }

    if(tree_x<= (DINO_INIT_X+DINO_WIDTH) && tree_x>= (DINO_INIT_X+(DINO_WIDTH/2))){
      if(dino_y>=(DINO_INIT_Y-3)){
        // Collision Happened
        Serial.println("Collision Happend");
        break;
      }    
    }

    if(tree1_x<= (DINO_INIT_X+DINO_WIDTH) && tree1_x>= (DINO_INIT_X+(DINO_WIDTH/2))){
      if(dino_y>=(DINO_INIT_Y-3)){
        // Collision Happened
        Serial.println("Collision Happend");
        break;
      }    
    }

    displayScore(score);
    displayRScore(rscore);
    moveTree(&tree_x,tree_type);
    moveTree(&tree1_x,tree_type1);
    moveDino(&dino_y);
    display.drawLine(0, 54, 127, 54, SSD1306_WHITE);
    

    tree_x--;
    tree1_x--;
    if(tree_x==0) {
      tree_x = 127;
      tree_type = random(0,1);
    }

    if(tree1_x==0) {
      tree1_x = 195;
      tree_type1 = random(0,1);
    }
    //กำหนดเมื่อจบเกม
    if (score==rscore){
      pubstart(); //Public Status ให้ Board A
      pubpass(); //Public Passcode ให้ Board A
      showpass(); //หน้าจอจบเกม
      delay(3000);
      mainshow(); //หน้าจอหลัก
      score=0;
      gamestart=0;
      break;
    }
    display.display();

  }

  Serial.println("Game Over");
  if (gamestart == 1){
  gameOver(score);}
}

void pubstart(){
  String payload("0");
  mqtt.publish(TOPIC_ST, payload.c_str());
}
void renderScene(int16_t i=0){
  display.drawBitmap(10, 29, dino1, 25, 26, SSD1306_WHITE);
  display.drawBitmap(50, TREE_Y, tree1, 11, 23, SSD1306_WHITE);
  display.drawBitmap(100, TREE_Y, tree2, 22, 23, SSD1306_WHITE);
  display.drawLine(0, 54, 127, 54, SSD1306_WHITE);
  display.drawPixel(i, 60, SSD1306_WHITE);
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

void showpass(){
  display.clearDisplay();
  display.setTextSize(2);             // Draw 2X-scale text
  display.setTextColor(BLACK, WHITE);
  display.setCursor(10,5);
  display.println("Winner");
  

  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
  display.setCursor(10,35);
  display.println("Passcode is.....");
  display.setTextColor(BLACK, WHITE);
  display.setTextSize(2);
  display.setCursor(10,45);
  display.println(password);

  display.display();
  digitalWrite(gled,LOW);
  digitalWrite(yled,LOW);
  digitalWrite(rled,HIGH);
  if(checksong == 0){
    song1();
  }
  ESP.restart();
}

void mainshow(){
  display.clearDisplay();
  display.setTextSize(2);             // Draw 2X-scale text
  display.setTextColor(BLACK, WHITE);
  display.setCursor(10,5);
  display.println("Notice");
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
  display.setCursor(10,35);
  display.println("Waiting Req. From  Board A");
  display.display();
}

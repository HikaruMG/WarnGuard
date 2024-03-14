# WarnGuard V.1 [CPE37-Hardware Project]
  The system warns you if you've been sitting for too long and there are activities to do to reduce office syndrome. using ESP32-S3 microcontrollers and data is sent between them via the MQTT protocol. Users can view the dashboard in Node-RED.
<hr></hr>

## Programs
- Arduino
- Node-RED

## List of Components used
- PCB WarnGuard v.1 (2x)
- Esp32-S3-DEVKITS (2x)
- OLED 128x96 (2x)
- Buzzer (2x)
- Numpad I/O (1x)
- Switch (12x)
- led R/G/B (2x)
- RTC (1x)

### Included Libraries
- <WiFi.h> 
- <PubSubClient.h>
- <SPI.h>
- <Wire.h> 
- <Adafruit_GFX.h>
- <Adafruit_SSD1306.h>
- <RTClib.h>

## Directory

```
├── Main
│   ├── BoardA
│   │   ├── BoardA.ino //Main File For BoardA
│   │   ├── Chords //SheetNote
│   │   ├── config.h //MQTT Setup
│   │   ├── pitches.h //Buzzer Define Tone
│   ├── BoardB
│   │   ├── BoardA.ino //Main File For BoardA
│   │   ├── Chords //SheetNote
│   │   ├── config.h //MQTT Setup
│   │   ├── pitches.h //Buzzer Define Tone
├── PCB-Design
│   ├── Gerber_WarnGuard_PCB_WarnGuard_2.zip //Gerber File
│   ├── PCB_PCB_WarnGuard_2 //Plate PCB Front
│   ├── PCB_PCB_WarnGuard_2_(1) //Plate PCB Back
├── DashBoard-WG.json //Node-RED DashBoard
├── README.txt //File describing the project
└── License
```

## Developed By
1) Jiraphat Sritawee | <a href="https://www.w3schools.com">@HikaruMG</a>
2) Thammapat Rattanataipop |

<hr>
<h5 align="center">
<a href="https://ecourse.cpe.ku.ac.th/tpm/project/hwdev-66s">Introduction to Computer Hardware Development</a> <br>
Computer Engineering [CPE] <br>
Faculty, Kasetsart University, Bangkok <br>
</h5>

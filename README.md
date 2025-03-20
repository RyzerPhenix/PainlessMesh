# GON131 Stückzahlerfassung dezentral drahtlos
**Author**: Stephan Bischof | Electronics Engineer  
**Responsible**:
- Jürg Schnidrig | Electronics Engineer
- Marc Strub | IT-, OT- and IS-Manager
**Date**: 04.11.2024  
For internal use only | Renata SA - Swatch Group Ltd.

## Project Description
The project serves to decentralize the unit count recording and realize it wirelessly. Seeed Studio ESP32S3 microcontroller units on a custom PCB are used for this purpose. Communication between the different nodes works via a 2.4GHz mesh network, for which the painlessMesh library is used.

## Overview
- [Working principle](#Overview)
  - [Mesh Network](#Mesh-Netwrok)
  - [painlessMesh Library](#painlessMesh-Library)
  - [Network Structure](#Network-Structure)
  - [Data Collection](#Data-Collection)
- [Hardware](#Hardware)
  - [Microcontroller](#Microcontroller)
  - [Device ID and Room Selection](#Device-ID-and-Room-Selection)
  - [Custom PCB](#Custom-PCB)
- [Software](#Software)
  - [Libraries and Dependencies](#Libraries-and-Dependencies)
  - [Code](#Code)
- [Network analytic Device](#Network-analytic-Device)
  - [Hardware](#Hardware)
  - [Software](#Software)
  - [Functionality](#Functionality)

## Working Principle
### Mesh Network
A mesh network works by creating a decentralized communication structure where each node (device) can directly communicate with other nearby nodes. This contrasts with the traditional network topology, where all communication passes through a central access point (such as a router).
- Decentralized Communication: Each node in a mesh network can connect to multiple other nodes. This creates multiple paths for data transmission, providing redundancy and potentially improving network robustness and reliability.
- Dynamic Routing: Nodes can find the best path to deliver messages by forwarding data through neighboring nodes. If one node goes offline, the network can reroute data through alternative nodes, maintaining connectivity.
- No Central Access Point Required: Unlike traditional networks, mesh networks do not rely on a single access point (such as a router) to manage connections. This allows for greater flexibility, scalability, and resilience, especially useful in large areas or outdoor environments.

![Normal Network vs Mesh Network](https://www.fastcabling.com/wp-content/uploads/2022/06/4-Top-4-Challenges-When-Using-Mesh-WiFi.jpg)  
Source: https://www.fastcabling.com

## Hardware
### Microcontroller
The microcontroller used is the Seeed Studio XIAO ESP32S3. This Microcontroller was chosen, because of its small formfactor, its two powerfull 32-bit Xtensa LX7 Cores running at up to 240MHz and its PSRAM. In comparison to other XIAO Boards, the ESP32S3 has a relatively big PSRAM with 8MB, this is important for mesh applications, since the size of the network is mostly limited by RAM capacity. Also it has 8MB of Flash storage onboard. The XIAO ESP32S3 has a USB type C port for programming and interfacing with the MCU, this is important for future proofness and compatability. For interfaces it has 1x UART, 1x I²C, 1x I²S, 1xSPI, 11x GPIO (all capable of PWM), 1 user programmable LED and 1 charge LED. It has an integrated LiPo battery charger circuit onboard, so a single cell LiPo can be hooked for simply battery powering the application. The battery charging current is 100mA. It consumes about 100mA when powered with 5V while operating with WiFi.
![Seeed Studio XIAO ESP32-S3](https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/)

## Software

### Libraries and Dependencies
#### Preparing the Arduino IDE
To use the Seeed Studio XIAO ESP32 Series microcontrollers, it is nececary to include the board package.
1. Open the Arduino IDE
2. Navigate to File > Preferences
3. Fill "Additional Boards Manager URLs" with this URL:
'''
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
'''
4. Navigate to Tools > Board > Boards Manager
5. Type the keyword esp32 in the search box, select the latest version of esp32, and install it.
6. Select your board (XIAO_ESP32-S3) and the port.

Read more about it ![here](https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/#software-preparation).

#### PainlessMesh Library
The [painlessMesh library](https://github.com/gmag11/painlessMesh) for Arduino IDE is a powerful option for creating a self-healing, true ad-hoc, multi-node mesh networks with ESP8266 and ESP32 devices. The goal is to allow the programmer to work with a mesh network without having to worry about how the network is structured or managed. painlessMesh does not create a TCP/IP network of nodes. Rather each of the nodes is uniquely identified by its 32bit chipId which is retrieved from the esp8266/esp32 using the system_get_chip_id() call in the SDK. Any system of 1 or more nodes will self-organize into fully functional mesh. The maximum size of the mesh is limited by the amount of memory in the heap that can be allocated to the sub-connections buffer. In the library, it is possible to specify a port and a channel for the WiFi connection, so you can create multiple seperate networks running near each other but they won't interfear.
Install the painlessMesh Library
- Open Arduino IDE, go to Sketch > Include Library > Manage Libraries
- Search for painlessMesh and install it.  


### Code

#### Master:
'''C++
#include <painlessMesh.h>

// Netzwerk-Parameter
#define MESH_PASSWORD   "12345678"
#define MESH_PORT       5555

painlessMesh mesh;

// Variablen
char roomIndex = 5;  // Raumindex für das Mesh-Netzwerk, zwischen 0 und 15
uint8_t slaveIndex = 0;  // Startindex für das Anpingen der Slaves (0 bis 255)
unsigned long previousMillis = 0;  // Zeit-Tracking für den Ping
const unsigned long interval = 1000;  // Ping-Intervall in Millisekunden (1 Sekunde)

// Funktionsprototypen
String createMeshPrefix(char index);
String createMasterName(char index);
String createSlaveName(uint8_t index);

// Callback für empfangene Nachrichten
void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("Message from %u: %s\n", from, msg.c_str());
}

// Funktion zur Erstellung des dynamischen Netzwerknamens
String createMeshPrefix(char index) {
  return "Room_" + String((int)index);  // Index als Zahl von 0-15 an den Präfix anhängen
}

// Funktion zur Erstellung des Master-Namens
String createMasterName(char index) {
  return "Master_Room_" + String((int)index);  // Master-Name mit Raumnummer
}

// Funktion zur Erstellung des Slave-Namens
String createSlaveName(uint8_t index) {
  return "Slave_" + String((int)index);  // Slave-Name mit laufender Nummerierung
}

// Setup
void setup() {
  Serial.begin(115200);
  delay(500);

  // Mesh-Netzwerkname
  String meshPrefix = createMeshPrefix(roomIndex);
  Serial.print("Networkname: ");
  Serial.println(meshPrefix);

  // Master-Name
  String masterName = createMasterName(roomIndex);
  Serial.print("Master-Name: ");
  Serial.println(masterName);

  mesh.setDebugMsgTypes(ERROR | MESH_STATUS | CONNECTION);
  mesh.init(meshPrefix.c_str(), MESH_PASSWORD, &MESH_PORT);
  mesh.onReceive(&receivedCallback);
}

// Loop
void loop() {
  mesh.update();

  // Überprüfe das Intervall für den Ping
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Nachricht an den aktuellen Slave senden
    String slaveName = createSlaveName(slaveIndex);
    Serial.printf("Pinging %s\n", slaveName.c_str());

    mesh.sendBroadcast("Ping at " + slaveName);

    // Index für den nächsten Slave erhöhen (0 bis 255)
    slaveIndex = (slaveIndex + 1) % 256;
  }
}
'''

####Slave
'''C++
#include <painlessMesh.h>

// Netzwerk-Parameter
#define MESH_PASSWORD   "12345678"
#define MESH_PORT       5555

painlessMesh mesh;

// Variablen
char roomIndex = 5;  // Raumindex für das Mesh-Netzwerk, zwischen 0 und 15
char slaveIndex = 3;  // Beispiel-Slave-Index, zwischen 0 und 255
String meshPrefix;
String slaveName;
int counter = 0;  // Zähler für empfangene Nachrichten

// Funktionsprototypen
String createMeshPrefix(char index);
String createSlaveName(char index);

// Callback für empfangene Nachrichten
void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("Message from %u: %s\n", from, msg.c_str());

  // Prüfen, ob die Nachricht den Namen dieses Slaves enthält
  if (msg.endsWith(slaveName)) {
    counter++;  // Zähler inkrementieren
    Serial.printf("Message for %s recieved. Counter: %d\n", slaveName.c_str(), counter);
  }
}

// Funktion zur Erstellung des dynamischen Netzwerknamens
String createMeshPrefix(char index) {
  return "Room_" + String((int)index);  // Erzeugt den Mesh-Präfix wie "Room_5"
}

// Funktion zur Erstellung des Slave-Namens
String createSlaveName(char index) {
  return "Slave_" + String((int)index);  // Erzeugt den Slave-Namen wie "Slave_3"
}

// Setup
void setup() {
  Serial.begin(115200);
  delay(500);

  // Mesh-Netzwerkname und Slave-Name basierend auf den Indizes
  meshPrefix = createMeshPrefix(roomIndex);
  slaveName = createSlaveName(slaveIndex);

  Serial.print("Networkname: ");
  Serial.println(meshPrefix);
  Serial.print("Slave-Name: ");
  Serial.println(slaveName);

  // Mesh-Initialisierung
  mesh.setDebugMsgTypes(ERROR | MESH_STATUS | CONNECTION);
  mesh.init(meshPrefix.c_str(), MESH_PASSWORD, &MESH_PORT);
  mesh.onReceive(&receivedCallback);
}

// Loop
void loop() {
  mesh.update();
}
'''

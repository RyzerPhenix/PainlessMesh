# Mesh Network using ESP32 and PainlessMesh
**Author**: Stephan Bischof | Electronics Engineer  
**Responsible**: Ryzer Phenix | CEO, CTO, ICT Manager and Co-Founder  
**Date**: 04.11.2024  
Commited for commercial use | Ryzer Industries

## Project Description
The project serves to teach about mesh networks and to use them with simple sensors. The Seeed Studio ESP32S3 microcontroller unit is used for this purpose. Communication between the different nodes works via a 2.4GHz mesh network, for which the PainlessMesh library is used.

## Overview
- [Working principle](#Overview)
  - [Mesh Network](#Mesh-Netwrok)
  - [painlessMesh Library](#painlessMesh-Library)
- [Hardware](#Hardware)
  - [Microcontroller](#Microcontroller)
- [Software](#Software)
  - [Libraries and Dependencies](#Libraries-and-Dependencies)
  - [Code](#Code)

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
[Seeed Studio XIAO ESP32-S3](https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/)

### Arduino Pinout Diagram
![Arduino Pinout Diagram](https://sigmdel.ca/michel/ha/xiao/img/xiao_esp32c3_pinout_bottom.png)

## Software

### Libraries and Dependencies
#### Preparing the Arduino IDE
To use the Seeed Studio XIAO ESP32 Series microcontrollers, it is nececary to include the board package.
1. Open the Arduino IDE
2. Navigate to File > Preferences
3. Fill "Additional Boards Manager URLs" with this URL:
```
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```
4. Navigate to Tools > Board > Boards Manager
5. Type the keyword esp32 in the search box, select the latest version of esp32, and install it.
6. Select your board (XIAO_ESP32S3) and the port.

Read more about it [here](https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/#software-preparation).

#### PainlessMesh Library
The [painlessMesh library](https://github.com/gmag11/painlessMesh) for Arduino IDE is a powerful option for creating a self-healing, true ad-hoc, multi-node mesh networks with ESP8266 and ESP32 devices. The goal is to allow the programmer to work with a mesh network without having to worry about how the network is structured or managed. painlessMesh does not create a TCP/IP network of nodes. Rather each of the nodes is uniquely identified by its 32bit chipId which is retrieved from the esp8266/esp32 using the system_get_chip_id() call in the SDK. Any system of 1 or more nodes will self-organize into fully functional mesh. The maximum size of the mesh is limited by the amount of memory in the heap that can be allocated to the sub-connections buffer. In the library, it is possible to specify a port and a channel for the WiFi connection, so you can create multiple seperate networks running near each other but they won't interfear.
Install the painlessMesh Library
- Open Arduino IDE, go to Sketch > Include Library > Manage Libraries
- Search for painlessMesh and install it.  


### Code

#### RSSI Example Code
This code analyzes the signal strength of the node to it's parent. It reports it, alongside with some other debug information, to the serial port.
```C++
#include <painlessMesh.h>

#define   MESH_PREFIX     "XIAOMesh"
#define   MESH_PASSWORD   "mesh12345"
#define   MESH_PORT       5555

Scheduler     userScheduler;
painlessMesh  mesh;

// === Create message with NodeID, ParentID, RSSI ===
String createNodeMessage() {
  uint32_t myId = mesh.getNodeId();
  int32_t rssi = WiFi.RSSI();

  uint32_t parentId = 0;
  auto nodeList = mesh.getNodeList(true);  // Include root if connected
  if (!nodeList.empty()) {
    parentId = *nodeList.begin();          // Use iterator to access first element
  }

  String message = "NodeID: " + String(myId) + " | ParentID: " + String(parentId) + " | RSSI: " + String(rssi) + "dBm";
  return message;
}

// === Send info to all nodes ===
void sendNodeInfo() {
  String msg = createNodeMessage();
  mesh.sendBroadcast(msg);
  Serial.println("Sent: " + msg);
}

// === Print incoming messages ===
void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("From %u: %s\n", from, msg.c_str());
}

// === Optional logging ===
void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("🔌 New connection: %u\n", nodeId);
}

// === Periodic task ===
Task taskSendInfo(TASK_SECOND * 10, TASK_FOREVER, &sendNodeInfo);

void setup() {
  Serial.begin(115200);
  delay(1000);

  mesh.setDebugMsgTypes(ERROR | STARTUP | CONNECTION);
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);

  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);

  userScheduler.addTask(taskSendInfo);
  taskSendInfo.enable();
}

void loop() {
  mesh.update();
}
```

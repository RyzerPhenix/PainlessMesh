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

### Network Structure
In every network there will be one master node which is responcible for requesting Data from the slave nodes, recieving them and passing them to a host computer where the data will be processed. At the startup the master is given a number ranging from 0 to 15 to specify a room (essentially just a channel and a port to separate the different networks) it is working in. The slaves must be configuered with the same room number in order to communicate on the same network as the corresponding master. So a total of 16 different networks are possible. Also the slaves are given a number between 0 and 255. This is their device number the master uses to send and recieve messages to and from the slaves. Those two numbers can be set with hex rotary switches. The devices only read those numbers when they start up or get reset. Everything after should connect automatically.
For a more detailed and visualized overview of the network consider looking at [/Doc/Network-Tree.md](/Doc/Network-Tree.md).

### Data Collection

## Hardware
### Microcontroller
The microcontroller used is the Seeed Studio XIAO ESP32S3. This Microcontroller was chosen, because of its small formfactor, its two powerfull 32-bit Xtensa LX7 Cores running at up to 240MHz and its PSRAM. In comparison to other XIAO Boards, the ESP32S3 has a relatively big PSRAM with 8MB, this is important for mesh applications, since the size of the network is mostly limited by RAM capacity. Also it has 8MB of Flash storage onboard. The XIAO ESP32S3 has a USB type C port for programming and interfacing with the MCU, this is important for future proofness and compatability. For interfaces it has 1x UART, 1x I²C, 1x I²S, 1xSPI, 11x GPIO (all capable of PWM), 1 user programmable LED and 1 charge LED. It has an integrated LiPo battery charger circuit onboard, so a single cell LiPo can be hooked for simply battery powering the application. The battery charging current is 100mA. It consumes about 100mA when powered with 5V while operating with WiFi.

### Device ID and Room Selection
To select the device ID and the room the device shall operate in, three hex rotary DIP switches are used, two for the device ID and one for the room. To save pins, PISO (Parallel In, Serial Out) shift registers are used, one for the device ID selection and one for the room selection. For more information consider the [Schematic]().

### Custom PCB

## Software

### Libraries and Dependencies
#### painlessMesh Library
The [painlessMesh library](https://github.com/gmag11/painlessMesh) for Arduino IDE is a powerful option for creating a self-healing, true ad-hoc, multi-node mesh networks with ESP8266 and ESP32 devices. The goal is to allow the programmer to work with a mesh network without having to worry about how the network is structured or managed. painlessMesh does not create a TCP/IP network of nodes. Rather each of the nodes is uniquely identified by its 32bit chipId which is retrieved from the esp8266/esp32 using the system_get_chip_id() call in the SDK. Any system of 1 or more nodes will self-organize into fully functional mesh. The maximum size of the mesh is limited by the amount of memory in the heap that can be allocated to the sub-connections buffer. In the library, it is possible to specify a port and a channel for the WiFi connection, so you can create multiple seperate networks running near each other but they won't interfear.
Install the painlessMesh Library
- Open Arduino IDE, go to Sketch > Include Library > Manage Libraries
- Search for painlessMesh and install it.  


### Code

## Network analytic Device
### Hardware
The Hardware is the same as of the production devices. The only difference are the three rotary switches to define the room name and the device ID as well as the reset button. Those are accessible from outside the case.

### Software
The software is responsible for recieving all the data and is able to send commands into the network.

### Functionality
- Get a list of all nodes
- Send a disconnect signal to all nodes
- View traffic on the network

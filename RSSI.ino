#include "painlessMesh.h"
#include <WiFi.h>

#define MESH_PREFIX     "MyMeshNetwork"
#define MESH_PASSWORD   "password123"
#define MESH_PORT       5555

Scheduler userScheduler;
painlessMesh mesh;

unsigned long lastSentTime = 0;
const unsigned long interval = 10000; // 10 Sekunden

void sendRSSI() {
    SimpleList<uint32_t> nodes = mesh.getNodeList();
    for (auto node : nodes) {
        String msg = "REQ_RSSI"; // Anforderung zur Messung der Signalstärke
        mesh.sendSingle(node, msg);
    }
}

void receivedCallback(uint32_t from, String &msg) {
    Serial.printf("Received from %u: %s\n", from, msg.c_str());
    
    if (msg == "REQ_RSSI") {
        int8_t rssi = WiFi.RSSI(); // Signalstärke des aktuellen Netzwerks messen
        String response = "RSSI: " + String(rssi) + " dB";
        mesh.sendSingle(from, response);
    }
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA); // WLAN in den Station-Modus versetzen
    
    mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
    mesh.onReceive(&receivedCallback);
}

void loop() {
    mesh.update();
    
    if (millis() - lastSentTime >= interval) {
        lastSentTime = millis();
        sendRSSI();
    }
}

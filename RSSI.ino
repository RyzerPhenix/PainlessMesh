#include "painlessMesh.h"

#define MESH_PREFIX     "MyMeshNetwork"
#define MESH_PASSWORD   "password123"
#define MESH_PORT       5555

Scheduler userScheduler;
painlessMesh mesh;

unsigned long lastSentTime = 0;
const unsigned long interval = 10000; // 10 seconds

void sendRSSI() {
    SimpleList<uint32_t> nodes = mesh.getNodeList();
    for (auto node : nodes) {
        int8_t rssi = mesh.connectionQuality(node);
        String msg = "Node " + String(mesh.getNodeId()) + " -> " + String(node) + " RSSI: " + String(rssi);
        mesh.sendSingle(node, msg);
    }
}

void receivedCallback(uint32_t from, String &msg) {
    Serial.printf("Received from %u: %s\n", from, msg.c_str());
}

void setup() {
    Serial.begin(115200);

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

#include <painlessMesh.h>

#define   MESH_PREFIX     "XIAOMesh"
#define   MESH_PASSWORD   "mesh12345"
#define   MESH_PORT       5555

Scheduler     userScheduler;
painlessMesh  mesh;

void sendOwnSignalStrength() {
  int32_t rssi = WiFi.RSSI();

  DynamicJsonDocument doc(256);
  doc["nodeId"] = mesh.getNodeId();
  doc["rssi"] = rssi;

  String msg;
  serializeJson(doc, msg);

  // Broadcast to all nodes
  mesh.sendBroadcast(msg);
  Serial.printf("Broadcasted RSSI: %s\n", msg.c_str());
}

void receivedCallback(uint32_t from, String &msg) {
  DynamicJsonDocument doc(256);
  DeserializationError err = deserializeJson(doc, msg);
  if (err) {
    Serial.printf("Error parsing JSON from %u: %s\n", from, err.c_str());
    return;
  }

  uint32_t senderId = doc["nodeId"];
  int32_t rssi = doc["rssi"];

  Serial.printf("Received RSSI from node %u: %d dBm\n", senderId, rssi);
}

Task taskSendRSSI(TASK_SECOND * 10, TASK_FOREVER, &sendOwnSignalStrength);

void setup() {
  Serial.begin(115200);

  mesh.setDebugMsgTypes(ERROR | STARTUP | CONNECTION);  // Optional
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);

  userScheduler.addTask(taskSendRSSI);
  taskSendRSSI.enable();
}

void loop() {
  mesh.update();
}

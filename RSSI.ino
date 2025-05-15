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

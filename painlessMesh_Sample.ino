#include <painlessMesh.h>

#define sensor 0

#define   MESH_PREFIX     "XIAOMesh"
#define   MESH_PASSWORD   "mesh12345"
#define   MESH_PORT       5555

painlessMesh  mesh;

bool sensorData = 0;

// === Timer Definitions and Variables ===
unsigned long sendInterval = 10000;  // In miliseconds => 10 seconds
unsigned long lastSendTime = 0;

// === Create message with NodeID and Content ===
String createNodeMessage(bool value) {
  uint32_t myId = mesh.getNodeId();
  String message = "NodeID: " + String(myId) +" | Content: The Sensor status is " + String(value) + " (Bool)";
  return message;
}

// === Send info to all nodes ===
void sendNodeInfo() {
  sensorData = digitalRead(sensor);
  String msg = createNodeMessage(sensorData);
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

void setup() {
  Serial.begin(115200);
  pinMode(sensor, INPUT_PULLDOWN);
  delay(1000);

  mesh.setDebugMsgTypes(ERROR | STARTUP | CONNECTION);
  mesh.init(MESH_PREFIX, MESH_PASSWORD, MESH_PORT);

  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
}

void loop() {
  mesh.update();

  // Send message every x seconds
  unsigned long now = millis();
  if (now - lastSendTime >= sendInterval) {
    lastSendTime = now;
    sendNodeInfo();
  }
}
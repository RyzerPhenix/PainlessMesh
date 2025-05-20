#include <painlessMesh.h>

#define   MESH_PREFIX     "XIAOMesh"
#define   MESH_PASSWORD   "mesh12345"
#define   MESH_PORT       5555

painlessMesh  mesh;

String userInput = "";  // Stores the serial input

// === Create message with NodeID and Serial Content ===
String createNodeMessage() {
  uint32_t myId = mesh.getNodeId();
  String message = "DeBischof | Content: " + userInput + " (Serial)";
  return message;
}

// === Send info to all nodes ===
void sendNodeInfo() {
  String msg = createNodeMessage();
  mesh.sendBroadcast(msg);
  Serial.println("Sent: " + msg);
}

// === Callback for received messages ===
void receivedCallback(uint32_t from, String &msg) {
  Serial.println("From " + String(from) + ": " + msg);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  mesh.setDebugMsgTypes(ERROR | STARTUP | CONNECTION);
  mesh.init(MESH_PREFIX, MESH_PASSWORD, MESH_PORT);

  mesh.onReceive(&receivedCallback);
}

void loop() {
  mesh.update();

  // Read complete line from Serial if available
  if (Serial.available()) {
    userInput = Serial.readStringUntil('\n');
    userInput.trim(); // Remove newline or extra spaces

    if (userInput.length() > 0) {
      sendNodeInfo(); // Send the input string over mesh
    }
  }
}

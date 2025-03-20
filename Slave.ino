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
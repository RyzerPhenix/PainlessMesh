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
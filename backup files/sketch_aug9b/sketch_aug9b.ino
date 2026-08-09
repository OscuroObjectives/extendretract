#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

const char* ssid = "Shortcake-Ace";
const char* wifiPassword = "Teddybearace1155";

const char* mqttServer = "8a667d6eb5234254958db7ce3a42fa44.s1.eu.hivemq.cloud";
const int mqttPort = 8883;

const char* mqttUser = "esp32-actuator";
const char* mqttPassword = "Sevvipes1!";

const char* topic = "actuator/control";

WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);

void mqttCallback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Received command: ");

  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }

  Serial.println();
}

void connectToMQTT() {

  while (!mqttClient.connected()) {

    Serial.println("Connecting to HiveMQ...");

    if (mqttClient.connect("ESP32-Actuator", mqttUser, mqttPassword)) {

      Serial.println("Connected to HiveMQ!");

      mqttClient.subscribe(topic);

      Serial.print("Subscribed to: ");
      Serial.println(topic);

    } else {

      Serial.print("MQTT connection failed, state: ");
      Serial.println(mqttClient.state());

      delay(2000);
    }
  }
}

void setup() {

  Serial.begin(115200);

  Serial.println("Connecting to WiFi...");

  WiFi.begin(ssid, wifiPassword);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected!");

  espClient.setInsecure();

  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback(mqttCallback);

  connectToMQTT();
}

void loop() {

  if (!mqttClient.connected()) {
    connectToMQTT();
  }

  mqttClient.loop();
}
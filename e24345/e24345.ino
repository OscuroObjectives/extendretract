#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

// ======================================================
// WIFI
// ======================================================

const char* ssid = "Shortcake-Ace";
const char* password = "Teddybearace1155";

// ======================================================
// HIVEMQ CLOUD
// ======================================================

const char* mqtt_server =
  "8a667d6eb5234254958db7ce3a42fa44.s1.eu.hivemq.cloud";

const int mqtt_port = 8883;

const char* mqtt_username = "esp32-actuator";
const char* mqtt_password = "Sevvipes1!";

// Use the SAME topic you used successfully in your MQTT test.
const char* mqtt_topic = "actuator/control";

// ======================================================
// MQTT
// ======================================================

WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);

// ======================================================
// MOTOR / FNG PINS
// ======================================================

const int PWM  = 25;
const int AIN2 = 26;
const int AIN1 = 27;
const int STBY = 14;

// ======================================================
// SAFETY
// ======================================================

// Maximum time the actuator is allowed to run
// continuously from one command.
//
// Change this if necessary.
const unsigned long MAX_RUN_TIME = 5000;

unsigned long actuatorStartTime = 0;
bool actuatorRunning = false;

// ======================================================
// STOP ACTUATOR
// ======================================================

void stopActuator() {

  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  digitalWrite(PWM, LOW);

  actuatorRunning = false;

  Serial.println("Actuator STOPPED");
}

// ======================================================
// EXTEND
// ======================================================

void extendActuator() {

  digitalWrite(STBY, HIGH);

  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  digitalWrite(PWM, HIGH);

  actuatorStartTime = millis();
  actuatorRunning = true;

  Serial.println("EXTEND command received");
}

// ======================================================
// RETRACT
// ======================================================

void retractActuator() {

  digitalWrite(STBY, HIGH);

  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  digitalWrite(PWM, HIGH);

  actuatorStartTime = millis();
  actuatorRunning = true;

  Serial.println("RETRACT command received");
}

// ======================================================
// MQTT MESSAGE RECEIVED
// ======================================================

void mqttCallback(char* topic, byte* payload, unsigned int length) {

  String message = "";

  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  message.trim();
  message.toUpperCase();

  Serial.print("MQTT message received: ");
  Serial.println(message);

  if (message == "EXTEND") {

    extendActuator();

  } 
  else if (message == "RETRACT") {

    retractActuator();

  } 
  else if (message == "STOP") {

    stopActuator();

  } 
  else {

    Serial.println("Unknown command");

  }
}

// ======================================================
// CONNECT TO WIFI
// ======================================================

void connectWiFi() {

  Serial.print("Connecting to WiFi");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
    Serial.print(".");

  }

  Serial.println();
  Serial.println("WiFi connected");

  Serial.print("ESP32 IP address: ");
  Serial.println(WiFi.localIP());
}

// ======================================================
// CONNECT TO HIVEMQ
// ======================================================

void reconnectMQTT() {

  while (!mqttClient.connected()) {

    Serial.print("Connecting to HiveMQ...");

    String clientID = "ESP32-Actuator-" + String(random(0xffff), HEX);

    if (mqttClient.connect(
          clientID.c_str(),
          mqtt_username,
          mqtt_password
        )) {

      Serial.println("CONNECTED");

      mqttClient.subscribe(mqtt_topic);

      Serial.print("Subscribed to: ");
      Serial.println(mqtt_topic);

    } 
    else {

      Serial.print("Failed, MQTT state = ");
      Serial.println(mqttClient.state());

      delay(5000);
    }
  }
}

// ======================================================
// SETUP
// ======================================================

void setup() {

  Serial.begin(115200);

  // Motor pins
  pinMode(PWM, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(STBY, OUTPUT);

  // Make sure actuator starts stopped
  stopActuator();

  // Connect WiFi
  connectWiFi();

  // HiveMQ TLS
  //
  // This is the same approach that allowed
  // your previous MQTT test to connect.
  espClient.setInsecure();

  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(mqttCallback);

  Serial.println();
  Serial.println("ESP32 actuator controller ready.");
}

// ======================================================
// LOOP
// ======================================================

void loop() {

  // Reconnect to HiveMQ if necessary
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }

  mqttClient.loop();

  // ----------------------------------------------------
  // SAFETY TIMEOUT
  // ----------------------------------------------------

  if (actuatorRunning) {

    if (millis() - actuatorStartTime >= MAX_RUN_TIME) {

      Serial.println("Safety timeout reached.");

      stopActuator();
    }
  }
}
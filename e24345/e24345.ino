#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Shortcake-Ace";
const char* password = "Teddybearace1155";

WebServer server(80);

// Motor pins
const int PWMA = 25;
const int AIN2 = 26;
const int AIN1 = 27;
const int STBY = 14;

void extendActuator() {
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  digitalWrite(PWMA, HIGH);

  server.send(200, "text/html",
    "<h1>Extending...</h1><br><a href='/'>Back</a>");
}

void retractActuator() {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  digitalWrite(PWMA, HIGH);

  server.send(200, "text/html",
    "<h1>Retracting...</h1><br><a href='/'>Back</a>");
}

void stopMotor() {
  digitalWrite(PWMA, LOW);

  server.send(200, "text/html",
    "<h1>Stopped</h1><br><a href='/'>Back</a>");
}

void homePage() {

  String page = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<title>Actuator Control</title>
</head>

<body style="text-align:center;font-family:Arial;">

<h1>ESP32 Actuator Control</h1>

<p><a href="/extend"><button style="width:200px;height:60px;font-size:22px;">EXTEND</button></a></p>

<p><a href="/retract"><button style="width:200px;height:60px;font-size:22px;">RETRACT</button></a></p>

<p><a href="/stop"><button style="width:200px;height:60px;font-size:22px;">STOP</button></a></p>

</body>
</html>
)rawliteral";

  server.send(200, "text/html", page);
}

void setup() {

  Serial.begin(115200);

  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(STBY, OUTPUT);

  digitalWrite(STBY, HIGH);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println(WiFi.localIP());

  server.on("/", homePage);
  server.on("/extend", extendActuator);
  server.on("/retract", retractActuator);
  server.on("/stop", stopMotor);

  server.begin();
}

void loop() {

  server.handleClient();

}
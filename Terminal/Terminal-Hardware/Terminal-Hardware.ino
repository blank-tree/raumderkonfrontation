#include <SPI.h>
#include <WiFi101.h>
#include <MQTT.h>

const char ssid[] = "BRIDGE";
const char pass[] = "internet";

WiFiClient net;
MQTTClient client;

unsigned long lastMillis = 0;

int moneyCollected;
String appointmentDate;
String appointmentTime;

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("arduino", "b23695cf", "36a044b175c04e97")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");

  client.subscribe("/hello");
  client.subscribe("/acceptMoney");
  client.subscribe("/appointmentDate");
  client.subscribe("/appointmentTime");
  // client.unsubscribe("/hello");
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
  switch (topic) {
      case "acceptMoney":
        // do something
        break;
      case "appointmentDate":
        // do something
        break;
      case "appointmentTime":
        // do something
        break;
      default:
        // do something
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);

  // Note: Local domain names (e.g. "Computer.local" on OSX) are not supported by Arduino.
  // You need to set the IP address directly.
  client.begin("broker.shiftr.io", net);
  client.onMessage(messageReceived);

  connect();

  moneyCollected = 0;
  appointmentDate = "";
  appointmentTime = "";
}

void loop() {
  client.loop();

  if (!client.connected()) {
    connect();
  }

  // publish a message roughly every second.
  if (millis() - lastMillis > 1000) {
    lastMillis = millis();
    client.publish("/arduino-online", "1");
  }
}

void resetTVM() {
  moneyCollected = 0;
  appointmentDate = "";
  appointmentTime = "";
}
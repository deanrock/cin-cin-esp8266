#include <ESP8266WiFi.h>
#include <Servo.h>
#include "wifi.h"

WiFiClient client;
Servo myservo;
String received = "";

void setup() {
  myservo.attach(5);
  
  Serial.begin(115200);
  delay(10);
  
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Print the IP address
  Serial.println(WiFi.localIP());
  
  // client
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    return;
  }
}

int lastReceived = 0;

void sendCMD(String cmd) {
  String s = "!";
  s += cmd;
  s += "$";
  char buf[s.length()+1];
  s.toCharArray(buf, s.length()+1);
  if (client.connected()) {
    client.write(buf, strlen(buf));
  }
}

void loop() {
  if (client.connected()) {
    if (client.available()) {
      while(client.available()) {
        lastReceived = millis();
        
        char c = client.read();
        if (c == '!') {
          received = "";
        }else if (c=='$') {
          Serial.print("Received: ");
          Serial.println(received);

          if (received == "ping") {
            sendCMD("pong");
          }else if (received.startsWith("position")) {
            String x = received.substring(8);
            myservo.write(x.toInt());
          }
        }else{
          received += c;
        }
      }
    }
  }
  
  if (!client.connected()) {
    delay(100);
    if (!client.connect(host, port)) {
      Serial.println("connection failed");
    }
  }

  int diff = millis() - lastReceived;
  if (diff > 300000) {//300 seconds
    ESP.reset();
  }
}


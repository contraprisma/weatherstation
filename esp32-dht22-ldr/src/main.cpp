/*
  Copyright (C) 1989-2022 Free Software Foundation, Inc.
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.

  esp32-dht22 example for using AWS IoT with ESP32

  >>> Need to use PlatformIO Project for this project to work <<<

  This example uses an ESP32 board and a DHT22 sensor sending weather data to AWS IoT. 
  You can check on your device after a successful connected to AWS IoT subscribing to topic #
  Please update the secrets.h file with your own certificate and private key.

  >>> IMPORTANT: This is not production code. It is intended to be a simple example for learning! <<<

  modified 8 Aug 2022 
  by Izak Schalk Smit (oegma2) 
*/

#include "secrets.h"
#include <MQTTClient.h>   
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Wire.h>               
#include <DHT.h>
#include <DHT_U.h>

#ifdef __cplusplus
extern "C" {
#endif

uint8_t temprature_sens_read();

#ifdef __cplusplus
}
#endif

#define AWS_IOT_PUBLISH_TOPIC "DEMO/WEATHER/DATA"
#define AWS_IOT_SUBSCRIBE_TOPIC "CONTROL/ESP32"   
#define DHTTYPE DHT22           
#define DHTPIN 27               

DHT_Unified dht(DHTPIN, DHTTYPE);
WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);  
unsigned long lastMillis = 900000; //force first publish after 10 minutes

void messageHandler(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
  // Note: Do not use the client in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `client.loop()`.
}

float read_dht22_Temp() {
  //This reading of temp might need some work to avoid spiky readings :)
  sensors_event_t event;                                             
  dht.temperature().getEvent(&event);
  return event.temperature;
}

float read_dht22_Hum() {
  //This reading of hum might need some work to avoid spiky readings :)
  sensors_event_t event;                                             
  dht.humidity().getEvent(&event);
  return event.relative_humidity;
}

void connect() {
  Serial.print("ESP32 checking Wifi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");
  Serial.print("ESP32 Connected to Wifi");

  // Configure WiFiClientSecure to use the AWS IoT devicD credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);
  client.setOptions(30, true, 1000);
  client.onMessage(messageHandler);
  client.begin(AWS_IOT_ENDPOINT, 8883, net);

  Serial.print("ESP32 connecting to AWS IoT");
  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");
  Serial.println("ESP32 Connected to AWS IoT");

  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC, 1);
}

void setup() {
  Serial.begin(9600);

  dht.begin();
  delay(500);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void loop() {
  client.loop();

  if (!client.connected()) {
    connect();
  }

  // publish a message roughly every 10min.
  if (millis() - lastMillis > 600000) {
    lastMillis = millis();

    char jsonBuffer[512];
    StaticJsonDocument<200> sensorData;
    sensorData["dht22_temp"]   = read_dht22_Temp();    //DHT22 Temp
    sensorData["dht22_hum"]    = read_dht22_Hum();     //DHT22 Hum
    serializeJson(sensorData, jsonBuffer);
    Serial.println(jsonBuffer);

    client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
  }
}

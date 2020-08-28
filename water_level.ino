#include <CayenneMQTTESP32.h>
#define CAYENNE_DEBUG
#define CAYENNE_PRINT Serial
#define trigPin 13
#define echoPin 12
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

// Define variables:
long duration;
int distance;
int potensio = 0;
int anaktangga = 60;
int kedalaman = 300;
int depth;
int tangga;
int baca;
int battery=100;

char ssid[] = "WaterLevelNode";
char password[] = "Wi070707";
char username[] = "595fe320-f9f9-11e9-84bb-8f71124cfdfb";
char mqtt_password[] = "ca1601088df82b4ba32cbda89696559250bf7743";
char cliend_id[] = "45eb9c90-e1c4-11ea-a67f-15e30d90bbf4";
long toggle = 0;

String openWeatherMapApiKey = "1e1b8a442c7fc5e296d4fddff5199c90";
String city = "Bandung";
String countryCode = "ID";

unsigned long lastTime = 0;
unsigned long timerDelay = 10000;

String jsonBuffer;

void setup() {
  Serial.begin(9600);
  Cayenne.begin(username, mqtt_password, cliend_id, ssid, password);
  pinMode(2, OUTPUT);
  pinMode(5, OUTPUT);
  digitalWrite(2, HIGH);
  //  digitalWrite(5, LOW);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

}

void loop() {
  Cayenne.loop();
  // Send an HTTP GET request
  if ((millis() - lastTime) > timerDelay) {
    // Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {
      String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey;

      jsonBuffer = httpGETRequest(serverPath.c_str());
      //  Serial.println(jsonBuffer);
      JSONVar myObject = JSON.parse(jsonBuffer);

      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(myObject) == "undefined") {
        // Serial.println("Parsing input failed!");
        return;
      }
      int temp = (myObject["main"]["temp"]);
      temp = temp - 273;
      int hum = (myObject["main"]["humidity"]);
      Serial.print ("kedalaman: ");
      Serial.println (depth);
      Serial.print ("Humidity: ");
      Serial.println (hum);
      Serial.print ("Tangga: ");
      Serial.println (tangga);
      Serial.print ("Temperature: ");
      Serial.println (temp);
      Serial.print ("Humidity: ");
      Serial.println (hum);

      //  Cayenne.virtualWrite (1, depth);
      Cayenne.virtualWrite (2, tangga);
      Cayenne.virtualWrite (3, temp);
      Cayenne.virtualWrite (4, hum);
      Cayenne.virtualWrite (6, battery);
      

    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
  // Charge Toggle
  toggle++;
  if (toggle == 6) //1 cycle sekitar 2 detik
  {
    digitalWrite(2, !digitalRead(2));
    toggle = 0;
  }
  Serial.println(toggle);
  /////////////////////////////////////
  // Sensor Part
  // Clear the trigPin by setting it LOW:

  //
  delay(100);
}

String httpGETRequest(const char* serverName) {
  HTTPClient http;

  // Your IP address with path or Domain name with URL path
  http.begin(serverName);

  // Send HTTP POST request
  int httpResponseCode = http.GET();

  String payload = "{}";

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}

CAYENNE_IN(0)
{
  Serial.print ("baca: ");
  Serial.println(baca = getValue.asInt());
  if (baca == 1) {
    for (int i = 0; i <= 9; i++) {
      digitalWrite(trigPin, LOW);
      delayMicroseconds(5);
      // Trigger the sensor by setting the trigPin high for 10 microseconds:
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);
      // Read the echoPin. pulseIn() returns the duration (length of the pulse) in microseconds:
      duration = pulseIn(echoPin, HIGH);
      // Calculate the distance:
      distance = (duration * 0.034 / 2) * 0.9671 - 1.3116;
      depth = kedalaman - distance;
      tangga = (depth / anaktangga) + 0.5;
      // Send to Cayenne
      Cayenne.virtualWrite (1, depth);
      delay(100);
      Serial.print("Kedalaman");
      Serial.println(depth);
    }
    Cayenne.virtualWrite (5, 0);
  }
  
  else {}
}

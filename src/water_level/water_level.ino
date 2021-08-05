#include <CayenneMQTTESP32.h>
#define CAYENNE_DEBUG
#define CAYENNE_PRINT Serial
#define trigPin 13
#define echoPin 12
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

// Define variables:
int modus;
const int battpin = 34;
long duration;
int distance;
int potensio = 0;
int anaktangga = 40;
int kedalaman = 250;
int depth;
int tangga;
int baca;
int grafik;
int volume;
int koreksi = 8;
int depth_array[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//int battery=0;

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
  Cayenne.virtualWrite (9, 1);
  //  digitalWrite(5, LOW);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

}

void loop() {
  Cayenne.loop();
  Cayenne.virtualWrite (7, 1);
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
      Cayenne.virtualWrite (3, temp);
      Cayenne.virtualWrite (4, hum);


    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
  // Charge Toggle
  toggle++;
  if (toggle == 3600) //1 cycle sekitar 1 detik
  {
    digitalWrite(2, !digitalRead(2));
    Cayenne.virtualWrite (9, !digitalRead(2));
    //    toggle = 0;
  }
  else if (toggle == 5400) //1 cycle sekitar 1 detik
  {
    digitalWrite(2, !digitalRead(2));
    Cayenne.virtualWrite (9, !digitalRead(2));
    toggle = 0;
  }
  Serial.println(toggle);
  /////////////////////////////////////
  // Battery Charge Estimation
  /*
    battery = analogRead(battpin);
    battery = map(battery,2482,3350,0,100);
    Serial.print("Battery: ");
    Serial.println(battery);
    Cayenne.virtualWrite (6, battery);
  */
  //Loop delay
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
    for (int i = 0; i <= 19; i++) {
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
      depth = (kedalaman - distance) - koreksi;
      //      tangga = depth / anaktangga;
      //      if (tangga > 5){tangga==5;}
      // Send to Cayenne
      volume = (26.64 * depth / 100) * 1000;
      grafik = depth;
      depth_array[i] = depth;
      //      Cayenne.virtualWrite (2, tangga);
      //      Cayenne.virtualWrite (1, depth);
      //      Cayenne.virtualWrite (6, grafik);
      delay(200);
      //      Cayenne.virtualWrite (10, volume);
      //      Serial.print("Kedalaman");
      //      Serial.println(depth);
      //      Serial.print("tangga: ");
      //      Serial.println(tangga);
    }
    byte jumlahSampel = sizeof(depth_array) / sizeof(depth_array[0]);
    //    Serial.print("modus = ");
    if (cariModus(depth_array, jumlahSampel, &modus))
    {
      grafik = modus;
      volume = (26.64 * modus / 100) * 1000;
      tangga = modus / anaktangga;
      if (tangga > 5) {
        tangga == 5;
      }
      Cayenne.virtualWrite (2, tangga);
      Cayenne.virtualWrite (1, modus);
      Cayenne.virtualWrite (6, grafik);
      Cayenne.virtualWrite (10, volume);
      Serial.print("Kedalaman");
      Serial.println(depth);
      Serial.print("tangga: ");
      Serial.println(tangga);
    }
    else
    {
      modus = depth_array[5];
      grafik = modus;
      volume = (26.64 * modus / 100) * 1000;
      tangga = modus / anaktangga;
      if (tangga > 5) {
        tangga == 5;
      }
      Cayenne.virtualWrite (2, tangga);
      Cayenne.virtualWrite (1, modus);
      Cayenne.virtualWrite (6, grafik);
      Cayenne.virtualWrite (10, volume);
      Serial.print("Kedalaman");
      Serial.println(depth);
      Serial.print("tangga: ");
      Serial.println(tangga);
      Serial.println("tidak ada");
    }
    //    Cayenne.virtualWrite (5, 0);
  }

  else {}
}

// Fungsi mencari modus

bool cariModus(int *dataArray, byte jumlahData, int *modus)
{
  byte frekuensi;
  float frekuensiData;
  byte frekuensiMax = 0;
  byte frekuensiMaxCount = 0;
  *modus = 0;
  for (byte i = 0; i < jumlahData; ++i)
  {
    frekuensi = 0;
    for (byte j = 0; j < jumlahData; j++)
    {
      if (dataArray[i] == dataArray[j])
      {
        frekuensi++;
      }
    }
    if (frekuensiMax < frekuensi)
    {
      frekuensiMax = frekuensi;
      frekuensiData = dataArray[i];
      *modus = dataArray[i];
      frekuensiMaxCount = 0;
    }
    else if ((frekuensiMax == frekuensi) && (frekuensiData != dataArray[i]))
    {
      frekuensiMaxCount++;
    }
  }
  if (!frekuensiMaxCount)
  {
    return true;
  }
  return false;
}

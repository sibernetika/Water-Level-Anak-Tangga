#include <CayenneMQTTESP8266.h>
#define CAYENNE_DEBUG
#define CAYENNE_PRINT Serial
#define trigPin 13
#define echoPin 12
// Define variables:
long duration;
int distance;
int potensio =0;
int anaktangga=0;
char ssid[] = "132"; //wifi ssid
char password[]="home0909"; //password wifi
char username[] = "595fe320-f9f9-11e9-84bb-8f71124cfdfb"; //username cayenne
char mqtt_password[] = "ca1601088df82b4ba32cbda89696559250bf7743"; //mqtt password cayenne
char cliend_id[] = "45eb9c90-e1c4-11ea-a67f-15e30d90bbf4"; //id cayenne


void setup() {
Serial.begin(9600);
Cayenne.begin(username, mqtt_password, cliend_id, ssid, password);
//pinMode(2, OUTPUT); //toogle manual dan otomatis
//digitalWrite(2, HIGH);
pinMode(trigPin, OUTPUT); //pin ultrasonic setup
pinMode(echoPin, INPUT);
}

void loop() {
potensio = analogRead(A0);
Serial.println(potensio);
// Clear the trigPin by setting it LOW:
  digitalWrite(trigPin, LOW);
  
  delayMicroseconds(5);
 // Trigger the sensor by setting the trigPin high for 10 microseconds:
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Read the echoPin. pulseIn() returns the duration (length of the pulse) in microseconds:
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance:
  distance = (duration*0.034/2)*0.9671-1.3116;
  // potensio ganti dengan distance untuk sensor ultrasonic
  if (potensio <= 100){
    anaktangga = 1;
  }
  else if (potensio <=200){
    anaktangga = 2;
  }
  else if (potensio <=300){
    anaktangga = 3;
  }
  else if (potensio <=400){
    anaktangga = 4;
  }
  else if (potensio <=500){
    anaktangga = 5;
  }
  Cayenne.loop();
  Cayenne.virtualWrite (1, potensio); // potensio ganti dengan distance untuk sensor ultrasonic
  Cayenne.virtualWrite (2, anaktangga);
  delay (100);
}

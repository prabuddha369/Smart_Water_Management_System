#include<ESP8266WiFi.h>
#include<ESP8266HTTPClient.h>

// Defining pins for ultrasonic sensor
#define TRIG_PIN D7
#define ECHO_PIN D8
#define BUZZER D3
#define PUMP D6

HTTPClient http;
WiFiClient wifiClient;

String ssid = "Router 1";
String pas = "Your Password";
String api_key = "Your API key";
const char *host = "blynk.cloud";
const int httpPort = 80;
String url = "/external/api/update?token="+api_key+"&v0=";
String newurl="";

double tank_depth= 20.0;
bool flg = false;

void setup() {
  Serial.begin(9600);
  
  // Set up pin modes for the ultrasonic sensor and buzzer
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(PUMP, OUTPUT);


  WiFi.disconnect();
  WiFi.mode(WIFI_STA); // set wifi to station mode
  WiFi.begin(ssid,pas); //connecting to the specified network
  //WiFi.begin("Wokwi-GUEST", "", 6);
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print("....");
    delay(200);
  }

  Serial.print("I am connected with "+ssid);
  Serial.println(" ");
  Serial.print("IP: http://");
  Serial.println(WiFi.localIP());
 
}



void loop() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Measuring the duration of the echo pulse
  unsigned long duration = pulseIn(ECHO_PIN, HIGH);
  
  double distance = (double)duration * 0.034 / 2.0; // Speed of sound is approximately 34cm/ms
  
   float per_empty = (float)(distance/tank_depth)*100.0;
   float per_filled = 100.0 - per_empty;
  
  //   Controling the buzzer and Pump based on distance
  if(distance <= 5){
    if(flg){
    digitalWrite(BUZZER, HIGH);//setting buzzer before turning off the pump
    delay(250);
    digitalWrite(BUZZER, LOW);}
    if(distance <= 3) {
    flg=false;
    digitalWrite(PUMP, LOW);//turning off pump
    digitalWrite(BUZZER, LOW);
    }
  }

 if(distance > 5 && distance <16)
 {
  digitalWrite(BUZZER, LOW);
 }

 if(distance >= 16)
 {
  if(!flg){
   digitalWrite(BUZZER, HIGH);//turning buzzer on before the tank gets empty
   delay(250);
   digitalWrite(BUZZER, LOW);}
  if(distance >= 18)
  {
    flg=true;
    digitalWrite(PUMP, HIGH);//turning on pump
    digitalWrite(BUZZER, LOW);
  }
 }


  //for visual purpose
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  Serial.print("Percentage: ");
  Serial.print(per_filled);
  Serial.println(" %");
  Serial.println();

  //eliminating garbage data
  if (per_filled>=0 && per_filled<=100)
  {
    //sending data to cloud
     newurl = url+String((int)per_filled);
      http.begin(wifiClient,host,httpPort,newurl); //Sending request
      int httpCode = http.GET(); // response code, if it is 200 means successfull request
      Serial.println(httpCode);
  }

  delay(250);
}

#include <WiFi.h>
#include <PubSubClient.h>
#include <PowerManagement.h>
#include "DHT.h"
#define DHTPIN 6    // Digital pin connected to the DHT sensor
#define redpin 10
#define greenpin 11
#define bluepin 12
#define mq4 2
#define light A2
#define fire 13

//#define DHTTYPE DHT22   // DHT 22
#define DHTTYPE DHT11   // DHT 11

char ssid[] = "Galaxy S21 Ultra";
char pass[] = "0966707109";
int status = WL_IDLE_STATUS;
char mqttServer[] = "120.108.111.227";
int mqttPort = 1883;
char clientId[] = "amebaClient";
char username[] = "test";
char password[] = "test";
char publishTopic[] = "108022062";
String message ="";
String tmpmessage ="";

WiFiClient wifiClient;
PubSubClient client(wifiClient);
DHT dht(DHTPIN, DHTTYPE);

void gotData(){
  
  // Read data from DHT sensor
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    message = "Failed to read from DHT sensor!";
  } else {
    message = "Humidity: " + String(h) + "% Temperature: " + String(t) + "°C ";
  }
  
  // Read data from light sensor
  int lightval = analogRead(light);
  if (isnan(lightval)) {
    tmpmessage = "Failed to read from light sensor!";
  } else {
    tmpmessage = String(lightval);
    message = message + " light: " + tmpmessage + " lux";
  }

  // Read data from MQ4 sensor
  int mq4val = digitalRead(mq4);
  if (isnan(mq4val)) {
    tmpmessage = "Failed to read from MQ4 sensor!";
  } else {
    tmpmessage = String(mq4val);
    message = message + " CH4: " + tmpmessage + " ppm";
  }
  
  // Read data from fire sensor
  int fireval = digitalRead(fire);
  if (isnan(fireval)) {
    tmpmessage = "Failed to read from fire sensor!";
  } else {
    tmpmessage = String(fireval);
    message = message + " fire: " + tmpmessage;
  }
  Serial.println(message);
}

void setColor(int red, int green, int blue)
{
  analogWrite(redpin, red);
  analogWrite(greenpin, green);
  analogWrite(bluepin, blue);
}

void LEDcontrol()
{
  if(tval)
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  char message[length+1];
  for (int i=0;i<length;i++) {
    message[i] = (char)payload[i];
    Serial.print(message[i]);
  }
  message[length] = '\0';
  Serial.println();
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.println("Attempting MQTT connection...");
    if (client.connect(clientId, username, password))
    {
      Serial.println("MQTT connected");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(38400);
  dht.begin();
  pinMode(redpin, OUTPUT);
  pinMode(greenpin, OUTPUT);
  pinMode(bluepin, OUTPUT);
  pinMode(mq4, INPUT);
  pinMode(fire, INPUT);
  while (status != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    //status = WiFi.begin(ssid);
    delay(5000);
  }
  printWifiData();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
}

void printWifiData()
{
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  static unsigned long lastPublish = 0;
  unsigned long now = millis();
  if (now - lastPublish >= 10000)
  {
    lastPublish = now;
    // DHTgotdata();
    // mq4gotdata();
    // lightgotdata();
    gotData();
    client.publish(publishTopic, message.c_str());
    Serial.println("Message published.");
  }
  LEDcontrol();
}
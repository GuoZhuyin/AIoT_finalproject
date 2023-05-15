#include <WiFi.h>
#include <PubSubClient.h>
#include <PowerManagement.h>
#include "DHT.h"
#define DHTPIN 6    // Digital pin connected to the DHT sensor

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
DHT dht(DHTPIN, DHTTYPE);

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void DHTgotdata(){
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
 
  if (isnan(h) || isnan(t) ) {
    Serial.println(F("Failed to read from DHT sensor!"));
    message="Failed to read from DHT sensor!";
  }
  else{
     Serial.print(F("Humidity: "));
     Serial.print(h);
     Serial.print(F("%  Temperature: "));
     Serial.println(t);
    message="Humidity: "+String(h)+"%  Temperature: "+String(t)+"°C ";
  }
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
    DHTgotdata();
    client.publish(publishTopic, message.c_str());
    Serial.println("Message published.");
  }
}
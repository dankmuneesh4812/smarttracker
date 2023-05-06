#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <ArduinoMqttClient.h>
  #include <WiFiNINA.h>

char ssid[] = "Ofi";    // your network SSID (name)
char pass[] = "12345678";    // your network password (use for WPA, or use as key for WEP)


WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "broker.hivemq.com";
int        port     = 1883;
const char spo2[]  = "/fit/spo";
const char heartbeat[]  = "/fit/hb";



#define REPORTING_PERIOD_MS   1000
PulseOximeter pox;
uint32_t tsLastReport = 0; 

void setup()
{
  Serial.begin(9600);
 
 
  
    Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();

   Serial.print ("Initializing pulse oximeter..");
  pox.begin();
  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

 
}
void loop()
{
  pox.update();
    if (millis() - tsLastReport > REPORTING_PERIOD_MS)
  {
 Serial.print("Heart rate:");
    Serial.println(pox.getHeartRate());

     mqttClient.poll();

    mqttClient.beginMessage(heartbeat);
    mqttClient.print(pox.getHeartRate());
    mqttClient.endMessage();



    Serial.print("bpm / SpO2:");
    Serial.print(pox.getSpO2());
    Serial.println("%");

    mqttClient.poll();

    mqttClient.beginMessage(spo2);
    mqttClient.print(pox.getSpO2());
    mqttClient.endMessage();

    Serial.println();

    tsLastReport = millis();
    
  }
}

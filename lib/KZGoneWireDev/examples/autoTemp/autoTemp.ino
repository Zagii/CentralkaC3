/* *******************************************************
** Przykład użycia biblioteki, 
** przy pierwszym starcie tworzy plik konfiguracyjny
** po każdym restarcie dodaje do pliku symbol *
** ******************************************************* */

#include <KZGwifi.h>
#include <KZGmqtt.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <KZGoneWireDev.h>

KZGwifi wifi;
KZGmqtt mqtt;
KZGoneWireDev oneW;


void mqttCallback(String topic, String msg)
{
  DPRINTLN("mqttCallback= topic: "+topic+", msg: "+msg);

}

void setup()
{
    Serial.begin(115200);
    wifi.begin("MqttTestAP");
    
    mqtt.begin();
   // mqtt.setCallbackOryginal(MQTTcallback);
    mqtt.setCallback(mqttCallback);
    String confMqtt=mqtt.loadConfigFile();
    if(confMqtt == "")
    {
        Serial.print("Przygotuj domyslny config MQTT");
        //mqtt.setMqtt("broker.hivemq.com",1883,"ESP","","KZGmqttTestIN","KZGmqttTestOUT","KZGmqttDebug");
        mqtt.setMqtt("192.168.1.132",1883,"ESP","","KZGmqttTestIN","KZGmqttTestOUT","KZGmqttDebug");
        confMqtt=mqtt.getConfigStr();
        Serial.println("Zapis konfiguracji MQTT: "+ mqtt.saveConfigFile(confMqtt));
    }else
    {
        Serial.print("Wczytywanie konfiguracji MQTT");
        mqtt.parseConfigStr(confMqtt);
    }

    oneW.begin(D4,12,20000); // pin, precyzja, co ile mierzyc
    Serial.println("Koniec Setup"); 
}

unsigned long m=0;
void loop()
{
    wifi.loop();
    mqtt.loop();
    oneW.loop();
    if(oneW.get18b20ready())
    {
      oneW.set18b20notReady();
      DPRINT(" ************* oneWire Temperatures ready liczba: ");DPRINTLN(oneW.getDevicesCount());
      for(int i=0;i<oneW.getDevicesCount();i++)
      {
        DPRINT(" ** i= ");DPRINT(i);DPRINT(", addr: ");DPRINT(oneW.getDeviceAddresStr(i));DPRINT(", T=");DPRINT(oneW.getDeviceTemperature(i));DPRINTLN(" C");
      }
      DPRINTLN(" ************************ koniec ***************************** ");
      
    }
    if(millis()-m>15000)
    {
        Serial.print("## ");Serial.print(wifi.getWifiStatusString());Serial.println(" ##");
        Serial.print("#### ");Serial.print(wifi.getTimeString());Serial.println(" ####");
        mqtt.mqttPub("KZGmqttTestIN",wifi.getTimeString());
        m=millis();
    }
}
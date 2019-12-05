/* *******************************************************
** Przykład użycia biblioteki, 
 *
** ******************************************************* */

#include <KZGlan_mqtt.h>

KZGlan_mqtt ethMqtt;

void callback(char* topic, uint8_t* payload, unsigned int length) 
{
	DPRINT("### mqtt<>Callback -> topic: ");
    DPRINT(topic);
   
   DPRINT(F(" MQTT_tresc= "));
  String msgStr="";
  for(int i=0;i<length;i++)
  {
    msgStr+=(char)payload[i];
  }
  msgStr[length]='\0';
  DPRINTLN(msgStr);

}


void mqttCallback(String topic, String msg)
{
    Serial.print("### mqttCallback -> topic: ");
    Serial.print(topic);
    Serial.print(", msg: ");
    Serial.println(msg);
}


void setup()
{
    Serial.begin(115200);
    byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
    IPAddress mqttHost(192, 168, 1, 3);

    ethMqtt.begin("c3",mac,mqttHost,"","","",1883,callback);
     
    Serial.println("Koniec Setup"); 
}

unsigned long m=0;
void loop()
{
    ethMqtt.loop();
    if(millis()-m>15000)
    {
        Serial.print("## ");Serial.println(ethMqtt.getEthStatusString());
        m=millis();
        ethMqtt.publish(String("c3/Sub/testFullPath/"),String(m));
        ethMqtt.publishPrefix(String("testPrefix"),String(m));

    }
}
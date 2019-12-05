#ifndef KZGcentralka_h
#define KZGcentralka_h

#include <Arduino.h>
#include <ArduinoJson.h>
//#include <OneWire.h>
//#include <DallasTemperature.h>
#include <KZGinput.h>
#include <KZGoutput.h>
#include <KZGlan_mqtt.h>
#include <KZGoneWireDev.h>
#include <MemoryFree.h>


#define DEBUG_CENTRALKA   //If you comment this line, the DPRINT & DPRINTLN lines are defined as blank.
#ifdef DEBUG_CENTRALKA    //Macros are usually in all capital letters.
  #define DPRINT_CENT(...)    Serial.print(__VA_ARGS__)     //DPRINT is a macro, debug print
  #define DPRINTLN_CENT(...)  Serial.println(__VA_ARGS__)   //DPRINTLN is a macro, debug print with new line
#else
  #define DPRINT(...)     //now defines a blank line
  #define DPRINTLN(...)   //now defines a blank line
#endif

#define MAX_INPUTS 20
#define MAX_OUPUTS 25
//#define MAX_TERMOMETERS 6


class KZGcentralka
{
  byte _mac[7];//    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
  String _name;
  uint8_t _input_num;
  uint8_t _output_num;
  KZGlan_mqtt _ethMqtt;
  KZGinput _inputs[MAX_INPUTS];
  KZGoutput _outputs[MAX_OUPUTS];
  //uint8_t _w1_pin;
  //uint8_t _ds18b20precision;
  //OneWire *_oneWire;
  //DallasTemperature *_sensors;
  KZGoneWireDev *_oneW;
  //unsigned long _tempFreq;
  //unsigned long _lastTempMeasure;
  //bool _termometerWaiting=false;
  //int _numberOfDevices; 
  //DeviceAddress _tempDeviceAddress[MAX_TERMOMETERS+1];

  //String printAddressToStr(DeviceAddress deviceAddress);
  public:
  KZGcentralka(){  };
  void mqttMyCallback(char* topic, uint8_t* payload, unsigned int length);
  void begin(String name,byte mac[], IPAddress mqttHostIP, String mqttHost,String mqttUser,String mqttPwd, uint16_t mqttPort,uint8_t w1_pin, uint8_t ds18b20precision, unsigned long tempFreq);
  void loop();
  void addInput(uint8_t pin,String nazwa, uint8_t initState, bool activeLow);
  void addOutput(String name, uint8_t pin, uint16_t on, uint16_t off, uint16_t initState,bool usePCA9685);
  void mqttMyCallbackStr(String topic, String msg);
  //void checkNewTermometers();
};
#endif

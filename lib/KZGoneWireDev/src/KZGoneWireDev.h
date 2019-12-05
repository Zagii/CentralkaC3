#ifndef KZGONEWIREDEV_h
#define KZGONEWIREDEV_h

#include <OneWire.h>
#include <DallasTemperature.h>

#define MAX_TERMOMETERS 10

#define DEBUG_1W   //If you comment this line, the DPRINT & DPRINTLN lines are defined as blank.
#ifdef DEBUG_1W    //Macros are usually in all capital letters.
  #define DPRINT_1W(...)    Serial.print(__VA_ARGS__)     //DPRINT is a macro, debug print
  #define DPRINTLN_1W(...)  Serial.println(__VA_ARGS__)   //DPRINTLN is a macro, debug print with new line
#else
  #define DPRINT(...)     //now defines a blank line
  #define DPRINTLN(...)   //now defines a blank line
#endif


class KZGoneWireDev
{
  uint8_t _1w_pin;
  uint8_t _ds18b20precision;
  OneWire* _oneWire;
  DallasTemperature* _sensors;
  unsigned long _tempFreq;
  unsigned long _lastTempMeasure;
  unsigned long _millisWaitForConversion;
  bool _termometerWaiting=false;
  int _numberOfDevices; 
  DeviceAddress _tempDeviceAddress[MAX_TERMOMETERS+1];
  double _temperaturaC[MAX_TERMOMETERS+1];
  bool _18b20ready; // oznacza ze wlasnie pojawily sie nowe odczyty temperatur

  String printAddressToStr(DeviceAddress deviceAddress);
public:
  KZGoneWireDev(){};
  void loop();
  void begin(uint8_t x1w_pin,uint8_t ds18b20precision, unsigned long tempFreq);
  void checkNewTermometers();
  int getDevicesCount(){return _numberOfDevices;}
  String getDeviceAddresStr(int id){return printAddressToStr(_tempDeviceAddress[id]);}
  double getDeviceTemperature(int id){return _temperaturaC[id];}
  void set18b20notReady(){ _18b20ready=false;}
  bool get18b20ready(){return _18b20ready;}
};
#endif

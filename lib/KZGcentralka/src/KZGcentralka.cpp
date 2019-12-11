#include <KZGcentralka.h>

bool globalIsMqttW8ing=false;
char globalTopic[70];
char globalMsg[200];

void mqttCallback(char* topic, uint8_t* payload, unsigned int length) 
{
  
	if(length>=200)
	{
		strcpy(globalMsg,"mqttCallbackLenErr: ");
	 	for(unsigned int i=strlen(globalMsg);i<197;i++)
		{
		    globalMsg[i+1]=(char)payload[i];
		}
		 globalMsg[198]='\0';
	//	_ethMqtt.publishDebugChar(globalMsg);
  DPRINTLN_CENT(globalMsg);
	}
  if(globalIsMqttW8ing)return; /// anty spam

	DPRINT_CENT("### mqtt<>Callback -> topic: ");
    DPRINT_CENT(topic);
   
   DPRINT_CENT(F(" MQTT_tresc= "));
 // String msgStr="";
  for(unsigned int i=0;i<length;i++)
  {
    globalMsg[i]=(char)payload[i];
  }
  globalMsg[length]='\0';
  DPRINTLN_CENT(globalMsg);
  //globalTopic=topic;
  strcpy(globalTopic,topic);
  //globalMsg=msgStr;
  //strcpy(globalMsg,msgStr.c_str());

  globalIsMqttW8ing=true;
//  KZGcentralka::mqttMyCallback(String(topic),msgStr);
}
/*
void KZGcentralka::mqttMyCallback(char* topic, uint8_t* payload, unsigned int length) 
{
DPRINT_CENT("### mqtt<>KZGcentralka::MyCallback -> topic: ");
    DPRINT_CENT(topic);
   
   DPRINT_CENT(F(" MQTT_tresc= "));
  String msgStr="";
  for(unsigned int i=0;i<length;i++)
  {
    msgStr+=(char)payload[i];
  }
  msgStr[length]='\0';
  DPRINTLN_CENT(msgStr);

}*/
void KZGcentralka::mqttMyCallbackChar(char* topic, char* msg)
{
	DPRINT_CENT("### KZGcentralka::mqttMyCallbackChar -> topic: ");
  	DPRINT_CENT(topic);
  	DPRINT_CENT(", msg: ");
  	DPRINTLN_CENT(msg);
 if (strstr(topic, "set") != NULL) 
 {
    	 //DPRINTLN_CENT("topic zawiera set");
    StaticJsonDocument<200> doc;
    //DynamicJsonDocument doc(msg.length());
    DeserializationError error = deserializeJson(doc, msg);
    if (error)
    {
      DPRINT_CENT("Deserialize Error!->");DPRINTLN_CENT(error.c_str());
    return;
    }
    const char* name = doc["out"];
    const char* c=doc["c"]; 
    char fullName[10];
    strcpy(fullName,c);
    strcat(fullName,name);
  //  DPRINT_CENT("   szukamy, name:");DPRINTLN_CENT(nameStr);
    uint8_t id=255;
    //////// szukanie odpowiedniego wyjscia
    for(uint8_t i=0;i<_output_num;i++)
    {
      if(strcmp(_outputs[i].getName().c_str(),fullName)==0)
      {
        id=i;
        break;
      }
    }
    if(id==255)
    {
      DPRINT_CENT("!! unknown name: ");
      DPRINTLN_CENT(fullName);
      return;
    }
        
   //     DPRINTLN_CENT(id);
    uint8_t value; 
    char* rozkaz=doc["set"]|"brak";
    if(strcmp(rozkaz,"brak")==0) return;
    if(strcmp(rozkaz,"ON")==0) value=_outputs[id].getOnValue();
    else if(strcmp(rozkaz,"OFF")==0) value=_outputs[id].getOffValue();
    else if(strcmp(rozkaz,"PWM")==0) value=doc["pwm"];
    else if(strcmp(rozkaz,"W8")==0) value=_outputs[id].getValue();
    unsigned long def=999999;
    uint8_t def8=111; //tylko zakres 0-100%
    double defD=-1.0; //ujemne nie mogą być
    double ttc=doc["ttc"]|defD; // za ile bedzie zmiana stanu
    uint8_t futureState	 =def8;
       if(doc["futSt"].is<char*>())
	{
	   if(strcmp(doc["futSt"],"ON")==0)
	     futureState=_outputs[id].getOnValue();
    	   else 
	     futureState=_outputs[id].getOffValue();
	}
	 else futureState=doc["futSt"]|def8; //na jaki stan zmieniamy
    double duration =doc["dur"]|defD;
    if((ttc!=defD)&&(futureState!=def8)) //timeToChange, czyli będzie zmiana stanu po czasie duratiuon
    {                 // zmieni sie na futureState
     if(strcmp(rozkaz,"PWM")==0)
      {
        if(duration!=defD)
          _outputs[id].setFadingDurationThenChange(value,duration,futureState,ttc);
        else 
          _outputs[id].setOutputThenChange(value,futureState,ttc);
      }else // ON, OFF, W8
      {
        _outputs[id].setOutputThenChange(value,futureState,ttc);
      }
    } else
    {
     if(strcmp(rozkaz,"PWM")==0)
      {
        _outputs[id].stopWaitingStopFading();
        if(duration!=defD)
          _outputs[id].setFadingDuration(value,duration);
        else 
          _outputs[id].setOutput(value);
      }else // ON, OFF, W8
      {
	_outputs[id].stopWaitingStopFading();
        _outputs[id].setOutput(value);
      }
    }  
}
}
void KZGcentralka::mqttMyCallbackStr(String topic, String msg)
{
  DPRINT_CENT("### KZGcentralka::mqttMyCallbackStr -> topic: ");
  DPRINT_CENT(topic);
  DPRINT_CENT(", msg: ");
  DPRINTLN_CENT(msg);
   
  if(topic.indexOf("set")>0)
  {
    //DPRINTLN_CENT("topic zawiera set");
    StaticJsonDocument<200> doc;
    //DynamicJsonDocument doc(msg.length());
    DeserializationError error = deserializeJson(doc, msg);
    if (error)
    {
      DPRINT_CENT("Deserialize Error!->");DPRINTLN_CENT(error.c_str());
    return;
    }
    const char* name = doc["out"];
     const char* c=doc["c"]; 
    String nameStr=String(c)+String(name);
  //  DPRINT_CENT("   szukamy, name:");DPRINTLN_CENT(nameStr);
    uint8_t id=255;
    //////// szukanie odpowiedniego wyjscia
    for(uint8_t i=0;i<_output_num;i++)
    {
      if(_outputs[i].getName()==nameStr)
      {
        id=i;
        break;
      }
    }
    if(id==255)
    {
      DPRINT_CENT("!! unknown name: ");
      DPRINTLN_CENT(nameStr);
      return;
    }
        
   //     DPRINTLN_CENT(id);
    uint16_t value; 
    String rozkaz=doc["set"]|"brak";
    if(rozkaz=="brak") return;
//DPRINTLN_CENT(rozkaz);
    if(rozkaz=="ON") value=_outputs[id].getOnValue();
    else if(rozkaz=="OFF") value=_outputs[id].getOffValue();
    else if(rozkaz=="PWM") value=doc["pwm"];
//DPRINTLN_CENT(value);
    //if(value==nullptr)return;
    unsigned long def=999999;
    uint8_t def8=111; //tylko zakres 0-100%
    double defD=-1.0; //ujemne nie mogą być
    double ttc=doc["ttc"]|defD; // za ile bedzie zmiana stanu
  //  DPRINTLN_CENT(ttc);
    uint8_t futureState=doc["futSt"]|def8; //na jaki stan zmieniamy
    //DPRINTLN_CENT(futureState);
    double duration =doc["dur"]|defD;
   // DPRINTLN_CENT(duration);
    //unsigned long speed=doc["speed"]|def;
    //DPRINTLN_CENT(speed);
    if((ttc!=defD)&&(futureState!=def8)) //timeToChange, czyli będzie zmiana stanu po czasie duratiuon
    {                 // zmieni sie na futureState
      if(rozkaz=="PWM")
      {
        if(duration!=defD)
          _outputs[id].setFadingDurationThenChange(value,duration,futureState,ttc);
      //  else if(speed!=def)
        //  _outputs[id].setFadingSpeedThenChange(value,speed,futureState,ttc);
        else 
          _outputs[id].setOutputThenChange(value,futureState,ttc);
      }else
      {
        _outputs[id].setOutputThenChange(value,futureState,ttc);
      }
    } else
    {
      if(rozkaz=="PWM")
      {
        _outputs[id].stopWaitingStopFading();
        if(duration!=defD)
          _outputs[id].setFadingDuration(value,duration);
        //else if(speed!=def)
//          _outputs[id].setFadingSpeed(value,speed);
        else 
          _outputs[id].setOutput(value);
      }else
      {
	_outputs[id].stopWaitingStopFading();
        _outputs[id].setOutput(value);
      }
    }  
  }
}

void KZGcentralka::begin(String name,byte mac[], IPAddress mqttHostIP, String mqttHost, String mqttUser,String mqttPwd, uint16_t mqttPort, uint8_t w1_pin,uint8_t ds18b20precision, unsigned long tempFreq)
{
  DPRINTLN_CENT("Debug KZGcentralka::begin start"); 
  _name=name;
  DPRINT("begin mqtt");DPRINTLN_CENT(freeMemory());
  _ethMqtt.begin(name,mac,mqttHostIP,mqttHost,mqttUser,mqttPwd,mqttPort,mqttCallback);
  
 // _ethMqtt.begin(name,mac,mqttHost,mqttUser,mqttPwd,mqttPort,[this] (String topic, String msg) { this->callback(String topic, String msg); }));
  _input_num=0;
  _output_num=0;
     DPRINT("begin oneWire");DPRINTLN_CENT(freeMemory());
  _oneW=new KZGoneWireDev();
  _oneW->begin(w1_pin,ds18b20precision,tempFreq);


  DPRINTLN_CENT("Debug KZGcentralka::begin end"); 
}
/*String KZGcentralka::printAddressToStr(DeviceAddress deviceAddress)
{
  String s="";
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) s+="0";
    s+=String(deviceAddress[i], HEX);
  }
  return s;
}*/

void KZGcentralka::addInput(uint8_t pin,String nazwa, uint8_t initState, bool activeLow)
{
  _inputs[_input_num].init(pin,nazwa,initState,activeLow);
  _input_num++;
}
void KZGcentralka::addOutput(String name, uint8_t pin, uint16_t on, uint16_t off, uint16_t initState,bool usePCA9685)
{
  _outputs[_output_num].begin(name,pin,on,off,initState,usePCA9685);
  _output_num++;
}

///////////////////////// freeRam
int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
/////////////////////
void KZGcentralka::loop()
{
  if(globalIsMqttW8ing)
  {
  //  String st=String(globalTopic);
  //  String sm=String(globalMsg);
  //  mqttMyCallbackStr(st,sm);
    mqttMyCallbackChar(globalTopic,globalMsg);
    globalIsMqttW8ing=false;
  }
  _ethMqtt.loop();
 ///////////////////////////// one wire start /////////////////
  _oneW->loop();
 if(_oneW->get18b20ready())
  {
      _oneW->set18b20notReady();
      //Serial.print(" ************* oneWire Temperatures ready liczba: ");Serial.println(_oneW->getDevicesCount());
      for(int i=0;i<_oneW->getDevicesCount();i++)
      {
      //  Serial.print(" ** i= ");Serial.print(i);Serial.print(", addr: ");Serial.print(_oneW->getDeviceAddresStr(i));
      //  Serial.print(", T=");Serial.print(_oneW->getDeviceTemperature(i));Serial.println(" C");

      //  String s="{\"name\":\""+_name+"\", \"tempAddr\": \""+_oneW->getDeviceAddresStr(i)+"\"";
        //s+=", \"t\": "+String(_oneW->getDeviceTemperature(i))+"}";
	      
	 strcpy(_jsonCharArr,"{\"name\":\""); strcat(_jsonCharArr,_name.c_str());
	 strcat(_jsonCharArr,"\", \"tempAddr\": \"");strcat(_jsonCharArr,_oneW->getDeviceAddresStr(i).c_str());
	 String tstr=String(_oneW->getDeviceTemperature(i));
	 strcat(_jsonCharArr,"\", \"t\": "); strcat(_jsonCharArr,tstr.c_str()); strcat(_jsonCharArr,"}");
	       DPRINT_CENT("    -");DPRINT_CENT(i);DPRINT_CENT(": ");DPRINTLN_CENT(_jsonCharArr);
        _ethMqtt.publishPrefixChar("temp/"+_oneW->getDeviceAddresStr(i)+"/",_jsonCharArr);
      }
      //Serial.println(" ************************ koniec ***************************** ");
      
  }
  ////////////////////////////// one wire koniec /////////////////////////////
      
  
//////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
  for(uint8_t i=0;i<_input_num;i++)
  {
    if(_inputs[i].loop())
    {
           // DPRINTLN_CENT("Inp MemFree: ");
     // DPRINTLN_CENT(freeMemory());
      //String s=_inputs[i].getStatusString();
     DPRINTLN_CENT("Btn event, new status: ");
     _inputs[i].getStatusChar(_jsonCharArr);
     DPRINT_CENT(strlen(_jsonCharArr));
      DPRINTLN_CENT(_jsonCharArr);
     // s="test";
      _ethMqtt.publishPrefixChar("input/"+String(i)+"/",_jsonCharArr);
    }
  }
  for(uint8_t j=0;j<_output_num;j++)
  {
    if(_outputs[j].loop())
    {
      DPRINTLN_CENT("MemFree: ");
      DPRINTLN_CENT(freeMemory());
      _outputs[j].getJsonStatusChar(_jsonCharArr);
         // String so= _outputs[j].getJsonStatusStr();
       DPRINT_CENT("@@@ output change finished: ");
       DPRINT_CENT(strlen(_jsonCharArr));
       DPRINTLN_CENT(_jsonCharArr);
       _ethMqtt.publishPrefixChar("output/"+String(j)+"/",_jsonCharArr);
    }
    if(_outputs[j].isFading())
    {
        if(millis()%300==0)
        {
	    _outputs[j].getJsonShortStatusChar(_jsonCharArr);
	    _ethMqtt.publishPrefixChar("output/"+String(j)+"/",_jsonCharArr);
           // String so= _outputs[j].getJsonStatusStr();
            //DPRINT_CENTLN("@@@@@@@@ analog output is fading: ");
            //DPRINTLN_CENT(so);
        }
    }
  }

}

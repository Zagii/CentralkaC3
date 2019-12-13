#include <KZGlan_mqtt.h>
/*void KZGlan_mqtt::callback(char* topic, byte* payload, unsigned int length) 
{
  String topicStr=String(topic);
  DPRINT_LAN(F("Topic:"));
  DPRINT_LAN(topicStr);

  DPRINT_LAN(F(" dlugosc= "));
  DPRINT_LAN(length);
 
  DPRINT_LAN(F(" MQTT_tresc= "));
  String msgStr="";
  for(int i=0;i<length;i++)
  {
    msgStr[i]+=(char)payload[i];
  }
  msgStr[length]='\0';
  DPRINTLN_LAN(msgStr);

    
  if(strcmp(_mojTopicIDPing.c_str(),topicStr.c_str())==0) //powrocil komunikat ping
  {
    _ostatniOdczytanyPingMqtt=millis();
    DPRINT_LAN(F("reveived ping at [ms]: "));DPRINTLN_LAN(_ostatniOdczytanyPingMqtt);
  }else if(strcmp(_mojTopicIDSubscribeConfig.c_str(),topicStr.c_str())==0)
  {
    DPRINT_LAN(F("reveived config msg: "));DPRINTLN_LAN(msgStr);
    
  }else if(strcmp(_mojTopicIDSubscribe.c_str(),topicStr.c_str())==0)
  {
    //_mqttCallback(topicStr,msgStr);
  }
}*/
void KZGlan_mqtt::publish(char* topic, char* msg)
{
  if (_mqttClient.connected())
  {
    _mqttClient.publish(topic,msg);
  }
}
void KZGlan_mqtt::publishPrefixCh(char* t,char* m)
{
    strcpy(tmpCharArr,_mojTopicIDPublishCh);
    strcat(tmpCharArr,t);
    publish(tmpCharArr,m);
  
}
void KZGlan_mqtt::publishDebugChar(char* m)
{  
    publish(_mojTopicIDPublishDebugCh,m); 
}
/*void KZGlan_mqtt::publishPrefixChar(String topic,char* m)
{
  if (_mqttClient.connected())
  {
    String t=_mojTopicIDPublish+topic;
    _mqttClient.publish(t.c_str(),m);
  }
}*/

/*void KZGlan_mqtt::publishPrefix(String topic, String msg)
{
  if (_mqttClient.connected())
  {
    String t=_mojTopicIDPublish+topic;
    _mqttClient.publish(t.c_str(),msg.c_str());
    DPRINTLN_LAN("*** KZGlan_mqtt::publishPrefix wyslano: ***");
    DPRINT_LAN("topic: ");DPRINTLN_LAN(t);
    DPRINT_LAN("  msg: ");DPRINTLN_LAN(msg);
    DPRINTLN_LAN("********************************************");
    
  }else
  {
    DPRINTLN_LAN("ERRR: KZGlan_mqtt::publishPrefix - mqttClient not connected");
  }
  
}*/

void KZGlan_mqtt::begin(String name,byte * mac, IPAddress mqttHostIP, String mqttHost,String mqttUser,String mqttPwd, uint16_t mqttPort, KZGlan_mqttCallback mqttCallback)
{
   begin(name.c_str(),mac,mqttHostIP,mqttHost.c_str(),mqttUser.c_str(),mqttPwd.c_str(),mqttPort,mqttCallback);
}
void KZGlan_mqtt::begin(char* name,byte * mac, IPAddress mqttHostIP, char* mqttHost,char* mqttUser,char* mqttPwd, uint16_t mqttPort, KZGlan_mqttCallback mqttCallback)
{
  DPRINTLN_LAN("Debug KZGlan_mqtt::begin start"); 
  	strcpy(_nameCh,name);
  _mac[0]=mac[0];_mac[1]=mac[1];_mac[2]=mac[2];
  _mac[3]=mac[3];_mac[4]=mac[4];_mac[5]=mac[5];
//  _mojTopicIDSubscribe=name+"/Sub/#"; //topic z komunikatami z serwera
	strcpy(_mojTopicIDSubscribeCh,_nameCh); strcat(_mojTopicIDSubscribeCh,"/Sub/#");
  //_mojTopicIDSubscribeConfig=name+"/ConfigSub/#"; //topic z komunikatami z serwera
	strcpy(_mojTopicIDSubscribeConfigCh,_nameCh); strcat(_mojTopicIDSubscribeConfigCh,"/ConfigSub/#");
  //_mojTopicIDPublish=name+"/Pub/"; // topic z komunikatami do serwera
	strcpy(_mojTopicIDPublishCh,_nameCh); strcat(_mojTopicIDPublishCh,"/Pub/");
  //_mojTopicIDPublishDebug="DebugTopic/"+name+"/"; // topic z komunikatami do serwera
	strcpy(_mojTopicIDPublishDebugCh,"DebugTopic/"); strcat(_mojTopicIDPublishDebugCh,_nameCh);
  //_mojTopicIDPing=name+"/Ping/"; //topic z komukatami keepalive
	strcpy(_mojTopicIDPingCh,_nameCh); strcat(_mojTopicIDPingCh,"/Ping/");
  //_mqttUsr=mqttUser;
	strcpy(_mqttUsrCh,mqttUser.c_str());
  //_mqttPwd=mqttPwd;
	strcpy(_mqttPwdCh,mqttPwd.c_str());
  _mqttPort=mqttPort;
  if(strlen(mqttHost)==0)
  {
    //_mqttHostDNS="";
    _mqttHostIP[0]=mqttHostIP[0];_mqttHostIP[1]=mqttHostIP[1];
    _mqttHostIP[2]=mqttHostIP[2];_mqttHostIP[3]=mqttHostIP[3];
    _mqttClient.setServer(_mqttHostIP, _mqttPort);
  }else
  {
   // _mqttHostDNS=mqttHost;
    //_mqttClient.setServer(_mqttHostDNS.c_str(), _mqttPort);
  }
  //_mqttClient.setCallback(MQTTcallback); <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
  _mqttCallback = mqttCallback;
  _mqttClient.setCallback(mqttCallback);
 //_mqttClient.setCallback([this] (char* topic, byte* payload, unsigned int length) { this->callback(topic, payload, length); });
  //_mqttClient.setCallback([this](void) (char* topic, uint8_t* payload, unsigned int length) { this->callback(topic, payload, length); });
  //_mqttClient.setCallback(callback);
  _mqttClient.setClient(_ethClient);

  ethReconnect();
  mqttReconnect();
  DPRINTLN_LAN("Debug KZGlan_mqtt::begin end"); 
}


void KZGlan_mqtt::ethReconnect()
{
  DPRINTLN_LAN(F("KZGlan_mqtt::ethReconnect")); 
 
  DPRINT_LAN(F("Starting ethernet..."));
  if(!Ethernet.begin(_mac))
  {
     DPRINTLN_LAN(F("failed"));
     auto link = Ethernet.linkStatus();
     DPRINT_LAN("Link status: ");
  switch (link) {
    case Unknown:
      DPRINTLN_LAN("Unknown");
      break;
    case LinkON:
      DPRINTLN_LAN("ON");
      break;
    case LinkOFF:
      DPRINTLN_LAN("OFF");
      break;
  }
  }
  else {
      DPRINTLN_LAN(Ethernet.localIP());
      DPRINTLN_LAN(Ethernet.gatewayIP());
  }
  DPRINTLN_LAN(getEthStatusChar());
}
void KZGlan_mqtt::mqttReconnect()
{
  DPRINTLN_LAN(F("KZGlan_mqtt::mqttReconnect")); 
  if (!_mqttClient.connected()) 
  {
    _mqttClient.disconnect();
    DPRINT_LAN(F("Proba polaczenia z MQTT .."));
    bool mqttconnStat=false;
    if(strlen(_mqttUsrCh)>0)
    {
      mqttconnStat=_mqttClient.connect(_nameCh,_mqttUsrCh,_mqttPwdCh);
    }else
    {
      mqttconnStat=_mqttClient.connect(_nameCh);
    }
    if(mqttconnStat) 
    {
	DPRINTLN_LAN(F(" [ok]"));
	DPRINTLN_LAN(F("Subskrypcja mqtt: "));
        _mqttClient.subscribe(_mojTopicIDSubscribeCh);
	DPRINT_LAN(F("-- "));DPRINT_LAN(_mojTopicIDSubscribeCh);DPRINTLN_LAN(F("; "));
        _mqttClient.subscribe(_mojTopicIDSubscribeConfigCh);
        DPRINT_LAN(F("-- "));DPRINT_LAN(_mojTopicIDSubscribeConfigCh);DPRINTLN_LAN(F("; "));
	_mqttClient.subscribe(_mojTopicIDPingCh);
        DPRINT_LAN(F("-- "));DPRINT_LAN(_mojTopicIDPing);DPRINTLN_LAN(F("; "));
	_mqttClient.publish(_mojTopicIDPublishDebugCh,"Polaczenie");
        
      }else
      {
        DPRINT_LAN(F("nieudane: "));
        DPRINTLN_LAN(_mqttClient.state());
      }
    }
}

char* KZGlan_mqtt::getEthStatusChar() 
{
  strcpy(tmpCharArr,""); 
  char a[4];
  if(_ethClient.connected())
  {
    IPAddress ip=Ethernet.localIP();
    strcat(tmpCharArr,"Eth connected: ");
	
	itoa(ip[0],a,10);strcat(tmpCharArr,a);strcat(tmpCharArr,".");
	itoa(ip[1],a,10);strcat(tmpCharArr,a);strcat(tmpCharArr,".");
	itoa(ip[2],a,10);strcat(tmpCharArr,a);strcat(tmpCharArr,".");
	itoa(ip[3],a,10);strcat(tmpCharArr,a);strcat(tmpCharArr,".");  
  }else
  {
  strcat(tmpCharArr,"Eth Connection Error. status= ");
  itoa(_ethClient.connected(),a,10); strcat(tmpCharArr,a);
    
  }
  return tmpCharArr;
}

/*String KZGlan_mqtt::getEthStatusString_wywalic() 
{
  String statStr=""; 
  if(_ethClient.connected())
  {
    IPAddress ip=Ethernet.localIP();
    statStr="Eth connected: "+ String(ip[0])+"."+String(ip[1])+"."+String(ip[2])+"."+String(ip[3]);
    
  }else
  {
    statStr="Eth Connection Error. status= "+ String(_ethClient.connected());
    
  }
  return statStr;
}*/

void KZGlan_mqtt::loop()
{
  unsigned long now = millis();

  /////////////////////////////// ETH  test connection ////////////////////////////////
  if(!_ethClient.connected())
  {
    if( _isEthNewDisconnecion)
    {
      DPRINTLN_LAN(F("*** -> Eth disconnected ***"));
      DPRINTLN_LAN(getEthStatusChar());
      _isEthNewDisconnecion=false;
    }
    if (now - _lastEthReconnectAttempt > 5000) 
    {
      _lastEthReconnectAttempt = now;
      DPRINTLN_LAN(F("*** Eth reconnecting.."));
      _isEthNewConnecion=true;
      ethReconnect();
      mqttReconnect();
    }
  } else
  {
    if(_isEthNewConnecion)
    {
      DPRINTLN_LAN(F("*** -> Eth new conn estabilished ***"));
      DPRINTLN_LAN(getEthStatusChar());
      _isEthNewConnecion=false;
      _isEthNewDisconnecion=true;
    }
    ////////////////////////////////////////////////////////////////////////////
  
  
    if (!_mqttClient.connected())
    {
      if (now - _lastMqttReconnectAttempt > 5000) 
      {
        _lastMqttReconnectAttempt = now;
        DPRINTLN_LAN(F("***** MQTT reconnecting.."));
        mqttReconnect();
      }
    } 
    else
    {    // Client connected
      _mqttClient.loop();
      if(now-_lastPingMillis>60000)
      {
        _lastPingMillis=now;
        
        unsigned long sec = millis() / 1000;
        unsigned long min = sec / 60;
        unsigned long hr = min / 60;
        unsigned long dd = hr /24;

        sec=sec%60;
        min=min%60;
        hr=hr%24;
        
        char tmp[50];
        sprintf ( tmp,"%s: %02lud%02lu:%02lu:%02lu ",_nameCh,dd,hr, min,sec);
        
        _mqttClient.publish(_mojTopicIDPingCh,tmp);
      }  
    }
  }


}

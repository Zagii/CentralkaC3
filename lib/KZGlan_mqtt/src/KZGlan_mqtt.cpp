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
void KZGlan_mqtt::publish(String topic, String msg)
{
  if (_mqttClient.connected())
  {
    _mqttClient.publish(topic.c_str(),msg.c_str());
  }
}
void KZGlan_mqtt::publishPrefix(String topic, String msg)
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
  
}


void KZGlan_mqtt::begin(String name,byte * mac, IPAddress mqttHostIP, String mqttHost,String mqttUser,String mqttPwd, uint16_t mqttPort, KZGlan_mqttCallback mqttCallback)
{
  DPRINTLN_LAN("Debug KZGlan_mqtt::begin start"); 
  _name=name;
  _mac[0]=mac[0];_mac[1]=mac[1];_mac[2]=mac[2];
  _mac[3]=mac[3];_mac[4]=mac[4];_mac[5]=mac[5];
  _mojTopicIDSubscribe=name+"/Sub/#"; //topic z komunikatami z serwera
  _mojTopicIDSubscribeConfig=name+"/ConfigSub/#"; //topic z komunikatami z serwera
  _mojTopicIDPublish=name+"/Pub/"; // topic z komunikatami do serwera
  _mojTopicIDPublishDebug="DebugTopic/"+name+"/"; // topic z komunikatami do serwera
  _mojTopicIDPing=name+"/Ping/"; //topic z komukatami keepalive
  _mqttUsr=mqttUser;
  _mqttPwd=mqttPwd;
  _mqttPort=mqttPort;
  if(mqttHost=="")
  {
    _mqttHostDNS="";
    _mqttHostIP[0]=mqttHostIP[0];_mqttHostIP[1]=mqttHostIP[1];
    _mqttHostIP[2]=mqttHostIP[2];_mqttHostIP[3]=mqttHostIP[3];
    _mqttClient.setServer(_mqttHostIP, _mqttPort);
  }else
  {
    _mqttHostDNS=mqttHost;
    _mqttClient.setServer(_mqttHostDNS.c_str(), _mqttPort);
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
  DPRINTLN_LAN(getEthStatusString());
}
void KZGlan_mqtt::mqttReconnect()
{
  DPRINTLN_LAN(F("KZGlan_mqtt::mqttReconnect")); 
  if (!_mqttClient.connected()) 
  {
    _mqttClient.disconnect();
    DPRINT_LAN(F("Proba polaczenia z MQTT .."));
    bool mqttconnStat=false;
    if(_mqttUsr.length()>0)
    {
      mqttconnStat=_mqttClient.connect(_name.c_str(),_mqttUsr.c_str(),_mqttPwd.c_str());
    }else
    {
      mqttconnStat=_mqttClient.connect(_name.c_str());
    }
    if(mqttconnStat) 
    {
		DPRINTLN_LAN(F(" [ok]"));
		DPRINTLN_LAN(F("Subskrypcja mqtt: "));
        _mqttClient.subscribe(_mojTopicIDSubscribe.c_str());
		DPRINT_LAN(F("-- "));DPRINT_LAN(_mojTopicIDSubscribe);DPRINTLN_LAN(F("; "));
        _mqttClient.subscribe(_mojTopicIDSubscribeConfig.c_str());
        DPRINT_LAN(F("-- "));DPRINT_LAN(_mojTopicIDSubscribeConfig);DPRINTLN_LAN(F("; "));
		_mqttClient.subscribe(_mojTopicIDPing.c_str());
        DPRINT_LAN(F("-- "));DPRINT_LAN(_mojTopicIDPing);DPRINTLN_LAN(F("; "));
		_mqttClient.publish(_mojTopicIDPublishDebug.c_str(),"Polaczenie");
        
      }else
      {
        DPRINT_LAN(F("nieudane: "));
        DPRINTLN_LAN(_mqttClient.state());
      }
    }
}



String KZGlan_mqtt::getEthStatusString() 
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
}

void KZGlan_mqtt::loop()
{
  unsigned long now = millis();

  /////////////////////////////// ETH  test connection ////////////////////////////////
  if(!_ethClient.connected())
  {
    if( _isEthNewDisconnecion)
    {
      DPRINTLN_LAN(F("*** -> Eth disconnected ***"));
      DPRINTLN_LAN(getEthStatusString());
      _isEthNewDisconnecion=false;
    }
    if (now - _lastEthReconnectAttempt > 5000) 
    {
      _lastEthReconnectAttempt = now;
      DPRINTLN_LAN(F("*** Eth reconnecting.."));
      _isEthNewConnecion=true;
      ethReconnect();
    }
  } else
  {
    if(_isEthNewConnecion)
    {
      DPRINTLN_LAN(F("*** -> Eth new conn estabilished ***"));
      DPRINTLN_LAN(getEthStatusString());
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
        sprintf ( tmp,"%s: %02lud%02lu:%02lu:%02lu ",_name.c_str(),dd,hr, min,sec);
        
        _mqttClient.publish(_mojTopicIDPing.c_str(),tmp);
      }  
    }
  }


}

#include "KZGoutput.h"

    
void KZGoutput::begin(String name, uint8_t pin, unsigned long on, unsigned long off, unsigned long initState, bool usePCA9685)
{ 
  
  _name=name;
  _pin=pin;
  _usePCA9685=usePCA9685;
  if(_usePCA9685)
  {
	_faboPWM.begin();
	_faboPWM.init(0);
	_faboPWM.set_hz(1000);
  }else
  {
	pinMode(_pin,OUTPUT);
  }
  _on=on;
  _off=off;
  if(_on>_off)
  {
    _max=_on;
    _min=_off;
  }else
  {
    _max=_off;
    _min=_on;
  }
  String s="outBegin(name: "+name+", pin: "+String(pin)+", on: "+String(on)+", off: "+String(off)+", initState: "+String(initState)+", usePCA: "+String(usePCA9685);
 DPRINTLN_OUT(s);
  setOutput(initState);
  loop();
}

void KZGoutput::setOutputStopWaiting(unsigned long state)
{
   _isWaitingForChange=false; /////<<<<< to dodałem ostatnio do wytestowania wplyw na pwm
   setOutput(state);
}
void KZGoutput::setOutput(unsigned long state)
{
  DPRINT_OUT("KZGoutput::setOutput = ");DPRINTLN_OUT(state);
  _currentState=state;
  _isFading=false;
 
}
    
/*******************************
 * aimState - docelowy stan PWM
 * speed - predkosc zmian PWM w krokach/s
 *****************************/
void KZGoutput::setFadingSpeed(unsigned long aimState, unsigned long speed)
{
  DPRINT_OUT("KZGoutput::setFadingSpeed = ");DPRINT_OUT(aimState);DPRINT_OUT(" speed=");DPRINTLN_OUT(speed);
  if(_currentState==aimState)return;
  _fadeSpeed=speed;
  _aimState=aimState;
  _isFading=true;
  _timerPWM=millis();
  _cyklPWM=T_PWM;
}

/******************************
 * aimState - docelowy PWM
 * duration - czas trwania fadingu w ms
 * *************************/
void KZGoutput::setFadingDuration(unsigned long aimState, unsigned long duration)
{
	if(_currentState==aimState)return;
    unsigned long speed,droga;
	if(aimState>_currentState)droga=aimState-_currentState; else droga = _currentState-aimState;
	_cyklPWM=duration/droga;
	_fadeSpeed=1;
	_aimState=aimState;
	_isFading=true;
	_timerPWM=millis();
	DPRINT_OUT("KZGoutput::setFadingDuration = ");DPRINT_OUT(aimState);DPRINT_OUT(", cykl=");DPRINT_OUT(_cyklPWM);DPRINT_OUT(" duration=");DPRINTLN_OUT(duration);
	DPRINT_OUT(" startMillis=");DPRINTLN_OUT(_timerPWM);

}
void KZGoutput::prepareAutoChangeState(unsigned long futureState, unsigned long timeToChangeState,bool fading)
{
	_isWaitingForChange=true; 
	_waitingForChangeDuration=timeToChangeState;
	_waitingForChangeStartTime=millis();
	_futureState=futureState;
	_futureChangeFading=fading;
}
void KZGoutput::setOutputThenChange(unsigned long state, unsigned long futureState, unsigned long timeToChangeState)
{
  DPRINTLN_OUT("setOutputThenChange(state: "+String(state)+", futState: "+String(futureState)+", ttc: "+String(timeToChangeState)+")");
	prepareAutoChangeState(futureState,timeToChangeState,false);
	setOutput(state);
}
void KZGoutput::setFadingSpeedThenChange(unsigned long aimState, unsigned long speed,unsigned long futureState, unsigned long timeToChangeState)
{
	prepareAutoChangeState(futureState,timeToChangeState,true);
	setFadingSpeed(aimState,speed);
}
void KZGoutput::setFadingDurationThenChange(unsigned long aimState, unsigned long duration, unsigned long futureState, unsigned long timeToChangeState)
{
	prepareAutoChangeState(futureState,timeToChangeState,true);
	setFadingDuration(aimState,duration);
}

/*************************
 * return true if 
 * fading reach the aim (for analogWrite) 
 * or switched on/off (for digitalWrite)
 * 
 * *************************/
bool KZGoutput::loop()    // if state has changed
{
//DPRINT_OUT("HardwareSate:");DPRINT_OUT(_hardwareState);DPRINT_OUT(" isFading:");DPRINT_OUT(_isFading);DPRINT_OUT(" currentState:");DPRINTLN_OUT(_currentState);
  if(_isWaitingForChange)
  {
    if(millis()-_waitingForChangeStartTime>_waitingForChangeDuration)
    {
      _isWaitingForChange=false;
      _isFading=false;
      if(_futureChangeFading)
      {
        setFadingDuration(_futureState,KZGoutput_autoOFFduration);
      }else
      {
        setOutput(_futureState);
      }
    }
  }
  if(_isFading)
  {
    if(millis() - _timerPWM>_cyklPWM)
    {
      _timerPWM=millis();
      if(_currentState>_aimState)
      {
        if(_currentState>_aimState+_fadeSpeed) 
        {
          _currentState-=_fadeSpeed;
	//	  DPRINT_OUT("KZGoutput::loop A");DPRINTLN_OUT(_currentState);
        }
        else
        {
		    _currentState=_aimState;
	//		DPRINT_OUT("KZGoutput::loop B");DPRINTLN_OUT(_currentState);
        }
      }else
      {
        if(_currentState+_fadeSpeed>_aimState)
        {
          _currentState=_aimState;
	//	  DPRINT_OUT("KZGoutput::loop C");DPRINTLN_OUT(_currentState);
        } else
        {
          _currentState+=_fadeSpeed;
	//	  DPRINT_OUT("KZGoutput::loop D");DPRINTLN_OUT(_currentState);
        } 
      }
      if(_currentState>_max)_currentState=_max;
      if(_currentState<_min)_currentState=_min;
    }
  }

  /////////////////////////// hardware state change //////////////////////
  if(_hardwareState!=_currentState)
  {
   // DPRINT_OUT("KZGoutput::loop hs");DPRINTLN_OUT(_currentState);
    _hardwareState=_currentState;
    _timeOfHardwareState=millis();
    if(_hardwareState==_on || _hardwareState == _off)
    {
  	//  DPRINT_OUT("KZGoutput::loop digitalChange =>");DPRINTLN_OUT(_hardwareState);
      if(_usePCA9685)
      {
        _faboPWM.set_channel_value(_pin, _hardwareState);
      }else
      {
        digitalWrite(_pin,_hardwareState);
      }
    //  DPRINT_OUT("d stopMillis=");DPRINTLN_OUT(millis());
      _isFading=false;
      return true;
    } else
    {
  //	  DPRINT_OUT("KZGoutput::loop analogChange");DPRINTLN_OUT(_hardwareState);
      if(_usePCA9685)
      {
        _faboPWM.set_channel_value(_pin, _hardwareState);
      }else
      {
        analogWrite(_pin,_hardwareState);
      }
      if(_aimState==_hardwareState)
      {
	      DPRINT_OUT("KZGoutput::loop stopFading");DPRINTLN_OUT(_hardwareState);
		    DPRINT_OUT("a stopMillis=");DPRINTLN_OUT(millis());
        _isFading=false;
        return true;
      }
    }
  }else 
  {
    if(_isFading)
    {	
      //_isFading=false;
      //return true;
    }
  }
  return false;
}

String KZGoutput::getJsonStatusStr()
{
  String f="0";
  if(isFading())f="1";
  String w=/*"{\"pin\":\""+String(_pin)+"\",*/"{\"name\":\""+String(_name)+"\", \"v\":"+String(_hardwareState);
  w+=", \"isFad\":"+f +", \"aimV\":"+String(_aimState);
  f="0";
  unsigned long ttc=0;
  if(_isWaitingForChange)
  {
  f="1";
  ttc=millis()-_waitingForChangeStartTime-_waitingForChangeDuration;
  }
  w+=", \"isW8t4Chng\":"+f+", \"futSt\":"+String(_futureState) + ", \"ttc\":"+String(ttc);
  return w+="}";
}
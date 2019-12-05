/* *******************************************************
** Przykład użycia biblioteki, 
 *
** ******************************************************* */

#include <KZGoutput.h>

KZGoutput o_digital;
KZGoutput o_analog;


void setup()
{
    Serial.begin(115200);
    randomSeed(analogRead(0));
    o_digital.begin("outDigital",0,0xFFF,0,0,true);
    o_analog.begin("outAnalog",1,0xFFF,0,0,true);
   
    Serial.println("Koniec Setup"); 
}

unsigned long md=0;

long rand1=0;
void loop()
{
  
    /////////////////////// digital pin output /////////////////////////
    if(o_digital.loop())
    {
        Serial.print("### digital output change: ");
        Serial.println(o_digital.getJsonStatusStr());
    }
    if(millis()-md>rand1)
    {
        md=millis();
        rand1=random(1000,10000);
        if(o_digital.getState()>0) o_digital.setOutputOFF(); else o_digital.setOutputON();
    }
    ////////////////////////////////////////////////////////////////////
    
    ////////////////////// analog output //////////////////////////////////
    if(o_analog.loop())
    {
        Serial.print("@@@ analog output fading finished: ");
        Serial.println(o_analog.getJsonStatusStr());
    }
    if(o_analog.isFading())
    {
        if(millis()%1000==0)
        {
            Serial.print("@@@@@@@@ analog output is fading: ");
            Serial.println(o_analog.getJsonStatusStr());
        }
    }else
    {
        // new random fading
        long randValue = random(0, 5)*50;
        long randDur =random(1000,8000);
        String s="@@@@@ new random fading: value= "+String(randValue)+", duration= "+String(randDur);
        Serial.println(s);
        o_analog.setFadingDuration(randValue,randDur);
    }
    

}
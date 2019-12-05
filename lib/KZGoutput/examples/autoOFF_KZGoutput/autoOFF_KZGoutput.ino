/* *******************************************************
** Przykład użycia biblioteki, 
 *
** ******************************************************* */

#include <KZGoutput.h>

KZGoutput o0;
KZGoutput o1;


void setup()
{
    Serial.begin(115200);
    randomSeed(analogRead(0));
    o0.begin("out0",0,0xFFF,0,0,true);
    o1.begin("out1",1,0xFFF,0,0,true);
   
	//o0.setOutputThenChange(0xFFF,0,5000);
	//o1.setFadingDurationThenChange(0xFF,7000,0xFFF,10000);
    Serial.println("Koniec Setup"); 
}

unsigned long m=0;

unsigned long m2=0;
void loop()
{
  o0.loop();
  o1.loop();  
	if(millis()-m>10000)
	{
	Serial.print("rozkaz, ");Serial.println(millis());
	o0.setFadingDurationThenChange(0xFFF,2000,0,12000);
	o1.setFadingDurationThenChange(0xFFF,5000,0,6000);
  
	m=millis();
	}
 if(millis()-m2>1000)
 {
  m2=millis();
  Serial.println(o0.getJsonStatusStr());
  Serial.println(o1.getJsonStatusStr());
  }
}
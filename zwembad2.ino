/********************************************************************/
// First we include the libraries
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>


/********************************************************************/
// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2
/********************************************************************/
// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
/********************************************************************/
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
/********************************************************************/
// Set the LCD address to 0x38 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x38, 16, 2);
/********************************************************************/


int PompPin = 3;
int ButtonPin = 5;


float TempFlowOut;
float TempPool;
float TempOutdoor;
float TempTopRack;


float GemTempFlowOut;
float GemTempPool;
float GemTempOutdoor;
float GemTempTopRack;


int ArrayElements = 10;


float TempFlowOutArray[10];
float TempPoolArray[10];
float TempOutdoorArray[10];
float TempTopRackArray[10];


float TempDelta = 5.00;
float MaxTempPool = 24.00;


int TimerMax = 300;
int Timer = TimerMax-ArrayElements-ArrayElements;
int Timer2 = 0;
int ArrayCounter =  0;


boolean PompAan = false;


void setup(void)
{
 // start serial port
 Serial.begin(9600);


 // initialize the LCD
 lcd.begin();
 // Turn on the blacklight and print a message.
 lcd.backlight();
 
 // Start up the library
 sensors.begin();


 pinMode(PompPin, OUTPUT);
 pinMode(ButtonPin, INPUT);
 digitalWrite(ButtonPin, HIGH); //activate arduino internal pull up
}


void loop(void)
{
 // call sensors.requestTemperatures() to issue a global temperature
 // request to all devices on the bus
/********************************************************************/
 
 sensors.requestTemperatures(); // Send the command to get temperature readings
 
/********************************************************************/


   //Vraag temperatuur op aan de sensors  
   TempPoolArray[ArrayCounter] = sensors.getTempCByIndex(2);
   TempFlowOutArray [ArrayCounter] = sensors.getTempCByIndex(1);
   TempOutdoorArray[ArrayCounter] = sensors.getTempCByIndex(3);
   TempTopRackArray[ArrayCounter] = sensors.getTempCByIndex(0);


   ArrayCounter++;
   if (ArrayCounter == ArrayElements)
   {
      ArrayCounter=0;
   }
   TempPool =0;
   
   for (int i = 0; i < ArrayElements; i++)
   {
      TempPool = TempPool + TempPoolArray[i];
   }
   TempPool = TempPool / ArrayElements;
   
   TempFlowOut =0;
   for (int i = 0; i < ArrayElements; i++)
   {
      TempFlowOut = TempFlowOut + TempFlowOutArray[i];
   }
   TempFlowOut = TempFlowOut  / ArrayElements;
   
   TempTopRack =0;
   for (int i = 0; i < ArrayElements; i++)
   {
      TempTopRack = TempTopRack + TempTopRackArray[i];
   }
   TempTopRack = TempTopRack / ArrayElements;
   TempOutdoor =0;
   for (int i = 0; i < ArrayElements; i++)
   {
      TempOutdoor = TempOutdoor + TempOutdoorArray[i];
   }
   TempOutdoor = TempOutdoor / ArrayElements;

    Serial.print("  TempPool: ");
      Serial.print(TempPool);
      Serial.print("  TempFlowOut: ");
      Serial.print(TempFlowOut);
      Serial.print("  TempTopRack: ");
      Serial.print(TempTopRack);
      Serial.print("  TempOutdoor: ");
      Serial.print(TempOutdoor);
      Serial.print("  Timer: ");
      Serial.print(Timer);
      Serial.print("/");
      Serial.println(TimerMax);


     
     
 
      


   //lcd.setCursor(0,0); //First line
   //lcd.print("In:");
   //lcd.print(TempPool,1);
   //lcd.setCursor(8,0); //First line
   //lcd.print("Out:");
   //lcd.print(TempOutdoor,1);
   //lcd.setCursor(0,1); //Second line
   //lcd.print("Fridge:");
   //lcd.print(TempFlowOut,1);


   if (TempPool > MaxTempPool)
   {
       Serial.println("Info: ZwembadTemp hoger dan 24 graden");
       if ( (TempPool - TempOutdoor) > TempDelta )
       {
           Serial.println("   Pomp aan: Zwembad is 5 graden warmer dan de buitenlucht --> afkoelen");
           PompAan=true;
       }
       else
       {
           Serial.println("   Pomp uit: Buitentemperatuur te hoog, kan niet afkoelen");
           PompAan=true;
       }
   }
   else
   {
       Serial.println("Info: ZwembadTemp lager dan 24 graden");
        if ((TempTopRack-TempPool) > TempDelta)
        {
            Serial.println("   Pomp aan: Verwarming op dak is 5 graden warmer dan het zwembadwater");
            PompAan=true;
        }
        else
        {
            Serial.println("   Pomp uit: Verwarming op dak NIET 5 graden warmer dan het zwembadwater");
            PompAan=false;
        }
   }
  
   //Om te voorkomen dat de pomp continue aan en uit gaat wordt er pas een beslissing genomen als de timer verlopen is
   Timer++;
   if (Timer>TimerMax)
   {
      if (PompAan)
      {
        Serial.println("Pomp Aangeschakeld");
        digitalWrite(PompPin, HIGH);
      }
      else
      {
        Serial.println("Pomp Uitgeschakeld");
        digitalWrite(PompPin, LOW);
      }
      Timer=0;
   }
      
   //Voor handmatig de pomp aan en uit zetten en een wachtlus te creeeren:
   for (int i = 0; i < 1000; i++)
   {
      //Als de knop wordt ingedrukt
      if (digitalRead(ButtonPin)==LOW)
      {
          delay(500);
          Serial.println("Button is pressed buiten loop");
       
          //Toggle de PompAan variable....
          if (PompAan)
          {
              Serial.println("Pomp Handmatig  Uit");
              digitalWrite(PompPin, LOW);
              PompAan = false;
          }
          else
          {
              Serial.println("Pomp Handmatig Aan");
              PompAan = true;
              digitalWrite(PompPin, HIGH);
          }


          //Houdt deze stand voor de tijd van TImer2 vast      
          for (Timer2 = 0; Timer2 < 600; Timer2++)
          {
             Serial.print("Timer2: ");
             Serial.println(Timer2);
       
             //Als er gedurende de wachttijd toch op de knop gedrukt wordt, dan verlaat de wachtloop
             if (digitalRead(ButtonPin)==LOW)
             {
                Timer2=600;
                Serial.println("Button is pressed binnen loop");
             }
             delay(100);
             //Zet timer weer op maximaal zodat er een nieuwe beslissing genomen wordt over het aan of uitschakelen van de pomp
             Timer = TimerMax;
          }
      }
      Timer2 = 0; 
      
   }
   delay(500); 
}


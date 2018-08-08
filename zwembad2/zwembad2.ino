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

boolean buttonActive = false;
boolean longPressActive = false;
long buttonTimer = 0;
long longPressTime = 500;

boolean ShortOverRule = false;
unsigned long ShortOverRuleTimer;
long ShortOverRuleTime = 60000;

boolean LongOverRule = false;
unsigned long LongOverRuleTimer;
long LongOverRuleTime = 18000000;



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


int TimerMax = 1000;
int Timer = TimerMax-ArrayElements-ArrayElements;
int Timer2 = 0;
int ArrayCounter =  0;

int Wacht =0;


boolean PompAan = false;

String Message;

void setup(void)
{
 // start serial port
 Serial.begin(19200);


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
   if ( TempPoolArray[ArrayCounter] < -100 )  // Als er een foute waarde wordt gelezen (-128) dan pak de gemiddelde waarde van de vorige metingen
   {
      TempPoolArray[ArrayCounter] = TempPool;
   }
   
   TempFlowOutArray [ArrayCounter] = sensors.getTempCByIndex(1);
   if ( TempFlowOutArray [ArrayCounter] < -100 )
   {
      TempFlowOutArray [ArrayCounter] = TempFlowOut; 
   }
   
   TempOutdoorArray[ArrayCounter] = sensors.getTempCByIndex(3);
   if ( TempOutdoorArray[ArrayCounter] < -100)
   {
      TempOutdoorArray[ArrayCounter] = TempOutdoor;
   }
   
   TempTopRackArray[ArrayCounter] = sensors.getTempCByIndex(0);
   if ( TempTopRackArray[ArrayCounter] < -100 )
   {
      TempTopRackArray[ArrayCounter] = TempTopRack;
   }


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

   
    
  

     
     
 
      


   //lcd.setCursor(0,0); //First line
   //lcd.print("In:");
   //lcd.print(TempPool,1);
   //lcd.setCursor(8,0); //First line
   //lcd.print("Out:");
   //lcd.print(TempOutdoor,1);
   //lcd.setCursor(0,1); //Second line
   //lcd.print("Fridge:");
   //lcd.print(TempFlowOut,1);

 if ((!ShortOverRule) && (!LongOverRule))
 {  
   if (TempPool > MaxTempPool)
   {
       //Serial.println("Info: ZwembadTemp hoger dan 24 graden");
       if ( (TempPool - TempOutdoor) > TempDelta )
       {
           Message="   Pomp aan: Zwembad is 5 graden warmer dan de buitenlucht --> afkoelen";
           PompAan=true;
       }
       else
       {
           Message="   Pomp uit: Buitentemperatuur te hoog, kan niet afkoelen";
           PompAan=true;
       }
   }
   else
   {
       //Serial.println("Info: ZwembadTemp lager dan 24 graden");
        if ((TempTopRack-TempPool) > TempDelta)
        {
            Message="   Pomp aan: Verwarming op dak is 5 graden warmer dan het zwembadwater";
            PompAan=true;
        }
        else
        {
            Message="   Pomp uit: Verwarming op dak NIET 5 graden warmer dan het zwembadwater";
            PompAan=false;
        }
   }
 }

   if (Wacht == 0)
   {
     for(int i = 0; i <= 13; i++)
     {
          Serial.println(" ");
     }
     for (int i = 0; i < ArrayElements; i++)
   {
      Serial.print(TempPoolArray[i]);
      Serial.print(" ");
   }
   Serial.println(" ");

     Serial.print("TempPool: ");
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

     Serial.println(Message);
     
     
     Serial.print("ShortOverRule = ");
     Serial.print(ShortOverRule);
     if (ShortOverRule)
     {
       Serial.print(" Timer = ");
       Serial.print( (ShortOverRuleTime - (millis() - ShortOverRuleTimer)) /1000);
      }
     Serial.print(" LongOverRule = ");
     Serial.print(LongOverRule);
     if (LongOverRule)
     {
       Serial.print(" Timer = ");
       long RemainingTimeSeconds = (LongOverRuleTime - (millis() - LongOverRuleTimer)) / 1000;
       int Uren = RemainingTimeSeconds  / 3600;
       int Minuten = (RemainingTimeSeconds - (3600 * Uren)) / 60;
       int Seconden = RemainingTimeSeconds - (3600 * Uren) - (60 * Minuten);
       Serial.print( RemainingTimeSeconds);
       Serial.print(" ");
       Serial.print(Uren);
       Serial.print(":");
       Serial.print(Minuten);
       Serial.print(":");
       Serial.println(Seconden);
       
      }

      lcd.setCursor(0,0); //First line
      lcd.print("Out:");
      lcd.print(TempOutdoor,1);
      lcd.setCursor(9,0); //First line
      lcd.print("Pl:");
      lcd.print(TempPool,1);
      lcd.setCursor(0,1); //Second line
      lcd.print("Rk:");
      lcd.print(TempTopRack,1);
      lcd.setCursor(9,1); //Second line
      lcd.print("Fw:");
      lcd.print(TempFlowOut,1);
    
   }
   Wacht++;
   if (Wacht == 10)
   {
     Wacht = 0;
   }
  
    
   //Om te voorkomen dat de pomp continue aan en uit gaat wordt er pas een beslissing genomen als de timer verlopen is
   Timer++;
   if ((Timer>TimerMax) || (ShortOverRule) || (LongOverRule))
   {
      if (PompAan)
      {
        //Serial.println("Pomp Aangeschakeld");
        digitalWrite(PompPin, HIGH);
      }
      else
      {
        //Serial.println("Pomp Uitgeschakeld");
        digitalWrite(PompPin, LOW);
      }
      Timer=0;
   }
      
   //Voor handmatig de pomp aan en uit zetten en een wachtlus te creeeren:
   
      //Als de knop wordt ingedrukt
      if (digitalRead(ButtonPin)==LOW)
      {
          if (buttonActive == false)
          {
            buttonActive = true;
            buttonTimer = millis();
          }

          if ((millis() - buttonTimer > longPressTime) && (longPressActive == false)) 
          {
            longPressActive = true;
            Serial.println("Knop lang gedrukt");
            LongOverRule = true;
            LongOverRuleTimer = millis();
            PompAan = !PompAan;
          }
      }
      else
      {
        if (buttonActive == true)
        {
          if (longPressActive == true)
          {
            longPressActive = false;
          }
          else
          {
            Serial.println("Knop kort gedrukt");
            ShortOverRule = true;
            LongOverRule = false;
            ShortOverRuleTimer = millis();
            PompAan = !PompAan;
          }
          buttonActive = false; 
        }
      }
        
      if ( (millis() - ShortOverRuleTimer) > ShortOverRuleTime) 
      {
        ShortOverRule = false;              
      }

      if ( (millis() - LongOverRuleTimer) > LongOverRuleTime) 
      {
        LongOverRule = false;             
      }
      //Serial.println(ShortOverRule);

      Timer2 = 0; 
      //delay(1000);
      
   
}


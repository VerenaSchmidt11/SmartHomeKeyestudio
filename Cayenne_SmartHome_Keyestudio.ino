// Implementierung eines smarten Home mit Keyestudio, Arduino und Cayenne


// benötigte Bibliotheken
#define CAYENNE_PRINT Serial  // Comment this out to disable prints and save space
#include <CayenneMQTTEthernet.h>
#include <Servo.h> // Servo function library


// Deklaration von globalen Variablen




// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
char username[] = "...";
char password[] = "...";
char clientID[] = "...";

Servo myservoWindow; // Servoantrieb für Rolloe
int pos = 0; // Start angle of servo
int LuminosityVal = 0; // Wert des Beleuchtungssensors
int moveFan = 0; // Variable für die Ansteuerung des Lichts
int light=0; // Variable für den Lampenzustand
int motionState = -1; // Zustand des Bewegungssensors

int valButton; // Variable für den Taster der Lampe

// Defintionen für Sensoren und Aktoren
#define SENSOR_PIN_PIR 2 //PIR
#define SENSOR_PIN_BUTTON 8 // Button


#define ACTUATOR_PIN_YELLOW 5 // YELLOW LED
#define ACTUATOR_PIN_WINDOW 9 // Do not use digital pins 0 or 1 since those conflict with the useb
#define ACTUATOR_PIN_FAN1 7
#define ACTUATOR_PIN_FAN2 6

// Definitionen im Rahmen vom Cayenne Dashboard
#define VIRTUAL_CHANNEL_PIR 15 // PIR Channel
#define VIRTUAL_CHANNEL_YELLOW 5 // YELLOW LED
#define VIRTUAL_CHANNEL_Luminosity 11 // Luminosity
#define VIRTUAL_CHANNEL_BUTTON 1 //Button
#define VIRTUAL_CHANNEL_FAN 12



void setup()
{
  pinMode (ACTUATOR_PIN_YELLOW, OUTPUT); //Setup für die Lampe
  pinMode (SENSOR_PIN_BUTTON, INPUT); // Setup für den Schalter
 
  
   pinMode (ACTUATOR_PIN_FAN1, OUTPUT); // Setup für den Ventilator
   pinMode (ACTUATOR_PIN_FAN2, OUTPUT); // Setup für den Ventilator
  
  myservoWindow.attach (ACTUATOR_PIN_WINDOW); // Setup für den Rollo
   
  
  Serial.begin(9600);  // Setup serieller Monitor
  Cayenne.begin(username, password, clientID); // Cayenne Setup
  WindowRelayOpen(); // Rollo in Grundposition
}

void loop()
{

  // Aus Performance-Gründen am besten nur einzelne Funktionen einschalten. 
  
  Cayenne.loop();
  
  //PIRSensor();
  //ButtonLED();
  Luminosity ();
 
 
 /* if (moveFan == 1)
  {


    //DoorRelay ();
    FanON();
  }
  else
  {
  FanOFF();
  }
/*

 if (motionState<1)
    Cayenne.virtualWrite(VIRTUAL_CHANNEL_PIR, 0, "digital_sensor", "d");
 else
  Cayenne.virtualWrite(VIRTUAL_CHANNEL_PIR, 1, "digital_sensor", "d");*/
}




void PIRSensor () {
// Auswertung des Bewegungssensors
  
   Serial.println (digitalRead (SENSOR_PIN_PIR));
     motionState = digitalRead(SENSOR_PIN_PIR);
    Serial.println ("Motion");
    Serial.println (motionState);
    Cayenne.virtualWrite(VIRTUAL_CHANNEL_PIR, motionState, "digital_sensor", "d");
   
}


void ButtonLED()
{
  // Ansteuerung der Lampe über den Schalter
  
  valButton = digitalRead (SENSOR_PIN_BUTTON); // Read the digital 4 level value and assign it to val
  Serial.println ("Switch");
  
  Serial.println (valButton);
  if (valButton == LOW) // Whether the key is pressed, the light will be on when pressed
  { 
    
    if (light==0)  //toggle light
    {
      digitalWrite (ACTUATOR_PIN_YELLOW, HIGH);
      light=1;                
    
    }
    
    else
    {
      digitalWrite (ACTUATOR_PIN_YELLOW, LOW);
      light=0;                
    
    }
  }
  
  Cayenne.virtualWrite(VIRTUAL_CHANNEL_YELLOW, light, "digital_sensor", "d");
  Cayenne.virtualWrite(VIRTUAL_CHANNEL_BUTTON, light, "digital_sensor", "d");
}



void WindowRelayOpen ()
{
 // Rollo aufmachen
  myservoWindow.write (270); // 0 = close
  
  
}

void WindowRelayClose ()
{
  // Rollo schließen
   myservoWindow.write (0); // 0 = close
 
}

void FanON ()
{
  // Ventilator einschalten
   digitalWrite (7, HIGH);
   digitalWrite (6, LOW); // The motor rotates in the forward direction
   
}

void FanOFF ()
{
  // Ventilator ausschalten
   digitalWrite (7, LOW);
   digitalWrite (6, LOW); // The motor stops rotating
   
}


void Luminosity () {
  // Beleuchtungssensor auslesen und Rollo ansteuern
  LuminosityVal = analogRead (A1); // Read the voltage value of A1 Pin
  Serial.println (LuminosityVal); // Serial port to view the change of voltage value
  Cayenne.virtualWrite(VIRTUAL_CHANNEL_Luminosity, analogRead(A1), "analog_sensor", "null");
  if (LuminosityVal< 200)
  {
    WindowRelayClose();
  }
  else
  {
    WindowRelayOpen();
  }
 
}

CAYENNE_IN(VIRTUAL_CHANNEL_FAN)
{
  // Ansteuerung des Ventilators über Cayenne
  if (getValue.asInt() == 1) {
 
    moveFan = 1;
 
  }
  else {
    moveFan = 0;
 
  }


}


CAYENNE_IN(VIRTUAL_CHANNEL_YELLOW)
{
  if (getValue.asInt() == 1) 
  
      {
        digitalWrite (ACTUATOR_PIN_YELLOW , HIGH); // LED light is on
        light=1;
        Cayenne.virtualWrite(VIRTUAL_CHANNEL_BUTTON, 1, "digital_sensor", "d");
      } else // If no person is detected walking
      {
        
     digitalWrite (ACTUATOR_PIN_YELLOW , LOW); // LED light is not on
     light=0;
     Cayenne.virtualWrite(VIRTUAL_CHANNEL_BUTTON, 0, "digital_sensor", "d");
      }

  //Cayenne.virtualWrite(VIRTUAL_CHANNEL_BUTTON, light, "digital_sensor", "d");
}

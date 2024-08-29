#include <SPI.h>
#include <RF24.h>

#include "joypad.h"       // Inclure la bibliothèque joystick

#define CE_PIN   9
#define CSN_PIN 10


RF24 radio(CE_PIN, CSN_PIN);
joypad manette;

// **Structure pour contenir le message radio**
typedef struct 
{
  int servoA;
  int servoB;
} radiomsg;

radiomsg msg;

uint8_t address[][6] = { "1NODE", "2NODE" };
uint8_t boutons;
uint8_t radioPowerLevel = RF24_PA_LOW;

void setup()
{
  delay(150);
  Serial.begin(115200);
  while (!Serial) {} // some boards need to wait to ensure access to serial over USB  
  if (!radio.begin())
  {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}  // hold in infinite loop
  }

  // Set the PA Level low to try preventing power supply related problems
  // because these examples are likely run with nodes in close proximity to
  // each other.
  radio.setPALevel(RF24_PA_LOW);  // RF24_PA_MAX is default.

  // save on transmission time by setting the radio to only transmit the
  // number of bytes we need to transmit a float
  radio.setPayloadSize(sizeof(msg));  // float datatype occupies 4 bytes

  // set the TX address of the RX node into the TX pipe
  radio.openWritingPipe(address[0]);  // always uses pipe 0

  // set the RX address of the TX node into a RX pipe
  radio.openReadingPipe(1, address[1]);  // using pipe 1

  radio.stopListening();               // Démarrer l'écoute radio

  // Serial.println(F("Setup finish"));
}

void loop()
{    

  boutons = manette.getButton();
  // Altitude haut A
  if (boutons & 0b00000001)
  {
      msg.servoA = 60;
  } else if (boutons & 0b00000100)  // Altitude haut C
    {
        msg.servoA = 120;
    } else 
      {
        msg.servoA = 90;  // retour normal
      }

  if (boutons & 0b00000010)   // Direction droite B
  {
      msg.servoB = 60;
  } else if (boutons & 0b00001000) // Direction gauche D
    {
        msg.servoB = 120;
    } else 
      {
        msg.servoB = 90;
      }
 
  if (!radio.write(&msg, sizeof(msg)))
  {
    Serial.println(F("msg not send"));
  }
  delay(200);
}


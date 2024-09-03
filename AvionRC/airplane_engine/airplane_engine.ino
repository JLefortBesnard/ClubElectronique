// pour le transeiver (SPI commande MOSI MISO)
#include <SPI.h>
#include <RF24.h>

// include the Servo library
#include <Servo.h>

// **Définition des broches utilisées**
#define CE_PIN 7
#define CSN_PIN 8

Servo servoDir;
Servo servoAlt;

// **Objet pour la communication radio**
RF24 radio(CE_PIN, CSN_PIN); // instantiate an object for the nRF24L01 transceiver

// **Tableaux contenant les adresses radio pour l'émetteur et le récepteur**
uint8_t address[][6] = { "1NODE", "2NODE" };

// **Niveau de puissance de la radio**
uint8_t radioPowerLevel = RF24_PA_LOW;

// **Structure pour contenir le message radio**
typedef struct 
{
  int servoA;
  int servoB;
} radiomsg;

radiomsg msg;

void setup()
{
  delay(3000);
  Serial.begin(115200); // Initialiser la communication série pour le débogage
  servoDir.attach(2);   // attaches the servo on pin 2 to the servo direction of airplane // yaw
  servoAlt.attach(3);   // attaches the servo on pin 2 to the servo direction of airplane // pitch
  // roll pour roulie (tonneaux autour de l'axe central)

  // reset the servo position
  servoAlt.write(90);
  servoDir.write(90);
  delay(150);

   // Configurer la radio
  if (!radio.begin())
  {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}  // hold in infinite loop
  }
  // Set the PA Level low to try preventing power supply related problems
  // because these examples are likely run with nodes in close proximity to
  // each other.
  radio.setPALevel(radioPowerLevel);  // RF24_PA_MAX is default.

  // save on transmission time by setting the radio to only transmit the
  // number of bytes we need to transmit a float
  radio.setPayloadSize(sizeof(msg));  // float datatype occupies 

  // set the TX address of the RX node into the TX pipe
  radio.openWritingPipe(address[1]);  // always uses pipe 0

  // set the RX address of the TX node into a RX pipe
  radio.openReadingPipe(1, address[0]);  // using pipe 1

  radio.startListening();               // Démarrer l'écoute radio

}


void loop()
{  
  uint8_t pipe;
  //Serial.println("A");
  if (radio.available(&pipe)) // Vérifier si un message est disponible
  {
    Serial.println(F("Message reçu !"));
    Serial.println(msg.servoA);
    Serial.println(msg.servoB);
    //Serial.println("B");
    uint8_t bytes = radio.getPayloadSize(); // Obtenir
    radio.read(&msg, bytes);                 // Lire le message radio
    // set the servo position
    servoAlt.write(msg.servoA);
    servoDir.write(msg.servoB);
  }
  else
  {
    Serial.println("Radio not available");
  }
  delay(150);
}


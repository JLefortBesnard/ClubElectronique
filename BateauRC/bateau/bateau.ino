/**
 * @file bateau.cpp
 * @author Votre nom (remplacez par votre nom)
 * @date 2024-03-06
 * @brief Point d'entrée principal du programme qui contréle un bateau à l'aide d'une télécommande radio.
 *
 * Ce programme utilise une radio nRF24L01 pour recevoir des instructions de la télécommande et 
 * piloter les moteurs du bateau en conséquence. Il utilise également la librairie `pontH.h` pour 
 * contréler les moteurs et la librairie `reboot.h` pour redémarrer le systéme si nécessaire.
 */

#define BATEAU_DEBUG

#include <SPI.h>
#include <RF24.h>

#include "common.h"
#include "radioMessage.h"
#include "pontH.h"
#include "reboot.h"

// **Définition des broches utilisées**
#define moteurGauchePWM       6
#define moteurDroitPWM        5
#define moteurGaucheDirection 4
#define moteurDroitDirection  3

#define CE_PIN 7
#define CSN_PIN 8

// **Variable pour stocker le timestamp**
unsigned long time = 0;

// **Objet pour la communication radio**
RF24    radio(CE_PIN, CSN_PIN); // instantiate an object for the nRF24L01 transceiver

// **Structure pour contenir le message radio**
radioMessage msg;

// **Objet pour piloter les moteurs**
pontH    pont(moteurGauchePWM, moteurGaucheDirection, moteurDroitPWM, moteurDroitDirection);


// **Tableaux contenant les adresses radio pour l'émetteur et le récepteur**
uint8_t address[][6] = { "1NODE", "2NODE" };

// **Niveau de puissance de la radio**
uint8_t radioPowerLevel = RF24_PA_LOW;



/**
 * @brief Fonction d'initialisation
 */
void setup()
{
  delay(1000);
  Serial.begin(115200); // Initialiser la communication série pour le débogage
  while (!Serial) {} // some boards need to wait to ensure access to serial over USB  

  debugln("coucou");

  // Arréter les moteurs
  pont.stopMoteurs();


  // Configurer la radio
  if (!radio.begin())
  {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}  // hold in infinite loop
  }
  debugln("coucou2");
  // Set the PA Level low to try preventing power supply related problems
  // because these examples are likely run with nodes in close proximity to
  // each other.
  radio.setPALevel(radioPowerLevel);  // RF24_PA_MAX is default.

  // save on transmission time by setting the radio to only transmit the
  // number of bytes we need to transmit a float
  radio.setPayloadSize(sizeof(msg));  // float datatype occupies 4 bytes


  // set the TX address of the RX node into the TX pipe
  radio.openWritingPipe(address[1]);  // always uses pipe 0

  // set the RX address of the TX node into a RX pipe
  radio.openReadingPipe(1, address[0]);  // using pipe 1
  debugln("coucou3");

  radio.startListening();               // Démarrer l'écoute radio
  debugln("coucou4");

}

/**
 * @brief Boucle principale du programme
 */
void loop()
{
  //while(1);
  uint8_t pipe;
  
  if (radio.available(&pipe)) // Vérifier si un message est disponible
  {
    uint8_t bytes = radio.getPayloadSize(); // Obtenir
    radio.read(&msg, bytes);                 // Lire le message radio

    if(messageIsValid(msg))// Vérifier la validité du message
    {
      // Mettre à jour le timestamp
      time = millis();
      debug((int)msg.gauche);
      debug(", ");
      debugln((int)msg.droit);
      pont.vitesseMoteurs(msg.gauche, msg.droit); // Piloter les moteurs en fonction des vitesses reçues
      controleBateau(msg.cmd);
    }
    else
    {
      messageInvalid(); // Signaler la réception d'un message invalide
    }
  }
  else
  {
    debugln("Radio not available");
  }


  // Arréter les moteurs après 100ms d'inactivité radio
  if(millis() > time+100)
  {
    pont.stopMoteurs();
  }
}

/**
 * @brief Fonction pour contréler le bateau en fonction de la commande reçue
 * @param cmd La commande reçue de la télécommande
 */
void controleBateau(char cmd)
{
  // Gérer la commande de redémarrage
  if(cmd & radioCmd::RESET) reboot();

  // Gérer la commande de changement de puissance radio
  if(cmd & 0b111)
  {
    radioPowerLevel = (cmd & radioCmd::PA_MIN) ? RF24_PA_MIN  : radioPowerLevel;
    radioPowerLevel = (cmd & radioCmd::PA_LOW) ? RF24_PA_LOW  : radioPowerLevel;
    radioPowerLevel = (cmd & radioCmd::PA_HI ) ? RF24_PA_HIGH : radioPowerLevel;
    radioPowerLevel = (cmd & radioCmd::PA_MAX) ? RF24_PA_MAX  : radioPowerLevel;
    radio.setPALevel(radioPowerLevel);

    debug(F("RF24_PA change "));
    debugln(radioPowerLevel);
  }
}

/**
 * @brief Fonction pour signaler un message radio invalide
 */
void messageInvalid()
{
  debugln("Reception d'un message invalid");
  debug(*reinterpret_cast<uint32_t*>(&msg), HEX);
  debugln();
}
/**
 * @file telecommande_bateau.ino
 * @author Florent LERAY, Jérémy Lefort Besnard
 * @date 2024-03-06
 * @brief Implémente la fonctionnalité de télécommande pour un bateau utilisant un émetteur-récepteur radio nRF24L01 et un joystick.
 *
 * Ce programme contrôle un bateau à distance à l'aide d'un joystick et envoie des messages via un émetteur-récepteur radio nRF24L01.
 * Il inclut des fonctionnalités pour contrôler la direction du bateau (gauche, droite), calibrer le joystick, envoyer une commande de réinitialisation et redémarrer le système.
 */

#define BATEAU_DEBUG

#include <SPI.h>
#include <RF24.h>
#include <math.h>

#include "common.h"           // Inclure la 
#include "joypad.h"           // Inclure la bibliothèque joystick
#include "joystickToMotors.h" // Inclure la bibliothèque de conversion joystick ver moteurs
#include "radioMessage.h"     // Inclure la définition de la structure du message radio
#include "reboot.h"           // Inclure la fonction de redémarrage

/**
 * @brief Broche CE (Chip Enable) connectée à l'émetteur-récepteur radio nRF24L01
 */
#define CE_PIN   9

/**
 * @brief Broche CSN (Chip Select) connectée à l'émetteur-récepteur radio nRF24L01
 */
#define CSN_PIN 10

/**
 * @brief Objet émetteur-récepteur radio nRF24L01
 */
RF24 radio(CE_PIN, CSN_PIN);

/**
 * @brief Objet joypad
 */
joypad manette;

/**
 * @brief Objet joystick vers moteurs
 */
joystickToMotors jm;

/**
 * @brief Algo du mapping joystick vers moteurs
 */
joystickToMotors::mapping mapping = joystickToMotors::smooth;

/**
 * @brief Structure du message radio
 */
radioMessage msg;

/**
 * @brief Adresses pour l'émetteur et le récepteur
 */
uint8_t address[][6] = { "jlb37", "jlb86" };

/**
 * @brief Stocke le masque binaire des boutons pressés
 */
uint8_t boutons;

/**
 * @brief Niveau de puissance de transmission radio
 */
uint8_t radioPowerLevel = RF24_PA_LOW;


/**
 * @brief Fonction de configuration
 *
 * Cette fonction initialise la radio, définit son niveau de puissance, sa taille de charge utile et ses adresses,
 * commence à écouter les messages entrants et active éventuellement la communication série pour le débogage.
 */
void setup()
{
  delay(1000); 
  Serial.begin(115200); // Initialiser la communication série pour le débogage
  while (!Serial) {} // some boards need to wait to ensure access to serial over USB  

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
  radio.setPayloadSize(sizeof(msg));  // float datatype occupies 4 bytes

  // set the TX address of the RX node into the TX pipe
  radio.openWritingPipe(address[0]);  // always uses pipe 0

  // set the RX address of the TX node into a RX pipe
  radio.openReadingPipe(1, address[1]);  // using pipe 1

  radio.stopListening();               // Démarrer la possibilité d'envois de messages radio

  manette.lightCalibration();
}

/**
 * @brief Fonction de boucle
 *
 * Cette fonction lit en permanence les entrées du joystick, traite les pressions sur les boutons, prépare et envoie des messages radio,
 * et gère les commandes d'étalonnage, de réinitialisation et de redémarrage.
 */
void loop()
{    
	  int8_t x = 0;
	  int8_t y = 0;
	  int8_t g = 0;
	  int8_t d = 0;
    msg.cmd = 0;
	
    /**
     * @brief Lit les valeurs des axes du joystick et les stocke dans la structure du message
     */
    manette.getAxis(x, y);
    // debug("x");
    // debug((int)x);
    // debug(", y");
    // debugln((int)y);
    
    /**
     * @brief Lit le masque binaire des boutons pressés
     */
    boutons = manette.getButton();

    jm.convert(x, y, g, d);
    msg.gauche = g;
    msg.droit  = d;
    
    // debug("g");
    // debug((int)msg.gauche);
    // debug(", d");
    // debugln((int)msg.droit);
    // debugln();

    /**
     * @brief Traite les événements de pression sur les boutons en fonction de leur position binaire
     */
    if (boutons & maskBoutonA)
    {
        debugln("Bouton A");
        manette.calibration(pinBoutonA);
    }
    if (boutons & maskBoutonB)
    {
        debugln("Bouton B");
        msg.gauche = 100;
        msg.droit = -100;
    }
    if (boutons & maskBoutonC)
    {
        debugln("Bouton C");
        radioPowerLevel = (radioPowerLevel + 1) % 4;
        switch (radioPowerLevel) // assige la puissance pour le tranceiver du bateau
        {
        case 0: msg.cmd |= PA_MIN; break;
        case 1: msg.cmd |= PA_LOW; break;
        case 2: msg.cmd |= PA_HI ; break;
        case 3: msg.cmd |= PA_MAX; break;
        }
        radio.setPALevel(radioPowerLevel); // assige la puissance pour le tranceiver de la telecomande
    }
    if (boutons & maskBoutonD)
    {
        debugln("Bouton D");
        msg.gauche = -100;
        msg.droit = 100;
    }
    if (boutons & maskBoutonE)
    {
        debugln("Bouton E");
        msg.cmd = radioCmd::RESET;
    }
    if (boutons & maskBoutonF)
    {
        debugln("Bouton F");
        reboot();
    }
    if (boutons & maskBoutonK)
    {
        debugln("Bouton K");
        mapping = (joystickToMotors::mapping)((uint8_t)(mapping+1) % joystickToMotors::mappinEnumSize);
        jm.changeMapping(mapping);
        debugln(mapping);
        delay(50);

    }

    assignCheck(msg);
    /**
     * @brief Evoi le message radio au bateau
     */
    if (!radio.write(&msg, sizeof(msg)))
    {
      //Serial.println(F("msg not send"));
    }
    delay(100);
}

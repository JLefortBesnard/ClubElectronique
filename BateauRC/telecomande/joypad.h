/**
 * @file joypad.h
 * @author Florent LERAY, Jérémy Lefort Besnard
 * @date 2024-03-06
 * @brief Définit la classe `joypad` pour lire les entrées du joystick et des boutons.
 *
 * Cette classe gère la lecture des axes et des boutons du joystick. Elle fournit également des fonctions pour le calibrage et l'accès à l'état des boutons individuels.
 */

#pragma once
#ifndef JOYPAD_h
#define JOYPAD_h

#include "Arduino.h"

 // **Déclaration des boutons et des broches correspondantes**
#define pinBoutonA 2 ///< Broche du bouton A
#define pinBoutonB 3 ///< Broche du bouton B
#define pinBoutonC 4 ///< Broche du bouton C
#define pinBoutonD 5 ///< Broche du bouton D
#define pinBoutonE 6 ///< Broche du bouton E
#define pinBoutonF 7 ///< Broche du bouton F
#define pinBoutonK 8 ///< Broche du bouton K

 // **Déclaration des boutons et des broches correspondantes**
#define maskBoutonA 0b00000001   ///< Masque binaire du bouton A
#define maskBoutonB 0b00000010   ///< Masque binaire du bouton B
#define maskBoutonC 0b00000100   ///< Masque binaire du bouton C
#define maskBoutonD 0b00001000   ///< Masque binaire du bouton D
#define maskBoutonE 0b00010000   ///< Masque binaire du bouton E
#define maskBoutonF 0b00100000   ///< Masque binaire du bouton F
#define maskBoutonK 0b01000000   ///< Masque binaire du bouton K

// **Broches des axes analogiques**
#define x_axis A0 ///< Broche de l'axe X
#define y_axis A1 ///< Broche de l'axe Y

/**
 * @class joypad
 * @brief Classe pour lire les entrées du joystick et des boutons.
 */
class joypad
{
public:
    /**
     * @brief Constructeur par défaut de la classe joypad
     *
     * Initialise les broches des boutons en entrée et définit les valeurs initiales pour les variables internes.
     */
    joypad();

    /**
     * @brief Destructeur par défaut de la classe joypad
     */
    ~joypad();

    /**
     * @brief Calibrer le joystick
     *
     * Cette fonction effectue un calibrage du joystick en stockant les valeurs minimales et maximales lues sur les axes.
     * @param pin La broche utilisée pour arrêter le calibrage (en appuyant dessus)
     */
    void calibration(uint8_t const & pin);


    /**
     * @brief Calibrer le joystick au repos
     *
     * Cette fonction effectue un calibrage du centre du joystick en stockant les valeurs lues au repo sur les axes.
     */
    void lightCalibration();

    /**
     * @brief Lire les valeurs des axes du joystick
     *
     * Cette fonction lit les valeurs des axes du joystick et les renvoie dans les variables passées en référence.
     * @param x Variable de référence pour stocker la valeur de l'axe X en pourcentage
     * @param y Variable de référence pour stocker la valeur de l'axe Y en pourcentage
     */
    void getAxis(int8_t &x, int8_t &y);

    /**
     * @brief Lire l'état de tous les boutons
     *
     * Cette fonction lit l'état de tous les boutons et renvoie un masque binaire représentant
     * l'état de chaque bouton (1 pour pressé, 0 pour non pressé).
     * @return Masque binaire représentant l'état de tous les boutons
     */
    uint8_t getButton();

    /**
     * @brief Lire l'état d'un bouton spécifique
     *
     * Cette fonction permet de lire l'état d'un bouton spécifique en fonction de sa broche.
     * @param pin La broche du bouton dont on veut lire l'état
     * @return true si le bouton est pressé, false sinon
     */
    bool getButton(uint8_t pin) const;
    /**
     * @brief Fonction utilitaire pour lire l'état d'un bouton spécifique en utilisant la fonction readButton
     * @param buttonMap Masque binaire contenant l'état de tous les boutons
     * @param pin La broche du bouton dont on veut lire l'état
     * @return true si le bouton est pressé, false sinon
     */
    static inline bool readButton(uint8_t const &buttons, uint8_t const buttonName) { return (bool)(buttons & (1 << (buttonName - 2))); }

    /**
     * @brief Fonctions utilitaires pour lire l'état individuel de chaque bouton en utilisant la fonction readButton
     */
    inline bool getButtonA() { return readButton(getButton(), pinBoutonA); }
    inline bool getButtonB() { return readButton(getButton(), pinBoutonB); }
    inline bool getButtonC() { return readButton(getButton(), pinBoutonC); }
    inline bool getButtonD() { return readButton(getButton(), pinBoutonD); }
    inline bool getButtonE() { return readButton(getButton(), pinBoutonE); }
    inline bool getButtonF() { return readButton(getButton(), pinBoutonF); }
    inline bool getButtonG() { return readButton(getButton(), pinBoutonK); }

    /**
     * @brief Fonction utilitaire pour vérifier si l'état des boutons a changé
     * @return true si l'état des boutons a changé, false sinon
     */
    inline uint8_t changed() { return m_changed; }

    inline void check();

private:
    /**
     * @brief Stocke l'état précédent des boutons
     */
    uint8_t m_oldPressed;

    /**
     * @brief Indique si l'état des boutons a changé depuis la dernière lecture
     */
    uint8_t m_changed;

    /**
     * @brief Valeurs minimale, à l'origine, et maximale de l'axe X lues lors du calibrage
     */
    int16_t m_xMin;
    int16_t m_xOri;
    int16_t m_xMax;

    /**
     * @brief Valeurs minimale, à l'origine, et maximale de l'axe Y lues lors du calibrage
     */
    int16_t m_yMin;
    int16_t m_yOri;
    int16_t m_yMax;
};



// **Définition du constructeur de la classe joypad**
joypad::joypad()
{
    // Déclaration des broches en entrée pour les boutons
    pinMode(pinBoutonA, INPUT); // Déclare la pin/broche du bouton A comme entrée
    pinMode(pinBoutonB, INPUT); // Déclare la pin/broche du bouton B comme entrée
    pinMode(pinBoutonC, INPUT); // Déclare la pin/broche du bouton C comme entrée
    pinMode(pinBoutonD, INPUT); // Déclare la pin/broche du bouton D comme entrée
    pinMode(pinBoutonE, INPUT); // Déclare la pin/broche du bouton E comme entrée
    pinMode(pinBoutonF, INPUT); // Déclare la pin/broche du bouton F comme entrée
    pinMode(pinBoutonK, INPUT); // Déclare la pin/broche du bouton K comme entrée

    m_oldPressed = 0;
    m_changed = 0;

    m_xMin = 0;                      // Valeur initiale pour la valeur minimale de l'axe X
    m_xOri = ((1 << 10) - 1) >> 2;   // Valeur initiale pour la valeur à l'origine de l'axe X
    m_xMax = ((1 << 10) - 1) >> 1;   // Valeur initiale pour la valeur maximale de l'axe X
    m_yMin = m_xMin;                 // Valeur initiale pour la valeur minimale de l'axe Y
    m_yOri = m_xOri;                 // Valeur initiale pour la valeur à l'origine de l'axe Y
    m_yMax = m_xMax;                 // Valeur initiale pour la valeur maximale de l'axe Y
}

// **Définition du destructeur de la classe joypad (ne fait rien)**
joypad::~joypad() {}

// **Définition de la fonction de calibration**
void joypad::calibration(uint8_t const & pin)
{
    m_xOri = analogRead(A0) >> 1;
    m_yOri = analogRead(A1) >> 1;

    m_xMax = 0;
    m_yMax = 0;

    delayMicroseconds(2000);

    // Attend que le bouton 'pin' soit pressé pour arrêter le calibrage
    while (digitalRead(pin))
    {
        int x = analogRead(A0) >> 1;
        int y = analogRead(A1) >> 1;

        m_xMax = x > m_xMax ? x : m_xMax;
        m_yMax = y > m_yMax ? y : m_yMax;

        m_xMin = x < m_xMin ? x : m_xMin;
        m_yMin = y < m_yMin ? y : m_yMin;
    }
}

// **Définition de la fonction de lightCalibration**
void joypad::lightCalibration()
{
    m_xOri = analogRead(A0) >> 1;
    m_yOri = analogRead(A1) >> 1;
}

// **Définition de la fonction de lecture des axes**
void joypad::getAxis(int8_t &x, int8_t &y)
{
    int16_t ax = analogRead(A0) >> 1; // Valeur lue sur l'axe X corrigée du LSB
    int16_t ay = analogRead(A1) >> 1; // Valeur lue sur l'axe Y corrigée du LSB

    if (ax < m_xOri)
    {
        x = map(ax, m_xMin, m_xOri, -100, 0); // Mappage de la valeur de l'axe X entre -100 et 0
    }
    else
    {
        x = map(ax, m_xOri, m_xMax, 0, 100); // Mappage de la valeur de l'axe X entre   0 et 100
    }

    if (ay < m_yOri)
    {
        y = map(ay, m_yMin, m_yOri, -100, 0); // Mappage de la valeur de l'axe Y entre -100 et 0
    }
    else
    {
        y = map(ay, m_yOri, m_yMax, 0, 100); // Mappage de la valeur de l'axe Y entre   0 et 100
    }
}

// **Définition de la fonction de lecture de l'état de tous les boutons**
uint8_t joypad::getButton()
{
    // Combine les broches des boutons A à K dans un masque binaire
    uint8_t buttonMap = ~((PIND >> 2) | ((PINB << 6)));

    // Détecte les changements d'état par comparaison avec la lecture précédente
    m_changed = m_oldPressed ^ buttonMap;
    m_oldPressed = buttonMap;

    return buttonMap;
}

// **Définition de la fonction de lecture de l'état d'un bouton spécifique**
bool joypad::getButton(uint8_t pin) const
{
    // Convertit la broche en masque binaire
    uint8_t bit = digitalPinToBitMask(pin);

    // Récupère le port associé à la broche
    uint8_t port = digitalPinToPort(pin);

    // Lit l'état du bit correspondant à la broche et le renvoie sous forme de booléen
    return static_cast<bool>(*portInputRegister(port) & bit);
}

inline void joypad::check()
{
    char c = 0;
    uint8_t boutons = 0;
    int8_t x = 0, y = 0;
    int8_t xOld = 0, yOld = 0;

    while (c != 'E')
    {
        if (Serial.available())
        {
            c = toupper(Serial.read());
        }

        getAxis(x, y);
        boutons = getButton();

        if(changed() != 0 || xOld != x || yOld != y)
        {
            for (unsigned char i = 0; i < 6; ++i)
            {
                Serial.print(F("Bouton "));
                Serial.print((char)('A' + i));
                Serial.print(F(" = "));
                Serial.print((char)('0' + readButton(boutons, pinBoutonA + i)));
                Serial.print('\n');
            }
            Serial.print('\n');
            Serial.print(F("Bouton "));
            Serial.print('K');
            Serial.print(F(" = "));
            Serial.print((char)('0' + readButton(boutons, pinBoutonK)));
            Serial.print('\n');
    
            Serial.print('\n');
            Serial.print(F("X = "));
            Serial.print(x);
            Serial.print(F(" Y = "));
            Serial.println(y);
            Serial.print('\n');
    
            delay(100);
        }
    }
}

#endif


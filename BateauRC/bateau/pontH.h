/**
 * @file pontH.h
 * @author Florent LERAY, Jérémy Lefort Besnard
 * @date 2024-03-06
 * @brief Définit la classe `pontH` pour contrôler un pont en H et piloter deux moteurs à courant continu.
 *
 * Cette classe permet de piloter deux moteurs à courant continu en fonction des valeurs de vitesse fournies 
 * pour la direction gauche et droite. Elle utilise des broches PWM et de direction pour contràler la vitesse 
 * et le sens de rotation des moteurs.
 */

#pragma once
#ifndef PONTH_h
#define PONTH_h

#include "common.h"

class pontH
{
public:
    inline pontH(int pwmGauchePin, int directionGauchePin, int pwmDroitePin, int directionDroitePin);
    inline ~pontH() {}


    inline void vitesseMoteurs(int8_t const &gauche, int8_t const &droit);
    inline void stopMoteurs();


    inline void setRegimeMinimum(uint8_t regimeMinimum);
    inline void setOverBoostDelay(uint8_t overBoostDelay);

private:    
    inline void speedToPwmDirection(int8_t &vitesse, uint8_t &pwm, bool &direction);
    inline void computeOverDriveDelay(int const leftRight, uint8_t const & pwm, bool direction, int8_t & delai);
    inline void applyDrive(uint8_t & pwmGauche, bool & directionGauche, int8_t & overdriveDelaiGauche, uint8_t & pwmDroit, bool & directionDroite, int8_t & overdriveDelaiDroit);


private:
    int m_pwmPin[2];          /// Tableau stockant les broches PWM des moteurs
    int m_directionPin[2];    /// Tableau stockant les broches de direction des moteurs
    uint8_t m_regimeMinimum;  /// Vitesse minimum autre que 0 pour un moteur. Exprimer en ratio PWM entre 0 et 255. Par défault 127.
    uint8_t m_overBoostDelay; /// Délai d'overdrive de référence quand un moteur est à sont régime minimum
    uint8_t m_vitesse[2];     /// Tableau stockant la vitesse des moteurs
    uint8_t m_pwmOld[2];
    bool    m_directionOld[2];
};







// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////
// //////////////////// Constructeurs et destructeurs /////////////////////////
// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////

/**
 * @brief Constructeur de la classe pontH
 *
 * Ce constructeur initialise les broches PWM et de direction pour les deux moteurs.
 *
 * @param pwmGauchePin Broche PWM du moteur gauche
 * @param directionGauchePin Broche de direction du moteur gauche
 * @param pwmDroitePin Broche PWM du moteur droit
 * @param directionDroitePin Broche de direction du moteur droit
 */
inline pontH::pontH(int pwmGauchePin, int directionGauchePin, int pwmDroitePin, int directionDroitePin)
{
    m_regimeMinimum = 127;
    m_overBoostDelay = 100;
    m_vitesse[0] = 0;
    m_vitesse[1] = 0;

    m_pwmPin[0] = pwmGauchePin;
    m_pwmPin[1] = pwmDroitePin;
    m_directionPin[0] = directionGauchePin;
    m_directionPin[1] = directionDroitePin;

    pinMode(m_pwmPin[0], OUTPUT);
    pinMode(m_pwmPin[1], OUTPUT);
    pinMode(m_directionPin[0], OUTPUT);
    pinMode(m_directionPin[1], OUTPUT);
}




// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////
// ///////////////////////// Fonctions publiques //////////////////////////////
// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////


/**
* @brief Définir le régime minimum des moteurs
*
* Cette fonction permet de définir le régime minimum des moteurs.
*
* @param regimeMinimum [In] Valeur du régime minimum (comprise entre 0 et 255)
*/
inline void pontH::setRegimeMinimum(uint8_t regimeMinimum) { m_regimeMinimum = regimeMinimum; }

/**
* @brief Définir le délai d'overboost des moteurs
*
* Cette fonction permet de définir le délai d'overboost des moteurs en millisecondes. Ce délai est appliqué
* lors de la variation de la vitesse des moteurs pour éviter une surintensité.
*
* @param overBoostDelay [In] Délai d'overboost en millisecondes
*/
inline void pontH::setOverBoostDelay(uint8_t overBoostDelay) { m_overBoostDelay = overBoostDelay; }

/**
 * @brief Définir la vitesse des moteurs
 *
 * Cette fonction définit la vitesse des deux moteurs en fonction des valeurs de vitesse fournies
 * pour la direction gauche et droite. Les valeurs de vitesse doivent être comprises entre -100 et 100.
 * @param gauche [In] Vitesse du moteur gauche (-100 pour la vitesse maximale en arrière, 0 pour à l'arrét, 100 pour la vitesse maximale en avant)
 * @param droit  [In] Vitesse du moteur droit  (-100 pour la vitesse maximale en arrière, 0 pour à l'arrét, 100 pour la vitesse maximale en avant)
 */
inline void pontH::vitesseMoteurs(int8_t const &gauche, int8_t const &droit)
{
    int8_t  vitesseGauche = gauche;
    int8_t  vitesseDroite = droit;
    uint8_t pwmGauche;
    uint8_t pwmDroite;
    bool    directionGauche;
    bool    directionDroite;
    int8_t  delaiGauche;
    int8_t  delaiDroite;

    speedToPwmDirection(vitesseGauche, pwmGauche, directionGauche);
    speedToPwmDirection(vitesseDroite, pwmDroite, directionDroite);

    computeOverDriveDelay(0, pwmGauche, directionGauche, delaiGauche);
    computeOverDriveDelay(1, pwmDroite, directionDroite, delaiDroite);

    m_pwmOld[0] = pwmGauche;
    m_pwmOld[1] = pwmDroite;
    m_directionOld[0] = directionGauche;
    m_directionOld[1] = directionDroite;

    pwmGauche = directionGauche ? pwmGauche : 255 - pwmGauche;
    pwmDroite = directionDroite ? pwmDroite : 255 - pwmDroite;

    applyDrive(pwmGauche, directionGauche, delaiGauche, pwmDroite, directionDroite, delaiDroite);

    m_vitesse[0] = vitesseGauche;
    m_vitesse[1] = vitesseDroite;
}

/**
* @brief Arrêter les moteurs
*
* Cette fonction arréte les deux moteurs en mettant les broches PWM à LOW et les broches de direction à LOW.
*/
inline void pontH::stopMoteurs()
{    
    debugln(F("Arrét du bateau"));
    digitalWrite(m_pwmPin[0], LOW);
    digitalWrite(m_pwmPin[1], LOW);
    digitalWrite(m_directionPin[0], LOW);
    digitalWrite(m_directionPin[1], LOW);
}



// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////
// ///////////////////////// Fonctions privés ////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////

/**
 * @brief Calculer la configuration d'un moteur en fonction de sa vitesse
 *
 * Cette fonction interne calcule la configuration d'un moteur en fonction de la valeur de vitesse fournie.
 *
 * @param vitesse   [in, out] Vitesse du moteur (-100 pour la vitesse maximale en arrière, 0 pour à l'arrêt, 100 pour la vitesse maximale en avant)
 * @param pwm       [out]     Valeur à écrire sur la broche PWM du moteur
 * @param direction [out]     Direction du moteur (true pour avancer, false pour reculer)
 */
inline void pontH::speedToPwmDirection(int8_t &vitesse, uint8_t &pwm, bool &direction)
{
    if (vitesse > +100) vitesse = +100;
    if (vitesse < -100) vitesse = -100;

    direction = vitesse >= 0;
    int8_t vitesseAbs = abs(vitesse);

    if (vitesseAbs)
    {
        pwm = map(vitesseAbs, 0, 100, m_regimeMinimum, 255);
    }
    else
    {
        pwm = 0;
    }
}

/**
 * @brief Calculer le délai d'overdrive pour un moteur
 *
 * Cette fonction interne calcule le délai d'overdrive à appliquer à un moteur en fonction de la variation de sa vitesse.
 *
 * @param leftRight [In]  Indice du moteur (0 pour gauche, 1 pour droit)
 * @param pwm       [In]  Vitesse du moteur
 * @param delai     [Out] Variable dans laquelle stocker le délai d'overdrive calculé
 */
inline void pontH::computeOverDriveDelay(int const leftRight, uint8_t const & pwm, bool direction, int8_t & delai)
{
    delai = 0;

    if(pwm)
    {
        int8_t vitesseOld = m_vitesse[leftRight];
        //uint8_t pwmOld = 0;
        //bool   directionOld = false;

        m_pwmOld[leftRight];
        m_directionOld[leftRight];

        //speedToPwmDirection(vitesseOld, pwmOld, directionOld);
        //debugln("********");
        //debugln(pwm);
        //debugln(direction);
        //debugln("-----");
        //debugln(m_vitesse[leftRight]);
        //debugln(m_pwmOld[leftRight]);
        //debugln(m_directionOld[leftRight]);
        if(m_directionOld[leftRight] != direction || m_pwmOld[leftRight] == 0)
        {
            uint8_t pwmDiff = pwm - m_regimeMinimum;
            delai = map(pwmDiff, m_regimeMinimum, 0, 0, m_overBoostDelay);
            //debugln(delai); 
        }
        else
        {
            //debugln("Drection is same");            
        }
        //debugln("********");
    }
    else
    {
        //debugln("pwm=0");
    }
}

/**
 * @brief Appliquer la configuration des moteurs aux broches
 *
 * Cette fonction interne applique la configuration calculée pour les deux moteurs (vitesse et direction)
 * aux broches PWM et de direction. Elle tient compte du délai d'overdrive si nécessaire.
 *
 * @param pwmGauche            [In] Valeur à écrire sur la broche PWM du moteur gauche
 * @param directionGauche      [In] Direction du moteur gauche (true pour avancer, false pour reculer)
 * @param overdriveDelaiGauche [In] Délai d'overdrive calculé pour le moteur gauche
 * @param pwmDroit             [In] Valeur à écrire sur la broche PWM du moteur droit
 * @param directionDroite      [In] Direction du moteur droit (true pour avancer, false pour reculer)
 * @param overdriveDelaiDroit  [In] Délai d'overdrive calculé pour le moteur droit
 */
inline void pontH::applyDrive(uint8_t & pwmGauche, bool & directionGauche, int8_t & overdriveDelaiGauche, uint8_t & pwmDroit, bool & directionDroite, int8_t & overdriveDelaiDroit)
{
    if (overdriveDelaiGauche > overdriveDelaiDroit)
    {
        uint16_t overlapTime = overdriveDelaiGauche - overdriveDelaiDroit;

        digitalWrite(m_directionPin[0], !directionGauche);
        analogWrite(m_pwmPin[0], directionGauche ? 255 : 0);

        digitalWrite(m_directionPin[1], !directionDroite);
        analogWrite(m_pwmPin[1], directionDroite ? 255 : 0);

        delay(overlapTime);

        analogWrite(m_pwmPin[1], pwmDroit);

        delay(overdriveDelaiGauche - overlapTime);

        analogWrite(m_pwmPin[0], pwmGauche);
    }
    else
    {
        uint16_t overlapTime = overdriveDelaiDroit - overdriveDelaiGauche;

        digitalWrite(m_directionPin[0], !directionGauche);
        analogWrite(m_pwmPin[0], directionGauche ? 255 : 0);

        digitalWrite(m_directionPin[1], !directionDroite);
        analogWrite(m_pwmPin[1], directionDroite ? 255 : 0);

        delay(overlapTime);

        analogWrite(m_pwmPin[0], pwmGauche);
        
        delay(overdriveDelaiDroit - overlapTime);

        analogWrite(m_pwmPin[1], pwmDroit);
    }
}


#endif

/**
 * @file joystickToMotors.h
 * @author Florent LERAY, Jérémy Lefort Besnard
 * @date 2024-03-06
 * @brief Définit les fonctions utiles à la conversion des direction xy du joystick en commandes avant/arrière pour les moteurs gauche et droite.
 */

#pragma once
#ifndef JOYSTICKTOMOTORS_h
#define JOYSTICKTOMOTORS_h


 /**
  * @brief Classe pour la conversion des commandes du joystick en commandes pour les moteurs
  *
  * Cette classe permet de convertir les valeurs brutes du joystick (axes X et Y) en commandes pour les moteurs gauche et droit (g et d).
  * Elle prend en compte l'angle du joystick (calculé à partir des axes X et Y) et l'algorithme de conversion sélectionné (simple ou lisse).
  */
class joystickToMotors
{
public:
    /**
     * @enum Défini les différents algorithmes de conversion disponibles
     */
    enum mapping : uint8_t {
        simple, /**< Algorithme de conversion simple */
        smooth, /**< Algorithme de conversion lisse */
        mappinEnumSize  /**< Taille de l'énumération (utilisé en interne) */
    };

public:
    joystickToMotors() { m_angle = 45; m_algo = smooth; }
    ~joystickToMotors() = default;

    void convert(int8_t x, int8_t y, int8_t &g, int8_t &d) const;
    void setAngleSeuil(int8_t angle) { m_angle = angle; }
    void changeMapping(mapping algo) { m_algo = algo; }

private:
    inline void xyToPolar(int8_t x, int8_t y, double &angle, uint8_t &magnitude) const;
    inline void polaireToMotor(double angle, int8_t magnitude, int8_t &g, int8_t &d) const;

    inline void simpleConversion(long uAngle, long &g, long &d) const;
    inline void smoothConversion(long uAngle, long &g, long &d) const;

private:
    int m_angle;    /**< Angle de seuil pour l'algorithme de conversion lisse (degrés) */
    mapping m_algo; /**< Algorithme de conversion sélectionné (simple ou lisse) */
};



/**
 * @brief Convertit les valeurs du joystick en commandes pour les moteurs
 *
 * Cette méthode prend les valeurs brutes des axes X et Y du joystick, et renvoie les commandes pour les moteurs gauche et droit dans les variables g et d (modifiées par référence).
 *
 * @param x Valeur de l'axe X du joystick (int8_t)
 * @param y Valeur de l'axe Y du joystick (int8_t)
 * @param g Référence vers la variable qui recevra la commande pour le moteur gauche (modifié)
 * @param d Référence vers la variable qui recevra la commande pour le moteur droit (modifié)
 */
inline void joystickToMotors::convert(int8_t x, int8_t y, int8_t & g, int8_t & d) const
{
    uint8_t magnitude;
    double angle;

    xyToPolar(x, y, angle, magnitude);
    polaireToMotor(angle, magnitude, g, d);
}

/**
 * @brief Convertit les valeurs des axes X et Y du joystick en angle et magnitude polaires
 *
 * Cette méthode calcule l'angle et la magnitude polaires à partir des valeurs brutes des axes X et Y du joystick.
 *
 * @param x Valeur de l'axe X du joystick (int8_t)
 * @param y Valeur de l'axe Y du joystick (int8_t)
 * @param angle Référence vers la variable qui recevra l'angle polaire (modifié)
 * @param magnitude Référence vers la variable qui recevra la magnitude polaire (modifié)
 */
inline void joystickToMotors::xyToPolar(int8_t x, int8_t y, double & angle, uint8_t & magnitude) const
{
    // Calcul de l'angle du joystick
    angle = round(atan2(((double)y) / 100.0, ((double)x) / 100.0) * 180 / M_PI);

    // Calcul de la magnitude du mouvement du joystick
    magnitude = round(sqrt(x*x + y * y));
    magnitude = magnitude > 100 ? 100 : magnitude;
}

/**
 * @brief Convertit l'angle et la magnitude polaires en commandes pour les moteurs
 *
 * Cette méthode convertit l'angle et la magnitude polaires calculés précédemment en commandes pour les moteurs gauche et droit.
 * Le choix de l'algorithme de conversion (simple ou lisse) est pris en compte.
 *
 * @param angle Angle polaire (double)
 * @param magnitude Magnitude polaire (uint8_t)
 * @param g Référence vers la variable qui recevra la commande pour le moteur gauche (modifié)
 * @param d Référence vers la variable qui recevra la commande pour le moteur droit (modifié)
 */
inline void joystickToMotors::polaireToMotor(double angle, int8_t magnitude, int8_t &g, int8_t &d) const
{
    long uAngle = (long)angle; // Angle non signé
    long gauche = 0;
    long droit = 0;

    if (angle < 0)
    {
        uAngle = 0 - uAngle; // Convertit l'angle en positif
        magnitude = 0 - magnitude; // Inverse la direction
    }

    switch (m_algo)
    {
        case simple: simpleConversion(uAngle, gauche, droit); break;
        case smooth: smoothConversion(uAngle, gauche, droit); break;
        default: break; // Gestion d'erreur pour un algorithme inconnu
    }

    g = (int8_t)gauche * magnitude / 100;
    d = (int8_t)droit  * magnitude / 100;
}

/**
 * @brief Algorithme de conversion simple
 *
 * Cette méthode implémente l'algorithme de conversion simple. Il mappe l'angle du joystick en commandes pour les moteurs de manière linéaire.
 *
 * @param uAngle Angle du joystick non signé (long)
 * @param g Référence vers la variable qui recevra la commande pour le moteur gauche (modifié)
 * @param d Référence vers la variable qui recevra la commande pour le moteur droit (modifié)
 */
inline void joystickToMotors::simpleConversion(long uAngle, long &g, long &d) const
{
    if (uAngle < 90)
    {
        g = 100;
        d = map(uAngle, 0, 90, 0, 100);
    }
    else if (uAngle < 180)
    {
        g = map(uAngle, 90, 180, 100, 0);
        d = 100;
    }
}

/**
 * @brief Algorithme de conversion lisse
 *
 * Cette méthode implémente l'algorithme de conversion lisse. Il prend en compte l'angle de seuil défini pour créer une zone morte autour de l'axe central du joystick.
 *
 * @param uAngle Angle du joystick non signé (long)
 * @param g Référence vers la variable qui recevra la commande pour le moteur gauche (modifié)
 * @param d Référence vers la variable qui recevra la commande pour le moteur droit (modifié)
 */
inline void joystickToMotors::smoothConversion(long uAngle, long &g, long &d) const
{
    if (uAngle > 180 - m_angle)
    {
        g = 0;
        d = map(uAngle, 180 - m_angle, 180, 100, 0);
    }
    else if (uAngle > 90)
    {
        g = map(uAngle, 90, 180 - m_angle, 100, 0);
        d = 100;
    }
    else if (uAngle > 90 - m_angle)
    {
        g = 100;
        d = map(uAngle, m_angle, 90, 0, 100);
    }
    else
    {
        g = map(uAngle, 0, m_angle, 0, 100);
        d = 0;
    }
}




/**
 * @fn joystickToMotors::joystickToMotors
 * @brief Constructeur par défaut
 *
 * Initialise l'angle de seuil à 45 degrés et l'algorithme de conversion à "smooth".
 */

/**
 * @fn joystickToMotors::~~joystickToMotors
 * @brief Destructeur
 */

/**
 * @fn joystickToMotors::setAngleSeuil
 * @brief Définit l'angle de seuil pour l'algorithme de conversion lisse
 *
 * Cette méthode permet de définir l'angle de seuil utilisé dans l'algorithme de conversion lisse. Cet angle définit une zone morte autour de l'axe central du joystick.
 *
 * @param angle Angle de seuil en degrés (int8_t)
 */

/**
 * @fn joystickToMotors::changeMapping
 * @brief Change l'algorithme de conversion utilisé
 *
 * Cette méthode permet de changer l'algorithme de conversion utilisé (simple ou lisse) en fonction de la valeur de l'énumération `mapping` fournie.
 *
 * @param algo Algorithme de conversion souhaité (joystickToMotors::mapping)
 */
#endif
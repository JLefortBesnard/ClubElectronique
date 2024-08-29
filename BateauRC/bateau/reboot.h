/**
 * @file reboot.h
 * @author Florent LERAY, Jérémy Lefort Besnard
 * @date 2024-03-06
 * @brief Définit la fonction `reboot()` pour redémarrer le système.
 *
 * Cette fonction utilise le watchdog timer pour redémarrer le système de manière sûre.
 */

#pragma once
#ifndef reboot_h
#define reboot_h

#include <avr/wdt.h>

 /**
  * @brief Redémarrer le système
  *
  * Cette fonction affiche un message sur le port série avant d'activer le watchdog timer et de boucler
  * indéfiniment, ce qui force un redémarrage du système.
  */
void reboot()
{
    /**
     * @brief Affiche un message sur le port série indiquant que le système va redémarrer
     */
    debugln(F("Redémarrage demandé"));

    /**
     * @brief Active le watchdog timer avec un délai de 15ms
     */
    wdt_enable(WDTO_15MS);

    /**
     * @brief Boucle infinie pour attendre le redémarrage du système
     */
    while (1) {};
}

#endif
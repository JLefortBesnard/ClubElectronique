/**
 * @file common.h
 * @author Florent LERAY, Jérémy Lefort Besnard
 * @date 2024-03-06
 * @brief Définit la classe `pontH` pour contrôler un pont en H et piloter deux moteurs à courant continu.
 *
 * Ce fichier regroupe... 
 */

#pragma once
#ifndef COMMON_h
#define COMMON_h

#ifdef BATEAU_DEBUG
#define debug(...) Serial.print(__VA_ARGS__);
#define debugln(...) Serial.println(__VA_ARGS__);
#else
#define debug(...)
#define debugln(...)
#endif

#endif
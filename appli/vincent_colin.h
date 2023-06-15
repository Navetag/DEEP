/**
 * @file vincent_colin.h
 * @author armand
 * @date 01 avril 2023
 * @brief Header du module vincent_colin.
 * @version 0.1
 */

#ifndef VINCENT_COLIN_H_
#define VINCENT_COLIN_H_

#include "config.h"

/**
 * @brief Process du module - non bloquant
 * 		- Appelle le process du module screen
 * 		- Appelle le process du module cube_servo
 * 		- Appelle la machine a etat du module
 */
void VINCENT_process(void);

#if MAIN_TEST

/**
 * @brief Fonction de test - bloquant
 * Test mineur de la communication Pixy2
 */
void VINCENT_testPixy(void); //Fonction bloquante

/**
 * @brief Fonction de test - bloquant
 * Test de la communication Pixy2 adapte a l'echelle de la machine a etat generale
 */
void VINCENT_testPixy2(void); //Fonction bloquante

/**
 * @brief Fonction de test - bloquant
 * Test des servo-moteurs dans le cadre d'utilisation par la machine a etat
 */
void VINCENT_testServos(void); //Fonction bloquante

/**
 * @brief Fonction de test - bloquant
 * Test des modes d'affichage de l'ecran
 */
void VINCENT_testScreen(void); //Fonction bloquante

/**
 * @brief Fonction de test - bloquant
 * Fonction de production sans les fonctionnalites defectueuses (recuperation d'une face du cube)
 */
void VINCENT_projetFacade(void); //Fontion bloquante

#endif

#endif /* VINCENT_COLIN_H_ */

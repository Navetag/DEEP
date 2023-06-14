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

void VINCENT_process(void);

#if MAIN_TEST

void VINCENT_testPixy(void); //Fonction bloquante

void VINCENT_testPixy2(void); //Fonction bloquante

void VINCENT_testServos(void); //Fonction bloquante

void VINCENT_testScreen(void); //Fonction bloquante

void VINCENT_projetFacade(void); //Fontion bloquante

#endif

#endif /* VINCENT_COLIN_H_ */

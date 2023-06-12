/*
 * vincentColin.h
 *
 *  Created on: 8 juin 2023
 *      Author: caill
 */

#ifndef VINCENT_COLIN_H_
#define VINCENT_COLIN_H_

#include "config.h"

void VINCENT_process(void);

#if MAIN_TEST

void VINCENT_testPixy(void); //Fonction bloquante

void VINCENT_testServos(void); //Fonction bloquante

void VINCENT_testScreen(void); //Fonction bloquante

#endif

#endif /* VINCENT_COLIN_H_ */

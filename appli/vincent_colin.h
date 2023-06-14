/**
  ******************************************************************************
  * @file    main.c
  * @author  Nirgal
  * @date    03-July-2019
  * @brief   Default main function.
  ******************************************************************************
*/

#ifndef VINCENT_COLIN_H_
#define VINCENT_COLIN_H_

#include "config.h"

void VINCENT_process(void);

#if MAIN_TEST

void VINCENT_testPixy2(void); //Fonction bloquante

void VINCENT_testPixy(void); //Fonction bloquante

void VINCENT_testServos(void); //Fonction bloquante

void VINCENT_testScreen(void); //Fonction bloquante

void VINCENT_projetFacade(void); //Fontion bloquante

#endif

#endif /* VINCENT_COLIN_H_ */

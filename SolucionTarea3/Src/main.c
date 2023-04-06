/**
 ******************************************************************************
 * @file           : main.c
 * @author         : josancheze
 * @brief          : Configuracion basica de un proyecto
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <stm32f4xx.h>
#include <stdint.h>
#include "GPIOxDriver.h"
#include "BasicTimer.h"

uint32_t counter;
uint32_t auxCounter;
uint32_t *ptr_Counter;
uint8_t byteVariable;
uint8_t *ptr_ByteVariable;
int main(void){
	//
	counter = 12345;

	auxCounter = counter; //Diferencia entre pasar el valor y utilizar un puntero
	ptr_Counter = &counter; // Se le asigna al puntero esa posicion de memoria

	*ptr_Counter = 4365789; //Con el asterisco cambia el valor de la memoria
//	ptr_Counter++; // Coge el puntero y hace que apunte a la siguiente posicion deuint32_t
//	*ptr_Counter = 4365789;

	byteVariable = 4;
	ptr_ByteVariable = &byteVariable;
	*ptr_ByteVariable = 8;

	//Mezclar punteros obligandolos meediante un casting
	auxCounter = (uint32_t)&counter; //El casting es poner a algo a que se parezca a otra cosa, se hace que el puntero pase como un numero 32 bits
	ptr_ByteVariable = (uint8_t *)auxCounter; //Se hace que el numero pase a ser puntero otra vez
	*ptr_ByteVariable = 1;

	while(1){

	}
}


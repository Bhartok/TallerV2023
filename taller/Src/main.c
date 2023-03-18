/*
 * IntroGPIOxMain.c
 *
 *  Created on: Mar 9, 2023
 *      Author: bhartok
 */


#include <stdint.h>

#include "stm32f411xx_hal.h"

#define FIBONACCI 0
#define FACTORIAL 1

//Funcion switch-case

uint32_t resultadoOperacion(uint8_t tipoDeOperacion, uint8_t A);

int main(void){
	uint16_t operacionFinal = 0;
	operacionFinal = resultadoOperacion(FACTORIAL,2);
	operacionFinal++;
}
uint32_t resultadoOperacion(uint8_t tipoDeOperacion, uint8_t A){
	uint32_t resultado = 0;
	uint32_t resultadoAnterior = 0;
	uint32_t contador = 1;

	switch(tipoDeOperacion){
		case FIBONACCI:{
			while(contador<=A){ //4
				if(contador == 1){
					resultado = 0;
				}else if(contador == 2){resultado = 1; resultadoAnterior = 0;}
				else if(contador == 3){resultado = 2; resultadoAnterior = 1;}
				resultado+=resultadoAnterior;
				resultadoAnterior = resultado;

			}

			}
			break;
		case FACTORIAL:{
			resultado = 1;
					while(A>0){
						resultado*=A;
						A--;
					}
					break;
				}

		default:{
			resultado = 0;
			break;
		}
		return resultado;
	}
}

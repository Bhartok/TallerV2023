/*
 * IntroGPIOxMain.c
 *
 *  Created on: Mar 9, 2023
 *      Author: bhartok
 */


#include <stdint.h>

#include "stm32f411xx_hal.h"
#include "GPIOxDriver.h"

#define SUMA '+'
#define RESTA '-'
#define MULTIPLICACION '*'
#define DIVISION '/'

//Funcion switch-case

uint16_t resultadoOperacion(uint8_t tipoDeOperacion, uint8_t A, uint8_t B);

int main(void){
	uint16_t operacionFinal = 0;
	operacionFinal = resultadoOperacion(SUMA,2,3);
	operacionFinal++;
}
uint16_t resultadoOperacion(uint8_t tipoDeOperacion, uint8_t A, uint8_t B){
	uint16_t resultado = 0;

	switch(tipoDeOperacion){
		case SUMA:{
			resultado = A+B;
			break;
		}
		case RESTA:{
					resultado = A-B;
					break;
				}
		case MULTIPLICACION:{
					resultado = A*B;
					break;
				}
		case DIVISION:{
					resultado = A/B;
					break;
				}
		default:{
			resultado = 0;
			break;
		}
		return resultado;
	}
}

/*
 * GPIOxDriver.h
 *
 *  Created on: Mar 9, 2023
 *      Author: bhartok
 */

#ifndef GPIOXDRIVER_H_
#define GPIOXDRIVER_H_

#include "stm32f411xx_hal.h"

//Estructura que define la configuracion del pin
typedef struct
{
	uint8_t GPIO_PinNumber;//Pin con el que se quiere trabajar
	uint8_t GPIO_PinMode;//Configuración: Entrada, salida, analogo, f.alternativa
	uint8_t GPIO_PinSpeed;//Velocidad de respuesta del pin(Solo digital)
	uint8_t GPIO_PinPuPdControl;//Salida Pull-up down o libre
	uint8_t GPIO_PinOType;//Selecciona salida PUPD o OpenDrain
	uint8_t GPIO_PinAltFunMode;//Selecciona la funcion alternativa
}GPIO_PinConfig_t;

//Estructura que contiene la direccion del puerto que se utiliza y la configuracion especifica del pin

typedef struct
{
	GPIOx_RegDef_t		*pGPIOx; //Direccion al puerto al que el pin corresponde
	GPIO_PinConfig_t	GPIO_PinConfig;//Configuracion del PIN
}GPIO_Handler_t;

//Definicion de las cabeceras de las funciones del GPIOxDriver

void GPIO_Config(GPIO_Handler_t *pGPIOHandler);
void GPIO_WritePin(GPIO_Handler_t *pPinHandler, uint8_t newState);
uint32_t GPIO_ReadPin(GPIO_Handler_t *pPinHandler);
void GPIOxTogglePin(GPIO_Handler_t *pPinHandler);

#endif /* GPIOXDRIVER_H_ */

/*
 * GPIOxDriver.h
 *
 *  Created on: Mar 9, 2023
 *      Author: bhartok
 */

#ifndef GPIOXDRIVER_H_
#define GPIOXDRIVER_H_

#include "stm32f4xx.h"
//VALORES ESTANDAR PARA LAS CONFIGURACIONES
//MODER DOS BIT POR CADA PIN
#define GPIO_MODE_IN			0
#define GPIO_MODE_OUT			1
#define GPIO_MODE_ALTFN			2
#define GPIO_MODE_ANALOG		3
//TYPER UN BIT POR PIN
#define GPIO_OTYPE_PUSHPULL		0
#define GPIO_OTYPE_OPENDRAIN	1
//OSPEEDR DOS BIT POR PIN
#define GPIO_OSPEED_LOW			0
#define GPIO_OSPEED_MEDIUM		1
#define GPIO_OSPEED_FAST		2
#define GPIO_OSPEED_HIGH		3
//PUPDR DOS BIT POR PIN
#define GPIO_PUPDR_NOTHING		0
#define GPIO_PUPDR_PULLUP		1
#define GPIO_PUPDR_PULLDOWN		2
#define GPIO_PUPDR_RESERVED		3

//Definición de los nombres de los pines

#define PIN_0	0
#define PIN_1	1
#define PIN_2	2
#define PIN_3	3
#define PIN_4	4
#define PIN_5	5
#define PIN_6	6
#define PIN_7	7
#define PIN_8	8
#define PIN_9	9
#define PIN_10	10
#define PIN_11	11
#define PIN_12	12
#define PIN_13	13
#define PIN_14	14
#define PIN_15	15

//Definición de las funciones alternativas

#define AF0		0b0000
#define AF1		0b0001
#define AF2		0b0010
#define AF3		0b0011
#define AF4		0b0100
#define AF5		0b0101
#define AF6		0b0110
#define AF7		0b0111
#define AF8		0b1000
#define AF9		0b1001
#define AF10	0b1010
#define AF11	0b1011
#define AF12	0b1100
#define AF13	0b1101
#define AF14	0b1110
#define AF15	0b1111


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
	GPIO_TypeDef		*pGPIOx; //Direccion al puerto al que el pin corresponde
	GPIO_PinConfig_t	GPIO_PinConfig;//Configuracion del PIN
}GPIO_Handler_t;

//Definicion de las cabeceras de las funciones del GPIOxDriver

void GPIO_Config(GPIO_Handler_t *pGPIOHandler);
void GPIO_WritePin(GPIO_Handler_t *pPinHandler, uint8_t newState);
void GPIOxTogglePin(GPIO_Handler_t *pPinHandler);
uint32_t GPIO_ReadPin(GPIO_Handler_t*pPinHandler);

#endif /* GPIOXDRIVER_H_ */

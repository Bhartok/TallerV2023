/*
 * IntroGPIOxMain.c
 *
 *  Created on: Mar 9, 2023
 *      Author: bhartok
 */


#include <stdint.h>

#include "stm32f411xx_hal.h"
#include "GPIOxDriver.h"

//Funcion principal del programa
int main(void){
	//Definicion del handler para el pin que se quiere utilizar
	GPIO_Handler_t handlerUserLedPin = {0};

	//Crear configuracion puerto GPIOA
	handlerUserLedPin.pGPIOx = GPIOA;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinNumber = 		PIN_5;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinMode = 		GPIO_MODE_OUT;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinOType = 		GPIO_OTYPE_PUSHPULL;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinPuPdControl = 	GPIO_PUPDR_NOTHING;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinSpeed = 		GPIO_OSPEED_MEDIUM;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinAltFunMode = 	AF0;

	//Cargar configuración al PIN
	GPIO_Config(&handlerUserLedPin);

	//Hacemos que el pin A5 quede encendido
	GPIO_WritePin(&handlerUserLedPin,SET);

	while(1){
		NOP();
	}





}

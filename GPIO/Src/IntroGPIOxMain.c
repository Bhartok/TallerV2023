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
	//Definicion de los handlers para los pines que se quiere utilizar
	GPIO_Handler_t handlerPinC9 = {0};
	GPIO_Handler_t handlerPinC6= {0};
	GPIO_Handler_t handlerPinB8 = {0};
	GPIO_Handler_t handlerPinA6 = {0};
	GPIO_Handler_t handlerPinC7 = {0};
	GPIO_Handler_t handlerPinC8 = {0};
	GPIO_Handler_t handlerPinA7 = {0};


	//Crear configuracion puerto GPIOC9
	handlerPinC9.pGPIOx = GPIOC;
	handlerPinC9.GPIO_PinConfig.GPIO_PinNumber = 		PIN_9;
	handlerPinC9.GPIO_PinConfig.GPIO_PinMode = 			GPIO_MODE_OUT;
	handlerPinC9.GPIO_PinConfig.GPIO_PinOType = 		GPIO_OTYPE_PUSHPULL;
	handlerPinC9.GPIO_PinConfig.GPIO_PinPuPdControl = 	GPIO_PUPDR_NOTHING;
	handlerPinC9.GPIO_PinConfig.GPIO_PinSpeed = 		GPIO_OSPEED_MEDIUM;
	handlerPinC9.GPIO_PinConfig.GPIO_PinAltFunMode = 	AF0;
	//Crear configuracion puerto GPIOC6
	handlerPinC6.pGPIOx = GPIOC;
	handlerPinC6.GPIO_PinConfig.GPIO_PinNumber = 		PIN_6;
	handlerPinC6.GPIO_PinConfig.GPIO_PinMode = 			GPIO_MODE_OUT;
	handlerPinC6.GPIO_PinConfig.GPIO_PinOType = 		GPIO_OTYPE_PUSHPULL;
	handlerPinC6.GPIO_PinConfig.GPIO_PinPuPdControl = 	GPIO_PUPDR_NOTHING;
	handlerPinC6.GPIO_PinConfig.GPIO_PinSpeed = 		GPIO_OSPEED_MEDIUM;
	handlerPinC6.GPIO_PinConfig.GPIO_PinAltFunMode = 	AF0;
	//Crear configuracion puerto GPIOB8
	handlerPinB8.pGPIOx = 								GPIOB;
	handlerPinB8.GPIO_PinConfig.GPIO_PinNumber = 		PIN_8;
	handlerPinB8.GPIO_PinConfig.GPIO_PinMode = 			GPIO_MODE_OUT;
	handlerPinB8.GPIO_PinConfig.GPIO_PinOType = 		GPIO_OTYPE_PUSHPULL;
	handlerPinB8.GPIO_PinConfig.GPIO_PinPuPdControl = 	GPIO_PUPDR_NOTHING;
	handlerPinB8.GPIO_PinConfig.GPIO_PinSpeed = 		GPIO_OSPEED_MEDIUM;
	handlerPinB8.GPIO_PinConfig.GPIO_PinAltFunMode = 	AF0;

	handlerPinA6.pGPIOx = 								GPIOA;
	handlerPinA6.GPIO_PinConfig.GPIO_PinNumber = 		PIN_6;
	handlerPinA6.GPIO_PinConfig.GPIO_PinMode = 			GPIO_MODE_OUT;
	handlerPinA6.GPIO_PinConfig.GPIO_PinOType = 		GPIO_OTYPE_PUSHPULL;
	handlerPinA6.GPIO_PinConfig.GPIO_PinPuPdControl = 	GPIO_PUPDR_NOTHING;
	handlerPinA6.GPIO_PinConfig.GPIO_PinSpeed = 		GPIO_OSPEED_MEDIUM;
	handlerPinA6.GPIO_PinConfig.GPIO_PinAltFunMode = 	AF0;

	handlerPinC7.pGPIOx = 								GPIOC;
	handlerPinC7.GPIO_PinConfig.GPIO_PinNumber = 		PIN_7;
	handlerPinC7.GPIO_PinConfig.GPIO_PinMode = 			GPIO_MODE_OUT;
	handlerPinC7.GPIO_PinConfig.GPIO_PinOType = 		GPIO_OTYPE_PUSHPULL;
	handlerPinC7.GPIO_PinConfig.GPIO_PinPuPdControl = 	GPIO_PUPDR_NOTHING;
	handlerPinC7.GPIO_PinConfig.GPIO_PinSpeed = 		GPIO_OSPEED_MEDIUM;
	handlerPinC7.GPIO_PinConfig.GPIO_PinAltFunMode = 	AF0;

	handlerPinC8.pGPIOx = 								GPIOC;
	handlerPinC8.GPIO_PinConfig.GPIO_PinNumber = 		PIN_8;
	handlerPinC8.GPIO_PinConfig.GPIO_PinMode = 			GPIO_MODE_OUT;
	handlerPinC8.GPIO_PinConfig.GPIO_PinOType = 		GPIO_OTYPE_PUSHPULL;
	handlerPinC8.GPIO_PinConfig.GPIO_PinPuPdControl = 	GPIO_PUPDR_NOTHING;
	handlerPinC8.GPIO_PinConfig.GPIO_PinSpeed = 		GPIO_OSPEED_MEDIUM;
	handlerPinC8.GPIO_PinConfig.GPIO_PinAltFunMode = 	AF0;

	handlerPinA7.pGPIOx = 								GPIOA;
	handlerPinA7.GPIO_PinConfig.GPIO_PinNumber = 		PIN_7;
	handlerPinA7.GPIO_PinConfig.GPIO_PinMode = 			GPIO_MODE_OUT;
	handlerPinA7.GPIO_PinConfig.GPIO_PinOType = 		GPIO_OTYPE_PUSHPULL;
	handlerPinA7.GPIO_PinConfig.GPIO_PinPuPdControl = 	GPIO_PUPDR_NOTHING;
	handlerPinA7.GPIO_PinConfig.GPIO_PinSpeed = 		GPIO_OSPEED_MEDIUM;
	handlerPinA7.GPIO_PinConfig.GPIO_PinAltFunMode = 	AF0;


	/*
	 * A)
	 * El problema con el read pin es que se obtenía el IDR y se le shifteaba PinNumber de veces a la derecha, haciendo que se obtuviera un binario
	 * que tuviera como primera posición el estado del pin, mas no el estado del pin.
	 * B)
	 * La solución del problema fue "eliminar" Los registros sobrantes que quedaban a la izquierda, haciendo una operación AND entre el binario
	 * que se obtenía, y la mascara(1<<0)
	*/

	uint32_t i = 1;
	uint32_t counter =1;




	while(1){
		for(i; i<100000;i++){
			NOP();
		}
		counter++;
		printf("El contador es %d",counter);

	}





}

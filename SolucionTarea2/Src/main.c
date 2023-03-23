/*
 * main.c
 *
 *  Created on: Mar 9, 2023
 *      Author: bhartok
 */


#include <stdint.h>

#include "stm32f411xx_hal.h"
#include "GPIOxDriver.h"

//Funcion principal del programa
int main(void){
	/* PUNTO 1
	 * A)La función GPIO_ReadPin() lo que hacía era correr el registro del pin que queríamos leer desde el IDR, pin number de veces a la derecha,
	 * dejando como primera posición el estado del pin, sin embargo, si habían más registros de otros pines a la izquierda del de interés
	 * se retornaban también, dando un return de un binario diferente a (0,1) que son los valores deseados de retorno.
	 *
	 * B)La manera de solucionar este error es extrayendo de ese binario solamente la primera posición, mediante el uso de una máscara y el operador
	 * 	 AND, la máscara a utilizar para extraer solo el primer elemento es (1<<0)(solo un 1 en la primera posición, el resto 0)
	 */



	//Definicion de los handlers para los pines que se van a utilizar, se inicializan todos en 0
	GPIO_Handler_t handlerPinC9 = {0};
	GPIO_Handler_t handlerPinC6= {0};
	GPIO_Handler_t handlerPinB8 = {0};
	GPIO_Handler_t handlerPinA6 = {0};
	GPIO_Handler_t handlerPinC7 = {0};
	GPIO_Handler_t handlerPinC8 = {0};
	GPIO_Handler_t handlerPinA7 = {0};
	GPIO_Handler_t handlerButtonC13 = {0};


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
	//Crear configuracion puerto GPIOA6
	handlerPinA6.pGPIOx = 								GPIOA;
	handlerPinA6.GPIO_PinConfig.GPIO_PinNumber = 		PIN_6;
	handlerPinA6.GPIO_PinConfig.GPIO_PinMode = 			GPIO_MODE_OUT;
	handlerPinA6.GPIO_PinConfig.GPIO_PinOType = 		GPIO_OTYPE_PUSHPULL;
	handlerPinA6.GPIO_PinConfig.GPIO_PinPuPdControl = 	GPIO_PUPDR_NOTHING;
	handlerPinA6.GPIO_PinConfig.GPIO_PinSpeed = 		GPIO_OSPEED_MEDIUM;
	handlerPinA6.GPIO_PinConfig.GPIO_PinAltFunMode = 	AF0;
	//Crear configuracion puerto GPIOC7
	handlerPinC7.pGPIOx = 								GPIOC;
	handlerPinC7.GPIO_PinConfig.GPIO_PinNumber = 		PIN_7;
	handlerPinC7.GPIO_PinConfig.GPIO_PinMode = 			GPIO_MODE_OUT;
	handlerPinC7.GPIO_PinConfig.GPIO_PinOType = 		GPIO_OTYPE_PUSHPULL;
	handlerPinC7.GPIO_PinConfig.GPIO_PinPuPdControl = 	GPIO_PUPDR_NOTHING;
	handlerPinC7.GPIO_PinConfig.GPIO_PinSpeed = 		GPIO_OSPEED_MEDIUM;
	handlerPinC7.GPIO_PinConfig.GPIO_PinAltFunMode = 	AF0;
	//Crear configuracion puerto GPIOC8
	handlerPinC8.pGPIOx = 								GPIOC;
	handlerPinC8.GPIO_PinConfig.GPIO_PinNumber = 		PIN_8;
	handlerPinC8.GPIO_PinConfig.GPIO_PinMode = 			GPIO_MODE_OUT;
	handlerPinC8.GPIO_PinConfig.GPIO_PinOType = 		GPIO_OTYPE_PUSHPULL;
	handlerPinC8.GPIO_PinConfig.GPIO_PinPuPdControl = 	GPIO_PUPDR_NOTHING;
	handlerPinC8.GPIO_PinConfig.GPIO_PinSpeed = 		GPIO_OSPEED_MEDIUM;
	handlerPinC8.GPIO_PinConfig.GPIO_PinAltFunMode = 	AF0;
	//Crear configuracion puerto GPIOA7
	handlerPinA7.pGPIOx = 								GPIOA;
	handlerPinA7.GPIO_PinConfig.GPIO_PinNumber = 		PIN_7;
	handlerPinA7.GPIO_PinConfig.GPIO_PinMode = 			GPIO_MODE_OUT;
	handlerPinA7.GPIO_PinConfig.GPIO_PinOType = 		GPIO_OTYPE_PUSHPULL;
	handlerPinA7.GPIO_PinConfig.GPIO_PinPuPdControl = 	GPIO_PUPDR_NOTHING;
	handlerPinA7.GPIO_PinConfig.GPIO_PinSpeed = 		GPIO_OSPEED_MEDIUM;
	handlerPinA7.GPIO_PinConfig.GPIO_PinAltFunMode = 	AF0;
	//Crear configuracion puerto GPIOC13(boton)
	handlerButtonC13.pGPIOx =								GPIOC;
	handlerButtonC13.GPIO_PinConfig.GPIO_PinNumber = 		PIN_13;
	handlerButtonC13.GPIO_PinConfig.GPIO_PinMode = 		GPIO_MODE_IN;
	handlerButtonC13.GPIO_PinConfig.GPIO_PinPuPdControl = 	GPIO_PUPDR_NOTHING;
	handlerButtonC13.GPIO_PinConfig.GPIO_PinSpeed = 		GPIO_OSPEED_MEDIUM;
	handlerButtonC13.GPIO_PinConfig.GPIO_PinAltFunMode = 	AF0;

	//Cargar configuraciones
	GPIO_Config(&handlerPinC9);
	GPIO_Config(&handlerButtonC13);
	GPIO_Config(&handlerPinA7);
	GPIO_Config(&handlerPinC8);
	GPIO_Config(&handlerPinC7);
	GPIO_Config(&handlerPinA6);
	GPIO_Config(&handlerPinB8);
	GPIO_Config(&handlerPinC6);

	//Variable que va a llevar el estado del contador para el punto 3
	uint32_t counter =1;
	//Variables que dictan el estado que debe tener cada uno de los pines, encendido-apagado
	uint32_t c1 = 0;
	uint32_t c2 = 0;
	uint32_t c3 = 0;
	uint32_t c4 = 0;
	uint32_t c5 = 0;
	uint32_t c6 = 0;
	uint32_t c7 = 0;
	//Variable que lee el estado del botón del usuario
	uint8_t estado;


	while(1){
		//se pone al microcontrolador a no hacer nada en ese rango de tiempo para que se tenga la noción de "delay" de aproximadamente 1seg
		for(uint32_t i = 1 ; i<1250000;i++){
			NOP();
		}
		//Si el contador pasa de 1, mandelo a 60; si pasa de 60 mandelo a 1
		switch(counter){
		case(0):
			counter = 60;
			break;
		case(61):
			counter = 1;
			break;
		default:
			break;
		}
		//C transforma el numero entero a un binario, para extraer la información necesaria se hace el uso de máscara para extraer la primer posición
		//como es el bit 0 se la posición deseada es la primera, simplemente se aplica la mascara
		c1 = counter &1;
		//En estos casos el valor del bit deseado no está en la primera posición, por eso se mueve mediante la operación shift a derecha para dejarlo
		//en la primera posición, y luego se extrae el valor con la operación AND
		c2 = (counter>>1) &1;
		c3 = (counter>>2) &1;
		c4 = (counter>>3) &1;
		c5 = (counter>>4) &1;
		c6 = (counter>>5) &1;
		c7 = (counter>>6) &1;
		//Se modifica el estado de cada uno de los pines a necesidad, si c_i=0 ->Led apagado; c_i=1->Led encendido
		GPIO_WritePin(&handlerPinA7,c1);
		GPIO_WritePin(&handlerPinC8,c2);
		GPIO_WritePin(&handlerPinC7,c3);
		GPIO_WritePin(&handlerPinA6,c4);
		GPIO_WritePin(&handlerPinB8,c5);
		GPIO_WritePin(&handlerPinC6,c6);
		GPIO_WritePin(&handlerPinC9,c7);
		estado = GPIO_ReadPin(ButtonC13);
		//Si el boton esta presionado, estado=0; de lo contrario, estado=1
		if(estado){
			//Si no está presionado, aumente valor
			counter++;
		}else{
			//Si está presionado, disminuya valor
			counter--;
		}

	}

}

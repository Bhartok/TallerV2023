/*
 * GPIOxDriver.c
 *
 *  Created on: Mar 9, 2023
 *      Author: bhartok
 */


#include "GPIOxDriver.h"

//Se activa la señal de reloj

void GPIO_Config (GPIO_Handler_t *pGPIOHandler){
	uint32_t auxConfig = 0;
	uint32_t auxPosition = 0;

	//Activar periférico
	//Los condicionales verifican cada uno de los puertos
	if(pGPIOHandler->pGPIOx == GPIOA){
		//Escribimos 1(Set) en la posicion correspondiente al GPIOA
		RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN);
	}else if(pGPIOHandler->pGPIOx == GPIOB){
		//Escribimos 1(Set) en la posicion correspondiente al GPIOB
		RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOBEN);
	}else if(pGPIOHandler->pGPIOx == GPIOC){
		//Escribimos 1(Set) en la posicion correspondiente al GPIOC
		RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOCEN);
	}else if(pGPIOHandler->pGPIOx == GPIOD){
		//Escribimos 1(Set) en la posicion correspondiente al GPIOD
		RCC->AHB1ENR |= (RCC_AHB1ENR_GPIODEN);
	}else if(pGPIOHandler->pGPIOx == GPIOE){
		//Escribimos 1(Set) en la posicion correspondiente al GPIOE
		RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOEEN);
	}else if(pGPIOHandler->pGPIOx == GPIOH){
		//Escribimos 1(Set) en la posicion correspondiente al GPIOH
		RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOHEN);
	}

	//Despues de activado se empieza a configurar

	//Configurando registro GPIOx_MODER
	//Se shiftea "Pin number" veces hacia la izquerda
	auxConfig = (pGPIOHandler->GPIO_PinConfig.GPIO_PinMode <<2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

	//Se limpian los bits especificos del registro
	pGPIOHandler->pGPIOx->MODER &= ~(0b11 << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);
	//Se setea el MODER
	pGPIOHandler->pGPIOx->MODER |= auxConfig;

	//Se configura el registro GPIOx_OTYPER
	auxConfig = (pGPIOHandler->GPIO_PinConfig.GPIO_PinOType << pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);
	pGPIOHandler->pGPIOx->OTYPER &= ~(SET<< pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);
	pGPIOHandler->pGPIOx->OTYPER |= auxConfig;

	//Configurando OPSPEEDR
	auxConfig = (pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);
	pGPIOHandler->pGPIOx->OSPEEDR &= ~(0b11<< 2*pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);
	pGPIOHandler->pGPIOx->OSPEEDR |= auxConfig;

	//Configurar pull up-down, flotante
	auxConfig = (pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);
	pGPIOHandler->pGPIOx->PUPDR &= ~(0b11<< 2*pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);
	pGPIOHandler->pGPIOx->PUPDR |= auxConfig;

	//Funciones alternativas
	if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_ALTFN){
		if(pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber < 8){
			//Registro AFRL controla desde pin 0 a 7
			auxPosition = 4 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber;
			pGPIOHandler->pGPIOx->AFR[0] &= ~(0b1111 << auxPosition);
			pGPIOHandler->pGPIOx->AFR[0] |= (pGPIOHandler->GPIO_PinConfig.GPIO_PinAltFunMode << auxPosition);

		}else{
			auxPosition = 4 * (pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber - 8);
			pGPIOHandler->pGPIOx->AFR[1] &= ~(0b1111 << auxPosition);
			pGPIOHandler->pGPIOx->AFR[1] |= (pGPIOHandler->GPIO_PinConfig.GPIO_PinAltFunMode << auxPosition);
		}
	}
}//Fin del GPIO_Config

//Funcion utilizada para leer el estado de un pin especifico
uint32_t GPIO_ReadPin(GPIO_Handler_t *pPinHandler){
	//Este va a ser la variable que retornaremos
	uint32_t pinValue = 0;
	/*
	 * 1.Se obtienen todos los valores del IDR como un numero binario
	 * 2.Se corren PinNumber veces a la derecha para tener el estado del pin como primer posición de ese numero binario
	 * 3.Se extrae esa primer posición con una operación AND y una mascara que tiene un 1 como primera posición que
	 *   corresponde al estado del pin
	  */
	pinValue = (pPinHandler->pGPIOx->IDR>>pPinHandler->GPIO_PinConfig.GPIO_PinNumber)&(1<<0);
	return pinValue;
}

//Funcion que cambia el estado del pin a su complemento
void GPIOxTogglePin(GPIO_Handler_t *pPinHandler){
	/*Variable que halla el complemento del estado del pin,mediante la operación XOR.
	 *	Si el pin tiene estado 0->Complemento=1
	 *	Si el pin tiene estado 1->Complemento=0
	 */
	uint8_t complemento = GPIO_ReadPin(pPinHandler)^(1);
	//Escribe en el pin el complemento de su estado, complemento=0->"Apaga el pin", complemento=1->"Enciende el pin"
	GPIO_WritePin(pPinHandler, complemento);
}





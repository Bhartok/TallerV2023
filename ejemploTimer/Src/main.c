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

uint8_t flag ;
GPIO_Handler_t handlerOnBoardLed = {0};
BasicTimer_Handler_t handlerTimer= {0};

int main(void){
	//Configurar  el handler del led
			handlerOnBoardLed.pGPIOx = GPIOA;
			handlerOnBoardLed.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
			handlerOnBoardLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
			handlerOnBoardLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
			handlerOnBoardLed.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;

			//Cargarle el config
			GPIO_Config(&handlerOnBoardLed);

	//Configurar el handler del timer
			handlerTimer.ptrTIMx = TIM2;
			handlerTimer.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
			handlerTimer.TIMx_Config.TIMx_speed = BTIMER_SPEED_1ms;
			handlerTimer.TIMx_Config.TIMx_period = 250;
			handlerTimer.TIMx_Config.TIMx_interruptEnable = 1;

			//Cargarle el config
			BasicTimer_Config(&handlerTimer);
			/*
			//Para configurar el registro con el timer, timer 2 esta en APB1
			RCC->APB1ENR |= RCC_AP1ENR_TIM2EN;//Es lo mismo a (1<<0); Se activa el periferico en el bus
			TIM2->CCR1 &= ~TIM_CR1_DIR;// Es lo mismo que ~(1<<4) se configura la direccion
			TIM2->PSC = 16000; //Para que se modifique el counter cada mS el counter suba un valor el reloj es 16M, 16M/16k => 1000 ciclos => 1mS
			TIM2->CNT = 0; //Para que empiece a contar desde 0
			TIM2->ARR = 250; //Para que cuente hasta 250 mS
			TIM2->CR1 |= TIM_CR1_CEN; //Para habilitar el Counter Enable*/

	while(1){
		if(flag){
			GPIOxTogglePin(&handlerOnBoardLed);//Se hace que se prenda el pin, esa toggle pin se tiene que realizar
			flag = 0;
		}
	}
}

void BasicTimer2_Callback(){
	flag = 1;
}

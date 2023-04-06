/*
 * extiMain.c
 *
 *  Created on: Mar 23, 2023
 *      Author: bhartok
 */

//el exti funciona con entradas digitales(GPIO) distingue falling edge y rising edge
/*
 * 1. GPIO
 * 2. Config Mux para seleccionar que puerto da interrupcion
 * 3. Configurar EXTI -> Seleccionar flanco, mascara
 * 4. a)Deshabilitar int. globales, b) matricular en NVIC c) crear la ISR(bajar bandera) d)Crear callback e) Activar int. globales
 * 5. Probar
 *
 * */

#include <stdint.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
void callback_exti13(void);
void initHardware(void);

GPIO_Handler_t handlerLed2 = {0};//PA5
GPIO_Handler_t handlerUserButton = {0};//PA5
BasicTimer_Handler_t handlerTimer = {0};//handler del blinky, TIM2
uint32_t counterExti13 = 0;

void initHardware(void){
	//Configuracion PA5
	handlerLed2.pGPIOx = GPIOA;
	handlerLed2.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
	handlerLed2.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerLed2.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerLed2.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	//Cargarle el config
	GPIO_Config(&handlerLed2);
	//Configuracion PC13
	handlerUserButton.pGPIOx = GPIOC;
	handlerUserButton.GPIO_PinConfig.GPIO_PinNumber = PIN_13;
	handlerUserButton.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	//Cargarle el config
	GPIO_Config(&handlerUserButton);

	//Configurar el handler del timer
	handlerTimer.ptrTIMx = TIM2;
	handlerTimer.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	handlerTimer.TIMx_Config.TIMx_speed = BTIMER_SPEED_1ms;
	handlerTimer.TIMx_Config.TIMx_period = 250;
	handlerTimer.TIMx_Config.TIMx_interruptEnable = 1;
	//Cargarle el config
	BasicTimer_Config(&handlerTimer);

	GPIO_WritePin(&handlerLed2,SET);

	//Para configurar el EXTI
	//2.a Se debe habilitr la señal de reloj del periferico SYSCFG en el APB2
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	//2.b Configurar el mux 13 asignando el puerto GPIOC, la posicion 4 del EXTICR
	SYSCFG->EXTICR[3] &=~(0xF<<SYSCFG_EXTICR4_EXTI13_Pos);//Se limpia la posición
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;//Se habilita el puerto C

	//3.a Confgurar el EXTI, se selecciona si el flanco que se quiere ver es el de subida o bajada, en este caso es de subida
	EXTI->FTSR = 0; //Se anulan todas las posibles detecciones en flanco de bajada
	EXTI->RTSR = 0; //Se limpia
	EXTI->RTSR |= EXTI_RTSR_TR13; // Se asigna el valor a la posición 13, activar deteccion flanco de subida de la entrada 13
	//3. b Configurar la mascara, IMR Interrupt Mask Register
	EXTI->IMR = 0;//Se limpia
	EXTI->IMR |= EXTI_IMR_IM13; //Activamos la interrupcion EXTI_13

	//4.a Desactivar las interrupciones globales
	__disable_irq();
	//4.b Matricular interrupción en el NVIC
	NVIC_EnableIRQ(EXTI15_10_IRQn);//En este caso, la EXTI13 tiene que irse en ese combo que habilita de la 15:10
	__enable_irq();

}

//4.c Crear la ISR, verifica la interrupcion-> Baja la bandera(Pending Register)-> Llamar al callback

void EXTI15_10_IRQHandler(void){

	if(EXTI->PR& EXTI_PR_PR13){
		EXTI->PR |= EXTI_PR_PR13;//Se escribe 1 en esa posición
		callback_exti13();
	}

}


int main(void){
	initHardware();

	while(1){}
	return 0;
}

void BasicTimer2_Callback(){
	GPIOxTogglePin(&handlerLed2);
}
void callback_exti13(void){
	counterExti13++;

}


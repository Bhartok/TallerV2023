#include <stm32f4xx.h>
#include <stdint.h>
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"

/* = = = = = Variables que se utilizarán a lo largo del código = = = = =  */

uint8_t rotacion;			// Variable utilizada para saber el sentido de la rotación
uint8_t decena = 0; 		// Qué display se utiliza, decena o unidad
uint8_t contador = 0; 		//Contador del contador
uint8_t modoContador = 1; 	//Qué modo se tiene, contador o culebrita
uint8_t contadorCulebra = 1;// Guarda la posición de la culebrita
/* = = = Handlers GPIO = = = */
GPIO_Handler_t handlerLED2 		= {0};		 // LED blinky

GPIO_Handler_t handlerSelUni 	= {0};		// Selector que activa el transistor correspondiente al display de unidades
GPIO_Handler_t handlerSelDec 	= {0};		// Selector que activa el transistor correspondiente al display de decenas

GPIO_Handler_t handlerSegA		= {0};		// Handler para el segmento A del display 7 segmentos
GPIO_Handler_t handlerSegB		= {0};		// Handler para el segmento B del display 7 segmentos
GPIO_Handler_t handlerSegC		= {0};		// Handler para el segmento C del display 7 segmentos
GPIO_Handler_t handlerSegD		= {0};		// Handler para el segmento D del display 7 segmentos
GPIO_Handler_t handlerSegE		= {0};		// Handler para el segmento E del display 7 segmentos
GPIO_Handler_t handlerSegF		= {0};		// Handler para el segmento F del display 7 segmentos
GPIO_Handler_t handlerSegG		= {0};		// Handler para el segmento G del display 7 segmentos

GPIO_Handler_t handlerEncoderA 	= {0};//Handler del GPIO que genera la interrupción cuando se mueve el encoder
GPIO_Handler_t handlerEncoderB 	= {0};//Handler del otro puerto del encoder para ver si es CW o CCW
GPIO_Handler_t handlerEncoderSW = {0};//Handler del GPIO que genera la interrupción cuando se presiona el botón del encoder-> Cambia de modo(contador-culebrita)
/* = = = Handler Timers = = = */
BasicTimer_Handler_t handlerBlinkyTimer 	= {0};		// Timer del LED blinky
BasicTimer_Handler_t handlerDisplayTimer	= {0};      // Timer para tasa de refresco entre display unidad-decena

/* = = = Handlers EXTI = = = */
EXTI_Config_t extiEncoderA = {0};//Handler EXTI para rotacion
EXTI_Config_t extiEncoderSW = {0};//Handler EXTI para pulsación de botón


/* = = = Cabeceras de las funciones = = = */
void init_Hardware(void); // Función para inicializar el hardware
void pinteNumero(uint8_t numero); //Función utilizada para prender los leds necesarios en el 7segmentos dependiendo el numero que se necesite
void pinteCulebra(uint8_t  contadorCulebra);//Función utilizada para prender el led necesario que indica la posición de la culebrita


/* = = = = = INICIO DEL CORE DEL PROGRAMA = = = = =  */
int main(void){

	// Inicialización de todos los elementos del sistema
	init_Hardware();

	while(1){

	}
}
/* = = = = = FIN DEL CORE DEL PROGRAMA = = = = =  */

/* = = = = = INICIO DE LA DEFINICIÓN DE LAS FUNCIONES = = = = = */
void init_Hardware(void){

	/* = = = INICIO DEL LED DE ESTADO (BLINKY) = = = */
	// Configuración del LED2 - PA5
	handlerLED2.pGPIOx								= GPIOA;
	handlerLED2.GPIO_PinConfig.GPIO_PinNumber		= PIN_5;
	handlerLED2.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerLED2.GPIO_PinConfig.GPIO_PinOType		= GPIO_OTYPE_PUSHPULL;
	handlerLED2.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_MEDIUM;
	handlerLED2.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	// Cargando la configuración
	GPIO_Config(&handlerLED2);


	// Configuración del TIM2 para que haga un blinky cada 250ms
	handlerBlinkyTimer.ptrTIMx								= TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode				= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed				= BTIMER_SPEED_1ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period				= 250; // Lanza una interrupción cada 250 ms
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable		= 1;

	BasicTimer_Config(&handlerBlinkyTimer);
	/* = = = FIN DEL LED DE ESTADO (BLINKY) = = = */
	// Cargando la configuración del TIM3

	handlerDisplayTimer.ptrTIMx								= TIM5;
	handlerDisplayTimer.TIMx_Config.TIMx_mode				= BTIMER_MODE_UP;
	handlerDisplayTimer.TIMx_Config.TIMx_speed				= BTIMER_SPEED_1ms;
	handlerDisplayTimer.TIMx_Config.TIMx_period				= 10; // Lanza una interrupción cada 10 ms
	handlerDisplayTimer.TIMx_Config.TIMx_interruptEnable	= 1;

	// Cargando la configuración del TIM3
	BasicTimer_Config(&handlerDisplayTimer);




	/* = = = INICIO DE CONFIGURACIÓN DE PINES = = = */

	// Se definen las configuraciones de los pines {PC9, PC8} como salidas que controlan
	// el switcheo de los transistores de unidades y decenas (respectivamente)

	// Parámetros para la configuración del PC9 que controla el transistor de unidades
	handlerSelUni.pGPIOx									= GPIOC;
	handlerSelUni.GPIO_PinConfig.GPIO_PinNumber				= PIN_9;
	handlerSelUni.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerSelUni.GPIO_PinConfig.GPIO_PinOType				= GPIO_OTYPE_PUSHPULL;
	handlerSelUni.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerSelUni.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_FAST;
	handlerSelUni.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;

	// Parámetros para la configuración del PB8 que controla el transistor de decenas
	handlerSelDec.pGPIOx									= GPIOC;
	handlerSelDec.GPIO_PinConfig.GPIO_PinNumber				= PIN_8;
	handlerSelDec.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerSelDec.GPIO_PinConfig.GPIO_PinOType				= GPIO_OTYPE_PUSHPULL;
	handlerSelDec.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerSelDec.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_FAST;
	handlerSelDec.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;

	// Se carga la configuración de los pines selectores
	GPIO_Config(&handlerSelUni);
	GPIO_Config(&handlerSelDec);

	// Se definen las configuraciones de los pines {PA7, PA6, PB6, PB13, PC7, PC6, PB9} como salidas que controlan
	// cada uno de los 7 segmentos del display

	// Parámetros para la configuración del PA7 que controla el segmento A
	handlerSegA.pGPIOx									= GPIOA;
	handlerSegA.GPIO_PinConfig.GPIO_PinNumber			= PIN_7;
	handlerSegA.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerSegA.GPIO_PinConfig.GPIO_PinOType			= GPIO_OTYPE_PUSHPULL;
	handlerSegA.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerSegA.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerSegA.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;

	// Parámetros para la configuración del PA6 que controla el segmento B
	handlerSegB.pGPIOx									= GPIOA;
	handlerSegB.GPIO_PinConfig.GPIO_PinNumber			= PIN_6;
	handlerSegB.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerSegB.GPIO_PinConfig.GPIO_PinOType			= GPIO_OTYPE_PUSHPULL;
	handlerSegB.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerSegB.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerSegB.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;

	// Parámetros para la configuración del PB6 que controla el segmento C
	handlerSegC.pGPIOx									= GPIOB;
	handlerSegC.GPIO_PinConfig.GPIO_PinNumber			= PIN_6	;
	handlerSegC.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerSegC.GPIO_PinConfig.GPIO_PinOType			= GPIO_OTYPE_PUSHPULL;
	handlerSegC.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerSegC.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerSegC.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;

	// Parámetros para la configuración del PB13 que controla el segmento D
	handlerSegD.pGPIOx									= GPIOB;
	handlerSegD.GPIO_PinConfig.GPIO_PinNumber			= PIN_13;
	handlerSegD.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerSegD.GPIO_PinConfig.GPIO_PinOType			= GPIO_OTYPE_PUSHPULL;
	handlerSegD.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerSegD.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerSegD.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;

	// Parámetros para la configuración del PC7 que controla el segmento E
	handlerSegE.pGPIOx									= GPIOC;
	handlerSegE.GPIO_PinConfig.GPIO_PinNumber			= PIN_7;
	handlerSegE.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerSegE.GPIO_PinConfig.GPIO_PinOType			= GPIO_OTYPE_PUSHPULL;
	handlerSegE.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerSegE.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerSegE.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;

	// Parámetros para la configuración del PC6 que controla el segmento F
	handlerSegF.pGPIOx									= GPIOC;
	handlerSegF.GPIO_PinConfig.GPIO_PinNumber			= PIN_6;
	handlerSegF.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerSegF.GPIO_PinConfig.GPIO_PinOType			= GPIO_OTYPE_PUSHPULL;
	handlerSegF.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerSegF.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerSegF.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;

	// Parámetros para la configuración del PB9 que controla el segmento G
	handlerSegG.pGPIOx									= GPIOB;
	handlerSegG.GPIO_PinConfig.GPIO_PinNumber			= PIN_9;
	handlerSegG.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerSegG.GPIO_PinConfig.GPIO_PinOType			= GPIO_OTYPE_PUSHPULL;
	handlerSegG.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerSegG.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerSegG.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;

	// Se carga la configuración de los pines de salida
	GPIO_Config(&handlerSegA);
	GPIO_Config(&handlerSegB);
	GPIO_Config(&handlerSegC);
	GPIO_Config(&handlerSegD);
	GPIO_Config(&handlerSegE);
	GPIO_Config(&handlerSegF);
	GPIO_Config(&handlerSegG);

	//Configuracion ENCODER A
	handlerEncoderA.pGPIOx									= GPIOC;
	handlerEncoderA.GPIO_PinConfig.GPIO_PinNumber			= PIN_4;
	handlerEncoderA.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_IN;
	handlerEncoderA.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerEncoderA.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerEncoderA.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;


	//Configuracion ENCODER B
	handlerEncoderB.pGPIOx									= GPIOB;
	handlerEncoderB.GPIO_PinConfig.GPIO_PinNumber			= PIN_5;
	handlerEncoderB.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_IN;
	handlerEncoderB.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerEncoderB.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerEncoderB.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;
	//solo se va a tener como entrada para valor de referencia y saber si es CW o CCW, mas no como EXTI
	GPIO_Config(&handlerEncoderB);

	//Configuracion ENCODER SW
	handlerEncoderSW.pGPIOx									= GPIOC;
	handlerEncoderSW.GPIO_PinConfig.GPIO_PinNumber			= PIN_5;
	handlerEncoderSW.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_IN;
	handlerEncoderSW.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerEncoderSW.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerEncoderSW.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;

	//Se carga la configuracion de los pines EXTI
	extiEncoderA.edgeType = EXTERNAL_INTERRUPT_FALLING_EDGE;
	extiEncoderA.pGPIOHandler = &handlerEncoderA;
	extInt_Config(&extiEncoderA);

	extiEncoderSW.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;
	extiEncoderSW.pGPIOHandler = &handlerEncoderSW;
	extInt_Config(&extiEncoderSW);



}

/* = = = = = INICIO DE LAS RUTINAS DE ATENCIÓN (CALLBACKS) = = = = = */

// Callback del timer 2 correspondiente al LED Blinky
void BasicTimer2_Callback(void){
	GPIOxTogglePin(&handlerLED2);//Cada 250 ms cambia el estado del led encendido-apagado
}

void BasicTimer5_Callback(void){
	//Cada 10mS se mira si el programa está en modo contador o en modo culebrita para saber qué display prender, y también cómo debe prenderlo
	if(modoContador){//Si está en modo contador
		if(decena){//Se coloca este condicional para hacer la alimentación correcta de los transistores(solo uno prendido a la vez)

			GPIO_WritePin(&handlerSelUni,RESET);//Se apaga el display de unidades
			GPIO_WritePin(&handlerSelDec,SET);//Se prende el display de decenas
			pinteNumero(contador/10);//Con esta operación se rescata la "decena" del numero
			decena = 0;//Se hace que decena sea = 0 para que en el siguiente ciclo vaya al otro condicional
		}else{
			GPIO_WritePin(&handlerSelDec,RESET);//Se apaga el display de decenas
			GPIO_WritePin(&handlerSelUni,SET);//Se prende el display de unidades
			pinteNumero(contador%10);//Con esta operación se rescata la "unidad" del numero
			decena = 1;//Se hace que decena sea = 1 para que el siguiente ciclo vaya al otro condicional
		}
	}
	else{//Si no está en modo contador está en modo Culebra
		//Se prende solamente el segmento necesario en la posición del display necesaria
		pinteCulebra(contadorCulebra);
	}
}

void callback_extInt5(void){//EXTI asociada a la pulsación del botón, actúa como un toggle del estado, de modo contador pasa a modo culebra y viceversa
	if(modoContador){
		modoContador = 0;
	}else{
		modoContador=1;
	}
}

//Callback de rotacion del encoder (C4)
void callback_extInt4(void){
	//Se lee el estado de la entrada de Data para saber si es CCW(0) o CW(1)
	rotacion = GPIO_ReadPin(&handlerEncoderB);
	//Determina si está en modo Contador o modo culebrita
	if(modoContador){
		//Si está en modo contador y es una rotacion CCW
		if(!rotacion){
			if(contador == 0){//Condicional utilizado para que el contador no pueda disminuir más de 0
				contador++;
			}
			contador--;//Disminuye en una unidad el contador
		}else{//Si es una rotación CW
			if(contador == 99){
				contador--;//Condicional utilizado para que el contador no pueda aumentar más de 99
			}
			contador++;//Aumenta en una unidad el contador
		}
	}else{//Si está en modo culebrita
		if(!rotacion){//Si es una rotación CCW
			if(contadorCulebra == 0){//Condicional utilizado para lograr que pase de la posición 1 a la posición 12 cuando se disminuya de 1
				contadorCulebra = 13;
			}
			contadorCulebra--;//Se disminuye en 1 la posición de la culebrita
		}else{//CW
			if(contadorCulebra == 13){//Condicional utilizado para lograr que pase de la posición 12 a la posición 1 cuando se pase de 12
				contadorCulebra = 0;
			}
			contadorCulebra++;//Se aumenta en 1 unidad


	}
}}

/* = = = = = FIN DE LAS RUTINAS DE ATENCIÓN (CALLBACKS) = = = = = */
/* Como es un display de ánodo común, sus leds se encienden con una señal de 0, RESET significa que se prenderá, SET lo contrario*/
//Función utilizada para solo encender el display necesario en la posición necesaria que va a tomar como argumento la posición de la culebra para saber qué led prender

void pinteCulebra(uint8_t contadorCulebra){
	switch(contadorCulebra){
	case 1:{
		GPIO_WritePin(&handlerSelUni,SET);
		GPIO_WritePin(&handlerSelDec,RESET);

		GPIO_WritePin(&handlerSegA,RESET);
		GPIO_WritePin(&handlerSegB,SET);
		GPIO_WritePin(&handlerSegC,SET);
		GPIO_WritePin(&handlerSegD,SET);
		GPIO_WritePin(&handlerSegE,SET);
		GPIO_WritePin(&handlerSegF,SET);
		GPIO_WritePin(&handlerSegG,SET);

		break;
	}
	case 2:{
		GPIO_WritePin(&handlerSelUni,RESET);
		GPIO_WritePin(&handlerSelDec,SET);

		GPIO_WritePin(&handlerSegA,RESET);
		GPIO_WritePin(&handlerSegB,SET);
		GPIO_WritePin(&handlerSegC,SET);
		GPIO_WritePin(&handlerSegD,SET);
		GPIO_WritePin(&handlerSegE,SET);
		GPIO_WritePin(&handlerSegF,SET);
		GPIO_WritePin(&handlerSegG,SET);

		break;
	}
	case 3:{
		GPIO_WritePin(&handlerSelUni,RESET);
		GPIO_WritePin(&handlerSelDec,SET);

		GPIO_WritePin(&handlerSegA,SET);
		GPIO_WritePin(&handlerSegB,SET);
		GPIO_WritePin(&handlerSegC,SET);
		GPIO_WritePin(&handlerSegD,SET);
		GPIO_WritePin(&handlerSegE,SET);
		GPIO_WritePin(&handlerSegF,RESET);
		GPIO_WritePin(&handlerSegG,SET);
		break;
	}
	case 4:{
		GPIO_WritePin(&handlerSelUni,RESET);
		GPIO_WritePin(&handlerSelDec,SET);

		GPIO_WritePin(&handlerSegA,SET);
		GPIO_WritePin(&handlerSegB,SET);
		GPIO_WritePin(&handlerSegC,SET);
		GPIO_WritePin(&handlerSegD,SET);
		GPIO_WritePin(&handlerSegE,RESET);
		GPIO_WritePin(&handlerSegF,SET);
		GPIO_WritePin(&handlerSegG,SET);

		break;
	}
	case 5:{
		GPIO_WritePin(&handlerSelUni,RESET);
		GPIO_WritePin(&handlerSelDec,SET);

		GPIO_WritePin(&handlerSegA,SET);
		GPIO_WritePin(&handlerSegB,SET);
		GPIO_WritePin(&handlerSegC,SET);
		GPIO_WritePin(&handlerSegD,RESET);
		GPIO_WritePin(&handlerSegE,SET);
		GPIO_WritePin(&handlerSegF,SET);
		GPIO_WritePin(&handlerSegG,SET);

		break;
	}
	case 6:{
		GPIO_WritePin(&handlerSelUni,SET);
		GPIO_WritePin(&handlerSelDec,RESET);

		GPIO_WritePin(&handlerSegA,SET);
		GPIO_WritePin(&handlerSegB,SET);
		GPIO_WritePin(&handlerSegC,SET);
		GPIO_WritePin(&handlerSegD,SET);
		GPIO_WritePin(&handlerSegE,RESET);
		GPIO_WritePin(&handlerSegF,SET);
		GPIO_WritePin(&handlerSegG,SET);
		break;
	}
	case 7:{
		GPIO_WritePin(&handlerSelUni,SET);
		GPIO_WritePin(&handlerSelDec,RESET);

		GPIO_WritePin(&handlerSegA,SET);
		GPIO_WritePin(&handlerSegB,SET);
		GPIO_WritePin(&handlerSegC,SET);
		GPIO_WritePin(&handlerSegD,SET);
		GPIO_WritePin(&handlerSegE,SET);
		GPIO_WritePin(&handlerSegF,RESET);
		GPIO_WritePin(&handlerSegG,SET);

		break;
	}
	case 8:{
		GPIO_WritePin(&handlerSelUni,RESET);
		GPIO_WritePin(&handlerSelDec,SET);

		GPIO_WritePin(&handlerSegA,SET);
		GPIO_WritePin(&handlerSegB,RESET);
		GPIO_WritePin(&handlerSegC,SET);
		GPIO_WritePin(&handlerSegD,SET);
		GPIO_WritePin(&handlerSegE,SET);
		GPIO_WritePin(&handlerSegF,SET);
		GPIO_WritePin(&handlerSegG,SET);
		break;
	}
	case 9:{
		GPIO_WritePin(&handlerSelUni,RESET);
		GPIO_WritePin(&handlerSelDec,SET);

		GPIO_WritePin(&handlerSegA,SET);
		GPIO_WritePin(&handlerSegB,SET);
		GPIO_WritePin(&handlerSegC,RESET);
		GPIO_WritePin(&handlerSegD,SET);
		GPIO_WritePin(&handlerSegE,SET);
		GPIO_WritePin(&handlerSegF,SET);
		GPIO_WritePin(&handlerSegG,SET);

		break;
	}
	case 10:{
		GPIO_WritePin(&handlerSelUni,SET);
		GPIO_WritePin(&handlerSelDec,RESET);

		GPIO_WritePin(&handlerSegA,SET);
		GPIO_WritePin(&handlerSegB,SET);
		GPIO_WritePin(&handlerSegC,SET);
		GPIO_WritePin(&handlerSegD,RESET);
		GPIO_WritePin(&handlerSegE,SET);
		GPIO_WritePin(&handlerSegF,SET);
		GPIO_WritePin(&handlerSegG,SET);
		break;
	}
	case 11:{
		GPIO_WritePin(&handlerSelUni,SET);
		GPIO_WritePin(&handlerSelDec,RESET);

		GPIO_WritePin(&handlerSegA,SET);
		GPIO_WritePin(&handlerSegB,SET);
		GPIO_WritePin(&handlerSegC,RESET);
		GPIO_WritePin(&handlerSegD,SET);
		GPIO_WritePin(&handlerSegE,SET);
		GPIO_WritePin(&handlerSegF,SET);
		GPIO_WritePin(&handlerSegG,SET);
		break;
	}
	case 12:{
		GPIO_WritePin(&handlerSelUni,SET);
		GPIO_WritePin(&handlerSelDec,RESET);

		GPIO_WritePin(&handlerSegA,SET);
		GPIO_WritePin(&handlerSegB,RESET);
		GPIO_WritePin(&handlerSegC,SET);
		GPIO_WritePin(&handlerSegD,SET);
		GPIO_WritePin(&handlerSegE,SET);
		GPIO_WritePin(&handlerSegF,SET);
		GPIO_WritePin(&handlerSegG,SET);
		break;
	}

	default: break;

	}

}

//Función utilizada para "pintar" determinado numero en el display
void pinteNumero(uint8_t numero){
	switch(numero){
	case 0:{
		GPIO_WritePin(&handlerSegA,RESET);
		GPIO_WritePin(&handlerSegB,RESET);
		GPIO_WritePin(&handlerSegC,RESET);
		GPIO_WritePin(&handlerSegD,RESET);
		GPIO_WritePin(&handlerSegE,RESET);
		GPIO_WritePin(&handlerSegF,RESET);
		GPIO_WritePin(&handlerSegG,SET);

		break;
	}
	case 1:{
		GPIO_WritePin(&handlerSegA,SET);
		GPIO_WritePin(&handlerSegB,RESET);
		GPIO_WritePin(&handlerSegC,RESET);
		GPIO_WritePin(&handlerSegD,SET);
		GPIO_WritePin(&handlerSegE,SET);
		GPIO_WritePin(&handlerSegF,SET);
		GPIO_WritePin(&handlerSegG,SET);


		break;
	}
	case 2:{
		GPIO_WritePin(&handlerSegA,RESET);
		GPIO_WritePin(&handlerSegB,RESET);
		GPIO_WritePin(&handlerSegC,SET);
		GPIO_WritePin(&handlerSegD,RESET);
		GPIO_WritePin(&handlerSegE,RESET);
		GPIO_WritePin(&handlerSegF,SET);
		GPIO_WritePin(&handlerSegG,RESET);

		break;
	}
	case 3:{
		GPIO_WritePin(&handlerSegA,RESET);
		GPIO_WritePin(&handlerSegB,RESET);
		GPIO_WritePin(&handlerSegC,RESET);
		GPIO_WritePin(&handlerSegD,RESET);
		GPIO_WritePin(&handlerSegE,SET);
		GPIO_WritePin(&handlerSegF,SET);
		GPIO_WritePin(&handlerSegG,RESET);

		break;
	}
	case 4:{
		GPIO_WritePin(&handlerSegA,SET);
		GPIO_WritePin(&handlerSegB,RESET);
		GPIO_WritePin(&handlerSegC,RESET);
		GPIO_WritePin(&handlerSegD,SET);
		GPIO_WritePin(&handlerSegE,SET);
		GPIO_WritePin(&handlerSegF,RESET);
		GPIO_WritePin(&handlerSegG,RESET);

		break;
	}
	case 5:{
		GPIO_WritePin(&handlerSegA,RESET);
		GPIO_WritePin(&handlerSegB,SET);
		GPIO_WritePin(&handlerSegC,RESET);
		GPIO_WritePin(&handlerSegD,RESET);
		GPIO_WritePin(&handlerSegE,SET);
		GPIO_WritePin(&handlerSegF,RESET);
		GPIO_WritePin(&handlerSegG,RESET);

		break;
	}
	case 6:{
		GPIO_WritePin(&handlerSegA,RESET);
		GPIO_WritePin(&handlerSegB,SET);
		GPIO_WritePin(&handlerSegC,RESET);
		GPIO_WritePin(&handlerSegD,RESET);
		GPIO_WritePin(&handlerSegE,RESET);
		GPIO_WritePin(&handlerSegF,RESET);
		GPIO_WritePin(&handlerSegG,RESET);

		break;
	}
	case 7:{
		GPIO_WritePin(&handlerSegA,RESET);
		GPIO_WritePin(&handlerSegB,RESET);
		GPIO_WritePin(&handlerSegC,RESET);
		GPIO_WritePin(&handlerSegD,SET);
		GPIO_WritePin(&handlerSegE,SET);
		GPIO_WritePin(&handlerSegF,SET);
		GPIO_WritePin(&handlerSegG,SET);

		break;
	}
	case 8:{
		GPIO_WritePin(&handlerSegA,RESET);
		GPIO_WritePin(&handlerSegB,RESET);
		GPIO_WritePin(&handlerSegC,RESET);
		GPIO_WritePin(&handlerSegD,RESET);
		GPIO_WritePin(&handlerSegE,RESET);
		GPIO_WritePin(&handlerSegF,RESET);
		GPIO_WritePin(&handlerSegG,RESET);


		break;
	}
	case 9:{
		GPIO_WritePin(&handlerSegA,RESET);
		GPIO_WritePin(&handlerSegB,RESET);
		GPIO_WritePin(&handlerSegC,RESET);
		GPIO_WritePin(&handlerSegD,RESET);
		GPIO_WritePin(&handlerSegE,SET);
		GPIO_WritePin(&handlerSegF,RESET);
		GPIO_WritePin(&handlerSegG,RESET);

		break;
	}
	default: break;

	}

}

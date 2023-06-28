#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stm32f4xx.h>
#include "arm_math.h"

#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "USARTxDriver.h"
#include "PLLDriver.h"
#include "SysTickDriver.h"
#include "ExtiDriver.h"
#include "PwmDriver.h"


/* Elementos para hacer la comunicacion serial */
GPIO_Handler_t handlerPinTX ={0};
GPIO_Handler_t handlerPinRX ={0};
USART_Handler_t handlerComTerminal={0};
uint8_t rxData = 0;
char bufferData[64];

/*Configuracion blinky simple- timer 250mS*/
GPIO_Handler_t handlerBlinkyPin = {0};
BasicTimer_Handler_t handlerBlinkyTimer = {0};
uint8_t cblinky = 0;

/*Configuracion HC*/
GPIO_Handler_t handlerEcho ={0};
GPIO_Handler_t handlerTrig ={0};
BasicTimer_Handler_t handlerMeasure = {0};//Timer a 2 uS para las medidas de distancia
uint8_t trigf = 0;						  //flag que indica si se debe contar el tiempo
float32_t trigc= 0;						  //Var. auxiliar donde se cuenta el tiempo cada 2uS
uint16_t advancedistance = 0;             //Var. auxiliar que sirve para contar avance longitudinal
uint8_t iniciarPrograma = 0;		      //Flag que indica que hay que empezar la rutina de parqueo
uint8_t iniciarParqueo = 0;				  //Flag que sirve para saber si se tiene que iniciar parqueo o no
uint8_t estadoMotores = 0;				  //Flag que sirve para saber si los motores estan encendidos o no
uint8_t iniciarReversa= 0;				  //Flag que indica si hay que iniciar la reversa
uint8_t rotarDerecha = 0;			      //Flag que indica que se tiene que empezar a rotar a la derecha
uint8_t primerinicio = 1;         	      //Flag que ayuda a tomar el estado cuando recien se inicia la rutina

/*Configuracion de señal-pin PWM para alimentar a los motores*/
GPIO_Handler_t handlerPinPwmAcoplador1 = {0};
PWM_Handler_t handlerSignalPwmAcoplador1 = {0};
GPIO_Handler_t handlerPinPwmAcoplador2 = {0};
PWM_Handler_t handlerSignalPwmAcoplador2 = {0};
GPIO_Handler_t handlerPinPwmAcoplador3 = {0};
PWM_Handler_t handlerSignalPwmAcoplador3 = {0};
GPIO_Handler_t handlerPinPwmAcoplador4 = {0};
PWM_Handler_t handlerSignalPwmAcoplador4 = {0};
uint32_t duttyciclemotores = 2000;
uint32_t periodoMotores = 5000;

/*Configuracion de EXTI para iniciar rutina de parqueo*/
EXTI_Config_t extiButonC13 = {0};//Handler EXTI para pulsación de botón
GPIO_Handler_t handlerButtonC13 = {0};//Handler exti boton micro




// Definicion de las cabeceras de las funciones
void initSystem(void);				//inicializa el sistema
float measDist(void);				//devuelve la distancia transversal en cm
void moverAdelante(void);			//Acciona los motores para que el carro se mueva hacia adelante
void moverAtras(void);				//Acciona los motores para que el carro se mueva hacia atras
void moverDerecha(void);			//Acciona los motores para que el carro rote hacia la derecha
void moverIzquierda(void);			//Acciona los motores para que el carro rote hacia la izquierda
void parar(void);					//Apaga los motores para que el carro detenga el movimiento
void rotar90gradosderecha(void);	//Acciona los motores para que el carro se rote 90 grados a la derecha
void rotar90gradosizquierda(void);  //Acciona los motores para que el carro se rote 90 grados a la izquierda
void reversa(void);					//Acciona los motores para que el carro se mueva hacia atras una distancia determinada
void avanza(void);					//Acciona los motores para que el carro se mueva hacia adelante una distancia determinada

int main (void){

	SCB->CPACR |= (0xF << 20);					// Activamos el coprocesador matematico FPU


	config_SysTick(2);							//Se configura que el systick avance cada 1mS a 100MHz
	configPLL(100);								//Se configura el micro a 100MHz
	initSystem();								//Se inicializa el sistema
	writeMsg(&handlerComTerminal,"\n USART funcionando correctamente a 100MHz \n");//Se manda comando por USART para mostrar buen funcionamiento

	while(1){
		//Hacemos un "eco" con el valor que nos llega por el serial
		if(iniciarPrograma){
			delay(10);// para tomar medidas cada 10mS
			if(primerinicio){
				writeMsg(&handlerComTerminal,"Iniciando programa \n");
				delay(1000);			//Espere 1segundo despues de pulsado el boton
				moverAdelante();
				primerinicio = 0;		//Para que no vuelva a "mover adelante" los motores
			}
			if(measDist()>20){
				writeMsg(&handlerComTerminal,"Se encontro posible parqueadero \n");
				delay(200);
				parar();       		//Pare los motores y espere 1 segundo para que luego empiece a medir distancia longitudinal
				delay(1000);
				iniciarParqueo = 1; //Se habilita para que empiece a medir distancia longitudinal
				iniciarPrograma = 0;//Para que no entre más al if iniciar programa
			}
		}
		if(iniciarParqueo){
			delay(10);//Para que se mida cada 10 mS
			if(measDist()>20){//Si la distancia es mayor que 20 cm:
				if(estadoMotores == 0){//Se prenden los motores solo una vez
					writeMsg(&handlerComTerminal,"Se empieza a medir distancia longitudinal \n");
					moverAdelante();
					estadoMotores = 1;//Para que no vuelva a encender los motores
				}
				if(advancedistance > 55){	//Si han pasado 55 ciclos tiene suficiente distancia para parquear
					parar();				//Pare
					iniciarParqueo = 0; 	//Para que no entre más al if de iniciarParqueo
					advancedistance = 0;	//Reinicie el advanced distance
					iniciarReversa = 1;		//Habilite el iniciar reversa
					estadoMotores = 0;		//reiniciar estado de los motores para que se muestre que estan apagados
					writeMsg(&handlerComTerminal,"Se hallo distancia \n");
				}
			}
			else{//Si no es mayor que 20cm:
				iniciarParqueo = 0; 	//Para que no entre más al if de iniciarParqueo
				advancedistance = 0;	//Reinicie el advanced distance
				estadoMotores = 0;		//reiniciar estado de los motores para que se muestre que estan apagados
				iniciarPrograma = 1;
				primerinicio = 1;
				parar();				//Pare el programa
				writeMsg(&handlerComTerminal,"No se encontro distancia suficiente \n");

			}
			advancedistance++;//cada 10mS aumenta el advanced distance

		}

		if(iniciarReversa){
				reversa(); 			//Reverse la cantidad indicada
				iniciarReversa = 0; //Para que no vuelva a entrar a esta etapa
				rotarDerecha = 1;	//Para que entre a la siguiente etapa
		}
		if(rotarDerecha){
				rotar90gradosderecha();
				rotarDerecha = 0;
				parar();		//Para que no vuelva a entrar a esta etapa
				delay(1000);	//Espere 1S
				avanza();
				delay(1000);	//Espere 1S
				rotar90gradosizquierda();

		}
		if(rxData != '\0'){
			if(rxData == 'a'){
				sprintf(bufferData,"La distancia en cm es %f \n",measDist());
				writeMsg(&handlerComTerminal,bufferData);
				rxData = '\0';
			}
			else if(rxData == 'b'){
				writeMsg(&handlerComTerminal,"reversa\n");
				reversa();
				rxData = '\0';
			}
			else if(rxData == 'c'){
				writeMsg(&handlerComTerminal,"90 grados izquierda\n");
				rotar90gradosizquierda();
				rxData = '\0';
			}
			else if(rxData == 'd'){
				writeMsg(&handlerComTerminal,"90 grados derecha \n");
				rotar90gradosderecha();
				rxData = '\0';
			}
			else if(rxData == 's' || rxData == 'S'){
				writeMsg(&handlerComTerminal,"Stop \n");
				parar();
				rxData = '\0';
			}
			else if(rxData == 'A'){
				avanza();
				rxData = '\0';
			}
			else if(rxData == 'B'){
				writeMsg(&handlerComTerminal,"Hacia atras");
				moverAtras();
				rxData = '\0';
			}
			else if(rxData == 'C'){
				writeMsg(&handlerComTerminal,"Hacia derecha");
				moverDerecha();
				rxData = '\0';
			}
			else if(rxData == 'D'){
				writeMsg(&handlerComTerminal,"Hacia izquierda");
				moverIzquierda();
				rxData = '\0';
			}
		}
	}
	return(0);
}

void reversa(void){
	writeMsg(&handlerComTerminal,"Se empezara a reversar \n");
	moverAtras();
	delay(300);  //Se usa el delay para asegurar la distancia
	writeMsg(&handlerComTerminal,"Se termino de reversar \n");
	parar();
	delay(1000);//Se espera 1 segundo
}
void avanza(void){
	writeMsg(&handlerComTerminal,"Se empezara a avanzar 15 cm \n");
	moverAdelante();
	delay(900); //Se usa el delay para asegurar la distancia
	writeMsg(&handlerComTerminal,"Se termino de avanzar \n");
	parar();
	delay(1000);
}

void rotar90gradosderecha(void){
	writeMsg(&handlerComTerminal,"Se empezara a rotar a derecha \n");
	moverIzquierda();
	delay(1450); //Se usa el delay para asegurar la distancia
	rotarDerecha = 0;
	writeMsg(&handlerComTerminal,"Se termino de rotar a derecha \n");
	parar();
	delay(1000);
}
void rotar90gradosizquierda(void){
	writeMsg(&handlerComTerminal,"Se empezara a rotar a izquierda\n");
	moverDerecha();
	delay(1000); //Se usa el delay para asegurar la distancia
	rotarDerecha = 0;
	writeMsg(&handlerComTerminal,"Se termino de rotar a izquierda\n");
	parar();
	delay(1000);
}

float measDist(void){
	float distance;
	GPIO_WritePin(&handlerTrig, SET);
	delay(1);
	GPIO_WritePin(&handlerTrig, RESET);//Se prende el pin por 1mS para asegurar que cuando se apague se va a mandar el pulso

	while(!GPIO_ReadPin(&handlerEcho)){
		__NOP();
	};				//Espera a que prenda el echo(empiece a recibir)
	trigf = 1;		//Habilita el trig flag para empezar a contar de a 2uS
	while(GPIO_ReadPin(&handlerEcho)){
		__NOP();
	}				//Espera a que apague el echo(deja de recibir)
	trigf = 0; 		//Se resetea la flag y el contador, se calcula la distancia y se devuelve
	distance = trigc*340.0/10000.0/2.0;
	trigc = 0;
	return distance;
}

void parar(void){
	updateDuttyCycle(&handlerSignalPwmAcoplador1, 0);
	updateDuttyCycle(&handlerSignalPwmAcoplador2, 0);
	updateDuttyCycle(&handlerSignalPwmAcoplador3, 0);
	updateDuttyCycle(&handlerSignalPwmAcoplador4, 0);
}

void moverAdelante(void){
	updateDuttyCycle(&handlerSignalPwmAcoplador1, 0);
	updateDuttyCycle(&handlerSignalPwmAcoplador2, duttyciclemotores+400);//Se le coloca el offset de 700 para calibrar y que los dos roten a la misma velocidad(aprox)
	updateDuttyCycle(&handlerSignalPwmAcoplador3, duttyciclemotores);
	updateDuttyCycle(&handlerSignalPwmAcoplador4, 0);
}
void moverAtras(void){
	updateDuttyCycle(&handlerSignalPwmAcoplador1, duttyciclemotores+400);
	updateDuttyCycle(&handlerSignalPwmAcoplador2, 0);
	updateDuttyCycle(&handlerSignalPwmAcoplador3, 0);
	updateDuttyCycle(&handlerSignalPwmAcoplador4, duttyciclemotores);
}
void moverDerecha(void){
	updateDuttyCycle(&handlerSignalPwmAcoplador1, 0);
	updateDuttyCycle(&handlerSignalPwmAcoplador2, duttyciclemotores);
	updateDuttyCycle(&handlerSignalPwmAcoplador3, 0);
	updateDuttyCycle(&handlerSignalPwmAcoplador4, duttyciclemotores);
}
void moverIzquierda(void){
	updateDuttyCycle(&handlerSignalPwmAcoplador1, duttyciclemotores);
	updateDuttyCycle(&handlerSignalPwmAcoplador2, 0);
	updateDuttyCycle(&handlerSignalPwmAcoplador3, duttyciclemotores);
	updateDuttyCycle(&handlerSignalPwmAcoplador4, 0);

}

void initSystem(void){

	/*Configuracion exti*/
	handlerButtonC13.pGPIOx =								GPIOC;
	handlerButtonC13.GPIO_PinConfig.GPIO_PinNumber = 		PIN_13;
	handlerButtonC13.GPIO_PinConfig.GPIO_PinMode = 		GPIO_MODE_IN;
	handlerButtonC13.GPIO_PinConfig.GPIO_PinPuPdControl = 	GPIO_PUPDR_NOTHING;
	handlerButtonC13.GPIO_PinConfig.GPIO_PinSpeed = 		GPIO_OSPEED_MEDIUM;
	handlerButtonC13.GPIO_PinConfig.GPIO_PinAltFunMode = 	AF0;
	GPIO_Config(&handlerButtonC13);

	extiButonC13.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;
	extiButonC13.pGPIOHandler = &handlerButtonC13;
	extInt_Config(&extiButonC13);

	/*Configuracion pwm*/
	handlerPinPwmAcoplador1.pGPIOx                                = GPIOA;
	handlerPinPwmAcoplador1.GPIO_PinConfig.GPIO_PinNumber         = PIN_6;
	handlerPinPwmAcoplador1.GPIO_PinConfig.GPIO_PinMode           = GPIO_MODE_ALTFN;
	handlerPinPwmAcoplador1.GPIO_PinConfig.GPIO_PinOPType         = GPIO_OTYPE_PUSHPULL;
	handlerPinPwmAcoplador1.GPIO_PinConfig.GPIO_PinSpeed          = GPIO_OSPEED_FAST;
	handlerPinPwmAcoplador1.GPIO_PinConfig.GPIO_PinPuPdControl    = GPIO_PUPDR_NOTHING;
	handlerPinPwmAcoplador1.GPIO_PinConfig.GPIO_PinAltFunMode     = AF2;
	/* Cargamos la configuracion en los registros del MCU */
	GPIO_Config(&handlerPinPwmAcoplador1);

	handlerSignalPwmAcoplador1.ptrTIMx                = TIM3;
	handlerSignalPwmAcoplador1.config.channel         = PWM_CHANNEL_1;
	handlerSignalPwmAcoplador1.config.duttyCicle      = 0;
	handlerSignalPwmAcoplador1.config.periodo         = periodoMotores;
	handlerSignalPwmAcoplador1.config.prescaler       = 1000;
	/* Cargamos la configuracion en los registros del MCU */
	pwm_Config(&handlerSignalPwmAcoplador1);
	enableOutput(&handlerSignalPwmAcoplador1);
	startPwmSignal(&handlerSignalPwmAcoplador1);


	/*Configuracion pwm*/
	handlerPinPwmAcoplador2.pGPIOx                                = GPIOA;
	handlerPinPwmAcoplador2.GPIO_PinConfig.GPIO_PinNumber         = PIN_7;
	handlerPinPwmAcoplador2.GPIO_PinConfig.GPIO_PinMode           = GPIO_MODE_ALTFN;
	handlerPinPwmAcoplador2.GPIO_PinConfig.GPIO_PinOPType         = GPIO_OTYPE_PUSHPULL;
	handlerPinPwmAcoplador2.GPIO_PinConfig.GPIO_PinSpeed          = GPIO_OSPEED_FAST;
	handlerPinPwmAcoplador2.GPIO_PinConfig.GPIO_PinPuPdControl    = GPIO_PUPDR_NOTHING;
	handlerPinPwmAcoplador2.GPIO_PinConfig.GPIO_PinAltFunMode     = AF2;
	/* Cargamos la configuracion en los registros del MCU */
	GPIO_Config(&handlerPinPwmAcoplador2);

	handlerSignalPwmAcoplador2.ptrTIMx                = TIM3;
	handlerSignalPwmAcoplador2.config.channel         = PWM_CHANNEL_2;
	handlerSignalPwmAcoplador2.config.duttyCicle      = 0;
	handlerSignalPwmAcoplador2.config.periodo         = periodoMotores;
	handlerSignalPwmAcoplador2.config.prescaler       = 1000;
	/* Cargamos la configuracion en los registros del MCU */
	pwm_Config(&handlerSignalPwmAcoplador2);
	enableOutput(&handlerSignalPwmAcoplador2);
	startPwmSignal(&handlerSignalPwmAcoplador2);




	handlerPinPwmAcoplador3.pGPIOx                                = GPIOB;
	handlerPinPwmAcoplador3.GPIO_PinConfig.GPIO_PinNumber         = PIN_0;
	handlerPinPwmAcoplador3.GPIO_PinConfig.GPIO_PinMode           = GPIO_MODE_ALTFN;
	handlerPinPwmAcoplador3.GPIO_PinConfig.GPIO_PinOPType         = GPIO_OTYPE_PUSHPULL;
	handlerPinPwmAcoplador3.GPIO_PinConfig.GPIO_PinSpeed          = GPIO_OSPEED_FAST;
	handlerPinPwmAcoplador3.GPIO_PinConfig.GPIO_PinPuPdControl    = GPIO_PUPDR_NOTHING;
	handlerPinPwmAcoplador3.GPIO_PinConfig.GPIO_PinAltFunMode     = AF2;
	/* Cargamos la configuracion en los registros del MCU */
	GPIO_Config(&handlerPinPwmAcoplador3);

	handlerSignalPwmAcoplador3.ptrTIMx                = TIM3;
	handlerSignalPwmAcoplador3.config.channel         = PWM_CHANNEL_3;
	handlerSignalPwmAcoplador3.config.duttyCicle      = 0;
	handlerSignalPwmAcoplador3.config.periodo         = periodoMotores;
	handlerSignalPwmAcoplador3.config.prescaler       = 1000;
	/* Cargamos la configuracion en los registros del MCU */
	pwm_Config(&handlerSignalPwmAcoplador3);
	enableOutput(&handlerSignalPwmAcoplador3);
	startPwmSignal(&handlerSignalPwmAcoplador3);

	handlerPinPwmAcoplador4.pGPIOx                                = GPIOB;
	handlerPinPwmAcoplador4.GPIO_PinConfig.GPIO_PinNumber         = PIN_1;
	handlerPinPwmAcoplador4.GPIO_PinConfig.GPIO_PinMode           = GPIO_MODE_ALTFN;
	handlerPinPwmAcoplador4.GPIO_PinConfig.GPIO_PinOPType         = GPIO_OTYPE_PUSHPULL;
	handlerPinPwmAcoplador4.GPIO_PinConfig.GPIO_PinSpeed          = GPIO_OSPEED_FAST;
	handlerPinPwmAcoplador4.GPIO_PinConfig.GPIO_PinPuPdControl    = GPIO_PUPDR_NOTHING;
	handlerPinPwmAcoplador4.GPIO_PinConfig.GPIO_PinAltFunMode     = AF2;
	/* Cargamos la configuracion en los registros del MCU */
	GPIO_Config(&handlerPinPwmAcoplador4);

	handlerSignalPwmAcoplador4.ptrTIMx                = TIM3;
	handlerSignalPwmAcoplador4.config.channel         = PWM_CHANNEL_4;
	handlerSignalPwmAcoplador4.config.duttyCicle      = 0;
	handlerSignalPwmAcoplador4.config.periodo         = periodoMotores;
	handlerSignalPwmAcoplador4.config.prescaler       = 1000;
	/* Cargamos la configuracion en los registros del MCU */
	pwm_Config(&handlerSignalPwmAcoplador4);
	enableOutput(&handlerSignalPwmAcoplador4);
	startPwmSignal(&handlerSignalPwmAcoplador4);


	/* Configuracion del LED de estado */
	handlerBlinkyPin.pGPIOx                             = GPIOA;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinNumber      = PIN_5;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_OUT;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinOPType      = GPIO_OTYPE_PUSHPULL;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_OSPEED_FAST;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerBlinkyPin);


	/* Configuracion del TIM2 para cada 250mS  y tim5 para cada 2uS*/
	handlerBlinkyTimer.ptrTIMx                               = TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode                 = BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed                = BTIMER_SPEED_100Mhz_100us;
	handlerBlinkyTimer.TIMx_Config.TIMx_period               = 100;
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable      = 1;
	BasicTimer_Config(&handlerBlinkyTimer);

	handlerMeasure.ptrTIMx                               = TIM5;
	handlerMeasure.TIMx_Config.TIMx_mode                 = BTIMER_MODE_UP;
	handlerMeasure.TIMx_Config.TIMx_speed                = BTIMER_SPEED_100Mhz_1us;
	handlerMeasure.TIMx_Config.TIMx_period               = 2;
	handlerMeasure.TIMx_Config.TIMx_interruptEnable      = 1;
	BasicTimer_Config(&handlerMeasure);

	/*Configuración del USART*/
	handlerPinTX.pGPIOx                               = GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber        = PIN_11;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode          = GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode    = AF8;
	GPIO_Config(&handlerPinTX);

	handlerPinRX.pGPIOx                               = GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber        = PIN_12;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode          = GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode    = AF8;
	GPIO_Config(&handlerPinRX);

	handlerComTerminal.ptrUSARTx                       = USART6;
	handlerComTerminal.USART_Config.USART_baudrate     = USART_BAUDRATE_115200;
	handlerComTerminal.USART_Config.USART_datasize     = USART_DATASIZE_8BIT;
	handlerComTerminal.USART_Config.USART_parity       = USART_PARITY_NONE;
	handlerComTerminal.USART_Config.USART_stopbits     = USART_STOPBIT_1;
	handlerComTerminal.USART_Config.USART_mode         = USART_MODE_RXTX;
	handlerComTerminal.USART_Config.USART_enableIntRX  = USART_RX_INTERRUP_ENABLE;
	handlerComTerminal.USART_Config.USART_enableIntTX  = USART_TX_INTERRUP_DISABLE;
	handlerComTerminal.USART_Config.USART_frequency    = 100;
	USART_Config(&handlerComTerminal);

	/*Configuracion sensor ultrasonico*/
	handlerTrig.pGPIOx                               = GPIOC;
	handlerTrig.GPIO_PinConfig.GPIO_PinNumber        = PIN_8;
	handlerTrig.GPIO_PinConfig.GPIO_PinMode          = GPIO_MODE_OUT;
	handlerTrig.GPIO_PinConfig.GPIO_PinOPType     	 = GPIO_OTYPE_PUSHPULL;
	handlerTrig.GPIO_PinConfig.GPIO_PinSpeed     	 = GPIO_OSPEED_FAST;
	handlerTrig.GPIO_PinConfig.GPIO_PinPuPdControl	 = GPIO_PUPDR_NOTHING;
	handlerTrig.GPIO_PinConfig.GPIO_PinAltFunMode    = AF0;
	GPIO_Config(&handlerTrig);

	handlerEcho.pGPIOx                               = GPIOC;
	handlerEcho.GPIO_PinConfig.GPIO_PinNumber        = PIN_6;
	handlerEcho.GPIO_PinConfig.GPIO_PinMode          = GPIO_MODE_IN;
	handlerEcho.GPIO_PinConfig.GPIO_PinAltFunMode    = AF0;
	handlerEcho.GPIO_PinConfig.GPIO_PinPuPdControl	 = GPIO_PUPDR_NOTHING;
	handlerEcho.GPIO_PinConfig.GPIO_PinSpeed	     = GPIO_OSPEED_FAST;
	GPIO_Config(&handlerEcho);



}



/* Callbacks de las interrupciones */
void usart6Rx_Callback(void){
	rxData = getRxData();
}

void BasicTimer2_Callback(void){
	cblinky++;
	if(cblinky==25){
		GPIOxTooglePin(&handlerBlinkyPin);
		cblinky = 0;
	}
}
/* Callbacks de los Timers */
void BasicTimer5_Callback(void){
	if(trigf){
		trigc+=2;//Si la flag está activada aumente 1 cada 2uS
	}
}

void callback_extInt13(void){//EXTI asociada a la pulsación del botón, actúa como un toggle del estado, de modo contador pasa a modo culebra y viceversa
	iniciarPrograma = 1;
}



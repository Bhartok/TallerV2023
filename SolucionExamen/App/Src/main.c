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
#include "I2CDriver.h"
#include "PwmDriver.h"
#include "PLLDriver.h"
#include "AdcDriver.h"
#include "RTCDriver.h"



/* Definicion de las macros */
#define ACCEL_ADRESS    0b1101001;

#define ACCEL_ZOUT_H    63
#define ACCEL_ZOUT_L    64


#define PWR_MGMT_1        107
#define WHO_AM_I        117

/* Definicion de variables */

/* Elementos para hacer la comunicacion serial */
GPIO_Handler_t handlerPinTX ={0};
GPIO_Handler_t handlerPinRX ={0};
USART_Handler_t handlerComTerminal={0};
char cmd[64];
char userMsg[64];
int firstParameter;
int secondParameter;
int thirdParameter;


/*Configuracion blinky simple- timer5mS*/
GPIO_Handler_t handlerBlinkyPin = {0};
BasicTimer_Handler_t handlerBlinkyTimer = {0};

/*Configuraciones para USART*/
uint8_t stringComplete = false;
uint8_t rxData = 0;
uint8_t counterReception = 0;
char bufferData[64];

/*Configuracion para el I2C */
GPIO_Handler_t handlerI2CAcelSDA = {0};
GPIO_Handler_t handlerI2CAcelSCL = {0};
I2C_Handler_t handlerAccelerometer = {0};
uint8_t i2cBuffer ={0};
int cont2i = 0;
uint8_t cont2e;
uint8_t cBlinky;
uint8_t coordinates[2];
float32_t coordinatez[512];//Array donde se van a guardar los datos para z
uint8_t cont2eimprimir;
uint8_t coordinatesbool = 0; //V. Auxiliar que es una flag para calcular las coordenadas

/*Configuracion para el ADC*/
ADC_Config_t adcConfig ={0};
uint16_t adcData1[256] = {0};
uint16_t adcData2[256] = {0};
uint16_t adcDataCounter = 0;
uint8_t adcCounter = 0;
bool adcIsComplete = false;
PWM_Handler_t handlerSignalPwm1 = {0};

/*Configuracion para RTC*/
RTC_Config_t rtcConfig = {0};
uint8_t calendar[6] = {0};

/*Configuracion para FFT*/
arm_rfft_fast_instance_f32 config_Rfft_fast_f32;
arm_status statusInitFFT = ARM_MATH_ARGUMENT_ERROR;
uint16_t fftSize = 512;
float32_t transformedSignalz[512];
float32_t abssignal[512];
float32_t transformadamela[512];
uint32_t ifftFlag = 0;



// Definicion de las cabeceras de las funciones
void initSystem(void);
void getCoordinates(void);
void parseCommands(char *ptrbufferData);

int main (void){

	SCB->CPACR |= (0xF << 20);					// Activamos el coprocesador matematico FPU

	RCC->BDCR |= RCC_BDCR_LSEON;		  		// Se habilita el LSE


	initSystem();								//Se inicializa el sistema
	configPLL(100);								//Se configura el micro a 100MHz
	i2c_writeSingleRegister(&handlerAccelerometer, PWR_MGMT_1 , 0x00); //Se setea el acelerometro
	writeMsg(&handlerComTerminal,"\n Usa el comando help para ver todos los comandos \n");//Se manda comando por USART para mostrar buen funcionamiento

	while(1){
        if(cont2eimprimir){//Cuando adquiera datos del acelerometro, imprimalos todos en SI
        	writeMsg(&handlerComTerminal,"Datos listos para FFT\n");
            cont2eimprimir = 0;
        }
		//Hacemos un "eco" con el valor que nos llega por el serial
		if (rxData != '\0'){
			bufferData[counterReception] = rxData;
			counterReception++;

		// Si el caracter que llega representa un cambio de línea, se levanta una
		// bandera para el loop main
		if (rxData == '@'){
			stringComplete = true;

			// Se agrega el caracter nulo al final del string
			bufferData[counterReception] = '\0';

			counterReception = 0;
		}

		// Para que no vuelva a entrar. Solo cambia debido a la interrupción
		rxData = '\0';
		}
		//cada 5mS va a calcular las aceleraciones[200Hz]
        if(coordinatesbool){
            getCoordinates();
            coordinatesbool = 0;
        }

		// Hacemos un análisis de la cadena de datos obtenida
		if (stringComplete){
			parseCommands(bufferData);
			stringComplete = false;
		}
		//Cuando se completen las conversiones ADC imprimalas por USART
		if(adcIsComplete){
			for(int contador = 0;contador<256;contador++){
				sprintf(bufferData,"%u\t%u \n",(unsigned int)adcData1[contador],(unsigned int)adcData2[contador]);
				writeMsg(&handlerComTerminal,bufferData);
			}
			adcIsComplete = false;
		}
	}
	return(0);
}


void initSystem(void){


	/* Configuracion del LED de estado */
	handlerBlinkyPin.pGPIOx                             = GPIOH;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinNumber      = PIN_1;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_OUT;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinOPType      = GPIO_OTYPE_PUSHPULL;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_OSPEED_FAST;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerBlinkyPin);

	/* Configuracion del TIM2 para cada 5mS */
	handlerBlinkyTimer.ptrTIMx                               = TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode                 = BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed                = BTIMER_SPEED_100Mhz_100us;
	handlerBlinkyTimer.TIMx_Config.TIMx_period               = 50;
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable      = 1;
	BasicTimer_Config(&handlerBlinkyTimer);

	/*Configuración del USART*/
	handlerPinTX.pGPIOx                               = GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber        = PIN_9;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode          = GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode    = AF7;
	GPIO_Config(&handlerPinTX);

	handlerPinRX.pGPIOx                               = GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber        = PIN_10;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode          = GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode    = AF7;
	GPIO_Config(&handlerPinRX);

	handlerComTerminal.ptrUSARTx                       = USART1;
	handlerComTerminal.USART_Config.USART_baudrate     = USART_BAUDRATE_115200;
	handlerComTerminal.USART_Config.USART_datasize     = USART_DATASIZE_8BIT;
	handlerComTerminal.USART_Config.USART_parity       = USART_PARITY_NONE;
	handlerComTerminal.USART_Config.USART_stopbits     = USART_STOPBIT_1;
	handlerComTerminal.USART_Config.USART_mode         = USART_MODE_RXTX;
	handlerComTerminal.USART_Config.USART_enableIntRX  = USART_RX_INTERRUP_ENABLE;
	handlerComTerminal.USART_Config.USART_enableIntTX  = USART_TX_INTERRUP_DISABLE;
	handlerComTerminal.USART_Config.USART_frequency    = 100;
	USART_Config(&handlerComTerminal);

	/*Configuracion del ADC*/
	adcConfig.dataAlignment = ADC_ALIGNMENT_RIGHT;
	adcConfig.resolution = ADC_RESOLUTION_12_BIT;
	adcConfig.channels[0] = ADC_CHANNEL_0;
	adcConfig.channels[1] = ADC_CHANNEL_1;
	adcConfig.samplingPeriods[0] = ADC_SAMPLING_PERIOD_112_CYCLES;
	adcConfig.samplingPeriods[1] = ADC_SAMPLING_PERIOD_112_CYCLES;
	adcConfig.numberOfChannels = 2;
	adcConfig.extsel = 0b1100;
	adcConfig.exten = 1;
	adc_Config(&adcConfig);

	handlerSignalPwm1.ptrTIMx                = TIM5;
	handlerSignalPwm1.config.channel         = PWM_CHANNEL_3;
	handlerSignalPwm1.config.duttyCicle      = 1;
	handlerSignalPwm1.config.periodo         = 67;
	handlerSignalPwm1.config.prescaler       = BTIMER_SPEED_100Mhz_1us;
	pwm_Config(&handlerSignalPwm1);
	enableOutput(&handlerSignalPwm1);

	handlerI2CAcelSCL.pGPIOx                                      = GPIOB;
	handlerI2CAcelSCL.GPIO_PinConfig.GPIO_PinNumber               = PIN_8;
	handlerI2CAcelSCL.GPIO_PinConfig.GPIO_PinMode                 = GPIO_MODE_ALTFN;
	handlerI2CAcelSCL.GPIO_PinConfig.GPIO_PinOPType               = GPIO_OTYPE_OPENDRAIN;
	handlerI2CAcelSCL.GPIO_PinConfig.GPIO_PinSpeed                = GPIO_OSPEED_FAST;
	handlerI2CAcelSCL.GPIO_PinConfig.GPIO_PinPuPdControl          = GPIO_PUPDR_NOTHING;
	handlerI2CAcelSCL.GPIO_PinConfig.GPIO_PinAltFunMode           = AF4;
	GPIO_Config(&handlerI2CAcelSCL);

	handlerI2CAcelSDA.pGPIOx                                      = GPIOB;
	handlerI2CAcelSDA.GPIO_PinConfig.GPIO_PinNumber               = PIN_9;
	handlerI2CAcelSDA.GPIO_PinConfig.GPIO_PinMode                 = GPIO_MODE_ALTFN;
	handlerI2CAcelSDA.GPIO_PinConfig.GPIO_PinOPType               = GPIO_OTYPE_OPENDRAIN;
	handlerI2CAcelSDA.GPIO_PinConfig.GPIO_PinSpeed                = GPIO_OSPEED_FAST;
	handlerI2CAcelSDA.GPIO_PinConfig.GPIO_PinPuPdControl          = GPIO_PUPDR_NOTHING;
	handlerI2CAcelSDA.GPIO_PinConfig.GPIO_PinAltFunMode           = AF4;
	GPIO_Config(&handlerI2CAcelSDA);

	handlerAccelerometer.ptrI2Cx                            = I2C1;
	handlerAccelerometer.modeI2C                            = I2C_MODE_FM;
	handlerAccelerometer.slaveAddress                       = ACCEL_ADRESS;
	i2c_config(&handlerAccelerometer);

}

void parseCommands(char *ptrbufferData){
	//Lee la cadena de caracteres a la que apunta el puntero y almacena tres elementos diferentes
	//un string llamado "cmd" y dos integer llamados  firstParameter y secondParameter
	sscanf(ptrbufferData, "%s %u %u %u %s", cmd, &firstParameter, &secondParameter, &thirdParameter, userMsg);

	if(strcmp(cmd,"help") == 0){
		writeMsg(&handlerComTerminal, "Menu de ayuda CMDs:\n");
		writeMsg(&handlerComTerminal, "1) help    -- Mostrar menu \n");
		writeMsg(&handlerComTerminal, "2) mco    -- Configure MCO \n");
		writeMsg(&handlerComTerminal, "		firstParameter: 1->Configurar reloj(0:HSI,1:LSE,2:PLL)\n");
		writeMsg(&handlerComTerminal, "		firstParameter: 2->Configurar preescaler a valor dado en segundo parametro(1,2,3,4,5)\n");
		writeMsg(&handlerComTerminal, "3)trim -- escribe el valor del firstParameter en el registro HSITRIM\n");
		writeMsg(&handlerComTerminal, "4)getTrim -- Devuelve el valor actual del registro HSITRIM\n");
		writeMsg(&handlerComTerminal, "5)setAdc -- Modifica la velocidad de muestreo de ADC al valor puesto en el primer parametro\n");
		writeMsg(&handlerComTerminal, "6)getAdc -- Hacer muestreo de 256 datos por ADC multicanal\n");
		writeMsg(&handlerComTerminal, "7)whoami -- Muestra registro de que el acelerometro esta bien configurado\n");
		writeMsg(&handlerComTerminal, "9)tomaAcel -- Toma datos a 200Hz para la FFT\n");
		writeMsg(&handlerComTerminal, "10)inicializarFFT -- inicia la función de FFT para poder usarla\n");
		writeMsg(&handlerComTerminal, "10)iniciarFFT -- Realiza FFT a los datos leídos del comando 9\n");
		writeMsg(&handlerComTerminal, "11)ponerfecha -- Establece fecha del RTC\n");
		writeMsg(&handlerComTerminal, "12)ponerhora -- Establece hora del RTC\n");
		writeMsg(&handlerComTerminal, "13)leerfecha -- lee fecha del RTC\n");
		writeMsg(&handlerComTerminal, "14)leerhora -- lee hora del RTC\n");
		writeMsg(&handlerComTerminal, "15)leer -- lee fecha y hora del RTC\n");
	}else if(strcmp(cmd,"mco") == 0){ //comandos MCO
		/*CONFIGURACION DEL MCO*/
		if(firstParameter == 1){//Configurar qué muestra el MCO
			if(secondParameter == 0){
				RCC->CFGR &= ~(RCC_CFGR_MCO1);
				RCC->CFGR |= 0b00 << RCC_CFGR_MCO1_Pos;
				writeMsg(&handlerComTerminal, "Mostrando HSI por MCO1\n");
			}
			else if(secondParameter == 1){
				// Se activa el LSE
				RCC->BDCR &= ~(RCC_BDCR_LSEON);
				RCC->BDCR |= 0b1 << RCC_BDCR_LSEON_Pos;
				// Se selecciona el LSE
				RCC->CFGR &= ~(RCC_CFGR_MCO1);
				RCC->CFGR |= 0b01 << RCC_CFGR_MCO1_Pos;
				writeMsg(&handlerComTerminal, "Mostrando LSE por MCO1\n");
			}
			else if(secondParameter == 2){
				RCC->CFGR &= ~(RCC_CFGR_MCO1);
				RCC->CFGR |= 0b11 << RCC_CFGR_MCO1_Pos;
				writeMsg(&handlerComTerminal, "Mostrando PLL por MCO1\n");
			}
			else{
				writeMsg(&handlerComTerminal, "Seleccion no valida \n");
			}

		}else{
			if(secondParameter == 1){
				RCC->CFGR &= ~(RCC_CFGR_MCO1PRE);
				RCC->CFGR |= 0b0 << RCC_CFGR_MCO1PRE_Pos;
				writeMsg(&handlerComTerminal, "Prescaler MCO1 a 1\n");
			}
			else if(secondParameter == 2){
				RCC->CFGR &= ~(RCC_CFGR_MCO1PRE);
				RCC->CFGR |= 0b100 << RCC_CFGR_MCO1PRE_Pos;
				writeMsg(&handlerComTerminal, "Prescaler MCO1 a 2 \n");
			}
			else if(secondParameter == 3){
				RCC->CFGR &= ~(RCC_CFGR_MCO1PRE);
				RCC->CFGR |= 0b101 << RCC_CFGR_MCO1PRE_Pos;
				writeMsg(&handlerComTerminal, "Prescaler MCO1 a 3\n");
			}
			else if(secondParameter == 4){
				RCC->CFGR &= ~(RCC_CFGR_MCO1PRE);
				RCC->CFGR |= 0b110 << RCC_CFGR_MCO1PRE_Pos;
				writeMsg(&handlerComTerminal, "Prescaler MCO1 a 4\n");
			}
			else if(secondParameter == 5){
				RCC->CFGR &= ~(RCC_CFGR_MCO1PRE);
				RCC->CFGR |= 0b111 << RCC_CFGR_MCO1PRE_Pos;
				writeMsg(&handlerComTerminal, "Prescaler MCO1 a 5\n");
			}
			else{
				writeMsg(&handlerComTerminal, "Valor ingresado no valido.\n");
			}
			firstParameter = 0;
			secondParameter = 0;

		}
	/*Sintonizar micro */
	}else if(strcmp(cmd,"trim") == 0){
		if(firstParameter>= 0 && firstParameter<=28){
			writeMsg(&handlerComTerminal, "Trimming HSI to new value:\n");
			RCC->CR &= ~RCC_CR_HSITRIM;
			RCC->CR |= firstParameter<<RCC_CR_HSITRIM_Pos;
			uint8_t auxVar = (RCC->CR>>3)&0b11111;
			sprintf(bufferData,"Nuevo valor %u \n",auxVar);
			writeMsg(&handlerComTerminal,bufferData);
		}
		else{
			writeMsg(&handlerComTerminal,"Valor no valido \n");
		}
		firstParameter = 0;
		secondParameter = 0;
	}else if(strcmp(cmd,"getTrim") == 0){
		uint8_t auxVar = (RCC->CR>>3)&0b11111;
		sprintf(bufferData,"Valor actual de trimming %u \n",auxVar);
		writeMsg(&handlerComTerminal,bufferData);
	}
	/*comandos ADC config vel, obtener lectura*/
	else if(strcmp(cmd,"setAdc") == 0){
		if(firstParameter>1 &&firstParameter<2147483647){
			float32_t frecuenciaADC = (1.0/firstParameter)*1000;
			sprintf(bufferData,"Con el periodo %d se tiene %.2f kHz\n",firstParameter,frecuenciaADC);
			writeMsg(&handlerComTerminal,bufferData);
			updateFrequency(&handlerSignalPwm1,firstParameter);
		}else{
			writeMsg(&handlerComTerminal,"Valor invalido ingresado\n");
		}
		firstParameter = 0;
		secondParameter = 0;

	}else if(strcmp(cmd,"getAdc") == 0){
		writeMsg(&handlerComTerminal,"Empezando adquisicion 256 datos ADC \n");
		startPwmSignal(&handlerSignalPwm1);
	}
	/*Comandos acelerometro, whoami, reset, adquirir datos a 200Hz, hacer fft*/
	else if(strcmp(cmd,"whoami")==0){
		sprintf(bufferData, "\n");
		writeMsg(&handlerComTerminal, bufferData);
		i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, WHO_AM_I);
		sprintf(bufferData, "Who am i 0x%x \n", (unsigned int) i2cBuffer);
		writeMsg(&handlerComTerminal, bufferData);
		rxData = '\0';
	}else if(strcmp(cmd,"tomaAcel")==0){
		sprintf(bufferData, "\n");
		writeMsg(&handlerComTerminal, bufferData);
		cont2e=1;
		writeMsg(&handlerComTerminal, "se empezara a leer\n ");
		rxData = '\0';
	}
	else if(strcmp(cmd,"inicializarFFT")==0){
		writeMsg(&handlerComTerminal, "\nSe inicializa FFT \n");
		statusInitFFT = arm_rfft_fast_init_f32(&config_Rfft_fast_f32,fftSize);
		if(statusInitFFT == ARM_MATH_SUCCESS){
			writeMsg(&handlerComTerminal, "FFT inicializada \n");
		}
	}
	else if(strcmp(cmd,"iniciarFFT")==0){
		sprintf(bufferData, "\n");
		writeMsg(&handlerComTerminal, "Se empieza a hacer FFT con los datos \n");
		if(statusInitFFT == ARM_MATH_SUCCESS){
			arm_rfft_fast_f32(&config_Rfft_fast_f32, coordinatez, transformedSignalz, ifftFlag);
			arm_abs_f32(transformedSignalz, abssignal, fftSize);
			for(int c = 1; c < fftSize; c++){
				if(c%2){
					transformadamela[c]=abssignal[c];
				}
			}
			float32_t transformedMaxz = transformadamela[0];
			uint8_t transformedMaxIndexz = 0;
			for(int c = 1; c < fftSize; c++){
				if(transformadamela[c]>transformedMaxz){
					transformedMaxIndexz = c;
					transformedMaxz = transformadamela[c];
				}
				transformadamela[c]=abssignal[c];
				sprintf(bufferData,"%d;%f\n",c,transformadamela[c]);
				writeMsg(&handlerComTerminal,bufferData);
			}
			sprintf(bufferData, "El valor maximo en z fue %f con indice %u\n",transformedMaxz,transformedMaxIndexz);
			writeMsg(&handlerComTerminal,bufferData);
			float frecuenciaFFT = (float)(transformedMaxIndexz*200/fftSize);
			sprintf(bufferData, "La frecuencia hallada es %f \n",frecuenciaFFT);
			writeMsg(&handlerComTerminal,bufferData);
		}
		else{
			writeMsg(&handlerComTerminal,"FFT no inicializada\n");
		}

		rxData = '\0';
	}

	else if(strcmp(cmd,"ponerfecha")==0){
		if (firstParameter > 0 && firstParameter <= 31 && secondParameter > 0 && secondParameter <= 12 && thirdParameter >= 0 && thirdParameter <= 99) {
			writeMsg(&handlerComTerminal, "Se configurara la fecha a: ");
			rtcConfig.RTC_DayValue = firstParameter;
			rtcConfig.RTC_Month = secondParameter;
			rtcConfig.RTC_Year = thirdParameter;
			rtcConfig.DRMod = 1;
			RTC_Config(&rtcConfig);
			rtcConfig.DRMod = 0;
			sprintf(bufferData, "%u/%u/%u \n",firstParameter,secondParameter,thirdParameter);
			writeMsg(&handlerComTerminal, bufferData);
		}
		else{
			writeMsg(&handlerComTerminal,"Se ha ingresado un formato invalido\n");
		}
		rxData = '\0';
		firstParameter = 0;
		secondParameter = 0;
		thirdParameter = 0;
	}
	else if(strcmp(cmd,"ponerhora")==0){
		if (firstParameter >= 0 && firstParameter <= 23 && secondParameter >= 0 && secondParameter <= 59 && thirdParameter >= 0 && thirdParameter <= 59){
			writeMsg(&handlerComTerminal, "Se configurara la hora a: ");
			rtcConfig.RTC_Hours = firstParameter;
			rtcConfig.RTC_Minutes = secondParameter;
			rtcConfig.RTC_Seconds = thirdParameter;
			rtcConfig.TRMod = 1;
			RTC_Config(&rtcConfig);
			rtcConfig.TRMod = 0;
			sprintf(bufferData, "%u:%u:%u \n",firstParameter,secondParameter,thirdParameter);
			writeMsg(&handlerComTerminal, bufferData);
		}
		else{
			writeMsg(&handlerComTerminal,"Se ha ingresado un formato invalido\n");
		}
		rxData = '\0';
		firstParameter = 0;
		secondParameter = 0;
		thirdParameter = 0;
	}
	else if(strcmp(cmd,"leer")==0){
		RTC_read(calendar);
		sprintf(bufferData,"la fecha es: %u/%u/%u  %u:%u:%u\n",calendar[4],calendar[5],calendar[6],calendar[2],calendar[1],calendar[0]);
		writeMsg(&handlerComTerminal,bufferData);

	}else if(strcmp(cmd,"leerfecha")==0){
		RTC_read(calendar);
		sprintf(bufferData,"la fecha es: %u/%u/%u\n",calendar[4],calendar[5],calendar[6]);
		writeMsg(&handlerComTerminal,bufferData);

	}else if(strcmp(cmd,"leerhora")==0){
		RTC_read(calendar);
		sprintf(bufferData,"la hora es:%u:%u:%u\n",calendar[2],calendar[1],calendar[0]);
		writeMsg(&handlerComTerminal,bufferData);

	}
}

/* Funcion para el muestreo inicial por 2 segundos */
void getCoordinates(void){
    uint8_t accel_z_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_H);
    uint8_t accel_z_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_L);
    if(cont2e){
        if(cont2i==0){
        }else{
            coordinatez[cont2i-1] =(accel_z_high<<8|accel_z_low)*2*9.78/32767;
            }
    }
    coordinatesbool = 0;
}
/* Callbacks de las interrupciones */
void usart1Rx_Callback(void){
	rxData = getRxData();
}

/* Callbacks de los Timers */
void BasicTimer2_Callback(void){
	cBlinky++;
	if(cBlinky==50){//para hacer blinky cada 250mS
	GPIOxTooglePin(&handlerBlinkyPin);
	cBlinky = 0;
	}
	coordinatesbool = 1;
    if(cont2i == 512){//Si el contador de iteraciones se pasa de las 512 iteraciones
        cont2e = 0;//baje la bandera de contar
        cont2i = 0;//Reinicie las iteraciones
        cont2eimprimir = 1;//Active el flag que le permite imprimir el mensaje de que se termino la adquisicion de datos.
    }
    if(cont2e){  //Si el flag de contar está activado
        cont2i++;//Aumente en 1 el numero de iteraciones
    }

}

void adcComplete_Callback(void){
	if(adcCounter==0){
		adcData1[adcDataCounter]=getADC();//se lleva un contador para saber si es el dato 1 o el dato 2
	}else{
		adcData2[adcDataCounter]=getADC();
		adcDataCounter++;				  //Se lleva un contador para saber cuántos parejas de datos se han convertidos
	}
	adcCounter++;
	if(adcDataCounter == 256){
		adcDataCounter = 0;
		stopPwmSignal(&handlerSignalPwm1);//Se para el pwm para detener la conversión ADC
		adcIsComplete = true;
	}
	if(adcCounter == 2){
		adcCounter = 0;
	}
}



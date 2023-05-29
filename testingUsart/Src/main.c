#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stm32f4xx.h>

#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "USARTxDriver.h"
#include "SysTickDriver.h"
#include "I2CDriver.h"
#include "PwmDriver.h"
#include "PLLDriver.h"
#include "LCDDriver.h"


#define PLL_80_CLOCK_CONFIGURED  3
#define PLL_80  0

/* Definicion de las macros */
#define ACCEL_ADRESS    0b1101001;
#define ACCEL_XOUT_H    59
#define ACCEL_XOUT_L    60
#define ACCEL_YOUT_H    61
#define ACCEL_YOUT_L    62
#define ACCEL_ZOUT_H    63
#define ACCEL_ZOUT_L    64


#define PWR_MGMT_1        107
#define WHO_AM_I        117

#define LCD_ADDRESS				0x23

/* Definicion de variables */

/* Elementos para hacer la comunicacion serial */
GPIO_Handler_t handlerPinTX ={0};
GPIO_Handler_t handlerPinRX ={0};
USART_Handler_t handlerCommTerminal={0};

// Handler del Blinky simple
GPIO_Handler_t handlerBlinkyPin = {0};

/* Handler de los timers */
BasicTimer_Handler_t handlerBlinkyTimer = {0};
BasicTimer_Handler_t handlerMuestreo = {0};
BasicTimer_Handler_t handlertiempodisplay = {0};

GPIO_Handler_t handlerPinPwmChannelX = {0};
GPIO_Handler_t handlerPinPwmChannelY = {0};
GPIO_Handler_t handlerPinPwmChannelZ = {0};
PWM_Handler_t handlerSignalPwmX = {0};
PWM_Handler_t handlerSignalPwmY = {0};
PWM_Handler_t handlerSignalPwmZ = {0};

uint16_t duttyValueX = 20000;
uint16_t duttyValueY = 20000;
uint16_t duttyValueZ = 20000;

uint8_t AccelX_low =0;
uint8_t AccelX_high=0;
int16_t AccelX=0;
uint8_t AccelY_low=0 ;
uint8_t AccelY_high=0;
int16_t AccelY=0;
uint8_t AccelZ_low=0;
uint8_t AccelZ_high=0;
int16_t AccelZ=0;

//uint8_t usart2DataReceived=0;
uint8_t rxData = 0;

char bufferData[64]= "Accel ADXL-345";

uint8_t systemTicks = 0;
uint8_t systemTicksStart = 0;
uint8_t systemTicksEnd = 0;

/*Configuracion para el I2C */
GPIO_Handler_t handlerI2CAcelSDA = {0};
GPIO_Handler_t handlerI2CAcelSCL = {0};
GPIO_Handler_t handlerI2CLCDSDA = {0};
GPIO_Handler_t handlerI2CLCDSCL = {0};
I2C_Handler_t handlerAccelerometer = {0};
I2C_Handler_t handlerLCD = {0};
uint8_t i2cBuffer ={0};
uint32_t interrupcion=0;
uint8_t bandera=0;
int cont2i = 0;
uint8_t cont2e;
uint8_t cont2ee;

uint8_t refreshdisplay = 0;
int16_t coordinates[3];//Array donde se van a actualizar los datos cada mS de las coordenadas
int16_t coordinatex[2000];//Array donde se van a guardar los datos para x
int16_t coordinatey[2000];//Array donde se van a guardar los datos para y
int16_t coordinatez[2000];//Array donde se van a guardar los datos para z
uint8_t cont2eimprimir;
uint8_t coordinatesbool = 0; //V. Auxiliar que es una flag para calcular las coordenadas
float datosAccel[3][2000];
char bufferx[64] = {0};
char buffery[64] = {0};
char bufferz[64] = {0};
char buffersum[64] = {0};

// Definicion de las cabeceras de las funciones
void initSystem(void);
void getCoordinates(void);
void PwmSignals(void);

int main (void){
	// Activamos el coprocesador matematico FPU
	SCB->CPACR |= (0xF << 20);

	//Inicio del sistema
	initSystem();
	//Frecuencia del micro a 80MHz
	configPLL(PLL_80);
	// Se configura a 80MHz
	i2c_writeSingleRegister(&handlerAccelerometer, PWR_MGMT_1, 0x00);
	config_SysTick_ms(PLL_80_CLOCK_CONFIGURED);
	clearScreenLCD(&handlerLCD);
	init_LCD(&handlerLCD);
	delay_10(); //EN EL DRIVER DE LA LCD
	clearLCD(&handlerLCD);
	delay_10();
	moveCursor_inLCD(&handlerLCD, 0, 1);
	sendSTR_toLCD(&handlerLCD, "Ax[m/s^2]=");
	moveCursor_inLCD(&handlerLCD, 1, 1);
	sendSTR_toLCD(&handlerLCD, "Ay[m/s^2]=");
	moveCursor_inLCD(&handlerLCD, 2, 1);
	sendSTR_toLCD(&handlerLCD, "Az[m/s^2]=");
	moveCursor_inLCD(&handlerLCD, 3, 0);
	sendSTR_toLCD(&handlerLCD, "Sensibilidad=0.0006");


	while(1){
		if(refreshdisplay>4){
			sprintf(bufferx,"%.4f",coordinates[0]*2*9.78/32767);
			sprintf(buffery,"%.4f",coordinates[1]*2*9.78/32767);
			sprintf(bufferz,"%.4f",coordinates[2]*2*9.78/32767);

			moveCursor_inLCD(&handlerLCD, 0, 11);
			sendSTR_toLCD(&handlerLCD, bufferx);
			moveCursor_inLCD(&handlerLCD, 1, 11);
			sendSTR_toLCD(&handlerLCD, buffery);
			moveCursor_inLCD(&handlerLCD, 2, 11);
			sendSTR_toLCD(&handlerLCD, bufferz);

			refreshdisplay = 0;
		PwmSignals();
		}
        if(coordinatesbool){//cada mS va a calcular las aceleraciones
            getCoordinates();
        }
        if(cont2eimprimir){
            for(int contador = 0;contador<2000;contador++){
                sprintf(bufferData,"%f;%f;%f dato #%d \n",coordinatex[contador]*2*9.78/32767,coordinatey[contador]*2*9.78/32767,coordinatez[contador]*2*9.78/32767,contador);
                writeMsg(&handlerCommTerminal,bufferData);
            }
            cont2eimprimir = 0;
        }


		//Hacemos un "eco" con el valor que nos llega por el serial
		if(rxData != '\0'){
			// WHOAMI
			if(rxData == 'w'){
				sprintf(bufferData, "\n");
				writeMsg(&handlerCommTerminal, bufferData);
				i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, WHO_AM_I);
				sprintf(bufferData, "Who am i 0x%x \n", (unsigned int) i2cBuffer);
				writeMsg(&handlerCommTerminal, bufferData);
				rxData = '\0';
			}
			// Power control status
			else if (rxData == 'p'){
				sprintf(bufferData, "PWR_MGMT_1 state \n");
				writeMsg(&handlerCommTerminal, bufferData);
				i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, PWR_MGMT_1);
				sprintf(bufferData, "status = 0x%x \n", (unsigned int) i2cBuffer);
				writeMsg(&handlerCommTerminal, bufferData);
				rxData = '\0';
			}
			// RESET
			else if (rxData == 'r'){
				sprintf(bufferData, "PWR_MGMT_1 reset \n");
				writeMsg(&handlerCommTerminal, bufferData);
				i2c_writeSingleRegister(&handlerAccelerometer, PWR_MGMT_1 , 0x2D);
				rxData = '\0';
			}
			// single accelerations
	        else if(rxData == 'x'){
	            sprintf(bufferData, "Axis X data (r)\n");
	            writeMsg(&handlerCommTerminal, bufferData);

	            sprintf(bufferData, "AccelX = %.4f \n", (float) coordinates[0]*2*9.78/32767);
	            writeMsg(&handlerCommTerminal, bufferData);
	            rxData = '\0';
	            }
	        else if(rxData == 'y'){
	            sprintf(bufferData, "Axis Y data (r)\n");
	            writeMsg(&handlerCommTerminal, bufferData);

	            sprintf(bufferData, "AccelY = %.4f \n", (float) coordinates[1]*2*9.78/32767);
	            writeMsg(&handlerCommTerminal, bufferData);
	            rxData = '\0';
	            }
	        else if(rxData == 'z'){
	            sprintf(bufferData, "Axis Z data (r)\n");
	            writeMsg(&handlerCommTerminal, bufferData);

	            sprintf(bufferData, "AccelZ = %.4f \n", (float) coordinates[2]*2*9.78/32767);
	            writeMsg(&handlerCommTerminal, bufferData);
	            rxData = '\0';
	            }
	        else if(rxData == 'a'){
	            cont2e = 1;
	            rxData = '\0';
			}
			else{
				writeChar(&handlerCommTerminal, rxData);
				rxData = '\0';
			}
		}
	}
	return(0);
}


void initSystem(void){


	/* Configuracion del LED de estado */
	handlerBlinkyPin.pGPIOx                             = GPIOA;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinNumber      = PIN_5;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_OUT;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinOPType      = GPIO_OTYPE_PUSHPULL;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_OSPEED_FAST;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;


	GPIO_Config(&handlerBlinkyPin);

	/* Configuracion del TIM2 para cada 250ms */
	handlerBlinkyTimer.ptrTIMx                               = TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode                 = BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed                = BTIMER_SPEED_80Mhz_100us;
	handlerBlinkyTimer.TIMx_Config.TIMx_period               = 2500;
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable      = 1;


	BasicTimer_Config(&handlerBlinkyTimer);

	/* Configuracion del TIM4 para 1ms */
	handlerMuestreo.ptrTIMx                               = TIM4;
	handlerMuestreo.TIMx_Config.TIMx_mode                 = BTIMER_MODE_UP;
	handlerMuestreo.TIMx_Config.TIMx_speed                = BTIMER_SPEED_80Mhz_100us;
	handlerMuestreo.TIMx_Config.TIMx_period               = 10;
	handlerMuestreo.TIMx_Config.TIMx_interruptEnable      = 1;


	BasicTimer_Config(&handlerMuestreo);

	//I2C
		//Acelerometro
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

		//LCD
	handlerI2CLCDSCL.pGPIOx                                      = GPIOA;
	handlerI2CLCDSCL.GPIO_PinConfig.GPIO_PinNumber               = PIN_8;
	handlerI2CLCDSCL.GPIO_PinConfig.GPIO_PinMode                 = GPIO_MODE_ALTFN;
	handlerI2CLCDSCL.GPIO_PinConfig.GPIO_PinOPType               = GPIO_OTYPE_OPENDRAIN;
	handlerI2CLCDSCL.GPIO_PinConfig.GPIO_PinSpeed                = GPIO_OSPEED_FAST;
	handlerI2CLCDSCL.GPIO_PinConfig.GPIO_PinPuPdControl          = GPIO_PUPDR_PULLUP;
	handlerI2CLCDSCL.GPIO_PinConfig.GPIO_PinAltFunMode           = AF4;
	GPIO_Config(&handlerI2CLCDSCL);

	handlerI2CLCDSDA.pGPIOx                                      = GPIOC;
	handlerI2CLCDSDA.GPIO_PinConfig.GPIO_PinNumber               = PIN_9;
	handlerI2CLCDSDA.GPIO_PinConfig.GPIO_PinMode                 = GPIO_MODE_ALTFN;
	handlerI2CLCDSDA.GPIO_PinConfig.GPIO_PinOPType               = GPIO_OTYPE_OPENDRAIN;
	handlerI2CLCDSDA.GPIO_PinConfig.GPIO_PinSpeed                = GPIO_OSPEED_FAST;
	handlerI2CLCDSDA.GPIO_PinConfig.GPIO_PinPuPdControl          = GPIO_PUPDR_PULLUP;
	handlerI2CLCDSDA.GPIO_PinConfig.GPIO_PinAltFunMode           = AF4;
	GPIO_Config(&handlerI2CLCDSDA);

	//USART
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

	handlerCommTerminal.ptrUSARTx                       = USART1;
	handlerCommTerminal.USART_Config.USART_baudrate     = USART_BAUDRATE_115200;
	handlerCommTerminal.USART_Config.USART_datasize     = USART_DATASIZE_8BIT;
	handlerCommTerminal.USART_Config.USART_parity       = USART_PARITY_NONE;
	handlerCommTerminal.USART_Config.USART_stopbits     = USART_STOPBIT_1;
	handlerCommTerminal.USART_Config.USART_mode         = USART_MODE_RXTX;
	handlerCommTerminal.USART_Config.USART_enableIntRX  = USART_RX_INTERRUP_ENABLE;
	handlerCommTerminal.USART_Config.USART_enableIntTX  = USART_TX_INTERRUP_DISABLE;
	handlerCommTerminal.USART_Config.USART_frequency    = 80;

	USART_Config(&handlerCommTerminal);

	//handlerI2C
	handlerAccelerometer.ptrI2Cx                            = I2C1;
	handlerAccelerometer.modeI2C                            = I2C_MODE_FM;
	handlerAccelerometer.slaveAddress                       = ACCEL_ADRESS;
	handlerAccelerometer.mainClock							= MAIN_CLOCK_80_MHz_FOR_I2C;
	handlerAccelerometer.maxI2C_FM							= I2C_MAX_RISE_TIME_FM_80MHz;
	handlerAccelerometer.modeI2C_FM							= I2C_MODE_FM_SPEED_400KHz_80MHz;

	i2c_config(&handlerAccelerometer);

	//handler LCD
	handlerLCD.ptrI2Cx                            = I2C3;
	handlerLCD.modeI2C                            = I2C_MODE_FM;
	handlerLCD.slaveAddress                       = LCD_ADDRESS	;
	handlerLCD.mainClock						  = MAIN_CLOCK_80_MHz_FOR_I2C;
	handlerLCD.maxI2C_FM						  = I2C_MAX_RISE_TIME_FM_80MHz;
	handlerLCD.modeI2C_FM						  = I2C_MODE_FM_SPEED_400KHz_80MHz;

	i2c_config(&handlerLCD);

	// ---------------------------- CONFIGURACION DEL PWM  ----------------------------------------
	handlerPinPwmChannelX.pGPIOx                                = GPIOA;
	handlerPinPwmChannelX.GPIO_PinConfig.GPIO_PinNumber         = PIN_6;
	handlerPinPwmChannelX.GPIO_PinConfig.GPIO_PinMode           = GPIO_MODE_ALTFN;
	handlerPinPwmChannelX.GPIO_PinConfig.GPIO_PinOPType         = GPIO_OTYPE_PUSHPULL;
	handlerPinPwmChannelX.GPIO_PinConfig.GPIO_PinSpeed          = GPIO_OSPEED_FAST;
	handlerPinPwmChannelX.GPIO_PinConfig.GPIO_PinPuPdControl    = GPIO_PUPDR_NOTHING;
	handlerPinPwmChannelX.GPIO_PinConfig.GPIO_PinAltFunMode     = AF2;
	/* Cargamos la configuracion en los registros del MCU */
	GPIO_Config(&handlerPinPwmChannelX);

	handlerPinPwmChannelY.pGPIOx                                = GPIOB;
	handlerPinPwmChannelY.GPIO_PinConfig.GPIO_PinNumber         = PIN_5;
	handlerPinPwmChannelY.GPIO_PinConfig.GPIO_PinMode           = GPIO_MODE_ALTFN;
	handlerPinPwmChannelY.GPIO_PinConfig.GPIO_PinOPType         = GPIO_OTYPE_PUSHPULL;
	handlerPinPwmChannelY.GPIO_PinConfig.GPIO_PinSpeed          = GPIO_OSPEED_FAST;
	handlerPinPwmChannelY.GPIO_PinConfig.GPIO_PinPuPdControl    = GPIO_PUPDR_NOTHING;
	handlerPinPwmChannelY.GPIO_PinConfig.GPIO_PinAltFunMode     = AF2;
	/* Cargamos la configuracion en los registros del MCU */
	GPIO_Config(&handlerPinPwmChannelY);

	handlerPinPwmChannelZ.pGPIOx                                = GPIOB;
	handlerPinPwmChannelZ.GPIO_PinConfig.GPIO_PinNumber         = PIN_0;
	handlerPinPwmChannelZ.GPIO_PinConfig.GPIO_PinMode           = GPIO_MODE_ALTFN;
	handlerPinPwmChannelZ.GPIO_PinConfig.GPIO_PinOPType         = GPIO_OTYPE_PUSHPULL;
	handlerPinPwmChannelZ.GPIO_PinConfig.GPIO_PinSpeed          = GPIO_OSPEED_FAST;
	handlerPinPwmChannelZ.GPIO_PinConfig.GPIO_PinPuPdControl    = GPIO_PUPDR_NOTHING;
	handlerPinPwmChannelZ.GPIO_PinConfig.GPIO_PinAltFunMode     = AF2;
	/* Cargamos la configuracion en los registros del MCU */
	GPIO_Config(&handlerPinPwmChannelZ);

	/* Configuracion del TIM3 para que genere la signal PWM*/
	handlerSignalPwmX.ptrTIMx                = TIM3;
	handlerSignalPwmX.config.channel         = PWM_CHANNEL_1;
	handlerSignalPwmX.config.duttyCicle      = duttyValueX;
	handlerSignalPwmX.config.periodo         = 20000;
	handlerSignalPwmX.config.prescaler       = 80;
	/* Cargamos la configuracion en los registros del MCU */
	pwm_Config(&handlerSignalPwmX);

	/* Configuracion del TIM3 para que genere la signal PWM*/
	handlerSignalPwmY.ptrTIMx                = TIM3;
	handlerSignalPwmY.config.channel         = PWM_CHANNEL_2;
	handlerSignalPwmY.config.duttyCicle      = duttyValueY;
	handlerSignalPwmY.config.periodo         = 20000;
	handlerSignalPwmY.config.prescaler       = 80;
	/* Cargamos la configuracion en los registros del MCU */
	pwm_Config(&handlerSignalPwmY);

	/* Configuracion del TIM3 para que genere la signal PWM*/
	handlerSignalPwmZ.ptrTIMx                = TIM3;
	handlerSignalPwmZ.config.channel         = PWM_CHANNEL_3;
	handlerSignalPwmZ.config.duttyCicle      = duttyValueZ;
	handlerSignalPwmZ.config.periodo         = 20000;
	handlerSignalPwmZ.config.prescaler       = 80;
	/* Cargamos la configuracion en los registros del MCU */
	pwm_Config(&handlerSignalPwmZ);

	enableOutput(&handlerSignalPwmX);
	enableOutput(&handlerSignalPwmY);
	enableOutput(&handlerSignalPwmZ);

	startPwmSignal(&handlerSignalPwmX);
	startPwmSignal(&handlerSignalPwmY);
	startPwmSignal(&handlerSignalPwmZ);

}

/* Funcion para el muestreo inicial por 2 segundos */
void getCoordinates(void){
    uint8_t AccelX_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_L);
    uint8_t AccelX_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_H);
    coordinates[0] = AccelX_high << 8 | AccelX_low;
    uint8_t AccelY_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_L);
    uint8_t AccelY_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_H);
    int16_t AccelY = AccelY_high << 8 | AccelY_low;
    coordinates[1]=AccelY;
    uint8_t AccelZ_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_L);
    uint8_t AccelZ_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_H);
    int16_t AccelZ = AccelZ_high << 8 | AccelZ_low;
    coordinates[2]=AccelZ;
    if(cont2e){
        if(cont2i==0){
        }else{
            coordinatex[cont2i-1] = coordinates[0];
            coordinatey[cont2i-1] = coordinates[1];
            coordinatez[cont2i-1] = coordinates[2];
            }
    }
    coordinatesbool = 0;
}


void PwmSignals(void){
	duttyValueX = (int)coordinates[0]*700+10000;
	duttyValueY = (int)coordinates[1]*700+10000;
	duttyValueZ = (int)coordinates[2]*700+10000;
	updateDuttyCycle(&handlerSignalPwmX, duttyValueX);
	updateDuttyCycle(&handlerSignalPwmY, duttyValueY);
	updateDuttyCycle(&handlerSignalPwmZ, duttyValueZ);
}


/* Callbacks de las interrupciones */
void usart1Rx_Callback(void){
	rxData = getRxData();
}

/* Callbacks de los Timers */
void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerBlinkyPin);
	refreshdisplay++;
}

void BasicTimer4_Callback(void){
    coordinatesbool = 1;//Se activa la flag cada mS de que se quiere leer el acelerometro
    if(cont2i == 2001){//Si el contador de iteraciones de 2 segundos se pasa de las 2000 iteraciones
        cont2e = 0;//baje la bandera de contar 2seg
        cont2i = 0;//Reinicie las iteraciones de contar 2 seg
        cont2eimprimir = 1;//Active el flag que le permite imprimir toda la lista.
    }
    if(cont2e){//Si el flag de contar 2 seg está activado
        cont2i++;//Aumente en 1 el numero de iteraciones de los 2 segundos
    }
}

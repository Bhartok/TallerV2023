/*
 * stm32f411xx_hal.h
 *
 *  Created on: Mar 2, 2023
 *      Author: bhartok
 */

 /*
 * Contiene la informacion mas basica del micro, valores de reloj principal,
 * Distribución basica de la memoria
 * Posiciones de memoria de los periféricos
 * Demás registros de los periféricos
 * Definiciones de las constantes más basicas
 */

#ifndef STM32F411XX_HAL_H_
#define STM32F411XX_HAL_H_

#include <stdint.h>
#include <stddef.h>

#define HSI_CLOCK_SPEED	16000000 //Valor de la señal reloj High Speed Internal
#define HSE_CLOCK_SPEED	4000000 /</Valor de la señal reloj High Speed External

#define NOP()	asm("NOP")//Preguntar nerio
#define __weak	__attribute__((weak))

//Direcciones de memoria RAM,FLASH

#define FLASH_BASE_ADDR	0x08000000U //512KB
#define SRAM_BASE_ADDR	0x20000000U //RAM128KB

//Dirección de memoria base para cada uno de los perifericos

#define APB1_BASE_ADDR	0x40000000U
#define APB2_BASE_ADDR	0x40010000U
#define AHB1_BASE_ADDR	0x40020000U
#define AHB2_BASE_ADDR	0x50000000U

//Posiciones memoria perifericos AHB2

#define USB_OTG_FS_BASE_ADDR	(AHB2_BASE_ADDR + 0x0000U)

//Posiciones memoria perifericos AHB1

#define RCC_BASE_ADDR	(AHB1_BASE_ADDR + 0x3800U)
#define GPIOH_BASE_ADDR	(AHB1_BASE_ADDR + 0x1C00U)
#define GPIOE_BASE_ADDR	(AHB1_BASE_ADDR + 0x1000U)
#define GPIOD_BASE_ADDR	(AHB1_BASE_ADDR + 0x0C00U)
#define GPIOC_BASE_ADDR	(AHB1_BASE_ADDR + 0x0800U)
#define GPIOB_BASE_ADDR	(AHB1_BASE_ADDR + 0x0400U)
#define GPIOA_BASE_ADDR	(AHB1_BASE_ADDR + 0x0000U)

//Macros Genéricos

#define ENABLE			1
#define DISABLE			0
#define SET				ENABLE
#define CLEAR			DISABLE
#define RESET			DISABLE
#define GPIO_PIN_SET	SET
#define GPIO_PIN_RESET	RESET
#define FLAG_SET		SET
#define FLAG_RESET		RESET
#define I2C_WRITE		0
#define I2C_READ		1

//DESCRIPCION DE ELEMENTOS QUE COMPONEN EL PERIFERICO
//Se define la estructura de datos que representa cada uno de los registros que componen el periferico RCC

typedef struct
{
	volatile uint32_t CR; //CLOCK CONTROL REGISTER ADDR_OFFSET:	0x00
	volatile uint32_t PLLCFGR; //PLL CONFIGURATION REGISTER ADDR_OFFSET:0x04
	volatile uint32_t CFGR; //CLOCK CONFIGURATION REGISTER ADDR_OFFSET: 0X08
	volatile uint32_t CIR; //CLOCK INTERRUPT REGISTER ADDR_OFFSET:0X0C
	volatile uint32_t AHB1RSTR; //AHB1 PERIPHERAL RESET REGISTER ADDR_OFFSET:0X10
	volatile uint32_t AHB2RSTR; //AHB2 PERIPHERAL RESET REGISTER ADDR_OFFSET:0X14
	volatile uint32_t reserved0; // reserved ADDR_OFFSET:0x18
	volatile uint32_t reserved1; // reserved ADDR_OFFSET:0x1C
	volatile uint32_t APB1RSTR; //APB1 PERIPHERAL RESET REGISTER ADDR_OFFSET:0X20
	volatile uint32_t APB2RSTR; //APB2 PERIPHERAL RESET REGISTER ADDR_OFFSET:0X24
	volatile uint32_t reserved2; //reserved ADDR_OFFSET:0x28
	volatile uint32_t reserved3; // ADDR_OFFSET:0x2C
	volatile uint32_t AHB1ENR; //AHB1 PERIPHERAL CLOCK ENABLE REGISTER ADDR_OFFSET:0X30
	volatile uint32_t AHB2ENR; //AHB2 PERIPHERAL CLOCK ENABLE REGISTER ADDR_OFFSET:0X34
	volatile uint32_t reserved4; //reserved ADDR_OFFSET:0x38
	volatile uint32_t reserved5; //reserved ADDR_OFFSET:0x3C
	volatile uint32_t APB1ENR; //APB1 PERIPHERAL CLOCK ENABLE REGISTER ADDR_OFFSET:0X40
	volatile uint32_t APB2ENR; //APB2 PERIPHERAL CLOCK ENABLE REGISTER ADDR_OFFSET:0X44
	volatile uint32_t reserved6; //reserved ADDR_OFFSET:0x48
	volatile uint32_t reserved7; //reserved ADDR_OFFSET:0x4C
	volatile uint32_t AHB1LPENR; //AHB1 CLOCK ENABLE LOW POWER REGISTER ADDR_OFFSET:0X50
	volatile uint32_t AHB2LPENR; //AHB2 CLOCK ENABLE LOW POWER REGISTER ADDR_OFFSET:0X54
	volatile uint32_t reserved8; //reserved ADDR_OFFSET:0x58
	volatile uint32_t reserved9; //reserved ADDR_OFFSET:0x5C
	volatile uint32_t APB1LPENR; //APB1 CLOCK ENABLE LOW POWER REGISTER ADDR_OFFSET:0X60
	volatile uint32_t APB2LPENR; //APB2 CLOCK ENABLE LOW POWER REGISTER ADDR_OFFSET:0X64
	volatile uint32_t reserved10; //reserved ADDR_OFFSET:0x68
	volatile uint32_t reserved11; //reserved ADDR_OFFSET:0x6C
	volatile uint32_t BDCR; //BACKUP DOMAIN CONTROL REGISTER ADDR_OFFSET:0X70
	volatile uint32_t CSR; //CLOCK CONTROL & STATUS REGISTER ADDR_OFFSET:0X74
	volatile uint32_t reserved12; //reserved ADDR_OFFSET:0x78
	volatile uint32_t reserved13; //reserved ADDR_OFFSET:0x7C
	volatile uint32_t SSCGR; //SPREAD SPECTRUM CLOCK GENETARION REG ADDR_OFFSET:0X80
	volatile uint32_t PLLI2SCFGR; //PLLI2S CONFIGURATION REGISTER ADDR_OFFSET:0X84
	volatile uint32_t reserved14; //reserved ADDR_OFFSET:0x88
	volatile uint32_t DCKCFGR; //DEDICATED CLOCKS CONFIGURATION REG ADDR_OFFSET:0x8C
	} RCC_RegDef_t;

//Puntero a RCC_RegDef_t

#define RCC	((RCC_RegDef_t *) RCC_BASE_ADDR)

//Descripcion bit a bit de cada uno de los registros del que componen al periférico RCC

#define RCC_AHB1ENR_GPIOA_EN	0
#define RCC_AHB1ENR_GPIOB_EN	1
#define RCC_AHB1ENR_GPIOC_EN	2
#define RCC_AHB1ENR_GPIOD_EN	3
#define RCC_AHB1ENR_GPIOE_EN	4
#define RCC_AHB1ENR_GPIOH_EN	7
#define RCC_AHB1ENR_CRCEN		12
#define RCC_AHB1ENR_DMA1_EN		21
#define RCC_AHB1ENR_DMA2_EN		22

//DESCRIPCION ELEMENTOS QUE COMPONEN EL PERIFERICO GPIOx

typedef struct
{
	volatile uint32_t MODER; //PORT MODE REGISTER	ADDR_OFFSET:0X00
	volatile uint32_t OTYPER; //PORT OUTPUT TYPE REGISTER	ADDR_OFFSET:0X04
	volatile uint32_t OSPEEDR; //PORT OUTPUT SPEED REGISTER	ADDR_OFFSET:0X08
	volatile uint32_t PUPDR; //PORT PULL-UP/DOWN REGISTER	ADDR_OFFSET:0X0C
	volatile uint32_t IDR; //PORT INPUT DATA REGISTER	ADDR_OFFSET:0X10
	volatile uint32_t ODR; //PORT OUTPUT  DATA REGISTER	ADDR_OFFSET:0X14
	volatile uint32_t BSRR; //PORT BIT SET/RESET REGISTER	ADDR_OFFSET:0X18
	volatile uint32_t LCKR; //PORT CONFIGURATION LOCK REGISTER	ADDR_OFFSET:0X1C
	volatile uint32_t AFRL; //ALTERNATE FUNCTION LOW REGISTER	ADDR_OFFSET:0X20
	volatile uint32_t AFRH; //ALTERNATE FUNCTION HIGH REGISTER	ADDR_OFFSET:0X24
} GPIOx_RegDef_t;
//Se crea puntero y se debe hacer que cada GPIOx quede ubicado exactamente sobre la posicion de memoria correcta

#define GPIOA	((GPIOx_RegDef_t *) GPIOA_BASE_ADDR)
#define GPIOB	((GPIOx_RegDef_t *) GPIOB_BASE_ADDR)
#define GPIOC	((GPIOx_RegDef_t *) GPIOC_BASE_ADDR)
#define GPIOD	((GPIOx_RegDef_t *) GPIOD_BASE_ADDR)
#define GPIOE	((GPIOx_RegDef_t *) GPIOE_BASE_ADDR)
#define GPIOH	((GPIOx_RegDef_t *) GPIOH_BASE_ADDR)

//VALORES ESTANDAR PARA LAS CONFIGURACIONES
//MODER DOS BIT POR CADA PIN
#define GPIO_MODE_IN		0
#define GPIO_MODE_OUT		1
#define GPIO_MODE_ALTFN		2
#define GPIO_MODE_ANALOG	3
//TYPER UN BIT POR PIN
#define GPIO_OTYPE_PUSHPULL		0
#define GPIO_OTYPE_OPENDRAIN	1
//OSPEEDR DOS BIT POR PIN
#define GPIO_OSPEED_LOW		0
#define GPIO_OSPEED_MEDIUM	1
#define GPIO_OSPEED_FAST	2
#define GPIO_OSPEED_HIGH	3
//PUPDR DOS BIT POR PIN
#define GPIO_PUPDR_NOTHING	0
#define GPIO_PUPDR_PULLUP	1
#define GPIO_PUPDR_PULLDOWN	2
#define GPIO_PUPDR_RESERVED	3

//Definición de los nombres de los pines

#define PIN_0	0
#define PIN_1	1
#define PIN_2	2
#define PIN_3	3
#define PIN_4	4
#define PIN_5	5
#define PIN_6	6
#define PIN_7	7
#define PIN_8	8
#define PIN_9	9
#define PIN_10	10
#define PIN_11	11
#define PIN_12	12
#define PIN_13	13
#define PIN_14	14
#define PIN_15	15

//Definición de las funciones alternativas

#define AF0	0b0000
#define AF1	0b0001
#define AF2	0b0010
#define AF3	0b0011
#define AF4	0b0100
#define AF5	0b0101
#define AF6	0b0110
#define AF7	0b0111
#define AF8	0b1000
#define AF9	0b1001
#define AF10	0b1010
#define AF11	0b1011
#define AF12	0b1100
#define AF13	0b1101
#define AF14	0b1110
#define AF15	0b1111

#endif /* STM32F411XX_HAL_H_ */



















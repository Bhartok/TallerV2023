#ifndef PLLDRIVER_H_
#define PLLDRIVER_H_

#include "stm32f4xx.h"


void configPLL(uint16_t frequency);
int getConfigPLL(void);

#endif /* PLLDRIVER_H_ */

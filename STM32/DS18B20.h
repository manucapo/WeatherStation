#ifndef DS18B20_H_
#define DS18B20_H_

#include <stdint.h>
#include "stm32f3xx.h"
#include "onewire.h"

int DS18ResetPulse();
void DS18SkipRom();
void DS18ConvertTemp();
double DS18ReadTemperature();
uint16_t DS18ReadTemp();

#endif /* DS18B20_H_ */

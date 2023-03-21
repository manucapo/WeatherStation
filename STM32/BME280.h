#ifndef BME280_H_
#define BME280_H_

#include <stdint.h>
#include "stm32f3xx.h"

// BME280 COMPENSATION
void BME280TempCompensation(int32_t adc_T);
int32_t BME280CalculatePressure(int32_t adc_P);
uint32_t BME280CalculateHumidity(int32_t adc_h);

extern int32_t t_fine;
extern int32_t rawPressure;
extern int32_t rawHumidity;
extern int32_t rawTemperature;


void BME280Reset();
void BME280ReadCompensation();
void BME280Config();
void BME280ReadRegisters();

void  BME280TempCompensation(int32_t adc_T);
int32_t BME280CalculatePressure(int32_t adc_P);
uint32_t BME280CalculateHumidity(int32_t adc_H);

#endif /* BME280_H_ */

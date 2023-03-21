#include "BME280.h"

uint16_t dig_T1 = 0;
int16_t dig_T2 = 0;
int16_t dig_T3 = 0;
uint16_t dig_P1 = 0;
int16_t dig_P2 = 0;
int16_t dig_P3 = 0;
int16_t dig_P4 = 0;
int16_t dig_P5 = 0;
int16_t dig_P6 = 0;
int16_t dig_P7 = 0;
int16_t dig_P8 = 0;
int16_t dig_P9 = 0;

uint8_t dig_H1 = 0;
int16_t dig_H2 = 0;
uint8_t dig_H3 = 0;
int16_t dig_H4 = 0;
int16_t dig_H5 = 0;
int8_t dig_H6 = 0;

int32_t t_fine = 0;
int32_t rawPressure = 0;
int32_t rawHumidity = 0;
int32_t rawTemperature = 0;


void BME280Reset(){
	I2C1->CR1 |= I2C_CR1_PE;   											// ENABLE I2C PERIPHERAL
	I2C1->CR2 &= ~I2C_CR2_NBYTES_Msk;
	I2C1->CR2 |= 0x0000000002<<I2C_CR2_NBYTES_Pos;
	I2C1->CR2 &= ~I2C_CR2_RD_WRN;						// SET WRITE MODE
	I2C1->CR2 |= I2C_CR2_START;

	I2C1->TXDR = 0x000000e0;
	while((I2C1->ISR & I2C_ISR_TXE) == 0);  		// WAIT FOR TRANSMISSION TO FINISH
	I2C1->TXDR = 0x000000b6;
	while((I2C1->ISR & I2C_ISR_TXE) == 0);  		// WAIT FOR TRANSMISSION TO FINISH

	I2C1->CR2 |= I2C_CR2_STOP;
	I2C1->CR1 &= ~I2C_CR1_PE;   											// DISABLE I2C PERIPHERAL
}

void BME280Config(){
	I2C1->CR1 |= I2C_CR1_PE;   											// ENABLE I2C PERIPHERAL

	I2C1->CR2 &= ~I2C_CR2_RD_WRN;						// SET WRITE MODE
	I2C1->CR2 &= ~I2C_CR2_NBYTES_Msk;
	I2C1->CR2 |= 0x0000000004<<I2C_CR2_NBYTES_Pos;
	I2C1->CR2 |= I2C_CR2_START;
	while((I2C1->ISR & I2C_ISR_TXIS) == 0);

	// SET HUMIDTY OVERSAMPLING TO 1x
	I2C1->TXDR = 0x000000f2;
	while((I2C1->ISR & I2C_ISR_TXIS) == 0);  		// WAIT FOR TRANSMISSION TO FINISH
	I2C1->TXDR = 0x00000001;
	while((I2C1->ISR & I2C_ISR_TXIS) == 0);  		// WAIT FOR TRANSMISSION TO FINISH

	// SET FORCED MODE AND TEMP/PRES OVERSAMPLING TO 1x
	I2C1->TXDR = 0x000000f4;
	while((I2C1->ISR & I2C_ISR_TXIS) == 0);  		// WAIT FOR TRANSMISSION TO FINISH
	I2C1->TXDR = 0x00000027;
	while((I2C1->ISR & I2C_ISR_TC) == 0);  		// WAIT FOR TRANSMISSION TO FINISH




	I2C1->CR2 |= I2C_CR2_STOP;
	I2C1->CR1 &= ~I2C_CR1_PE;   											// DISABLE I2C PERIPHERAL
}

void BME280ReadCompensation(){
	volatile uint16_t throwAway = 0;
	I2C1->CR1 |= I2C_CR1_PE;   											// ENABLE I2C PERIPHERAL


	I2C1->CR2 &= ~I2C_CR2_RD_WRN;						// SET WRITE MODE
	I2C1->CR2 |= I2C_CR2_START;

	I2C1->CR2 &= ~I2C_CR2_NBYTES_Msk;
	I2C1->CR2 |= 0x0000000001<<I2C_CR2_NBYTES_Pos;
	I2C1->TXDR = 0x00000088;
	while((I2C1->ISR & I2C_ISR_TC) == 0);  		// WAIT FOR TRANSMISSION TO FINISH

	I2C1->CR2 &= ~I2C_CR2_NBYTES_Msk;
	I2C1->CR2 |= 0x000000001A<<I2C_CR2_NBYTES_Pos;    // READ 26 BYTES
	I2C1->CR2 |= I2C_CR2_RD_WRN;						// SET READ MODE
	I2C1->CR2 |= I2C_CR2_START;

	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	dig_T1 += I2C1->RXDR;
	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	dig_T1 += I2C1->RXDR<<8;

	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	dig_T2 += I2C1->RXDR;
	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	dig_T2 += I2C1->RXDR<<8;

	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	dig_T3 += I2C1->RXDR;
	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	dig_T3 += I2C1->RXDR<<8;

	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	dig_P1 += I2C1->RXDR;
	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	dig_P1 += I2C1->RXDR<<8;

	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	dig_P2 += I2C1->RXDR;
	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	dig_P2 += I2C1->RXDR<<8;

	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	dig_P3 += I2C1->RXDR;
	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	dig_P3 += I2C1->RXDR<<8;

	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	dig_P4 += I2C1->RXDR;
	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	dig_P4 += I2C1->RXDR<<8;

	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	dig_P5 += I2C1->RXDR;
	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	dig_P5 += I2C1->RXDR<<8;

	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	dig_P6 += I2C1->RXDR;
	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	dig_P6 += I2C1->RXDR<<8;

	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	dig_P7 += I2C1->RXDR;
	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	dig_P7 += I2C1->RXDR<<8;

	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	dig_P8 += I2C1->RXDR;
	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	dig_P8 += I2C1->RXDR<<8;

	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	dig_P9 += I2C1->RXDR;
	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	dig_P9 += I2C1->RXDR<<8;


	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	throwAway = I2C1->RXDR<<8;
	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	dig_H1 += I2C1->RXDR;
	while((I2C1->ISR & I2C_ISR_TC) == 0);

	I2C1->CR2 |= I2C_CR2_STOP;
	I2C1->CR1 &= ~I2C_CR1_PE;   											// DISABLE I2C PERIPHERAL

	I2C1->CR1 |= I2C_CR1_PE;   											// ENABLE I2C PERIPHERAL

	I2C1->CR2 &= ~I2C_CR2_RD_WRN;						// SET WRITE MODE
	I2C1->CR2 |= I2C_CR2_START;

	I2C1->CR2 &= ~I2C_CR2_NBYTES_Msk;
	I2C1->CR2 |= 0x0000000001<<I2C_CR2_NBYTES_Pos;
	I2C1->TXDR = 0x000000e1;
	while((I2C1->ISR & I2C_ISR_TC) == 0);  		// WAIT FOR TRANSMISSION TO FINISH

	I2C1->CR2 &= ~I2C_CR2_NBYTES_Msk;
	I2C1->CR2 |= 0x0000000008<<I2C_CR2_NBYTES_Pos;    // READ 8 BYTES
	I2C1->CR2 |= I2C_CR2_RD_WRN;						// SET READ MODE
	I2C1->CR2 |= I2C_CR2_START;

	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	dig_H2 += I2C1->RXDR;
	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	dig_H2 += I2C1->RXDR<<8;

	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	dig_H3 += I2C1->RXDR;

	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	dig_H4 += I2C1->RXDR<<4;
	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	dig_H4 += I2C1->RXDR;

	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	dig_H5 += I2C1->RXDR;
	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	dig_H5 += I2C1->RXDR<<4;

	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	dig_H6 += I2C1->RXDR;
	while((I2C1->ISR & I2C_ISR_TC) == 0);

	I2C1->CR2 |= I2C_CR2_STOP;
	I2C1->CR1 &= ~I2C_CR1_PE;   											// DISABLE I2C PERIPHERAL
}

void BME280ReadRegisters(){
	volatile uint16_t throwAway = 0;
	uint16_t pressureMSB = 0;
	uint16_t pressureLSB = 0;
	uint16_t humidityMSB = 0;
	uint16_t humidityLSB = 0;
	uint16_t temperatureMSB = 0;
	uint16_t temperatureLSB = 0;

	I2C1->CR1 |= I2C_CR1_PE;   											// ENABLE I2C PERIPHERAL


	I2C1->CR2 &= ~I2C_CR2_RD_WRN;						// SET WRITE MODE
	I2C1->CR2 |= I2C_CR2_START;

	I2C1->CR2 &= ~I2C_CR2_NBYTES_Msk;
	I2C1->CR2 |= 0x0000000001<<I2C_CR2_NBYTES_Pos;

	I2C1->TXDR = 0x000000f7;
	while((I2C1->ISR & I2C_ISR_TC) == 0);  		// WAIT FOR TRANSMISSION TO FINISH

	I2C1->CR2 &= ~I2C_CR2_NBYTES_Msk;
	I2C1->CR2 |= 0x0000000008<<I2C_CR2_NBYTES_Pos;
	I2C1->CR2 |= I2C_CR2_RD_WRN;						// SET READ MODE
	I2C1->CR2 |= I2C_CR2_START;
	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	pressureMSB = I2C1->RXDR;
	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	pressureLSB = I2C1->RXDR;
	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	throwAway = I2C1->RXDR;

	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	temperatureMSB = I2C1->RXDR;
	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	temperatureLSB = I2C1->RXDR;
	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	throwAway = I2C1->RXDR;

	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	humidityMSB = I2C1->RXDR;
	while((I2C1->ISR & I2C_ISR_RXNE) == 0); 			// WAIT UNTIL READY TO READ
	humidityLSB = I2C1->RXDR;
	while((I2C1->ISR & I2C_ISR_TC) == 0);

	I2C1->CR2 |= I2C_CR2_STOP;
	I2C1->CR1 &= ~I2C_CR1_PE;   											// DISABLE I2C PERIPHERAL

	rawPressure = 0;
	rawPressure += pressureLSB << 4;
	rawPressure += pressureMSB << 12;

	rawHumidity = 0;
	rawHumidity += humidityLSB;
	rawHumidity += humidityMSB << 8;

	rawTemperature = 0;
	rawTemperature += temperatureLSB << 4;
	rawTemperature += temperatureMSB << 12;

}

void  BME280TempCompensation(int32_t adc_T){
	int32_t var1, var2;
	var1 = ((((adc_T>>3) - ((int32_t) dig_T1<<1))) * ((int32_t) dig_T2)) >> 11;
	var2 = (((((adc_T>>4) - ((int32_t) dig_T1)) * ((adc_T >> 4) - ((int32_t) dig_T1))) >> 12 )  * ((int32_t)dig_T3)) >> 14;
	t_fine = (var1 + var2);
}

int32_t BME280CalculatePressure(int32_t adc_P){
	int64_t var1, var2, p;
	var1 = ((int64_t) t_fine) - 128000;
	var2 = var1 * var1 * (int64_t)dig_P6;
	var2 = var2 + (((int64_t)dig_P4) << 35);
	var1 = ((var1 * var1 * (int64_t)dig_P3) >> 8) + ((var1 * (int64_t)dig_P2) << 12);
	var1 = (((((int64_t)1)<<47) + var1 )) * ((int64_t)dig_P1) >> 33;
	if(var1 == 0){
		return 0;    // avoid division by zero
	}
	p = 1048576-adc_P;
	p = (((p<<31)-var2) * 3125) / var1;
	var1 = (((int64_t)dig_P9) * (p>>13) * (p>>13)) >> 25;
	var2 = (((int64_t)dig_P8) * p) >> 19;
	p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7) << 4);
	return (uint32_t)p;
}

uint32_t BME280CalculateHumidity(int32_t adc_H){
	int32_t v_x1_u32r;
	v_x1_u32r = (t_fine - ((int32_t)76800));
	v_x1_u32r = (((((adc_H) << 14) - (((int32_t)dig_H4) << 20) - (((int32_t)dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >>15)
				* (((((((v_x1_u32r * ((int32_t)dig_H6)) >> 10   * (((v_x1_u32r * ((int32_t)dig_H3)) >> 11)  + ((int32_t)32768))) >> 10) + ((int32_t)2097152))
				*  ((int32_t)dig_H2)  + 8192) >> 14));
	v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) *  (v_x1_u32r >> 15)) >> 7) *  ((int32_t) dig_H1)) >> 4));
	v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
	return (uint32_t) (v_x1_u32r >> 12);
}

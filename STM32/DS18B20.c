/*
 * DS18B20.c
 *
 *  Created on: 21 Mar 2023
 *      Author: manol
 */

#include "DS18B20.h"

int DS18ResetPulse(){
	int sucess = 0;

	GPIOA->MODER |= GPIO_MODER_MODER10_0;   // SET GPIOA 10 TO OUTPUT MODE
	GPIOA->MODER &= ~GPIO_MODER_MODER10_1;

	GPIOA->ODR &= ~GPIO_ODR_10;    // transmit reset pulse
	Delay5((uint16_t) 100, TIM15);


	GPIOA->ODR |= GPIO_ODR_10;

	GPIOA->MODER &= ~GPIO_MODER_MODER10_0;   // SET GPIOA 10 TO INPUT MODE
	GPIOA->MODER &= ~GPIO_MODER_MODER10_1;
	Delay5((uint16_t) 12, TIM15);

	if(GPIOA->IDR & GPIO_IDR_10){   // read present pulse. (low = read)
		sucess = 0;
	} else {
		sucess = 1;
	}

	GPIOA->MODER |= GPIO_MODER_MODER10_0;   // SET GPIOA 10 TO OUTPUT MODE
	GPIOA->MODER &= ~GPIO_MODER_MODER10_1;

	Delay5((uint16_t) 100, TIM15);

	return sucess;
}

void DS18SkipRom(){
	GPIOA->MODER |= GPIO_MODER_MODER10_0;   // SET GPIOA 10 TO OUTPUT MODE
	GPIOA->MODER &= ~GPIO_MODER_MODER10_1;

	OneWireWrite(0,0);             // transmit 0

	OneWireWrite(0,0);             // transmit 0

	OneWireWrite(1,0);             // transmit 1

	OneWireWrite(1,0);             // transmit 1

	OneWireWrite(0,0);             // transmit 0

	OneWireWrite(0,0);             // transmit 0

	OneWireWrite(1,0);    			// transmit 1

	OneWireWrite(1,10);             // transmit 1
}

void DS18ConvertTemp(){
	GPIOA->MODER |= GPIO_MODER_MODER10_0;   // SET GPIOA 10 TO OUTPUT MODE
	GPIOA->MODER &= ~GPIO_MODER_MODER10_1;

	OneWireWrite(0,0);             // transmit 0

	OneWireWrite(0,0);             // transmit 0

	OneWireWrite(1,0);    			// transmit 1

	OneWireWrite(0,0);             // transmit 0

	OneWireWrite(0,0);             // transmit 0

	OneWireWrite(0,0);             // transmit 0

	OneWireWrite(1,0);    			// transmit 1

	OneWireWrite(0,20);             // transmit 0

}

double DS18ReadTemperature(){

	uint16_t readOutL;
	uint16_t readOutH;

	GPIOA->MODER |= GPIO_MODER_MODER10_0;   // SET GPIOA 10 TO OUTPUT MODE
	GPIOA->MODER &= ~GPIO_MODER_MODER10_1;

	OneWireWrite(0,0);             // transmit 0
	OneWireWrite(1,0);    			// transmit 1
	OneWireWrite(1,0);    			// transmit 1
	OneWireWrite(1,0);    			// transmit 1
	OneWireWrite(1,0);    			// transmit 1
	OneWireWrite(1,0);    			// transmit 1
	OneWireWrite(0,0);             // transmit 0
	OneWireWrite(1,10);    			// transmit 1


	 readOutL = OneWireReadByte();
	 readOutH = OneWireReadByte();

	 double temperature;
	 double add = 0.125;

	 for(int i = 1; i < 8; i++){			// Convert LSB 2s complement
		 if(readOutL & 1 << i){
			 temperature += add;
		 }
		 add *= 2;
	 }

	 add = 16;
	 for(int i = 0; i < 2; i++){			// Convert MSB 2s complement
		 if(readOutH & 1 << i){
			 temperature += add;
		 }
		 add *= 2;
	 }
	 if(readOutH & 1 << 5){					// Check sign
		 temperature *= -1;
	 }
	Delay5((uint16_t) 40, TIM15);

	return temperature;
}

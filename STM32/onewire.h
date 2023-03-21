/*
 * OneWire.h
 *
 *  Created on: 17 Mar 2023
 *      Author: manol
 */

#ifndef ONEWIRE_H_
#define ONEWIRE_H_

#include "delay.h"
#include "stm32f3xx.h"

static inline void OneWireWrite(int bit, int postDelay);
static inline  uint16_t OneWireReadByte();

static inline void OneWireWrite(int bit, int postDelay){
	if(bit == 0){
		GPIOA->ODR &= ~GPIO_ODR_10;    // transmit 0
		Delay5((uint16_t) 12, TIM15);
		GPIOA->ODR |= GPIO_ODR_10;
		Delay5((uint16_t) 2, TIM15);
		if(postDelay > 0){
			Delay5((uint16_t) postDelay, TIM15);
		}
	} else {
		GPIOA->ODR &= ~GPIO_ODR_10;    // transmit 1
		Delay5((uint16_t) 2, TIM15);
		GPIOA->ODR |= GPIO_ODR_10;
		Delay5((uint16_t) 12, TIM15);
		if(postDelay > 0){
			Delay5((uint16_t) postDelay, TIM15);
		}
	}
}

static inline uint16_t OneWireReadByte(){

uint16_t readOut = 0;

	for(uint16_t i = 0; i < 8; i++){
		GPIOA->MODER |= GPIO_MODER_MODER10_0;   // SET GPIOA 10 TO OUTPUT MODE
		GPIOA->MODER &= ~GPIO_MODER_MODER10_1;

		GPIOA->ODR &= ~GPIO_ODR_10;    // Transmit Read Slot
		Delay5((uint16_t) 1, TIM15);
		GPIOA->ODR |= GPIO_ODR_10;

		GPIOA->MODER &= ~GPIO_MODER_MODER10_0;   // SET GPIOA 10 TO INPUT MODE
		GPIOA->MODER &= ~GPIO_MODER_MODER10_1;

		Delay5((uint16_t) 4, TIM15);

		uint16_t bit =  0;

		if(GPIOA->IDR & GPIO_IDR_10){   // read bit
			bit = 1;
		} else {
			bit = 0;
		}

		readOut |= bit << i;

		Delay5((uint16_t) 11, TIM15);
	}

Delay5((uint16_t) 20, TIM15);

return readOut;
}

#endif /* ONEWIRE_H_ */

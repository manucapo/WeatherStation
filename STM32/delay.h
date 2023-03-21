/*
 * Delay.h
 *
 *  Created on: 17 Mar 2023
 *      Author: manol
 */

#ifndef DELAY_H_
#define DELAY_H_

static inline void Delay5(uint16_t amount, TIM_TypeDef * timer);

static inline void Delay5(uint16_t amount, TIM_TypeDef * timer){
	 timer->CNT = 0;
	while(TIM15->CNT < (amount * 5)){};
	 timer->CNT = 0;
}

#endif /* DELAY_H_ */

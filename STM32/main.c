/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "stm32f3xx.h"

#include "delay.h"
#include "onewire.h"
#include "BME280.h"
#include "DS18B20.h"

static void RCC_Init();
static void GPIO_Init();
static void TIM_Init();
static void NVIC_Init();
static void USART_Init();
static void I2C_Init();
static void HSI_Calib(int i);

void IntToSerial(int number, int id);

void TransmitData();

unsigned long IC1HSIValue1;
unsigned long IC1HSIValue2;
unsigned long HSIMeasurement;
unsigned long HSIerror;
unsigned long HSIcorrection;
unsigned long HSITrim;
unsigned long HSIOptimumError = 99999999;
int HSIMeasureStage;  // stage = 3 => Calibration is over.
int CalibRoutineCounter;

int TransmissionState;

int temperature = 9999;
unsigned int humidity = 9999;
int pressure = 9999;

unsigned int uSCounter;

int main(void)
{
	RCC_Init();
	GPIO_Init();
	TIM_Init();
	NVIC_Init();
	USART_Init();
	I2C_Init();

	BME280Reset();
	BME280ReadCompensation();
}

void IntToSerial(int number, int id){
	char string[5] = "";
	sprintf(string, "%d",number + id * 10000);

	for(int i = 0; i <  5; i++){
		while((USART2->ISR & USART_ISR_TC) == 0);
		USART2->TDR = string[i];
	}
}

void TransmitData(){

	if( TransmissionState == 0){
		IntToSerial(temperature,1);
		TransmissionState++;
	} else if( TransmissionState == 1){
		IntToSerial(humidity,2);
		TransmissionState++;
	} else if (TransmissionState == 2){
		IntToSerial(pressure,3);
		TransmissionState = 0;
	}
}

static void RCC_Init(){

	RCC->CR |= RCC_CR_HSION;          // ENABLE HSI CLOCK
	RCC->CR = CalibRoutineCounter<<3;                   // SET HSI TRIM TO 0 BEFORE CALIBRATION

	FLASH->ACR &=  ~FLASH_ACR_LATENCY_0;  // SET FLASH LATENCY TO 2
	FLASH->ACR |=  FLASH_ACR_LATENCY_1;
	FLASH->ACR &=  ~FLASH_ACR_LATENCY_2;
	FLASH->ACR |= FLASH_ACR_PRFTBE;       // ENABLE FLASH PREFETCH BUFFER

	RCC->CFGR |=  RCC_CFGR_PLLMUL_0;   // SET PLL MULT TO 16 ( 0110 )  (8 MHz)/2 * 16 = 64 MHz
	RCC->CFGR |=  RCC_CFGR_PLLMUL_1;
	RCC->CFGR |=  RCC_CFGR_PLLMUL_2;
	RCC->CFGR |=  RCC_CFGR_PLLMUL_3;
	RCC->CR |= RCC_CR_PLLON;          //ENABLE PLL

	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;  // PRESCALE AHB CLOCK BY 1 (64MHz)
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV8;  // PRESCALE APB1 BY 8 (8MHz)
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;  // PRESCALE APB2 BY 1 (64MHz)

	RCC->CFGR &= ~RCC_CFGR_MCOPRE_0; // SET MCO PRESCALER TO 1 (000)
	RCC->CFGR &= ~RCC_CFGR_MCOPRE_1;
	RCC->CFGR &= ~RCC_CFGR_MCOPRE_2;

	RCC->CFGR |= RCC_CFGR_MCO_0;   // CONNECT LSE TO MCO (011)
	RCC->CFGR |= RCC_CFGR_MCO_1;
	RCC->CFGR &= ~RCC_CFGR_MCO_2;

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;    // ENABLE I/O PORT A
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN; // ENABLE I/O PORT B

	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;  // ENABLE SYS CONFIG CLOCK

	RCC->APB2ENR  |= RCC_APB2ENR_TIM15EN;  // ENABLE GENERAL TIMER 15
	RCC->APB2ENR  |= RCC_APB2ENR_TIM16EN;   // ENABLE GENERAL TIMER 16

	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;  // ENABLE USART2 CLOCK
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;  // ENABLE I2C1 CLOCK

	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;   // ENABLE BASIC TIMER 6
	RCC->APB1ENR |= RCC_APB1ENR_PWREN; // ENABLE POWER INTERFACE
	RCC->APB1ENR |= RCC_APB1ENR_WWDGEN; // ENABLE WINDOW WATCHDOG

	PWR->CR |= PWR_CR_DBP;           // DISABLE RTC DOMAIN WRITE PROTECTION
	RCC->BDCR |= RCC_BDCR_RTCEN;  // ENABLE RTC CLOCK
	RCC->BDCR |= RCC_BDCR_LSEON;  // ENABLE LSE CLOCK

	RCC->CFGR &= ~RCC_CFGR_SW_0;      // SET SYSTEM CLOCK TO PLL (10)
	RCC->CFGR |= RCC_CFGR_SW_1;
}

static void GPIO_Init(){

	GPIOA->MODER |= GPIO_MODER_MODER5_0;   // SET GPIOA 5 TO OUTPUT MODE
	GPIOA->MODER &= ~GPIO_MODER_MODER5_1;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT_5;   // SET GPIOA 5 TO PUSH-PULL TYPE
	GPIOA->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR5_0;  // SET GPIOA 5 TO LOW SPEED
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR5_0;         // SET GPIO TO PULL DOWN MODE
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR5_1;

	GPIOA->MODER &=	~GPIO_MODER_MODER2_0;   // SET GPIOA 2 TO AF MODE FOR USART
	GPIOA->MODER |= GPIO_MODER_MODER2_1;
	GPIOA->AFR[0] |= 0x00000007<<GPIO_AFRL_AFRL2_Pos; // SET GPIOA 2 AF TO AF7

	GPIOA->ODR |= GPIO_ODR_10;
	GPIOA->MODER |= GPIO_MODER_MODER10_0;   // SET GPIOA 10 TO OUTPUT MODE
	GPIOA->MODER &= ~GPIO_MODER_MODER10_1;
	GPIOA->OTYPER |= GPIO_OTYPER_OT_10;   			// SET GPIOA 10 TO OPEN DRAIN
	GPIOA->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR10_0;  // SET GPIOA 10 TO HIGH SPEED
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR10_1;
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR10_0; 			// SET GPIO TO NO PUPD MODE (00)  DSB USES EXTERNAL PULL UP
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR10_1;

	GPIOB->MODER  &= ~GPIO_MODER_MODER8_0;   // SET GPIOB 8 TO AF MODE FOR I2C
	GPIOB->MODER  |= GPIO_MODER_MODER8_1;
	GPIOB->AFR[1] |= 0x00000004<<GPIO_AFRH_AFRH0_Pos ; // SET GPIOB 8 AF TO AF4

	GPIOB->MODER  &= ~GPIO_MODER_MODER9_0;   // SET GPIOB 9 TO AF MODE FOR I2C
	GPIOB->MODER  |= GPIO_MODER_MODER9_1;
	GPIOB->AFR[1] |= 0x00000004<<GPIO_AFRH_AFRH1_Pos; // SET GPIOB 9 AF TO AF4
}

static void TIM_Init(){
	TIM6->PSC = 0x0000063F;   // PRESCALE BY 1600 ( 1599 + 1 ) (8MHz * 2 / 1600 = 10 KHz)
	TIM6->ARR  = 0x00009C40;  // COUNT TO 40000              (10 Khz / 40K = 0.25 Hz)
	TIM6->DIER |= TIM_DIER_UIE;  // ENABLE TIM INTERRUPT
	TIM6->CR1 |= TIM_CR1_CEN;  // ENABLE COUNTER

	TIM16->PSC = 0x00000007;   // PRESCALE BY 8 ( 7 + 1 )  ( 64Mhz / 8 = 8 MHz)
	TIM16->ARR  = 0xffffffff;  // COUNT TO MAX

	TIM16->OR |= TIM16_OR_TI1_RMP_0;     // CONNECT TIM16 INPUT TO MCO (11)
	TIM16->OR |= TIM16_OR_TI1_RMP_1;

	TIM16->CCMR1 |= TIM_CCMR1_CC1S_0;  // CONFIGURE CC1 CHANNEL AS INPUT (01)
	TIM16->CCMR1 &= ~TIM_CCMR1_CC1S_1;
	TIM16->CCMR1 &= ~TIM_CCMR1_IC1PSC_0;    // DISABLE INPUT PRESCALER
	TIM16->CCMR1 &= ~TIM_CCMR1_IC1PSC_1;

	TIM16->CCER &= ~TIM_CCER_CC1P;   // SET CAPTURE COMPARE POLARITY TO ACTIVE HIGH
	TIM16->CCER |= TIM_CCER_CC1E;

	TIM16->DIER |= TIM_DIER_CC1IE;          // ENABLE TIM CC INTERRUPT
	TIM16->DIER |= TIM_DIER_UIE;          // ENABLE TIM UPDATE INTERRUPT
	TIM16->CR1 |= TIM_CR1_CEN;             // ENABLE COUNTER

	TIM15->PSC = 0x0000003F;   // PRESCALE BY 64 ( 63 + 1 ) (64MHz * 1 / 64 = 1MHz)
	TIM15->ARR  = 0xffffffff;  // COUNT TO MAX
	TIM15->CR1 |= TIM_CR1_CEN;  // ENABLE COUNTER
}

static void NVIC_Init(){
	NVIC_EnableIRQ(TIM6_DAC_IRQn);
	NVIC_EnableIRQ(TIM16_IRQn);
}

static void USART_Init(){
	USART2->CR1 &= ~USART_CR1_M;  // SET WORD LENGHT TO 8 BITS
	USART2->CR1 &= ~USART_CR1_M1;

	USART2->BRR |= 0x0000000341; // SET USART DIVISOR FOR 9600 BAUD (8MHZ / 9600)  = 833

	USART2->CR2 &= ~USART_CR2_STOP_0;  // SET 2 STOP BITS (10)
	USART2->CR2 |= USART_CR2_STOP_1;

	USART2->CR1 |= USART_CR1_UE;  // ENABLE USART
	USART2->CR1  |= USART_CR1_TE;   // SEND IDLE FRAME AS FIRST TRANSMISSION
}

static void I2C_Init(){
	I2C1->TIMINGR |= 0x0000000001<<I2C_TIMINGR_PRESC_Pos;			// CONFIGURE FOR NORMAL MODE WITH 8MHZ I2C CLOCK
	I2C1->TIMINGR |= 0x00000000c3<<I2C_TIMINGR_SCLH_Pos;
	I2C1->TIMINGR |= 0x00000000c7<<I2C_TIMINGR_SCLL_Pos;
	I2C1->TIMINGR |= 0x0000000002<<I2C_TIMINGR_SDADEL_Pos;
	I2C1->TIMINGR |= 0x0000000004<<I2C_TIMINGR_SCLDEL_Pos;
	I2C1->CR2  |= 0x0000000076<<(I2C_CR2_SADD_Pos+1);
}

static void HSI_Calib(int i){                  // CALIBRATE HSI TRIM
		RCC->CR = i<<3;
		HSIerror = abs(8000000 - (HSIMeasurement * 32406));
		if(HSIOptimumError > HSIerror){
			HSIOptimumError = HSIerror;
			HSITrim = i;
		}
		HSIMeasureStage = 0;
}

void TIM6_DAC_IRQHandler(){

	if (TIM6->SR & TIM_SR_UIF){      // HANDLE UPDATE INTERRUPT
		if(GPIOA->ODR & GPIO_ODR_5){               // TOGGLE PIN A5
			GPIOA->BSRR = GPIO_BSRR_BR_5;
		} else {
			GPIOA->BSRR = GPIO_BSRR_BS_5;
		}

		if(DS18ResetPulse()){
			DS18SkipRom();
			temperature = DS18ReadTemperature();
		}

		if(DS18ResetPulse()){
			DS18SkipRom();
			DS18ConvertTemp();
		}

		BME280ReadRegisters();
		BME280Config();
		BME280TempCompensation(rawTemperature);

		humidity = BME280CalculateHumidity(rawHumidity) / 1024;
		pressure = (BME280CalculatePressure(rawPressure) / 256)  * 0.01;  // CONVERT PA TO MBAR

		TransmitData();

		TIM6->SR &= ~TIM_SR_UIF;              // CLEAR INTERRUPT FLAG
	}
}

void TIM16_IRQHandler(){

	if(TIM16->SR & TIM_SR_CC1IF ){         // HANDLE CC INTERUPT
		if(HSIMeasureStage == 0){
			IC1HSIValue1 = TIM16->CCR1;
			HSIMeasureStage += 1;
		} else if (HSIMeasureStage == 1){
			IC1HSIValue2 = TIM16->CCR1;
			HSIMeasureStage += 1;
		}else if (HSIMeasureStage == 2){
			HSIMeasurement =  IC1HSIValue2 - IC1HSIValue1;
			if(CalibRoutineCounter < 32){
				HSI_Calib(CalibRoutineCounter);
			CalibRoutineCounter++;
			} else{
				RCC->CR = HSITrim<<3; // SET FINAL HSI TRIM
				HSIMeasureStage++;

				NVIC_DisableIRQ(TIM16_IRQn);
				RCC->APB2ENR  &= ~RCC_APB2ENR_TIM16EN;   // DISABLE GENERAL TIMER 16
			}
		}
		TIM16->SR &= ~TIM_SR_CC1IF;  // CLEAR INTERRUPT FLAG
	}

}

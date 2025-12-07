#include "servo.h"

#include "stm32f3xx.h"
#include "stm32f303x8.h"
#include "pinAccess.h"
#include "pwm.h"



void servoInit (){
	//1
	pinAlt(GPIOB,0,2);

	//pinMode(GPIOB,0,OUTPUT);


	//2 - timer configuration (use TIM2@10KHz)
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	//reset peripheral (mandatory!)
	RCC->APB1RSTR |= RCC_APB1RSTR_TIM3RST;
	RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM3RST;

	//config timer@10KHz, with 100 ticks (duty cycle at 1%)
	TIM3->PSC = 64-1; //prescaler : tick@1us
	TIM3->ARR = 20000-1; //auto-reload: counts 20000 ticks = 20 ms
	
	//Q2
	// TIM3->DIER |= TIM_DIER_UIE; //Interuption
	// NVIC_EnableIRQ(TIM3_IRQn);


	// //3- PWM configuration (with driver function)
	PWMEnableChannel(TIM3,3); //channel TIM2_CH3
	TIM3->CCR3 = 1500; //1.5 ms
	TIM3->CR1 |= TIM_CR1_CEN; //start timer (count enable)
}


/* setpoint in .1% steps :
* 0 = > 0 % = > 0 degree
* 500 = > 50% = > 45 degrees
* 1000 = > 100% = > 90 degrees
*
* saturations :
* - below 0 = > same as 0%
* - above 1000 = > same as 100 %*/

void servoSet(int setpoint){
	if (setpoint > 1000) setpoint = 1000;
	if (setpoint < 0) setpoint = 0;

	// CCR3 = 1000 (0°) à 2000 (90°)
	TIM3->CCR3 = setpoint+1000; 
}




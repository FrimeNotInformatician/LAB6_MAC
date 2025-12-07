#include "encoder.h"

#include "stm32f3xx.h"
#include "stm32f303x8.h"

unsigned int encoderValue = 0;

extern "C" void EXTI0_IRQHandler ( )
{
	if(EXTI->PR & EXTI_PR_PR0){//Vérifie si on a l'interruption sur le pin 10
		
		if(GPIOA->IDR & (1 << 1)) { // 1 :Rising else // 0:Falling }
			// If LEFT :
			if (encoderValue < 45 ) encoderValue ++;

		}else{
			// IF RIGHT :
			if (encoderValue > 0) encoderValue--;
		}
		EXTI->PR |= EXTI_PR_PR0 ; //reset le flag
	}
}

void encoderInit(){
	//Config EXTI10:-----------PA0-------------
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	__asm("nop");

	EXTI->IMR |= EXTI_IMR_MR0; //Intéruption pour les pin 10 
	EXTI->RTSR |= EXTI_RTSR_TR0; //rising
	SYSCFG->EXTICR[2] |= SYSCFG_EXTICR1_EXTI0_PA; //Port A
	NVIC_SetPriority(EXTI0_IRQn, 3); //Nvic level 3
	NVIC_EnableIRQ(EXTI0_IRQn); //enable les interruptions

	encoderValue = 0;
}


int getEncoderValue (){
	return encoderValue;
}
#include "stm32f3xx.h"
#include "stm32f303x8.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ST7735.h"
#include "adc.h"
#include "tft.h"

//Privates
#include "./app/rotationApp.h"


//Q1-Q2

// extern "C" void TIM3_IRQHandler(){
//  	GPIOB->ODR ^= (1 << 0);   // toggle PB0
//     TIM3->SR &= ~TIM_SR_UIF;   // clear le flag d'overflow
// }





void setup()
{
	
	ADCInit();


	rotationApp_init();

}

//this is a simple application that uses the TFT display.
// => it first write a string in the top of the screen (in setup)
// => it writes the ADC value of the potentiometer in green, and
//    refreshes it each time the value differs more than 5.
int main()
{
	// static int prevPot = -1;
	// int setpoint = 0;

	setup();
	while(1)
	{
		//potentiometer
		// int pot = ADCRead(); //12 bits -> 4096 -> 4 digits

		// //update only we value changes significantly
		// if(abs(prevPot - pot) > 100)
		// {
		// 	//set cursor centered on line 4.
		// 	Tft.setTextCursor(Tft.getTextWidth()/2-2,4);
		// 	Tft.eraseText(4);	//remove previous value (4 digits)
		// 	Tft.print(pot);
		// 	prevPot = pot;


		// 	setpoint = (pot * 1000) / 4095; 
		// 	servoSet(setpoint);

		// }

		rotationApp_main();

	
	}
}








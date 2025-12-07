#include "tft.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ST7735.h"
#include "pinAccess.h"

#include "stm32f3xx.h"
#include "stm32f303x8.h"

//Private : 
#include "rotationApp.h"
#include "../include/servo.h"
#include "../include/encoder.h"
#include <string>

//Initialize
ServoStruct servoStruct = {LEFT,0,0,0};

extern "C" void TIM7_DAC2_IRQHandler()
{
	servoStruct.update = 1;
	TIM7->CNT = 0; //RAZ de cnt
    TIM7->SR &= ~TIM_SR_UIF;   // clear le flag d'overflow
}

void rotationApp_init(){

    //Config TIM7:-----------------------------
    //TIM7 - input clock = 64MHz.
    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
    __asm("nop");

    //reset peripheral (mandatory!)
    RCC->APB1RSTR |= RCC_APB1RSTR_TIM7RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM7RST;
    __asm("nop");

    TIM7->PSC = 64-1; //tick@1us 64Mhz/64 = 1 MHz -> 1us
    TIM7->ARR = 50000-1; //counts max 50 000 ticks  > 50 ms
    TIM7->CR1 |= TIM_CR1_CEN; // démarrage du timer

	TIM7->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM7_DAC2_IRQn);
	// ----------------------------------------


	//Configuration du servo moteur (45°) -----
	servoInit();
	// ----------------------------------------

	//Configuration de l'encoder (45°) --------
	encoderInit();
	// ----------------------------------------


	//Configuration des bouton (45°) ----------
	pinMode(GPIOB,1,INPUT_PULLUP); //Bouton
	// ----------------------------------------


	//Initalisation de l'affichage  -----------
	Tft.setup();
	Tft.setTextColor(0xFFFF);
	Tft.setTextCursor(Tft.getTextWidth()/2-7,0);
	Tft.print("TP servoMoteur");

	Tft.setTextCursor(Tft.getTextWidth()/2-5,1);
	Tft.print("Mode : Scn");
	Tft.setTextColor(0b0000000000011111);
	Tft.setTextCursor(Tft.getTextWidth()/2+2 ,1);
	Tft.print("Scn");

	//Ligne
	Tft.drawFastHLine(0,35,Tft.width(),0xFFFF);

	Tft.setTextColor(0xFFFF);

	Tft.setTextCursor(1,3);
	Tft.print("Servo :");

	Tft.drawRect(8,63,Tft.width()-22,18,0xFFFF);

	Tft.setTextCursor(1,6);
	Tft.print("Encoder :");

	Tft.drawRect(8,105,Tft.width()-22,18,0xFFFF);
	// ----------------------------------------

}



void manualMode(){
	static int prevEncoder = -1;

	int encoder = getEncoderValue();

	if (encoder != prevEncoder)
		{
			prevEncoder = encoder;
			servoStruct.value = (encoder * 1000) / 45; 

			//showDistance();
			servoSet(servoStruct.value);
		}
}


void scanMode(){

	if (servoStruct.update){
		servoStruct.update = 0; //reset flag

		if (servoStruct.value >= 1000 || servoStruct.value <= 0)
		{
			servoStruct.direction = !servoStruct.direction;
		}
		
		switch (servoStruct.direction)
		{
		case 1:
			servoStruct.value += 10;
		break;
		
		case 0:			
			servoStruct.value -= 10;

		break;
		}
		
		servoSet(servoStruct.value) ;
		
	}
}



void showDistance(int distance, int Y){
	const short X = 9;
	const short HEIGHT = 16;
	// largeur totale de la barre
	const short WIDTH_BAR = Tft.width() - 24;   // = 137 si écran 160px

	// conversion 0–100 vers 0–WIDTH_BAR
	distance = (distance * WIDTH_BAR) / 100;

	short remaining = WIDTH_BAR - distance;

	//white part
	Tft.fillRect(X, Y, distance, HEIGHT, 0xFFFF);

	//black part
	Tft.fillRect(X + distance, Y, remaining, HEIGHT, 0x0000);
}



void displayMode(RotationMode a){
	Tft.setTextCursor(Tft.getTextWidth()/2+2 ,1);
	Tft.eraseText(3);
	switch (a)
	{
	case SCAN_MODE:
		Tft.setTextColor(0b11111);
		Tft.print("Scn");
	break;
	
	case MANUAL_MODE:
		Tft.setTextColor(0b1111100000000000);
		Tft.print("Man");
	break;

	case TRANSITION_MODE:
		Tft.setTextColor(0b11111100000);
		Tft.print("trs");
	break;

	default:

	break;
	}
}

void rotationApp_main(){
	static bool lastButtonState = 0;
	static int encoderTransValue = 0;
	static RotationMode mode = SCAN_MODE;

	//Butto nvaue
	bool buttonState = digitalRead(GPIOB,1); // PB1

        //Bouton avec "mémoire"
        if ( (buttonState != lastButtonState) && (buttonState == 0))
        {
			if (mode == SCAN_MODE ){
				encoderTransValue = getEncoderValue();
				displayMode(TRANSITION_MODE);
				mode = TRANSITION_MODE;
			}// Transition scan to manual
			
			if (mode == MANUAL_MODE ){
				displayMode(SCAN_MODE);
				mode = SCAN_MODE;
			}  // Manual to scan
        }
        lastButtonState = buttonState;


		switch (mode)
		{
		case TRANSITION_MODE:// Transition scan to manual
			
			scanMode();
			//Pour les 2 sens
			if (servoStruct.value/10 == (encoderTransValue * 100)/45 ) {
				displayMode(MANUAL_MODE);
    			mode = MANUAL_MODE;
			}
		break;

		case MANUAL_MODE:
			manualMode();
		break;
		
		case SCAN_MODE:
			scanMode();
		break;
		}

		if(servoStruct.value != servoStruct.preValue ){
			Tft.setTextColor(0xFFFF);
		
			//Servo
			Tft.setTextCursor(9,3);
			Tft.eraseText(3);	//remove previous value (11 digits)

			Tft.print(servoStruct.value/10);
			showDistance(servoStruct.value/10,64);

			//Encoder
			Tft.setTextCursor(11,6);
			Tft.eraseText(3);	//remove previous value (11 digits)

			Tft.print(getEncoderValue());

			showDistance( ((getEncoderValue()*100)/45), 106 );


			//showDistance(servoStruct.value);
		}
		servoStruct.preValue  = servoStruct.value;
}


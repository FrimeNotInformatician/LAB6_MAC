#pragma once


//Variables et structure :
const bool  LEFT = 0;
const bool  RIGHT = 1;

 //Encoder structure 
 typedef struct {
     bool direction;
     int value ;
     int preValue;
     bool update;
 } ServoStruct;

 // Définition des modes :
typedef enum {
    SCAN_MODE = 0,
    MANUAL_MODE = 1,
    TRANSITION_MODE = 2 // Scan → Manual
} RotationMode;

//------------------------------

// Prototypes :
void rotationApp_init();

void scanMode();
void manualMode();
void rotationApp_main();

//Bonus
void showDistance(int distance, int Y);
void displayMode(RotationMode a);




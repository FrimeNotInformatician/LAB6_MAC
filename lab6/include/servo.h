#pragma once


/* init servo motor at 45 degrees . */
void servoInit ();

/* setpoint in .1% steps :
* 0 = > 0 % = > 0 degree
* 500 = > 50% = > 45 degrees
* 1000 = > 100% = > 90 degrees
*
* saturations :
* - below 0 = > same as 0%
* - above 1000 = > same as 100 %
*/
void servoSet (int setpoint);

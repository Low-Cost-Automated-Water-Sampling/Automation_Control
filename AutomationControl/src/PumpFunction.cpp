/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/joejo/OneDrive/Desktop/Automation_Control/AutomationControl/src/PumpFunction.ino"
/*
 * Project Pump_Function
 * Description: Function for Pump
 * Author: Brendan Lestange
 * Date: 9/28/2023
 */

/**
 * Joe: Need some pump power off safety mechanism in case flow is never triggered
*/
#include "DataStructures.h"


void RunPump();
void pumpOff();
#line 14 "c:/Users/joejo/OneDrive/Desktop/Automation_Control/AutomationControl/src/PumpFunction.ino"
void RunPump(){
	digitalWrite(pump, HIGH);	
}

//Joe 10/15
void pumpOff(){
	digitalWrite(pump, LOW);
}

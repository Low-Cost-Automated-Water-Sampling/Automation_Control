/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/joejo/OneDrive/Desktop/Automation_Control/AutomationControl/src/FlowMeter.ino"
/**
 * Joseph DeMarco
 * Senior Design
 * Automated Water Sampler
 * 
 * Flow Meter Reading
*/

//#include <Particle.h>
//#include "PumpFunction.ino"
#include "DataStructures.h"

// GPIO pin
void flowMeter_ISR();
void calcWaterFlow();
#line 14 "c:/Users/joejo/OneDrive/Desktop/Automation_Control/AutomationControl/src/FlowMeter.ino"
#define flowMeter D2//TEMPORARY RANDOM PIN TO BE REPLACED

void flowMeter_ISR(){
    flowPulseCount++;
}

// Function to read the sensor output and calculate the flow
void calcWaterFlow() {
    flow = flowPulseCount/pulsesPermL; // flow in liters
}

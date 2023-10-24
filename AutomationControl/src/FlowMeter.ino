/**
 * Joseph DeMarco
 * Senior Design
 * Automated Water Sampler
 * 
 * Flow Meter Reading
*/

#include <Particle.h>
//#include "PumpFunction.ino"
#include "DataStructures.h"

// GPIO pin
#define flowMeter D2//TEMPORARY RANDOM PIN TO BE REPLACED


void flowMeter_ISR(){
    flowPulseCount++;
    if(flow >= (TestSampler.sampleVolume-50)){ //50 mL threshold
        pumpOff();
    }
}

// Function to read the sensor output and calculate the flow
void calcWaterFlow() {
    flow = flowPulseCount/pulsesPerLiter; // flow in liters
}

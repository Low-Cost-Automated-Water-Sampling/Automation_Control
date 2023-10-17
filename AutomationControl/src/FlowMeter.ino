/**
 * Joseph DeMarco
 * Senior Design
 * Automated Water Sampler
 * 
 * Flow Meter Reading
*/

#include <Particle.h>
#include <PumpFunction.ino>

// GPIO pin
#define flowMeter D2//TEMPORARY RANDOM PIN TO BE REPLACED

//Global variables
const float pulsesPerLiter = 450; //output pulses/liters
volatile int flowPulseCount;
volatile float flow;
extern float sampleVolume;

void flowMeter_ISR(){
    flowPulseCount++;
    if(flow == sampleVolume){
        pumpOff();
    }
}

// Function to read the sensor output and calculate the flow
void calcWaterFlow() {
    flow = flowPulseCount/pulsesPerLiter; // flow in liters
}

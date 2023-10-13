/**
 * Joseph DeMarco
 * Senior Design
 * Automated Water Sampler
 * 
 * Flow Meter Reading
*/

#include <Particle.h>

// GPIO pin
#define flowMeter D2//TEMPORARY RANDOM PIN TO BE REPLACED

const float pulsesPerLiter = 450; //output pulses/liters
const float freq = 21.0; // Frequency-to-flow conversion constant

//Global variables
volatile int pulseCount;

void flowMeter_ISR(){
    pulseCount++;
}

// Function to read the sensor output and calculate the flow
float calcWaterFlow() {
    float flow = pulseCount/pulsesPerLiter;
    return flow;
}

void setup() {
    // Initialize the Particle Boron LTE device
    // Particle.begin();

    // Set the sensor pin as an input
    pinMode(flowMeter, INPUT); //will need to change to PULLUP or PULLDOWN unsure
    attachInterrupt(flowMeter, flowMeter_ISR, RISING);
}

void loop() {
    // Read the sensor output and calculate the flow rate
    float flow = calcWaterFlow();

    // 
}

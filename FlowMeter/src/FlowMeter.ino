/**
 * Joseph DeMarco
 * Senior Design
 * Automated Water Sampler
 * 
 * Flow Meter Reading
*/

#include <Particle.h>

// GPIO pin
#define flowMeter D2; //TEMPORARY RANDOM PIN TO BE REPLACED

const float pulsesPerLiter = 450; //output pulses/liters
const float freq = 21.0; // Frequency-to-flow conversion constant

//Global variables
volatile int pulseCount;

void flowMeter_ISR(){

}

// Function to read the sensor output and calculate the flow
float readWaterFlowSensor() {
    float flow = 0;
    int signalCounter = 0;
    // Read the digital state of the sensor pin
    bool sensorState = digitalRead(flowMeterPin); //not sure if this is right

    if (sensorState) {
        // Sensor is ON, increment flow counter
        signalCounter++; 
    }
    flow = signalCounter/pulsesPerLiter;
    return flow;
}

void setup() {
    // Initialize the Particle Boron LTE device
    //Particle.begin();

    // Set the sensor pin as an input
    pinMode(flowMeterPin, INPUT);
}

void loop() {
    // Read the sensor output and calculate the flow rate
    float flow = readWaterFlowSensor();

    // 
}

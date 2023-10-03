/**
 * Joseph DeMarco
 * Senior Design
 * Automated Water Sampler
 * 
 * Flow Meter Reading
*/

#include <Particle.h>

// GPIO pin
const int sensorPin = D2; //TEMPORARY RANDOM PIN TO BE REPLACED


const float freq = 21.0; // Frequency-to-flow conversion constant


// Function to read the sensor output and calculate the flow rate
float readWaterFlowSensor() {
    // Read the digital state of the sensor pin
    bool sensorState = digitalRead(sensorPin);

    // Calculate the flow rate (L/min) based on the sensor's square wave output
    float flowRate = 0.0;

    if (sensorState) {
        // Sensor is ON, calculate flow rate
        flowRate = freq / 450.0; // Assuming 450 output pulses per liter
    }

    return flowRate;
}

void setup() {
    // Initialize the Particle Boron LTE device
    Particle.begin();

    // Set the sensor pin as an input
    pinMode(sensorPin, INPUT);
}

void loop() {
    // Read the sensor output and calculate the flow rate
    float flowRate = readWaterFlowSensor();

    // Check if the voltage is within the specified range
    float voltage = analogRead(sensorPin) * (voltageMax - voltageMin) / 4095.0 + voltageMin;

    if (voltage < voltageMin || voltage > voltageMax) {
        Particle.publish("WaterFlowStatus", "Voltage out of range", PRIVATE);
    } else {
        // Publish the flow rate to the Particle cloud
        Particle.publish("WaterFlowRate", String(flowRate, 2), PRIVATE);
    }

    // Delay for a period before reading again (adjust as needed)
    delay(1000);
}

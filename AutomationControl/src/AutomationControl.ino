/*
 * Project AutomationControl
 * Description: MAIN for Sampler Control
 * Author: Joseph DeMarco
 * Date: 10/15/2023
 */

#include <Particle.h>
#include <FlowMeter.ino>
#include <PumpFunction.ino>

// Pins
// GPIO pin
#define flowMeter D2//TEMPORARY RANDOM PINS TO BE REPLACED
#define servo D1
#define pump D4  

//Global variables
volatile extern int flowPulseCount;
volatile float flow;
//will be user defined
float sampleVolume = 450; //500mL bottle assuming leave room to not overflow
int numSamples = 24; //define number of samples available while empty
int sampleCounter = 0; //increment
float degreesPerSample = 360 / (numSamples+1); //sample bottles and flush spot split evenly
bool retrieveSamples = false;


// setup() runs once, when the device is first turned on.
void setup() {
    // Put initialization like pinMode and begin functions here.
    // Set the sensor pin as an input
    pinMode(pump, OUTPUT);
    pinMode(servo, OUTPUT);
    pinMode(flowMeter, INPUT); //will need to change to PULLUP or PULLDOWN unsure
    attachInterrupt(flowMeter, flowMeter_ISR, RISING);
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
    // The core of your code will likely live here.
    flow = calcWaterFlow();

}

/**
 * This is slightly sample data structure ignorant. It might be better to make these class functions instead of a regular function
 * perhaps the sample objects could be in an array indexed by a sample counter to know which is next
*/

// Sample Process
// Default sample arm position will be flush, at the end of a sample cycle
// On Scheduled sample time, user input, or specified rain event
// TIMER?
void takeSample(){ //will likely turn sample into an interrupt
// First check sample availability/count
    if(sampleCounter < numSamples){ // Samples available:  //sampleFull will be a sample bottle attribute
    //maybe: if(Samples[sampleCounter].sampleFull == false)
		flushSystem(); // FlushSystem //tbd 
        // need to check if water is actually flowing when pump is on

        // TakeSample
        // maybe change to Samples[sampleCounter]
        servoSample(sampleCounter, degreesPerSample); // Position servo to next available sample
        RunPump();
        // while(flow < sampleVolume){} // Power pump until sample size reached -- first idea

        // Flow meter ISR calls pumpOff if flow == sampleVolume
        delay(3000);
        if(flow == 0){
            pumpOff();
        }

        // Samples[sampleCounter].sampleFull // Change status of sample, sample count? – Need to define DATA STRUCTURES
    }
    else{
        retrieveSamples = true;
    }

    //increment and reset 
    sampleCounter++;
    flow = 0;
    servoFlush();
}




void flushSystem() {
	servoFlush(); // ServoFlush: Set servo motor to flush position
    //default position should be flush, but need a way to check. perhaps when I learn more about servos

	//while(flow < sampleVolume){ //Change sample volume to volume of system to be flushed
	RunPump();

    // flow meter ISR shuts off pump pumpOff() when flow == sampleVolume

    // need to check if water is actually flowing when pump is on
    // perhaps:
    delay(3000);
    if(flow == 0){
        pumpOff();
    }
	//}
    flow = 0; //reset flow measure
}


void servoFlush(){ // will be Minh
	//set the servo to flush position
}
void servoSample(int sample, float degrees){
    //move servo to sample * degrees
}

// Include monitoring if water is not flowing



// •	If the sample count == maximum, a flag is raised, and a notice is sent to the cloud along with the sample info. 
// 	Reset servo to flush position
// •	ServoFlush
// o	No sample containers available
// 	Set flag, notify user, device to sleep 
// •	samplesFull = TRUE
// 	Remains until user collects/resets samples
// o	After each sample event:
// 	The device publishes the sample info (for each sample or the given sample?) after each sample is taken. That info includes: timestamp? (sampleTime), sampleNumber, is sample flag raised (?), type of sample (sampleType, i.e., scheduled, commanded, rain gauge), etc.
// 	RESET flow measure counter to 0 
// reset servo to flush

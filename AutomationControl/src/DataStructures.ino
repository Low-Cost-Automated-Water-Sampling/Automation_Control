/**
 * Joseph DeMarco
 * 
 * Data Structures Definitions
 * Data Structures source file
 * 
 * 10/23/2023
 * 
*/

#include "DataStructures.h"

/**
 * Joseph DeMarco
 * 
 * Flush system function: move to flush servo, run pump
*/
void flushSystem() {
	servoFlush(); // ServoFlush: Set servo motor to flush position
    //default position should be flush, but need a way to check. perhaps when I learn more about servos

	RunPump();
    // flow meter ISR shuts off pump pumpOff() when flow == sampleVolume

    // need to check if water is actually flowing when pump is on
    pumpingFails();
	
    flow = 0; //reset flow measure
    return;
}

void servoFlush(){ // will be Minh
	//set the servo to flush position
}
void servoSample(int sample, float degrees){
    //move servo to sample * degrees
}


/**
 * Joseph DeMarco
 * 
 * monitoring if water is not flowing
 */ 
void pumpingFails(){
    // if no water after 3s
    delay(3000);
    if(flow == 0){ //if no water, turn off pump
        Samples[TestSampler.sampleCounter].sampleFailed = true;
        pumpOff();
        // NEED ERROR MESSAGES
        //include some message, maybe sample failed
    }
    //if sample insufficient water in 40s, fail, pump off
    delay(40000);
    if(flow<(1/2*TestSampler.sampleVolume)){
        Samples[TestSampler.sampleCounter].sampleFailed = true;
        pumpOff();
        // NEED ERROR MESSAGE
    }
    return;
}


// need to flesh out 
void rainMeasure_ISR(){
    rainPulseCounter++;
    if(rainAmount >= TestSampler.rainEvent){ //50 mL threshold
        takeSample("Rain Event Sample");
        rainPulseCounter = 0; //reset rain pulse counter
    }
}
/**
 * calculates rain fall amount in inches based on defined conversion and rain sensor pulses
*/
void calculateRainfall(){
    rainAmount = rainPulseCounter/pulsesPerInchRain; // flow in liters
    return;
}

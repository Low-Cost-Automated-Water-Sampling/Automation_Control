/*
 * Project AutomationControl
 * Description: MAIN for Sampler Control
 * Author: Joseph DeMarco
 * Date: 10/15/2023
 * 
 * Updated: 11/2
 */

#include <Particle.h>
#include <string>
#include "DataStructures.h"
// #include "FlowMeter.ino"
// #include "PumpFunction.ino"

SYSTEM_MODE(SEMI_AUTOMATIC);
SYSTEM_THREAD(ENABLED); 


void setup() {
    // Put initialization like pinMode and begin functions here.
    // Set the sensor pin as an input
    pinMode(pump, OUTPUT);
    pinMode(servo, OUTPUT);
    pinMode(flowMeter, INPUT); //will need to change to PULLUP or PULLDOWN unsure
    attachInterrupt(flowMeter, flowMeter_ISR, RISING);
    attachInterrupt(RainSensor, rainMeasure_ISR, FALLING); //Not positive this is how this will work

    pinMode(led1, OUTPUT); //LEDs to show sleeping or awake
    pinMode(LED0, OUTPUT);

    //bottle array instantiation
    for(int i=0; i<24; i++){
        Samples[i] = SampleBottle(i+1); // ID = 0->23 +1-> 1->24
    }

    //Sleep configuration
    config.mode(SystemSleepMode::ULTRA_LOW_POWER)       // define the properties of our sleep config object
          //.network(NETWORK_INTERFACE_CELLULAR)        // should wakeup due to network or cellular connection
          //.flag(SystemSleepFlag::WAIT_CLOUD)
		  .gpio(RainSensor, FALLING)                    // specify wakeup if falling edge on WAKEUP_PIN
		  .duration(sleepTime*1000);                    // or wakeup after duration in ms (unlike classic sleep fn)
    //    .duration(2min);       // alternative way to specify duration in minutes

    // cloud functions
    Particle.function("Take Sample Now", takeSampleNow);
    Particle.function("Set Sampler Configuration", setSampleConfig);
}




void loop() {
    calcWaterFlow();
    calculateRainfall();

    //Including Sleep control and rain meter
    
                           		  
    // digitalWrite(LED0,LOW);                             // turn off led before going to sleep
	// SystemSleepResult result = System.sleep(config);    // go to sleep
    // digitalWrite(LED0, HIGH);                           // turn on the led when we wake up
    // delay(10000);       

}

/**
 * This is slightly sample data structure ignorant. It might be better to make these class functions instead of a regular function
 * perhaps the sample objects could be in an array indexed by a sample counter to know which is next
 * 
 * NEED CLOUD PUBLISHINGS FOR DIFFERENT FLAGS AND SAMPLE INFORMATION
*/

// Sample Process
// Default sample arm position will be flush, at the end of a sample cycle
// On Scheduled sample time, user input, or specified rain event
// TIMER?
void takeSample(String triggerType = "unknown"){ //will likely turn sample into an interrupt or something similar
    // First check sample availability/count
    if(TestSampler.sampleCounter < TestSampler.numSamples){ // Samples available 
        if(Samples[TestSampler.sampleCounter].sampleFull == false){ // and this sample is empty
            flushSystem(); // Flush System //tbd 

            // fillSample
            // maybe change to Samples[sampleCounter].fill or something with degree values internal
            // servo control TO BE DEFINED - will adjust arg needs
            servoSample(TestSampler.sampleCounter, TestSampler.degreesPerSample); // Position servo to next available sample
            RunPump(); // Flow meter ISR calls pumpOff if flow == sampleVolume
            // might be better to put pump control conditions

            //check water is flowing
            pumpingFails();

            // update necessary sample bottle statuses
            Samples[TestSampler.sampleCounter].sampleFull;
            // Samples[TestSampler.sampleCounter].sampleTime //sampleTime tbd
            Samples[TestSampler.sampleCounter].triggerType = triggerType;
            // others?
        } //if this sample is full, skip and increment

        //publish this sample info
        void publishSampleState();
        TestSampler.sampleCounter++; //increment counter when sample full
    }else {// If the TestSampler.sampleCounter >= TestSampler.numSamples, a notice is sent to the cloud along with the sample info. 
        // No sample containers available
        // Set flag, notify user
        TestSampler.samplesFull = true;       
    }
    

    // Reset flow and servo to flush position
    flow = 0;
    servoFlush(); //might not reset

    //publish status
    void publishSamplerState();

    // sleep after publishing everything necessary
    SystemSleepResult result = System.sleep(config); //device to sleep 
    //return; //do I need return here?
}




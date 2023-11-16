/*
 * Project AutomationControl
 * Description: MAIN for Sampler Control
 * Author: Joseph DeMarco
 * Date: 10/15/2023
 * 
 * Updated: 11/11
 */

#include <Particle.h>
#include <string>
#include "DataStructures.h"
// #include "FlowMeter.ino"
// #include "PumpFunction.ino"

SYSTEM_MODE(SEMI_AUTOMATIC);
SYSTEM_THREAD(ENABLED); 


void setup() {
    // pin setup
    pinMode(pump, OUTPUT);
    pinMode(servo, OUTPUT);
    pinMode(flowMeter, INPUT_PULLUP); //will need to change to PULLUP or PULLDOWN unsure
    // pinMode(led1, OUTPUT); //LEDs to show sleeping or awake
    // pinMode(LED0, OUTPUT);

    //interrupt set up
    attachInterrupt(flowMeter, flowMeter_ISR, FALLING);
    //attachInterrupt(RainSensor, rainMeasure_ISR, FALLING); //Not positive this is how this will work

    //bottle array instantiation
    for(int i=0; i<24; i++){
        Samples[i] = SampleBottle(i+1); // ID = 0->23 +1-> 1->24
    }

    //Sleep configuration
    // config.mode(SystemSleepMode::ULTRA_LOW_POWER)       // define the properties of our sleep config object
    //       //.network(NETWORK_INTERFACE_CELLULAR)        // should wakeup due to network or cellular connection
    //       //.flag(SystemSleepFlag::WAIT_CLOUD)
	// 	  .gpio(RainSensor, FALLING)                    // specify wakeup if falling edge on WAKEUP_PIN
	// 	  .duration(sleepTime*1000);                    // or wakeup after duration in ms (unlike classic sleep fn)
    //    .duration(2min);       // alternative way to specify duration in minutes

    // cloud functions
    Particle.function("Take Sample Now", takeSampleNow);
    Particle.function("Set Sampler Configuration", setSampleConfig);
}




void loop() {

    // take sample if flag calls
    if(immSampleFlag == true){
        takeSample("Manual Sample"); 
    }
    if(periodicSampleFlag == true){
        takeSample("Periodic Sample");
    }
        

}


/**
 * Main sample functionality function
 * 
 * flush, sample, publish
*/
// Sample Process
// Default sample arm position will be flush, at the end of a sample cycle
// On Scheduled sample time, user input, or //specified rain event
void takeSample(String triggerType = "unknown"){ 
    // First check sample availability/count
    if(TestSampler.sampleCounter < TestSampler.numSamples){ // Samples available 
        if(Samples[TestSampler.sampleCounter].sampleFull == false){ // and this sample is empty
            flushSystem(); // Flush System //tbd 
            if(Samples[TestSampler.sampleCounter].sampleFailed == false){ //if flush successful
                // fillSample
                servoSample(TestSampler.sampleCounter, TestSampler.degreesPerSample); // Position servo to next available sample
                RunPump();
                int sampleStart = Time.now();

                //// While loop with condition on flow and timeout
                while(flow < (TestSampler.sampleVolume-50) && (Time.now()-sampleStart < 50)){
                    calcWaterFlow();
                }
                pumpOff();
                
                //sample failure: if flow doesn't reach sample volume
                if(flow<TestSampler.sampleVolume-50){
                    // set fail message and flag
                    Samples[TestSampler.sampleCounter].statusMessage = "Sample failed: insufficient flow";
                    Samples[TestSampler.sampleCounter].sampleFailed = true;
                    Samples[TestSampler.sampleCounter].sampleFull = false;
                } else {
                    Samples[TestSampler.sampleCounter].statusMessage = "Sample successful";
                    Samples[TestSampler.sampleCounter].sampleFailed = false;
                    Samples[TestSampler.sampleCounter].sampleFull = true;
                }
            }
            // update necessary sample bottle statuses
            // Samples[TestSampler.sampleCounter].sampleTime //sampleTime tbd
            Samples[TestSampler.sampleCounter].triggerType = triggerType;
            Samples[TestSampler.sampleCounter].volumeOfSample = flow;
            
            // others?
        } //if this sample is full, skip and increment

        //publish this sample info
        void publishSampleState();
        TestSampler.sampleCounter++; //increment counter when sample full
    }else {// If the TestSampler.sampleCounter >= TestSampler.numSamples, a notice is sent to the cloud along with the sample info. 
        // No sample containers available
        // Set flag
        TestSampler.samplesFull = true;      
    }
    
    // reset flow counter and publish sampler state
    flowPulseCount = 0;
    publishSamplerState();

    ////////////// Reset Sample Triggers ////////////////////
    if(triggerType == "Manual Sample"){
        immSampleFlag = false;
    }
    

    // sleep after publishing everything necessary
    // SystemSleepResult result = System.sleep(config); //device to sleep 
}




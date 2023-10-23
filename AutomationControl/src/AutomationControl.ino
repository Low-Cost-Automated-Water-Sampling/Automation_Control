/*
 * Project AutomationControl
 * Description: MAIN for Sampler Control
 * Author: Joseph DeMarco
 * Date: 10/15/2023
 * 
 * Updated: 10/16
 */

#include <Particle.h>
#include <string>
#include <DataStructures.h>


SYSTEM_MODE(SEMI_AUTOMATIC);
SYSTEM_THREAD(ENABLED);


// setup() runs once, when the device is first turned on.
void setup() {
    // Put initialization like pinMode and begin functions here.
    // Set the sensor pin as an input
    pinMode(pump, OUTPUT);
    pinMode(servo, OUTPUT);
    pinMode(flowMeter, INPUT); //will need to change to PULLUP or PULLDOWN unsure
    attachInterrupt(flowMeter, flowMeter_ISR, RISING);

    pinMode(led1, OUTPUT);
    pinMode(LED0, OUTPUT);
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
    // The core of your code will likely live here.
    calcWaterFlow();

    //Including Sleep control and rain meter
    SystemSleepConfiguration config;                    // declare a sleep config object called config
    config.mode(SystemSleepMode::ULTRA_LOW_POWER)                  // define the properties of our sleep config object
          //.network(NETWORK_INTERFACE_CELLULAR)          // should wakeup due to network or cellular connection
          //.flag(SystemSleepFlag::WAIT_CLOUD)
		  .gpio(WAKEUP_PIN, FALLING)                    // specify wakeup if falling edge on WAKEUP_PIN
		  .duration(sleepTime*1000);                    // or wakeup after duration in ms (unlike classic sleep fn)
    //    .duration(2min);                              // alternative way to specify duration in minutes		  
    digitalWrite(LED0,LOW);                             // turn off led before going to sleep
	SystemSleepResult result = System.sleep(config);    // go to sleep
    digitalWrite(LED0, HIGH);                           // turn on the led when we wake up
    delay(10000);       

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
void takeSample(){ //will likely turn sample into an interrupt or something similar
// First check sample availability/count
    if(sampleCounter < numSamples){ // Samples available:  //sampleFull will be a sample bottle attribute
    //maybe: if(Samples[sampleCounter].sampleFull == false)
		flushSystem(); // Flush System //tbd 

        // TakeSample
        // maybe change to Samples[sampleCounter].fill or something with degree values internal
        servoSample(sampleCounter, degreesPerSample); // Position servo to next available sample
        RunPump();
        // while(flow < sampleVolume){} // Power pump until sample size reached -- first idea
        // Flow meter ISR calls pumpOff if flow == sampleVolume

        //check water is flowing
        pumpingFails();

        // update necessary sample bottle statuses
        // Samples[sampleCounter].sampleFull // Change status of sample, sample count? – Need to define DATA STRUCTURES
        // Samples[sampleCounter].sampleTime
        // Samples[sampleCounter].triggerType
        // others?
    }
    else{// If the sampleCounter >= numSamples, a flag is raised, and a notice is sent to the cloud along with the sample info. 
        samplesFull = true;
        // No sample containers available
        // Set flag, notify user, device to sleep 
        // samplesFull = TRUE
        // SOMEONE ELSE PUBLISH DESIRED INFO TO CLOUD
    }
    
    // increment sampleCounter and reset 
    // Reset servo to flush position
    sampleCounter++;
    flow = 0;
    servoFlush();

    //PUBLISH DESIRED INFO

    // sleep after publishing everything necessary
    //deviceSleep(); //whatever sleep function is, I haven't seen it
}




void flushSystem() {
	servoFlush(); // ServoFlush: Set servo motor to flush position
    //default position should be flush, but need a way to check. perhaps when I learn more about servos

	//while(flow < sampleVolume){ //Change sample volume to volume of system to be flushed
	RunPump();
    // flow meter ISR shuts off pump pumpOff() when flow == sampleVolume

    // need to check if water is actually flowing when pump is on
    pumpingFails();
	
    flow = 0; //reset flow measure
}


void servoFlush(){ // will be Minh
	//set the servo to flush position
}
void servoSample(int sample, float degrees){
    //move servo to sample * degrees
}


// Include monitoring if water is not flowing
void pumpingFails(){
    // if no water after 3s
        delay(3000);
        if(flow == 0){ //if no water, turn off pump
            pumpOff();
            // NEED ERROR MESSAGES
            //include some message, maybe sample failed
        }
        //if sample insufficient water in 40s, fail, pump off
        delay(40000);
        if(flow<(1/2*sampleVolume)){
            pumpOff();
            // NEED ERROR MESSAGE
        }
}







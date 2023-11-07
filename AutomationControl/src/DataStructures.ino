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
    SystemSleepResult result = System.sleep(config); //sleep after incrementing or call calculate first?
}
/**
 * calculates rain fall amount in inches based on defined conversion and rain sensor pulses
*/
void calculateRainfall(){
    rainAmount = rainPulseCounter/pulsesPerInchRain; // flow in liters
    return;
}


/**
 * Beginnings of JSON digesting
*/

// void JSONHandler(){
//     // something to receive

//     if()
// }

/**
 * publish status variables and others about the sampler machine
 * 
 * need to include rain event needs
*/
void publishSamplerState(){
    if (Particle.connected()){
            Particle.publish("testEvent", String::format("{\"deviceName\":\"%s\",\"siteName\":\"%s\",\"sampleVolume\":%f,\"numSamples\":%d,\"samplesFull\":%d,\"sampleCounter\":%d,\"bottleStatus\":[%d, %d, %d, %d, %d, %d, %d, %d ,%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d ,%d, %d, %d, %d]}", 
                TestSampler.deviceName, TestSampler.siteName, TestSampler.sampleVolume, TestSampler.numSamples, TestSampler.samplesFull, TestSampler.sampleCounter,Samples[0].sampleFull,Samples[1].sampleFull,Samples[2].sampleFull,Samples[3].sampleFull,Samples[4].sampleFull,Samples[5].sampleFull,
                Samples[6].sampleFull,Samples[7].sampleFull,Samples[8].sampleFull,Samples[9].sampleFull,Samples[10].sampleFull,Samples[11].sampleFull,Samples[12].sampleFull,Samples[13].sampleFull,Samples[14].sampleFull,Samples[15].sampleFull,Samples[16].sampleFull,Samples[17].sampleFull,
                Samples[18].sampleFull,Samples[19].sampleFull,Samples[20].sampleFull,Samples[21].sampleFull,Samples[22].sampleFull,Samples[23].sampleFull));
    }
}


/**
 * publish status of current sample
 * 
 * used after taking a sample
 * 
 * need to add sampleTime
*/
void publishSampleState(){
    if (Particle.connected()){
            Particle.publish("testEvent", String::format("{\"deviceName\":\"%s\",\"siteName\":\"%s\",\"id\":%d,\"sampleFull\":%d,\"sampleFailed\":%d,\"volumeOfSample\":%f,\"triggerType\":\"%s\"}", 
                TestSampler.deviceName, TestSampler.siteName, Samples[TestSampler.sampleCounter].id,Samples[TestSampler.sampleCounter].sampleFull, 
                Samples[TestSampler.sampleCounter].sampleFailed, Samples[TestSampler.sampleCounter].volumeOfSample), Samples[TestSampler.sampleCounter].triggerType);

    }
}


/**
 * takeSampleNow cloud function
 * 
 * Joseph DeMarco
*/
int takeSampleNow(String sampleNowIn) {
    String sampleNow = "false";
    //{command:"bool"}

    jsonParser.clear();                 // clear the parser buffer
    jsonParser.addString(sampleNowIn);  // copy the received json cmmand data into the parser buffer // command = cmd
 
    // split up json data internally
    if(jsonParser.parse()) {
        // traverse simple json command structure
        sampleNow = jsonParser.getReference().key("command").valueString();
    }
    
    if(sampleNow == "true") {
        takeSample("Manual Sample"); 
        sampleNow = "false";
        return 1;
    }
    else if(sampleNow == "false") {
        return 0;
    }
    else { //DO WE HAVE ERRORS OR SHOULD IT NOT MATTER BC IT IS BACKEND WORK?
        return -1;
    }
}



int setSampleConfig(String configuration) {
    String sampleNow = "false";
    //{command:"bool"}

    jsonParser.clear();                 // clear the parser buffer
    jsonParser.addString(configuration);  // copy the received json cmmand data into the parser buffer // command = cmd
 
    // split up json data internally
    if(jsonParser.parse()) {
        // traverse simple json command structure
        sampleNow = jsonParser.getReference().key("command").valueString();
    }
    
    if(sampleNow == "true") {
        takeSample("Manual Sample"); 
        sampleNow = "false";
        return 1;
    }
    else if(sampleNow == "false") {
        return 0;
    }
    else { //DO WE HAVE ERRORS OR SHOULD IT NOT MATTER BC IT IS BACKEND WORK?
        return -1;
    }
}



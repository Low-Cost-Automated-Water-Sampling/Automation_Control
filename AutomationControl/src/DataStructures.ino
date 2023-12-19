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
#include "AutomationControl.ino"


/**
 * Joseph DeMarco
 * 
 * Flush system function: move to flush servo, run pump
*/
void flushSystem() {
	servoFlush(); // ServoFlush: Set servo motor to flush position

	RunPump();
    //pump off control, flow reached or timeout
    int sampleStart = Time.now();
	while((flow < 180) && (Time.now()-sampleStart < 50)){
       calcWaterFlow(); 
    }
    pumpOff();
    //if flow wasn't reached, sample failed
    if(flow<180){
        Samples[TestSampler.sampleCounter].statusMessage = "Flushing failed: insufficient flow";
        Samples[TestSampler.sampleCounter].sampleFailed = true;
        Samples[TestSampler.sampleCounter].sampleFull = false;
    }
    flowPulseCount = 0; //reset flow measure
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
// void pumpingFails(){
//     // if no water after 4s
//     delay(4000);
//     if(flow == 0){ //if no water, turn off pump
//         Samples[TestSampler.sampleCounter].sampleFailed = true;
//         pumpOff();
//         // NEED ERROR MESSAGES
//         //include some message, maybe sample failed
//     }
//     //if sample insufficient water in 40s, fail, pump off
//     delay(40000);
//     if(flow<(1/2*TestSampler.sampleVolume)){
//         Samples[TestSampler.sampleCounter].sampleFailed = true;
//         pumpOff();
//         // NEED ERROR MESSAGE
//     }
//     return;
// }


void rainMeasure_ISR(){
	int timespan = 6 * 3600; //hours * seconds
	if ((time.now() - lastrainevent) >= timespan) {
		rainPulseCounter++;
		if(rainAmount >= TestSampler.rainEvent) { // 50 mL threshold
			takeSample("Rain Event Sample"); // is there a way to call this asycronously (ex. PublishQueuePosixRK lib)
			rainPulseCounter = 0; // reset rain pulse counter
			lastrainevent = time.now(); // resync time for next round
		}
	}

    // SystemSleepResult result = System.sleep(config); //sleep after incrementing or call calculate first?
}

/**
 * calculates rain fall amount in inches based on defined conversion and rain sensor pulses
*/
void calculateRainfall() {
    rainAmount = rainPulseCounter/pulsesPerInchRain; // flow in liters
    return;
}



/**
 * publish status variables and others about the sampler machine
 * 
 * need to include rain event needs
 * 
 * MAY NEED TO CHANGE sampleFull DATA TYPE TO BE ABLE TO SEND AS STRING
*/
void publishSamplerState(){
    if (Particle.connected()){
            Particle.publish("Sampler State", String::format("{\"deviceName\":\"%s\",\"siteName\":\"%s\",\"sampleVolume\":%f,\"numSamples\":%d,\"samplesFull\":%d,\"sampleCounter\":%d,\"bottleStatus\":[%d, %d, %d, %d, %d, %d, %d, %d ,%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d ,%d, %d, %d, %d]}", 
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
            Particle.publish("Sample State", String::format("{\"deviceName\":\"%s\",\"siteName\":\"%s\",\"sampleFull\":%d,\"sampleFailed\":%d,\"volumeOfSample\":%f,\"triggerType\":\"%s\",\"statusMessage\":\"%s\"}", 
                TestSampler.deviceName, TestSampler.siteName,Samples[TestSampler.sampleCounter].sampleFull, 
                Samples[TestSampler.sampleCounter].sampleFailed, Samples[TestSampler.sampleCounter].volumeOfSample), Samples[TestSampler.sampleCounter].triggerType, Samples[TestSampler.sampleCounter].statusMessage);
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
        immSampleFlag = true; // trigger takeSample in loop
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



/**
 * Author: Ella Leon
 * Edited: Joseph DeMarco, 11/7
 * 
 * need to review to make sure all inputs are taken
 */
int setSampleConfig(String configuration) {
    String command = "false";
    /* 
        {
            "command": "setSamplingConfiguration",
            "deviceName": "ExampleWaterSamplerName",
            "siteName": "VillanovaExampleLake",
            "startTime": "2023-11-01T09:00:00Z", 
            "sampleInterval": "3600",
            "numSamples": 4,
            "sampleVolume": 250,
            "degreesPerSample": 360/(numSamples+1)// to change based on sample management solution 
            "rainEvent": 2.5;  //# inches of rain before automatic sample 
        }
    */

    jsonParser.clear();                   // clear the parser buffer
    jsonParser.addString(configuration);  // copy the received json command data into the parser buffer // command = cmd
 
    // split up json data internally
    if(jsonParser.parse()) {
        // traverse simple json command structure
        command = jsonParser.getReference().key("command").valueString();
        TestSampler.deviceName = jsonParser.getReference().key("deviceName").valueString();
        TestSampler.siteName = jsonParser.getReference().key("siteName").valueString();
        //startTime = jsonParser.getReference().key("startTime").valueString();

        TestSampler.sampleInterval = jsonParser.getReference().key("sampleInterval").valueInt();
        TestSampler.numSamples = jsonParser.getReference().key("numSamples").valueInt();
        TestSampler.sampleVolume = jsonParser.getReference().key("sampleVolume").valueFloat();
    }
    
    // if(command == "true") {
        
    //     command = "false";
    return 1;
    // }
    // else if(command == "false") {
    //     return 0;
    // }
    // else { //DO WE HAVE ERRORS OR SHOULD IT NOT MATTER BC IT IS BACKEND WORK?
    //     return -1;
    // }
}



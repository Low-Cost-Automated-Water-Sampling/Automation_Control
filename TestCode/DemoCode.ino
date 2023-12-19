/**
 * Joseph DeMarco
 * 
 * Demo code to show working functionalities
 */ 

// This #include statement was automatically added by the Particle IDE.
//#include <TimeAlarms.h>

// This #include statement was automatically added by the Particle IDE.
#include <JsonParserGeneratorRK.h>
JsonParserStatic<512, 40> jsonParser;
// let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);
// run the application and system concurrently in separate threads   
SYSTEM_THREAD(ENABLED);

#define LED D7
#define flowMeter A2

// device configuration
String deviceName = "Sampler";
String siteName = "CEER 005";
int numSamples = 12;
int sampleVolume = 500; // in mL
bool samplesFull = false;
int sampleCounter = 0;
int sampleReset = 0;
//int resetPublish = 0;

//imm sample
bool immSampleFlag = false;

//periodic sample
int period = 0;
int Dy, ho, mo, so;
int periodicActive = 0;
int start = Time.now();

// for scheduled
int mon, d, yr, hr, mins, sec;

// flow meter
volatile int flowPulseCount = 0; //pulses
// const float pulsesPerLiter = .45; //output pulses/mL
// const float pulsesPerLiter = 22;
const float pulsesPerLiter = 1;
volatile float flow = 0;

int pumpTimeout = 50;
int flushTimeout = 20;



// Status variables for publishing
String statusMessage = "default";
bool sampleFailed = false;
bool sampleFull = false;
String sampleTime = "default";
String triggerType = "default";
float volumeSampled = 0;



void takeSample(String triggerType);

void setup() {

    pinMode(LED, OUTPUT);
    
    // flow meter and interrupt
    pinMode(flowMeter, INPUT_PULLUP); //will need to change to PULLUP or PULLDOWN unsure
    attachInterrupt(flowMeter, flowMeter_ISR, FALLING);
    
    // cloud functions
    Particle.function("Set Period", setPeriod);
    Particle.function("Set Scheduled", setScheduled);
    Particle.function("Take Sample Now", takeSampleNow);
    Particle.function("Set Sampler Config", setSampleConfig);
    Particle.function("Reset Samples", resetSamples);
    
    Time.zone(-5); //set time zone utc -5, est with DST
}

void loop() {
    
    if(sampleCounter<numSamples){
        //imm sample
        if(immSampleFlag == true){
            takeSample("Manual Sample");  //"Manual Sample"
            immSampleFlag = false;
        }
        //periodic sample
        if(Time.now()-start >= period && periodicActive == 1){
            takeSample("Periodic Sample");
            start = Time.now(); //reset start to continue looping on period
        } 
        int schedCheck = Time.now();
        if((Time.month(schedCheck) == mon) && (Time.day(schedCheck) == d) && (Time.year(schedCheck) == yr) && (Time.hour(schedCheck) == hr) && (Time.minute(schedCheck) == mins) && (Time.second(schedCheck) == sec)){
            takeSample("Scheduled Sample");
        }
        
    }
        
    if(sampleReset == 1){
        //reset
        sampleCounter = 0;
        samplesFull = false;
        
        publishSamplerState();
        sampleReset = 0;
        //resetPublish = 0;
        
        immSampleFlag = false; // if sample triggered while samplesFull, need to reset imm sample because it will trigger right away after reset
        periodicActive = 0; // disable periodic so it doesn't continue to attempt 
    
    }

}

void publishFlush(){
    if (Particle.connected())
        {
            Particle.publish("Flush flow", String::format("{\"flow\":%f,\"flowCounter\":%i}", 
                flow, flowPulseCount));
        }
}

void takeSample(String triggerTypeIn = "unknown"){
    // digitalWrite(LED, HIGH);
    // delay(5000);
    // digitalWrite(LED, LOW);
    
    // Flush
    int flushPass = flushSystem();
    
    //if flush successful, sample
    if(flushPass){
        
        time_t sampleStart = Time.now();
        sampleTime = Time.format(sampleStart);
        runPump();
        //// While loop with condition on flow and timeout
        while(flow < (sampleVolume-50) && (Time.now()-sampleStart < pumpTimeout)){
            calcWaterFlow();
        }
        pumpOff();
        //sample failure: if flow doesn't reach sample volume
        if(flow<sampleVolume-50){
            // set fail message and flag
            statusMessage = "Sample failed: insufficient flow";
            sampleFailed = true;
            sampleFull = false;
        } else {
            statusMessage = "Sample successful";
            sampleFailed = false;
            sampleFull = true;
        }
        sampleCounter++; //even if sample failed, could be water in container, increment
    }
    triggerType = triggerTypeIn;
    volumeSampled = flow;
    
    publishSampleState();
    flowPulseCount = 0;
    flow = 0;
    

    if(sampleCounter >= numSamples){
        samplesFull = true;
    }
    publishSamplerState();
}

int flushSystem() {
	runPump();
    //pump off control, flow reached or timeout
    int flushStart = Time.now();
	while((flow < 180) && (Time.now()-flushStart < flushTimeout)){
       calcWaterFlow(); 
    }
    pumpOff();
    
    //if flow wasn't reached, sample failed
    if(flow<180){
        statusMessage = "Flushing failed: insufficient flow";
        sampleFailed = true;
        sampleFull = false;
        return 0;
    }
    publishFlush();
    flowPulseCount = 0; //reset flow measure
    flow = 0;
    
    return 1;
}


void runPump(){
	digitalWrite(LED, HIGH);
}

void pumpOff(){
	digitalWrite(LED, LOW);
}

void flowMeter_ISR(){
    flowPulseCount++;
    //noInterrupts();
}

void calcWaterFlow() {
    flow = flowPulseCount/pulsesPerLiter; // flow in mL
}

int takeSampleNow(String sampleNowIn) {
    String sampleNow = "false";
    //{command:true}

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

int setPeriod(String periodIn){
    int newPeriod = 0;
    int periodSet = 0;
    //input
    // {flag:1, D:0, H:0, M:0, S:5}
    
    jsonParser.clear();             // clear the parser buffer
    jsonParser.addString(periodIn);  // copy the received json cmmand data into the parser buffer // command = cmd
    if(jsonParser.parse()) {
        periodSet = jsonParser.getReference().key("flag").valueInt();
        // newPeriod = jsonParser.getReference().key("period").valueInt();
        Dy = jsonParser.getReference().key("D").valueInt();
        ho = jsonParser.getReference().key("H").valueInt();
        mo = jsonParser.getReference().key("M").valueInt();
        so = jsonParser.getReference().key("S").valueInt();
    }
    
    newPeriod = (Dy*86400)+(ho*3600)+(mo*60)+so;//(_hr_ * SECS_PER_HOUR + _min_ * SECS_PER_MIN + _sec_)

    //if(period != 0) {
    if(periodSet == 1){
        period = newPeriod;
        periodicActive = 1;
        start = Time.now();
        return 1;
    }
    //else if(period == 0) {
    else if(periodSet == 0) {
        periodicActive = 0;
        return 0;
    }
    else {
        return -1;
    }
}

int setScheduled(String ScheduledIn){
    int confirm = 0;
    //input
    // {confirm:1, month:11, day:30, year:2023, hour:9, minute:45, second:0}
    
    jsonParser.clear();             // clear the parser buffer
    jsonParser.addString(ScheduledIn);  // copy the received json cmmand data into the parser buffer // command = cmd
    if(jsonParser.parse()) {
        confirm = jsonParser.getReference().key("confirm").valueInt();
        mon = jsonParser.getReference().key("month").valueInt();
        d = jsonParser.getReference().key("day").valueInt();
        yr = jsonParser.getReference().key("year").valueInt();
        hr = jsonParser.getReference().key("hour").valueInt();
        mins = jsonParser.getReference().key("minute").valueInt();
        sec = jsonParser.getReference().key("second").valueInt();

    }
    
    if(confirm == 1){
        return 1;
    }
    else if(confirm == 0) {
        return 0;
    }
    else {
        return -1;
    }
}

// String statusMessage = "";
// bool sampleFailed = true;
// bool sampleFull = false;
// String sampleTime = "";
// String triggerType = "";
// int volumeSampled = 0;
void publishSampleState(){
    if (Particle.connected()){
            // Particle.publish("Sample State", String::format("{\"statusMessage\":\"%s\",\"sampleTime\":\"%s\",\"sampleFull\":%d,\"sampleFailed\":%d,\"volumeOfSample\":%f,\"triggerType\":\"%s\"}", 
            //      statusMessage.c_str(), sampleTime.c_str(), sampleFull, sampleFailed, volumeSampled, triggerType.c_str()));
            
            // publishes about 1/s or bursts of 4/s with recovery time of 4s
            Particle.publish("Sample State Messages", String::format("{\"statusMessage\":\"%s\",\"triggerType\":\"%s\",\"sampleTime\":\"%s\"}", statusMessage.c_str(), triggerType.c_str(), sampleTime.c_str()));
            Particle.publish("Sample State Flags", String::format("{\"sampleFull\":%d,\"sampleFailed\":%d,\"volumeOfSample\":%f}", sampleFull, sampleFailed, flow));
            
    }
}

int setSampleConfig(String configuration) {
    /* 
        {"deviceName": "Sampler","siteName": "CEER 005","numSamples": 4,"sampleVolume": 500}
    */

    jsonParser.clear();                   // clear the parser buffer
    jsonParser.addString(configuration);  // copy the received json command data into the parser buffer // command = cmd

    // split up json data internally
    if(jsonParser.parse()) {
        deviceName = jsonParser.getReference().key("deviceName").valueString();
        siteName = jsonParser.getReference().key("siteName").valueString();
        numSamples = jsonParser.getReference().key("numSamples").valueInt();
        sampleVolume = jsonParser.getReference().key("sampleVolume").valueFloat();
    }
    publishSamplerState();
    return 1;
}

// String deviceName = "Test Sampler";
// String siteName = "CEER 005";
// int numSamples = 12;
// int sampleVolume = 500; // in mL
// bool samplesFull = false;
// int sampleCounter = 0;
// int sampleReset = 0;
void publishSamplerState(){
    if (Particle.connected()){
            Particle.publish("Sampler", String::format("{\"device\":\"%s\",\"site\":\"%s\"}", 
                deviceName.c_str(), siteName.c_str()));
            Particle.publish("Sampler State", String::format("{\"volume\":%i,\"numSamples\":%d,\"samplesFull\":%d,\"sampleCount\":%d}", 
                sampleVolume, numSamples, samplesFull, sampleCounter));
    }
}


int resetSamples(String resetIn) {
    String reset = "false";
    //{command:true}

    jsonParser.clear();                 // clear the parser buffer
    jsonParser.addString(resetIn);  // copy the received json cmmand data into the parser buffer // command = cmd
 
    // split up json data internally
    if(jsonParser.parse()) {
        // traverse simple json command structure
        reset = jsonParser.getReference().key("command").valueString();
    }
    
    if(reset == "true") {
        sampleReset = 1;
        reset = "false";
        return 1;
    }
    else if(reset == "false") {
        return 0;
    }
    else { //DO WE HAVE ERRORS OR SHOULD IT NOT MATTER BC IT IS BACKEND WORK?
        return -1;
    }
}


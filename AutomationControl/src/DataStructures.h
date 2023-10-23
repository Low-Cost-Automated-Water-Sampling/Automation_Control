/**
 * Joseph DeMarco
 * 
 * Water Sampler Data Structures and Definitions
 * 10/23/2023
*/

#include <AutomationControl.ino>
#include <FlowMeter.ino>
#include <PumpFunction.ino>

// Pins
#define flowMeter D0//TEMPORARY RANDOM PINS TO BE REPLACED
#define servo D1
#define pump D2 
#define WAKEUP_PIN D3
#define LED0 D4
#define led1 D5

//constants
#define sleepTime 120        // sleep time in sec = 2 min
const float pulsesPerLiter = 450; //output pulses/liters

//Global variables
volatile int flowPulseCount;
volatile float flow;
//will be user defined
float sampleVolume = 450; //500mL bottle assuming leave room to not overflow
int numSamples = 24; //define number of samples available while empty
int sampleCounter = 0; //increment
float degreesPerSample = 360 / (numSamples+1); //sample bottles and flush spot split evenly
bool samplesFull = false;


/**
 * Sample bottle class
*/
class SampleBottle {
    public: 
        // adjust based on cloud publishing needs. Needs to be string???
        int id;
        //can be status (Available, Pending, Full) 
        bool sampleFull;
        bool sampleFailed;
        string triggerType; // (scheduled, commanded, rain gauge)
        float volumeOfSample;
        //sampleTime // data type tbd based on type used


};
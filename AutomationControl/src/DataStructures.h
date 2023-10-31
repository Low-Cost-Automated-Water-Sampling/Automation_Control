/**
 * Joseph DeMarco
 * 
 * Water Sampler Data Structures and Declarations
 * Data Structures header file
 * 10/23/2023
*/

// #include "AutomationControl.ino"
// #include "FlowMeter.ino"
// #include "PumpFunction.ino"

#include <Particle.h>
#include <string> //ISSUES
#include <array>

// Pins
#define flowMeter D0//TEMPORARY RANDOM PINS TO BE REPLACED
#define servo D1
#define pump D2 
#define RainSensor D3
#define LED0 D4
#define led1 D5

//constants
#define sleepTime 120        // sleep time in sec = 2 min


////////////////////////////////// Variables ///////////////////////////////////////////////////
const float pulsesPerLiter = 450; //output pulses/liters
const float pulsesPerInchRain = 2; //random definition, 2 pulses for 1 inch of rain

//Global variables
volatile int flowPulseCount;
volatile int rainPulseCounter;
volatile float flow;
volatile float rainAmount;

// object instantiations
class Samplebottle;
//set up sample array for 24 samples
SampleBottle Samples[24];


SystemSleepConfiguration config; // declare a sleep config object called config


///////////////////////////////// Classes //////////////////////////////////////////////

/**
 * Sampler object info class
*/
//will be user input defined
//CONSIDERING MAKING INTO "SAMPLER" class
class Sampler {
    public: 

        String siteName; 
        String deviceName;
        float sampleVolume; 
        int numSamples; //define number of samples available while empty
        int sampleCounter = 0; //increment
        float degreesPerSample = 360 / (numSamples+1); //sample bottles and flush spot split evenly
        bool samplesFull = false;
        float rainEvent; //# rain pulses?
        //need sample interval variable

        //constructor
        Sampler(String site, String device, int Volume = 500, int numberOfSamples = 24, float rain = 2){
            siteName = site;
            deviceName = device;
            sampleVolume = Volume;
            numSamples = numberOfSamples;
            rainAmount = rain;
        };
};

//Get user input somehow???? Is this in Setup or here?
Sampler TestSampler("CEER 005", "Sampler1");


/**
 * Sample bottle class
 */
// class SampleBottle; // declaration to potentially be used to export definition
class SampleBottle {
    public: 

        // adjust based on cloud publishing needs. Needs to be string???
        int id;
        //can be string status (Available, Pending, Full) 
        bool sampleFull = false;
        bool sampleFailed = false;
        //perhaps sample failed message
        String triggerType; // (scheduled, commanded, Rain Event Sample)
        float volumeOfSample = 0;
        //sampleTime // data type tbd based on type used

        //Constructor
        SampleBottle(int idInitializer){
            id = idInitializer;
        }

};

///////////////////////////////////////////////// Functions /////////////////////////////////////////////
// function declarations
void takeSample(String triggerType = "unknown");
void flowMeter_ISR();
void calcWaterFlow();

void RunPump();
void pumpOff();
void pumpingFails();

void flushSystem();
void servoFlush();
void servoSample(int sample, float degrees);

void rainMeasure_ISR();
void calculateRainfall();

void publishSamplerState();
void publishSampleState();
     
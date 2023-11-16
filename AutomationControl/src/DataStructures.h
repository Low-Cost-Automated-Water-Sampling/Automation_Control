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
#include "../lib/JsonParserGeneratorRK/src/JsonParserGeneratorRK.h"

// Pins
#define flowMeter A2//TEMPORARY RANDOM PINS TO BE REPLACED
#define servo D1
#define pump D2 
#define RainSensor D3
#define LED0 D4
#define led1 D5

//constants
//#define sleepTime 120        // sleep time in sec = 2 min


////////////////////////////////// Variables ///////////////////////////////////////////////////
volatile bool immSampleFlag = false;
volatile bool periodicSampleFlag = false;



////// Global variables
// flow meter
const float pulsesPermL = .45; //pulses per mL //450; //output pulses/liters
volatile int flowPulseCount = 0;
volatile float flow;

// rain sensor
// const float pulsesPerInchRain = 2; //random definition, 2 pulses for 1 inch of rain
// volatile int rainPulseCounter;
// volatile float rainAmount;

// object instantiations
class Samplebottle;
class Sampler;
//Get user input somehow???? Is this in Setup or here?
Sampler TestSampler = Sampler();
JsonParserStatic<256, 10> jsonParser;
//set up sample array for 24 samples
SampleBottle Samples[24];


//SystemSleepConfiguration config; // declare a sleep config object called config


///////////////////////////////// Classes //////////////////////////////////////////////

/**
 * Sampler object info class
*/
//will be user input defined
class Sampler {
    public: 
        String siteName; 
        String deviceName;

        float sampleVolume;
        //float rainEvent; //# rain pulses?
        float degreesPerSample = 360 / (numSamples+1); //sample bottles and flush spot split evenly

        int numSamples; //define number of samples available while empty
        int sampleCounter = 0; //increment

        bool samplesFull = false;
        int sampleInterval; // tbd sample period

        //constructor
        Sampler(String site = "CEER 005", String device = "Demo Sampler", int Volume = 500, int numberOfSamples = 24, float rain = 2){
            siteName = site;
            deviceName = device;
            sampleVolume = Volume;
            numSamples = numberOfSamples;
            //rainAmount = rain;
        };
};




/**
 * Sample bottle class
 */
// class SampleBottle; // declaration to potentially be used to export definition
class SampleBottle {
    public: 
        // adjust based on cloud publishing needs. Needs to be string???
        int id;

        bool sampleFull = false;
        String triggerType; // (scheduled, "Manual Sample", Rain Event Sample)
        float volumeOfSample = 0;

        String statusMessage;
        bool sampleFailed = false;

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

// cloud functions
int takeSampleNow(String sampleNow);
int setSampleConfig(String configuration);
     



#include "Particle.h"

SYSTEM_MODE(SEMI_AUTOMATIC);
SYSTEM_THREAD(ENABLED);

#define sleepTime 120        // sleep time in sec = 2 min
#define WAKEUP_PIN D2
#define LED0 D7
#define led1 D0;

void setup() {
    pinMode(led1, OUTPUT);
    pinMode(LED0, OUTPUT);
}

void loop() {
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
    delay(10000);                                       // stay awake for 10s before looping and going to sleep again
}

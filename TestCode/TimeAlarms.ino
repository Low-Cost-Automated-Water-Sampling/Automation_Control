/**
 * Joseph DeMarco and Ella Leon 
 * 
 * Cloud controled 
*/

#include <TimeAlarms.h>
#include <JsonParserGeneratorRK.h>



// let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

// run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

// instantiations
JsonParserStatic<256, 10> jsonParser;
TimeAlarmsClass Alarm1;
// TimeAlarmsClass Once;

AlarmID_t Alarm1ID;
AlarmID_t OnceID;
// //have to declare this function in the scope to be able to use it
// void disable(AlarmID_t ID);



//for alarm
int led2 = D0;
bool on7 = false;
int ho, mo, so;



//for timer
int led1 = D7; 
bool on = false;
int timerPeriod = 0;
int h, m, s;
int counter = 0;
bool isTimerSet = false;




int speedIn = 1000;

//const char *eventName = "testEvent";

void blink();
// int blinkControl(const char *data);
int blinkControl(String command);
void publishState(int speedIn, String control);
int D7Ctrl(String OCtrl);
void D7On();

void setup() {

    pinMode(led1, OUTPUT);
    pinMode(led2, OUTPUT);
    Particle.function("Alarm Period", blinkControl);
    Particle.function("Once time", D7Ctrl);
    // Alarm.timerRepeat(10, OnceOnlyTask);            // called once after 10 seconds 
}

void loop() {

    if(on7 == false){
            digitalWrite(led2, HIGH);
    }
    Alarm1.delay(1000);
    if(on == true){
        blink(speedIn);
        //publishState(speedIn, control);
    }else{
        digitalWrite(led1, LOW);
        //digitalWrite(led2, LOW);
        //publishState(speedIn, control);
    }
    // if(counter == 3){
    //     Alarm1.disable(Alarm1ID);
    //     on = false;
    // }
    //digitalWrite(led2, LOW);
  
}

int D7Ctrl(String OCtrl){

    int setD7On = 0;
    
    //input
    //{speed:10}
    //{Alarm:1, H:1, M:9, S:0}

    jsonParser.clear();             // clear the parser buffer
    jsonParser.addString(OCtrl);  // copy the received json cmmand data into the parser buffer // command = cmd
 

    // split up json data internally
    if(jsonParser.parse()) {
        // traverse simple json command structure
        // commandValue = jsonParser.getReference().key("command").valueString();
        // period = jsonParser.getReference().key("speed").valueInt();
        setD7On = jsonParser.getReference().key("Alarm").valueInt();
        ho = jsonParser.getReference().key("H").valueInt();
        mo = jsonParser.getReference().key("M").valueInt();
        so = jsonParser.getReference().key("S").valueInt();
    }
    
    
    //if(period != 0) {
    if(setD7On == 1){
        //timerPeriod = period;
        //Alarm.timerRepeat(timerPeriod, OnceOnlyTask);            // called once after 10 seconds 
        OnceID =  Alarm1.alarmOnce(h, m, s, D7On);
        return 1;
    }
    //else if(period == 0) {
    else if(setD7On == 0) {
        return 0;
    }
    else {
        return -1;
    }
}

void D7On(){
    on7 = true;
}




void OnceOnlyTask(){
    on = !on;
    counter++;
}


int blinkControl(String command) {
    //String commandValue;
    //int period = 0;
    int setAlarmOn = 0;

    //input
    //{speed:10}
    //{Alarm:1, H:0, M:0, S:5}

    // const char * const cmd = "{\"command\":\"on\",\"speed\":1000}";

    jsonParser.clear();             // clear the parser buffer
    jsonParser.addString(command);  // copy the received json cmmand data into the parser buffer // command = cmd
 

    // split up json data internally
    if(jsonParser.parse()) {
        // traverse simple json command structure
        // commandValue = jsonParser.getReference().key("command").valueString();
        // period = jsonParser.getReference().key("speed").valueInt();
        setAlarmOn = jsonParser.getReference().key("Alarm").valueInt();
        h = jsonParser.getReference().key("H").valueInt();
        m = jsonParser.getReference().key("M").valueInt();
        s = jsonParser.getReference().key("S").valueInt();
    }
    
    if(isTimerSet == true){ //if there is a timer already set, disable before starting next
        Alarm1.disable(Alarm1ID);
    }
    
    
    //if(period != 0) {
    if(setAlarmOn == 1){
        //timerPeriod = period;
        //Alarm.timerRepeat(timerPeriod, OnceOnlyTask);            // called once after 10 seconds 
        Alarm1ID =  Alarm1.timerRepeat(h, m, s, OnceOnlyTask);
        isTimerSet = true;
        return 1;
    }
    //else if(period == 0) {
    else if(setAlarmOn == 0) {
        return 0;
    }
    else {
        return -1;
    }
    
}











void blink(int speed){
        digitalWrite(led1, HIGH);
        //digitalWrite(led2, HIGH);
    
        delay(speed);
    
        digitalWrite(led1, LOW);
        //digitalWrite(led2, LOW);
    
        delay(speed); 
}


// void publishState(int speedIn, String control){
//     if (Particle.connected())
//         {
//             // char buf[256];
//             // snprintf(buf, sizeof(buf), 
//             //     "{\"speed\":%d,\"command\":\"%s\"}", 
//             //     speedIn, control.c_str());
//             // Particle.publish(eventName, buf);
//             Particle.publish("testEvent", String::format("{\"speed\":%d,\"command\":\"%s\"}", 
//                 speedIn, control.c_str()));
//             //Log.info("published %s", buf);
//         }
// }











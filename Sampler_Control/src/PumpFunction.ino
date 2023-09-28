/*
 * Project Pump_Function
 * Description: Function for Pump
 * Author: Brendan Lestange
 * Date: 9/28/2023
 */


int Pump = D4;  //Sets pin D4 to the Pump
// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.
pinmode(Pump, OUTPUT);
}
void RunPump(){
  digitalWRITE(Pump, HIGH);
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.
}
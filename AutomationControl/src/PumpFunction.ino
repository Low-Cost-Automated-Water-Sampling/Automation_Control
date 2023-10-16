/*
 * Project Pump_Function
 * Description: Function for Pump
 * Author: Brendan Lestange
 * Date: 9/28/2023
 */


int pump = D4;  //Sets pin D4 to the Pump

void RunPump(){
	digitalWrite(pump, HIGH);
  
}

void pumpOff(){
	digitalWrite(pump, LOW);
}

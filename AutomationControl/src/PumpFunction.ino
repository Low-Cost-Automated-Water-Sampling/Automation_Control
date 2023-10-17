/*
 * Project Pump_Function
 * Description: Function for Pump
 * Author: Brendan Lestange
 * Date: 9/28/2023
 */

/**
 * Joe: Need some pump power off safety mechanism in case flow is never triggered
*/
int pump = D4;  //Sets pin D4 to the Pump

void RunPump(){
	digitalWrite(pump, HIGH);
  
}

//Joe 10/15
void pumpOff(){
	digitalWrite(pump, LOW);
}

/**
 * Servo control
 * 
 */


#define servoPin D0

Servo myServo;

void setup() {
    
    pinMode(servoPin, OUTPUT);
    myServo.attach(servoPin); 
    
}

void loop() {
    myServo.write(0);   delay(3000); // Wait for 1 second     
    myServo.write(90);   delay(6000); // Wait for 1 second   
    myServo.write(180);   delay(10000); // Wait for 1 second}
}

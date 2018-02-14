/* 
 *  Claw Control Software
 *  Rai McKenzie, Group A9
 *  Created for UBC Engineering APSC 101 project
 *  Version: v0.3
 */

#include <NewPing.h> //Include NewPing library for higher accuracy
#include <Servo.h>

//Sonar sensor pin configurations
#define PIN_GND 8
#define PIN_ECHO 9
#define PIN_TRIGGER 10
#define PIN_VCC 11
#define PIN_LED_1 13
#define PIN_LED_2 12
#define SERVO_PIN 7

#define SERVO_SPEED_DELAY 5

/* ======================= */
#define SERVO_CLOSE 180
#define SERVO_OPEN 40
#define TRIGGER_DISTANCE 28 //(in cm)
/* ======================= */

#define MAX_DISTANCE 200

NewPing sonar(PIN_TRIGGER, PIN_ECHO, MAX_DISTANCE);
Servo servo;

int STATE = 0;
int TICK_SPEED = 100; //60
int countDown = 0;

/*
 * States:
 * 0 - Waiting open [Searching for object]
 * 1 - Pickup object [Put object in claw]
 * 2 - Waiting closed [Waiting for drop spot]
 * 3 - Open Claw [Drop object]
 * 4 - Waiting to reset
 */

void setup() {
  Serial.begin(9600); //Set data transmission rate to communicate with computer

  //Pins init
  pinMode(PIN_VCC, OUTPUT);
  digitalWrite(PIN_VCC, HIGH);
  pinMode(PIN_GND, OUTPUT);
  digitalWrite(PIN_GND, LOW);

  //LED pin init
  pinMode(PIN_LED_1, OUTPUT);
  pinMode(PIN_LED_2, OUTPUT);

  //Servo init
  servo.attach(SERVO_PIN);

  //Program init
  Serial.println("Starting...");
  openClaw();
  delay(1000);
}

void loop() {
  int distance = sonar.ping_cm();

  //Constant countdown decrement.
  countDown -= TICK_SPEED - 1;

  //Output sensor distance for debugging
  Serial.print("NewPing: ");
  Serial.print(distance);
  Serial.println("cm");

  Serial.println(STATE);
  Serial.println(countDown);

  if (STATE == 0) {
    openClaw();
    
  } else if (STATE == 1){
    
    if (countDown < 0) {
      STATE = 2;
      closeClaw();
      statusLED(LOW);
    } else if (countDown < 2500) {
      statusLEDToggle();
    } else if (countDown < 5000) {
      if (countDown % 4 == 0) {
        statusLEDToggle();
      }
    } else {
      statusLED(HIGH);
    }
    
  } else if (STATE == 2) {
    closeClaw();
    
  }

  if (STATE == 0 && distance < TRIGGER_DISTANCE && distance != 0) {
    STATE = 1;
    countDown = 10 * 1000;
  }

  if (STATE == 2 && distance < TRIGGER_DISTANCE && distance != 0) {
    STATE = 3;
    countDown = 3 * 1000;
    statusLED(HIGH);
  }

  if (STATE == 3){
    statusLED(LOW);
    if (distance < TRIGGER_DISTANCE && distance != 0) {
      if (countDown < 0) {
        openClaw();
        STATE = 4;
      }
    } else {
      STATE = 2;
    }
  }

  if (STATE == 4 && distance > TRIGGER_DISTANCE) {
    STATE = 0;
  }
  

  if (distance < TRIGGER_DISTANCE && distance > 0) {
    digitalWrite(PIN_LED_1, HIGH);
  } else {
    digitalWrite(PIN_LED_1, LOW);
  }


  delay(TICK_SPEED); //Tick speed
}

/* 
 * Close the claw.
 */
void closeClaw(){
  
  int loc = servo.read();
  for (int i = loc; i <= SERVO_CLOSE; i++) {
    servo.write(i);
    delay(SERVO_SPEED_DELAY);
  }
  return;
  
}

/* 
 * Open the claw.
 */
void openClaw(){
  
  int loc = servo.read();
  for (int i = loc; i >= SERVO_OPEN; i--) {
    servo.write(i);
    delay(SERVO_SPEED_DELAY);
  }
  return;
}

/* 
 * Set the status LED
 */
void statusLED(int status){
  digitalWrite(PIN_LED_2, status);
}

/* 
 * Toggle the status LED
 */
void statusLEDToggle() {
  digitalWrite(PIN_LED_2, !digitalRead(PIN_LED_2));
}


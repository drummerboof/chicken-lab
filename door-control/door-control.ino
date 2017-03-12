#include <AFMotor.h>

AF_DCMotor motor(3);

const String COMMAND_OPEN = "OPEN";
const String COMMAND_CLOSE = "CLOSE";
const String COMMAND_STOP = "STOP";

const int PIN_OPEN = 2;
const int PIN_CLOSE = 13;

bool motorOn = false;
int motorDirection;

String command = "";
bool commandReceived = false;

void setup () {
  command.reserve(10);
  Serial.begin(9600);
  motor.setSpeed(255);
  motor.run(RELEASE);
  pinMode(PIN_OPEN, INPUT);
  pinMode(PIN_CLOSE, INPUT);
  Serial.println("Door control ready...");
}

void loop () {
  bool newMotorOn = motorOn;
  int newMotorDirection = motorDirection;
  if (commandReceived) {
    Serial.println("Received command: " + command);
    
    if (command.equals(COMMAND_OPEN)) {
      Serial.println("opening...");
      newMotorDirection = FORWARD;
      newMotorOn = true;      
    } else if (command.equals(COMMAND_CLOSE)) {
      Serial.println("closing...");
      newMotorDirection = BACKWARD;
      newMotorOn = true;
    } else if (command == COMMAND_STOP) {
      newMotorOn = false;
    }
    
    commandReceived = false;
    command = "";
  }

  if (motorOn && (switchDown(PIN_OPEN) || switchDown(PIN_CLOSE))) {
    Serial.println("switch!");
    newMotorOn = false;
  }


  if (newMotorOn != motorOn) {
    if (newMotorOn) {
      Serial.println("motor on");
      motor.run(newMotorDirection);
    } else {
      Serial.println("motor off");
      motor.run(RELEASE);
    }
  }

  if (motorOn && newMotorDirection != motorDirection) {
    motor.setSpeed(0);
    delay(1000);
    motor.run(newMotorDirection);
    motor.setSpeed(255);
  }

  motorOn = newMotorOn;
  motorDirection = newMotorDirection;
}

void serialEvent () {  
  while (Serial.available()) {
    char inChar = (char)Serial.read(); 
    if (inChar == '\n') {
      commandReceived = true;
      break;
    } else {
      command += inChar;
    }
  }
}

bool switchDown (int pin) {
  int previousState;
  for (int i = 0; i < 25; i++) {
    int state = digitalRead(pin);
    if (state != previousState) {
      i = 0;
    }
    previousState = state;
  }
  return previousState == LOW;
}


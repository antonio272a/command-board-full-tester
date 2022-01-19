#define RELAY_220 2
#define RELAY_FCA 3
#define RELAY_FCF 4
#define RELAY_BUTTON 5
#define MOTOR_CLOSE_CHECK 6
#define MOTOR_OPEN_CHECK 7
#define ON_SWITCH 8
#define ON_LED 9
#define POWER_CHECK_LED 10
#define POWER_CHECK_BUTTON 11
#define RESET_BUTTON 12
#define LED_NOT_WORKING A0
#define LED_NOT_STOPPING A1
#define LED_OPEN A2
#define LED_CLOSE A3

#define RELAY_ACTING_TIME 1000
#define MOTOR_COURSE_TIME 3000
#define STOP_CHECK_DELAY 1000
#define LED_BLINK_DELAY 100
#define DELAY_TO_START_TEST 7000
#define DELAY_BETWEEN_TESTS 1000
#define DELAY_TO_CLOSE 1000

bool isOn = false;
bool isOpenCommand = true;
bool isWorking = true;
bool isStopping = true;
bool firstRun = true;
bool reset = false;

void setupPinModes() {
  pinMode(RELAY_220, OUTPUT);
  pinMode(RELAY_FCA, OUTPUT);
  pinMode(RELAY_FCF, OUTPUT);
  pinMode(RELAY_BUTTON, OUTPUT);
  pinMode(MOTOR_OPEN_CHECK, INPUT_PULLUP);
  pinMode(MOTOR_CLOSE_CHECK, INPUT_PULLUP);
  pinMode(ON_SWITCH, INPUT_PULLUP);
  pinMode(ON_LED, OUTPUT);
  pinMode(POWER_CHECK_LED, OUTPUT);
  pinMode(POWER_CHECK_BUTTON, INPUT_PULLUP);
  pinMode(RESET_BUTTON, INPUT_PULLUP);
  pinMode(LED_NOT_WORKING, OUTPUT);
  pinMode(LED_NOT_STOPPING, OUTPUT);
  pinMode(LED_OPEN, OUTPUT);
  pinMode(LED_CLOSE, OUTPUT); 
}

void setupRelayOutputsHigh() { // Relay Module requires HIGH on signal to stay off
  digitalWrite(RELAY_220, HIGH);
  digitalWrite(RELAY_FCA, HIGH);
  digitalWrite(RELAY_FCF, HIGH);
  digitalWrite(RELAY_BUTTON, HIGH);
}

void fireEvent() {
  digitalWrite(RELAY_BUTTON, LOW);
  delay(RELAY_ACTING_TIME);
  digitalWrite(RELAY_BUTTON, HIGH);
}

void checkEvent() {
  int motorSide = (isOpenCommand) ? MOTOR_OPEN_CHECK : MOTOR_CLOSE_CHECK;
  delay(MOTOR_COURSE_TIME);
  isWorking = digitalRead(motorSide) == LOW;
}

void stopMotor() {
  int motorSide = (isOpenCommand) ? RELAY_FCA : RELAY_FCF;
  digitalWrite(motorSide, LOW);
  delay(RELAY_ACTING_TIME);
  digitalWrite(motorSide, HIGH);
}

void checkIsStopping() {
  int motorSide = (isOpenCommand) ? MOTOR_OPEN_CHECK : MOTOR_CLOSE_CHECK;
  delay(STOP_CHECK_DELAY);
  isOpenCommand = !isOpenCommand;
  isStopping = digitalRead(MOTOR_OPEN_CHECK) == HIGH;
}

void checkForPowerChecker() {
  digitalWrite(POWER_CHECK_LED, digitalRead(POWER_CHECK_BUTTON) == LOW);
}

void checkIsOn() {
  int onInput = digitalRead(ON_SWITCH) == LOW;
  isOn = onInput;
  digitalWrite(ON_LED, onInput);
}

void blinkLeds() {
  digitalWrite(LED_NOT_WORKING, HIGH);
  digitalWrite(LED_NOT_STOPPING, HIGH);
  delay(LED_BLINK_DELAY);
  digitalWrite(LED_NOT_WORKING, LOW);
  digitalWrite(LED_NOT_STOPPING, LOW);
  delay(LED_BLINK_DELAY);
  digitalWrite(LED_NOT_WORKING, HIGH);
  digitalWrite(LED_NOT_STOPPING, HIGH);
  delay(LED_BLINK_DELAY);
  digitalWrite(LED_NOT_WORKING, LOW);
  digitalWrite(LED_NOT_STOPPING, LOW);
  delay(LED_BLINK_DELAY);
  digitalWrite(LED_NOT_WORKING, HIGH);
  digitalWrite(LED_NOT_STOPPING, HIGH);
  delay(LED_BLINK_DELAY);
  digitalWrite(LED_NOT_WORKING, LOW);
  digitalWrite(LED_NOT_STOPPING, LOW);
}

void resetTest() {
//  Serial.println(digitalRead(RESET_BUTTON));
  isWorking = true;
  isStopping = true;
  blinkLeds();
}

void switchLedSides() {
    int onLedSide = (isOpenCommand) ? LED_OPEN : LED_CLOSE;
    int offLedSide = (isOpenCommand) ? LED_CLOSE : LED_OPEN;
    digitalWrite(onLedSide, HIGH);
    digitalWrite(offLedSide, LOW);
  }

void checkFirstRun() {
  if(firstRun && isOn) {
    firstRun = false;
    delay(DELAY_TO_START_TEST);
    } else if (!isOn && !firstRun) {
      firstRun = true;
      }
  }

void setup() {
  setupPinModes();
  setupRelayOutputsHigh();
}

void checkForDelay() {
  if (!isOpenCommand){
    delay(DELAY_TO_CLOSE);
  } else {
    delay(DELAY_BETWEEN_TESTS);
  }
}

void loop() {
  checkForPowerChecker();
  checkIsOn();
  digitalWrite(RELAY_220, !isOn);
  checkFirstRun();
  reset = digitalRead(RESET_BUTTON) == LOW;

  if (isOn && isWorking && isStopping) {
    switchLedSides();
    fireEvent();
    checkEvent();
    stopMotor();
    checkIsStopping();
  }
  
  if (!isWorking) {
    digitalWrite(LED_NOT_WORKING, HIGH);
  }
  if (!isStopping) {
    digitalWrite(LED_NOT_STOPPING, HIGH);
  }  

  if(isWorking && isStopping) {
    checkForDelay();
  }
  
  if((reset && isOn) && (!isWorking || !isStopping)) {
    resetTest();
  }
}

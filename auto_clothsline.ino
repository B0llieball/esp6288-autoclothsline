int enable1Pin = D1;
int motorPin1 = D2;
int motorPin2 = D3;
int ledPin = D0;
int analogPin = A0; // Analog pin to read sensor value
int limitSwitchPinStop = D4; // Limit switch pin to stop motor if activated (immediate stop)
int limitSwitchPinToggleStart = D5; // Limit switch pin to toggle forward movement (start by default)
int limitSwitchPinToggleStop = D6; // Limit switch pin to toggle backward movement (stop by default)

int val = 0;
int dutyCycle = 170; // PWM duty cycle (0-255)

bool motorForwardState = false; // Track forward motor state (running or stopped)
bool motorBackwardState = false; // Track backward motor state (running or stopped)
bool lastStateToggleStart = HIGH; // Previous state of D5 toggle switch
bool lastStateToggleStop = HIGH; // Previous state of D6 toggle switch

unsigned long motorDelayStart = 0; // Start time for motor delay
bool motorDelayActive = false; // Whether the delay is active

void setup() {
  // Motor pins setup
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(enable1Pin, OUTPUT);

  // LED and Serial setup
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);

  // Limit switch setup
  pinMode(limitSwitchPinStop, INPUT_PULLUP); // Limit switch for immediate stop
  pinMode(limitSwitchPinToggleStart, INPUT_PULLUP); // Limit switch for toggling forward movement (default start)
  pinMode(limitSwitchPinToggleStop, INPUT_PULLUP); // Limit switch for toggling backward movement (default stop)

  // Set initial motor duty cycle
  analogWrite(enable1Pin, dutyCycle);
}

void loop() {
  // Read the sensor value
  val = analogRead(analogPin);  // Read analog signal from sensor
  Serial.print("val = "); // Print the value to the Serial monitor
  Serial.println(val);

  // Check the limit switch status for immediate stop
  int limitSwitchStateStop = digitalRead(limitSwitchPinStop);

  // If limit switch for immediate stop is pressed, stop the motor immediately
  if (limitSwitchStateStop == LOW) {
    // Stop the motor
    motorForwardState = false;
    motorBackwardState = false;
    motorDelayActive = false; // Cancel any active delay
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    Serial.println("Immediate stop limit switch activated. Stopping the motor.");
    delay(100);
    return; // Exit the loop early to avoid further motor actions
  }

  // Check the toggle switch on D5 for starting/stopping the motor forward
  int currentToggleStartState = digitalRead(limitSwitchPinToggleStart);
  if (currentToggleStartState == LOW && lastStateToggleStart == HIGH) {
    motorForwardState = !motorForwardState; // Toggle the forward motor state
    motorBackwardState = false; // Ensure backward state is off
    Serial.println("Toggle Start/Stop limit switch (D5) pressed. Forward motor state toggled.");
    delay(100); // Debounce delay
  }
  lastStateToggleStart = currentToggleStartState;

  // Check the toggle switch on D6 for stopping/starting the motor backward
  int currentToggleStopState = digitalRead(limitSwitchPinToggleStop);
  if (currentToggleStopState == LOW && lastStateToggleStop == HIGH) {
    motorBackwardState = !motorBackwardState; // Toggle the backward motor state
    motorForwardState = false; // Ensure forward state is off
    Serial.println("Toggle Stop/Start limit switch (D6) pressed. Backward motor state toggled.");
    delay(100); // Debounce delay
  }
  lastStateToggleStop = currentToggleStopState;

  // LED Control Section
  if (val < 500) { // Check sensor value, turn LED on if less than 500
    digitalWrite(ledPin, HIGH); // Turn LED on
  } else {
    digitalWrite(ledPin, LOW); // Turn LED off
  }

  // Motor Control Section
  if (motorForwardState && val < 900) {
    if (!motorDelayActive) {
      // Start the 5-second delay
      motorDelayActive = true;
      motorDelayStart = millis();
      Serial.println("Sensor value below 900. Starting delay before activating motor forward.");
    } else if (millis() - motorDelayStart >= 5000) {
      // If 5 seconds have passed, activate the motor forward
      digitalWrite(motorPin1, HIGH);
      digitalWrite(motorPin2, LOW);
      analogWrite(enable1Pin, dutyCycle); // Set motor speed
      Serial.println("Motor moving forward after 5-second delay.");
      motorDelayActive = false; // Reset delay state
    }
  } else if (motorBackwardState && val < 900) {
    if (!motorDelayActive) {
      // Start the 5-second delay
      motorDelayActive = true;
      motorDelayStart = millis();
      Serial.println("Sensor value below 900. Starting delay before activating motor backward.");
    } else if (millis() - motorDelayStart >= 5000) {
      // If 5 seconds have passed, activate the motor backward
      digitalWrite(motorPin1, LOW);
      digitalWrite(motorPin2, HIGH);
      analogWrite(enable1Pin, dutyCycle); // Set motor speed
      Serial.println("Motor moving backward after 5-second delay.");
      motorDelayActive = false; // Reset delay state
    }
  } else {
    // If neither motor state is active or val is >= 900, stop the motor
    motorDelayActive = false; // Cancel any active delay
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    Serial.println("Motor deactivated.");
  }

  // Add delay to balance the operations
  delay(100);
}

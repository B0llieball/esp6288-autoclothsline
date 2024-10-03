int enable1Pin = D1;
int motorPin1 = D2;
int motorPin2 = D3;
int ledPin = D0;
int analogPin = A0; // Analog pin to read sensor value
int limitSwitchPin = D4; // Limit switch pin
int val = 0;
int dutyCycle = 170; // PWM duty cycle (0-255)

void setup() {
  // Motor pins setup
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(enable1Pin, OUTPUT);

  // LED and Serial setup
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);

  // Limit switch setup
  pinMode(limitSwitchPin, INPUT_PULLUP); // Assuming the limit switch pulls LOW when activated

  // Set initial motor duty cycle
  analogWrite(enable1Pin, dutyCycle);
}

void loop() {
  // Read the sensor value
  val = analogRead(analogPin);  // Read analog signal from sensor
  Serial.print("val = "); // Print the value to the Serial monitor
  Serial.println(val);

  // Check the limit switch status
  int limitSwitchState = digitalRead(limitSwitchPin);

  // If limit switch is pressed, stop the motor immediately
  if (limitSwitchState == LOW) {
    // Stop the motor
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    Serial.println("Limit switch activated. Stopping the motor.");
    delay(100);
    return; // Exit the loop early to avoid further motor actions
  }

  // LED Control Section
  if (val < 500) { // Check sensor value, turn LED on if less than 500
    digitalWrite(ledPin, HIGH); // Turn LED on
  } else {
    digitalWrite(ledPin, LOW); // Turn LED off
  }

  // Motor Control Section
  if (val < 900) { // Activate motor if the sensor value is below 900
    // Move motor forward
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW);
    analogWrite(enable1Pin, dutyCycle); // Set motor speed
    delay(1000);

    // Check the limit switch again during the motor movement
    limitSwitchState = digitalRead(limitSwitchPin);
    if (limitSwitchState == LOW) {
      // Stop the motor if the limit switch is triggered
      digitalWrite(motorPin1, LOW);
      digitalWrite(motorPin2, LOW);
      Serial.println("Limit switch activated during movement. Stopping the motor.");
      delay(100);
      return;
    }

    // Stop the motor
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    delay(1000);

    // Move forward with increasing speed
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW);

    for (dutyCycle = 170; dutyCycle < 255; dutyCycle++) {
      analogWrite(enable1Pin, dutyCycle); // Adjust speed
      delay(50);

      // Check the limit switch during speed increase
      limitSwitchState = digitalRead(limitSwitchPin);
      if (limitSwitchState == LOW) {
        // Stop the motor if the limit switch is triggered
        digitalWrite(motorPin1, LOW);
        digitalWrite(motorPin2, LOW);
        Serial.println("Limit switch activated during speed increase. Stopping the motor.");
        delay(100);
        return;
      }
    }

    // Reset speed to initial duty cycle
    dutyCycle = 170;
    analogWrite(enable1Pin, dutyCycle);
  } else {
    // If sensor value is above 900, stop the motor
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
  }

  // Add delay to balance the operations
  delay(100);
}

/*
    Finger tremor detector code
*/

const bool DEBUG = true;

// main LED for detector
const int lSrc = 0;

// LEDs for display
const int l1 = 2;
const int l2 = 3;
const int l4 = 4;
const int l8 = 5;

int LEDpins[] = {lSrc, l1, l2, l4, l8};
int numLEDs = sizeof(LEDpins) / sizeof(int);

// switch pins
const int resetPin = 6;
const int powerPin = 7;

// booleans for keeping track of button state
bool resetDown = false;
bool powerDown = false;

// analog read pin for photoresistor
const int sensorPin = 5;

// calibration and data collection variables
int calData = 0;    // will hold analogRead values of sensor pin so that we can take the average value over a few seconds
int threshold = 0;  // average of calData

int delta = 15;     // hysteresis threshold
int interval = 50; // interval to check for differences with

int prevValue = 0;
unsigned long currentTime = 0;
unsigned long prevTime = 0;

// status variables
bool poweredOn = false;
int counter = 0;

// Setup Loop --------------------------------------------------------------//

void setup() {
  // setup LEDs
  for (int i = 0; i < numLEDs; ++i)
    pinMode(LEDpins[i], OUTPUT);

  // setup switches
  pinMode(resetPin, INPUT);
  pinMode(powerPin, INPUT);

  if (DEBUG) {
    Serial.begin(9600);
    //testBinary();
  }
}

// Main Loop ---------------------------------------------------------------//

void loop() {
  bool resetPressed = buttonPressed(resetPin, &resetDown);
  bool powerPressed = buttonPressed(powerPin, &powerDown);

  if (DEBUG) {
    if (resetPressed)
      Serial.println("Reset button pressed");
    if (powerPressed)
      Serial.println("Power button pressed");
  }

  if (poweredOn) { // normal functionality
    updateCounter(); // update counter value

    if (resetPressed) {
      counter = 0; // reset counter
      binaryDisplay(0);
    }

    // turn device off
    if (powerPressed) {
      Serial.println("Device OFF");
      poweredOn = false;
      terminate();
    }
  } else {
    // turn device on
    if (powerPressed) {
      Serial.println("Device ON");
      poweredOn = true;
      initialize();
    }
  }
}

// Helper functions --------------------------------------------------------//

// intialization routine
void initialize() {
  counter = 0; // reset counter
  calibrate();
  initLEDs();
}

void initLEDs () {
  for (int l = 0; l < 2; ++l) {
    for (int i = 0; i < numLEDs; ++i) {
      digitalWrite(LEDpins[i], HIGH);
      delay(100);
    }

    for (int i = 0; i < numLEDs; ++i) {
      digitalWrite(LEDpins[i], LOW);
      delay(100);
    }
  }
}

// gather data for 3 seconds to account for ambient light
int sample = 20;
void calibrate() {
  for (int z = 0; z < sample ; z++) {
    calData = analogRead(sensorPin) + calData;
    delay(10);
  }

  // calculate the average value
  prevValue = calData / sample;
}

// counts times finger crosses the beam
bool startedTremor = false;
void updateCounter() {
  currentTime = millis();
  if (currentTime - prevTime >= interval) {
    prevTime = currentTime;
    
    int pinValue = analogRead(sensorPin);
    //Serial.println(pinValue);
    
    if (pinValue > prevValue + delta) { // rising edge
      startedTremor = true;
      Serial.println("rising edge");
    } else if (startedTremor && (pinValue < prevValue - delta)) { // record a tremor
      Serial.println("falling edge: tremor detected");
      startedTremor = false;
  
      // change counter value
      if (counter < 15)
        ++counter;
      else
        counter = 0;
      binaryDisplay(counter); // if counter changes in value, display counter value
    }

    prevValue = pinValue;
  }
}

// displays value on LEDs in binary
void binaryDisplay(int value) {
  Serial.println(value);

  // NOTE: this loop starts at 1 to account for the LED source pin being
  // the fist pin in the array
  for (int i = 1; i < numLEDs; ++i) {
    digitalWrite(LEDpins[i], value & 1);
    value /= 2;
  }
}

// power down routine, blinks all LEDs on and off
void terminate() {
  for (int l = 0; l < 3; ++l) {
    for (int i = 0; i < numLEDs; ++i) {
      digitalWrite(LEDpins[i], HIGH);
    }
    delay(300);
    for (int i = 0; i < numLEDs; ++i) {
      digitalWrite(LEDpins[i], LOW);
    }
    delay(200);
  }
}

// returns if the button was just pressed
bool buttonPressed (int pinNumber, bool *pinDown) {
  bool buttonState = digitalRead(pinNumber);
  if (buttonState == HIGH) { // if button pressed
    if (!*pinDown) { // only on initial press
      *pinDown = true;
      return true;
    }
  } else {
    *pinDown = false;
  }
  return false;
}

// Testing functions --------------------------------------------------------//

void testBinary() {
  for (int i = 0; i <= 15; ++i) {
    binaryDisplay(i);
    delay(500);
  }
}




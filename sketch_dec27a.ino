/* Quiz Bowl Buzzer System program
 * Implements an Arduino Leonardo with its I/O ports used
 * for different buzzers and lights.
 * Date: December 27, 2012
 */
#define DEBUG true
#define BUZZERS 10
#define SOUNDPIN 10
#define RESETPIN 11
#define INDICATOR 13

/** Tells us whether someone has buzzed or not. */
boolean lockout = false;

/** The input array that tells us the state of the switches denoting
 *  the players. 
 */
int playerArray [10];

/** Describes the player that has buzzed. -1 if no one has buzzed. */
int activePlayer = -1;

/** Determines the inputs given to the board and acts accordingly. */
void getInputs();

/** Sounds the buzzer, indicating that someone has pushed a button. */
void activateBuzzer();

/** 
 * Lights up the LED of the player who buzzed.
 * Unfortunately it's rather brute-force for now.
 * We use a binary coding scheme to denote which LED should light up.
 */
void lightLED();

/** Shuts off all LED lights and such. */
void turnOffAllLights();

/** Waits for the user to press the reset button. */
void checkReset();

/**
 * Sets up the output pins and the input pins.
 * Current schema is:
 * Pins 0-9: Input pins from players. 
 * Pin 10: This pin activates the buzzer sound. We'll probably be
 * sending PWM through it or activating some oscillator.
 * Pin 11: This pin is connected to a reset switch.
 * Pin 12: This pin is unused.
 * Pin 13: This pin is used as a debug indicator.
 * Pins A0-A3: Output LED pins for a binary decoder (tbd - if this
 * proves to be too complicated, we'll be using more pins.)
 * Pin A4-A5: These pins are unused.
 */
void setup() {
  Serial.begin(9600);
  for(int i = 0; i < BUZZERS; i++) {
    pinMode(i, INPUT_PULLUP);
  }
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, INPUT);
  pinMode(RESETPIN, INPUT_PULLUP);
  pinMode(SOUNDPIN, OUTPUT);
  pinMode(INDICATOR, OUTPUT);
  digitalWrite(SOUNDPIN, LOW);
  digitalWrite(INDICATOR, LOW);
  digitalWrite(A0, LOW);
  digitalWrite(A1, LOW);
  digitalWrite(A2, LOW);
  digitalWrite(A3, LOW);
  randomSeed(analogRead(A4));
}

void loop() {
  if (!lockout) {
    getInputs();
  } else {
    checkReset();
  }
}

void getInputs() {
  int swap [10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  for (int i = 0; i < 10; i++) {
    int x = random(i, 9);
    int y = swap[x];
    int temp = swap[i];
    swap[i] = y;
    swap[x] = temp;
  }
  for (int i = 0; i < 10; i++) {
    boolean input = digitalRead(swap[i]);
    if (!input) {
      lockout = true;
      activePlayer = swap[i];
      if (DEBUG) {
        Serial.print("Player ");
        Serial.print(activePlayer + 1);
        Serial.println(" hit the buzzer!");
      }
      activateBuzzer();
      break;
    }
  }
}

void activateBuzzer() {
  lightLED();
  digitalWrite(SOUNDPIN, HIGH);
  digitalWrite(INDICATOR, HIGH);
  for (int i = 0; i < 1000; i++) {
    delay(1);
    if (!digitalRead(RESETPIN)) {
      performReset();
      return;
    }
  }
  digitalWrite(SOUNDPIN, LOW);
  digitalWrite(INDICATOR, LOW);
}

void lightLED() {
  switch(activePlayer) {
    case 0:
      digitalWrite(A0, HIGH);
      break;
    case 1:
      digitalWrite(A1, HIGH);
      break;
    case 2:
      digitalWrite(A0, HIGH);
      digitalWrite(A1, HIGH);
      break;
    case 3:
      digitalWrite(A2, HIGH);
      break;
    case 4:
      digitalWrite(A0, HIGH);
      digitalWrite(A2, HIGH);
      break;
    case 5:
      digitalWrite(A1, HIGH);
      digitalWrite(A2, HIGH);
      break;
    case 6:
      digitalWrite(A0, HIGH);
      digitalWrite(A1, HIGH);
      digitalWrite(A2, HIGH);
      break;
    case 7:
      digitalWrite(A3, HIGH);
      break;
    case 8:
      digitalWrite(A0, HIGH);
      digitalWrite(A3, HIGH);
      break;
    case 9:
      digitalWrite(A1, HIGH);
      digitalWrite(A3, HIGH);
      break;
    default:
      break;
  }
}

void turnOffAllLights() {
  digitalWrite(A0, LOW);
  digitalWrite(A1, LOW);
  digitalWrite(A2, LOW);
  digitalWrite(A3, LOW);
}

void checkReset() {
  if (!digitalRead(RESETPIN)) {
    performReset();
  }
}

void performReset() {
  lockout = false;
  turnOffAllLights();
  activePlayer = -1;
  if (DEBUG) {
    Serial.println("Reset.");
  }
  randomSeed(analogRead(A4));
}

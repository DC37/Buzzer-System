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

/** 
 * The array that tells us in which order we will be checking the buzzers
 * this time around.
 */
int randArray[10];

/** Describes the player that has buzzed. -1 if no one has buzzed. */
int activePlayer = -1;

/** Gives the number of turns left until we reseed the random number generator. */
int resetTimer;

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

/** 
 * Decodes the multiplexed matrix of LED lighting and outputs to the 
 * according pins.
 */
void decodeMatrix(boolean aZero, boolean aOne, boolean aTwo, boolean aThree,
    boolean aFour, boolean eleven, boolean twelve);

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
 * Pins A0-A4: This is a multiplexed LED array output. We'll be setting
 * these pins alternately to HIGH and LOW to code for 9 different LEDs.
 * Pin A5: This is used to seed the random number generator.
 * Pin 12: This is the last pin for the multiplexed LED array output.
 * Pin 13: This pin is for player 10.
 */
void setup() {
  if (DEBUG) {
    Serial.begin(9600);
  }
  for(int i = 0; i < BUZZERS; i++) {
    pinMode(i, INPUT_PULLUP);
  }
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(A5, INPUT);
  pinMode(RESETPIN, INPUT_PULLUP);
  pinMode(SOUNDPIN, OUTPUT);
  pinMode(INDICATOR, OUTPUT);
  digitalWrite(SOUNDPIN, LOW);
  digitalWrite(INDICATOR, LOW);
  digitalWrite(A0, LOW);
  digitalWrite(A1, LOW);
  digitalWrite(A2, LOW);
  digitalWrite(A3, LOW);
  digitalWrite(A4, LOW);
  digitalWrite(11, LOW);
  digitalWrite(12, LOW);
  digitalWrite(13, LOW);
  randomSeed(analogRead(A5));
  resetTimer = random(1, analogRead(A5));
}

void loop() {
  for (int i = 0; i < 10; i++) {
    randArray[i] = i;
  }  
  for (int i = 0; i < 10; i++) {
    int x = random(i, 9);
    int y = randArray[x];
    int temp = randArray[i];
    randArray[i] = y;
    randArray[x] = temp;
  }
  while (!lockout) {
    getInputs();
  }
  activateBuzzer();
  while (lockout) {
    checkReset();
  }
}

void getInputs() {
  for (int i = 0; i < 10; i++) {
    boolean input = digitalRead(randArray[i]);
    if (!input) {
      lockout = true;
      activePlayer = randArray[i];
      if (DEBUG) {
        Serial.print("Player ");
        Serial.print(activePlayer + 1);
        Serial.println(" hit the buzzer!");
      }
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

/**
 * The LED matrix is defined as follows:
 * Sources: A0, A1, A2
 * Sinks: A4, 11, 12
 */
void lightLED() {
  switch(activePlayer) {
    case 0:
      decodeMatrix(true, false, false, false, true, true);
      break;
    case 1:
      decodeMatrix(false, true, false, false, true, true);
      break;
    case 2:
      decodeMatrix(false, false, true, false, true, true);
      break;
    case 3:
      decodeMatrix(true, false, false, true, false, true);
      break;
    case 4:
      decodeMatrix(false, true, false, true, false, true);
      break;
    case 5:
      decodeMatrix(false, false, true, true, false, true);
      break;
    case 6:
      decodeMatrix(true, false, false, true, true, false);
      break;
    case 7:
      decodeMatrix(false, true, false, true, true, false);
      break;
    case 8:
      decodeMatrix(false, false, true, true, true, false);
      break;
    case 9:
      decodeMatrix(false, false, false, false, false, false);
      digitalWrite(13, true);
      break;
    default:
      break;
  }
}

void decodeMatrix(boolean aZero, boolean aOne, boolean aTwo, boolean aThree,
  boolean aFour, boolean twelve) {
  digitalWrite(A0, aZero);
  digitalWrite(A1, aOne);
  digitalWrite(A2, aTwo);
  digitalWrite(A3, aThree);
  digitalWrite(A4, aFour);
  digitalWrite(12, twelve);
}


void turnOffAllLights() {
  digitalWrite(A0, LOW);
  digitalWrite(A1, LOW);
  digitalWrite(A2, LOW);
  digitalWrite(A3, LOW);
  digitalWrite(A4, LOW);
  digitalWrite(12, LOW);
  digitalWrite(13, LOW);
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
  resetTimer--;
  if (resetTimer <= 0) {
    randomSeed(analogRead(A5));
  }
  resetTimer = random(1, analogRead(A5));
}

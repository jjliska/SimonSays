/*
 * Project M3
 *  Teammates: 
 *    Joshua Liska
 *    Paysen Bramblett
 *    Fernando Hernandez-Borboa
 *    
 *  References:
 *    keypadExample01
 *    https://howtomechatronics.com/tutorials/arduino/how-to-use-a-rgb-led-with-arduino/
 *    Part_1_03_Playing_Music (Teensy Audio Example)
 *    
 *  Use-case:
 *    - Press the * to start the game
 *    - Press the corresponding button on the keypad to the corresponding light
 *    - After a series of 5 numbers if the correct buttons are pressed in succession then the user will "win".
 */

#include <Keypad.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioPlaySdWav           playSdWav1;     //xy=266,291
AudioMixer4              mixer1;         //xy=464,298
AudioOutputAnalog        dac1;           //xy=630,297
AudioConnection          patchCord1(playSdWav1, 0, mixer1, 0);
AudioConnection          patchCord2(playSdWav1, 1, mixer1, 1);
AudioConnection          patchCord3(mixer1, dac1);
// GUItool: end automatically generated code

//Pin definitions
#define SDCARD_CS_PIN    BUILTIN_SDCARD
#define SDCARD_MOSI_PIN  11  // not actually used
#define SDCARD_SCK_PIN   13  // not actually used

#define GRID_ONE 2
#define GRID_TWO 3
#define GRID_THREE 4
#define GRID_FOUR 5
#define GRID_FIVE 6
#define GRID_SIX 7
#define GRID_SEVEN 8
#define GRID_EIGHT 9
#define GRID_NINE 10

#define LED_ONE 23
#define LED_TWO 22
#define LED_THREE 21
#define LED_FOUR 20
#define LED_FIVE 14

#define STATE_LED_RED 38
#define STATE_LED_GREEN 37
#define STATE_LED_BLUE 36

//Keypad variables
const int ROW_NUM = 4; //four rows
const int COLUMN_NUM = 4; //three columns

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte pin_rows[ROW_NUM] = {24, 25, 26, 27};   //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {28, 29, 30, 31}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

//Array declaration
int gridArray[9] = {GRID_ONE,GRID_TWO,GRID_THREE,GRID_FOUR,GRID_FIVE,GRID_SIX,GRID_SEVEN,GRID_EIGHT,GRID_NINE};
int ledArray[5] = {LED_ONE, LED_TWO, LED_THREE, LED_FOUR, LED_FIVE};

const int listSize = 5;
int playSize = 1;
int inputArray[listSize] = {0,0,0,0,0};
int generatedList[listSize] = {0,0,0,0,0};

//Array pointers
int pointer = 0;
int pointerPos = 0;
int inputPos = 0;
int arrayCheckPointer = 0;

//Timers
unsigned long bufferTimer = millis();
unsigned long delayTimer = millis();

//Buffer for bufferTimer
const int bufferT = 500;

//Controls the length of time the leds blink for x = on, x/2 = off
const int timeOn = 1000;
int timerSet = 0;

bool playLedState = false;

//Loss Variables
unsigned long redFadeTimer = millis();
const int redFadeT = 10;

bool redPing = false;
int redVal = 0;

//Win Variables
unsigned long greenFadeTimer = millis();
const int greenFadeT = 10;

bool greenPing = false;
int greenVal = 0;

bool playedOnce = false;

const String String1  = "BUTTON";
const String String2  = ".WAV";

/*
 * Int that controls the state of the machine
 *  0 = idle
 *  1 = playing 
 *  2 = waiting for input
 *  3 = win
 *  4 = loss
 */
int state = 0;

void setup() {
  Serial.begin(9600);
  //Audio Memory
  AudioMemory(20); 

  // Detecting the SD card
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if(!(SD.begin(SDCARD_CS_PIN))){
    while(1){
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }

  mixer1.gain(0, 1.0);
  mixer1.gain(1, 1.0);

  pinMode(STATE_LED_RED, OUTPUT);
  pinMode(STATE_LED_GREEN, OUTPUT);
  pinMode(STATE_LED_BLUE, OUTPUT);
  
  pinMode(GRID_ONE, OUTPUT);
  pinMode(GRID_TWO, OUTPUT);
  pinMode(GRID_THREE, OUTPUT);
  pinMode(GRID_FOUR, OUTPUT);
  pinMode(GRID_FIVE, OUTPUT);
  pinMode(GRID_SIX, OUTPUT);
  pinMode(GRID_SEVEN, OUTPUT);
  pinMode(GRID_EIGHT, OUTPUT);
  pinMode(GRID_NINE, OUTPUT);
  
  pinMode(LED_ONE, OUTPUT);
  pinMode(LED_TWO, OUTPUT);
  pinMode(LED_THREE, OUTPUT);
  pinMode(LED_FOUR, OUTPUT);
  pinMode(LED_FIVE, OUTPUT);

  //Initializes the system led to red
  stateLed(255,0,0);
}

void loop() {
  keypadListener();
  stateHandler();
  if(!checkArrayEmpty()){
    createRandomArray();
    writeArray();
  }
}

/*stateHandler()
 *  handles which state the machine is on and outputs the corresponding color or performs an action
 */
void stateHandler(){
  if(state == 0){
    //Serial.println("State 0");
    stateLed(255,0,0);
  }
  else if(state == 1){
    //Serial.println("State 1");
    playGeneratedArray();
    stateLed(255,0,0);
  }
  else if(state == 2){
    //Serial.println("State 2");
    stateLed(0,255,0);
  }
  else if(state == 3){
    //Serial.println("State 3");
    win();
  }
  else if(state == 4){
    loss();
  }
}

/*stateLed(int red, int green, int blue)
 *  pwm method for stateLed that indicates when the user can press the keypad
 */
void stateLed(int red, int green, int blue){
  analogWrite(STATE_LED_RED, red);
  analogWrite(STATE_LED_GREEN, green);
  analogWrite(STATE_LED_BLUE, blue);
}

/*keypadListener()
 *  * = switches the system between on and of
 *  1-9 = takes input from the user when prompted and parses it to an array
 */
void keypadListener(){
  char key = keypad.getKey();
  
  if (key) {
    //Serial.println(key);
    if(key == '*'){
      if(state == 0){
        //Serial.println("State is 1.");
        state = 1;
        playSdWav1.play("OPENGAME.WAV");
        bufferTimer = millis();
      }
      else{
        //Serial.println("State is 0.");
        clearSystem();
        state = 0;
      }
    }
    if(state == 2){
      if(key >= '1' && key <= '9'){
        inputArray[inputPos] = key - 48;
        String wavFile = String1 + inputArray[inputPos] + String2;
        Serial.print("WaveFile name: ");Serial.println(wavFile.c_str());
        playSdWav1.play(wavFile.c_str());
        inputPos++;
        arrayCheck();
        /*for(int i = 0; i < 5; i++){
          Serial.print(inputArray[i]); Serial.print(", ");
        }
        Serial.println("");
        for(int i = 0; i < 5; i++){
          Serial.print(generatedList[i]); Serial.print(", ");
        }
        Serial.println("");
        */
      }
    }
  }
}

/*arrayCheck()
 *  checks the inputArray from the user against the generatedList, if they are not the same it will throw
 *  the player into state 4 which is the lost state else it will increase the position of input
 */
void arrayCheck(){
  for(int i = 0; i < inputPos; i++){
    if(inputArray[i] != 0){
      if(inputArray[i] != generatedList[i]){
        Serial.println("You have lost");
        state = 4;
        clearSystem();
      }
    }
  }
  if(state == 2){
    if(inputPos >= playSize){
      Serial.println("Next Stage");
      positionHandler();
    }
  }
}

/*positionHandler()
 *  determines the position of the system, if the system reaches the end of the indetified integer it will set
 *  the state to 3 indicating the player has won the game
 */
void positionHandler(){
  //Serial.println("Position Increased");
  pointerPos++;
  playSize = pointerPos;
  state = 1;
  clearLed();
  clearInputArray();
  if(pointerPos > 5){
    state = 3;
    pointerPos = 1;
  }
}

/*clearSystem()
 *  resets all variables that need the be reset by the system in order to start from scratch as well as
 *  generating a new generatedList
 */
void clearSystem(){
  //Serial.println("You have cleared the system");
  playLedState = false;
  delayTimer = millis();
  bufferTimer = millis();
  pointer = 0;
  pointerPos = 1;
  playSize = 1;
  playedOnce = false;
  //playSize = 0;
  clearLed();
  clearArray();
  clearInputArray();
}

/*createRandomArray()
 *  creates a random array of 5 distinct numbers to be used by the 3x3 grid leds
 */
void createRandomArray(){
  for (int i = 0; i < listSize; i++){
    int val;
    do{
      val = random(1,9);
    }
    while([&generatedList, &val](){
        for (auto j : generatedList){
          if (val == j){
            return true;
          }
        }
        return false;
      }());
    generatedList[i] = val;
  }
}

/*playGeneratedArray()
 *  Plays every button in the generatedArray along with turning on the indicator light above the display
 *  this corresponds to what pointer the system is on, between 1 and 5
 */
void playGeneratedArray(){
  if((millis()-bufferTimer) > bufferT){
    //Serial.println(millis()-bufferTimer);
    if(!playLedState)timerSet = timeOn;
    else timerSet = timeOn / 2;
    
    if((millis()-delayTimer) > timerSet){
      if(pointer < playSize){
        if(!playLedState){
          //Serial.print("pointer is : ");Serial.println(pointer);
          //Serial.print("Grid on : ");Serial.println(millis()-delayTimer);
          digitalWrite(gridArray[generatedList[pointer]-1], HIGH);
          for(int j = 0; j < pointer+1; j++){
            analogWrite(ledArray[j], 50);
          }
          playLedState = true;
        }
        else{
          //Serial.print("Grid off : ");Serial.println(millis()-delayTimer);
          digitalWrite(gridArray[generatedList[pointer]-1], LOW);    
          playLedState = false;
          pointer++;
        }
      }
      else{
        state = 2;
        pointer = 0;
      }
      delayTimer = millis();
    }
    bufferTimer = millis();
  }
}

/*writeArray()
 *  simple Serial.print loop
 */
void writeArray(){
  // output the final array
  Serial.print(F("Button Order: "));
  for (int i = 0; i < listSize; i++){
    Serial.print(generatedList[i]);
    if (i < listSize - 1)
    Serial.print(F(", "));
  }
  Serial.println();
}

/*checkArrayEmpty()
 *  determines if the GeneratedList is empty
 */
bool checkArrayEmpty(){
  for(int i = 0; i < listSize; i++){
    if(generatedList[i] != 0){
      return true;
    }
  }
  //Serial.println("Array is empty");
  return false;
}

/*clearLed()
 *  clears the leds in the case of the system being reset
 */
void clearLed(){
  for(int i  = 0; i < 5; i++){
    analogWrite(ledArray[i], 0);
  }
  for(int i = 0; i < 9; i++){
    digitalWrite(gridArray[i], LOW);
  }
}

/*clearArray()
 *  clears the array so that a new array may be created on system reset
 */
void clearArray(){
  for(int i = 0; i < listSize; i++){
    generatedList[i] = 0;
  }
}

/*clearInputArray()
 *  clears the input array
 */
void clearInputArray(){
  inputPos = 0;
  for(int i = 0; i <listSize; i++){
    inputArray[i] = 0;
  }
}

/*win()
 *  If the user enters the correct sequence of integers a winning sound is played as well as the led flashing green
 */
void win(){
  if(!playedOnce){
    if(!playSdWav1.isPlaying()){
      playSdWav1.play("WINGAME.WAV");
      playedOnce = true;
    }
  }
  if((millis()-greenFadeTimer) > greenFadeT){
    if(greenVal >= 255) greenPing = false;
    else if(greenVal <= 0) greenPing = true;
    
    if(greenPing)greenVal+=2;
    else greenVal-=2;

    stateLed(0,greenVal,0);
    greenFadeTimer = millis();
  }
}

/*loss()
 *  If the user enters in the incorrect array this state is called, the led begins to flash red as well
 *  as a sound indicating failure is made 
 */
void loss(){
  if(!playedOnce){
    if(!playSdWav1.isPlaying()){
      playSdWav1.play("FAILED.WAV");
      playedOnce = true;
    }
  }
  if((millis()-redFadeTimer) > redFadeT){
    if(redVal >= 255) redPing = false;
    else if(redVal <= 0) redPing = true;
    
    if(redPing)redVal+=2;
    else redVal-=2;

    stateLed(redVal,0,0);
    redFadeTimer = millis();
  }
}

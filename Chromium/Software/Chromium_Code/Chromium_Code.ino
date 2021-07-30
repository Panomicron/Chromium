#include <Rotary.h> //https://github.com/buxtronix/arduino/tree/master/libraries/Rotary
#include <SPI.h>
#include <EEPROM.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//Adafruit_SSD1306 display(OLED_RESET);

int addr = 0;

const int counterRead = 5; // IR sensor of the counter
const int counterLed = 6; // IR LED of the counter

const int shutterRead = 12; // INPUT PULL-UP shutter button, when it gets pulled down the shutter is fired.

const int motorPWM = 7; // motors pins
const int motorDir = 8;
const int motorEnable = 9;

const int firePin = 10; // connected to the base of a NPN transistor, when put to high the shutter opens

Rotary rotary = Rotary(2, 3); //set up for the rotary encoder library 

const int encoderSwitchPin = 4; //push button switch

const float SHUTTER_TABLE[] = {30.0, 15.0, 8.0, 4.0, 2.0, 1.0, 2.0, 4.0, 8.0, 15.0, 30.0, 60.0, 125.0, 250.0, 500.0};

int counter = 10;
float invShutterDisplay;
float shutterDisplay = 0;
int lastCounter = 10;

int shutterSpeed = 17;

unsigned long onTime = 0;

int rewindState = 0;
int lastRewindState = 1;

int counter_incr = 0;
int frame = EEPROM.read(0);


void setup() {

  delay(2000);
  Serial.begin (9600);

  //SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();
  display.display();
  frameDisplay();
  shutterSet();


  pinMode(A6, OUTPUT);
  digitalWrite(A6, LOW); //A6 is used as GND for the display.

  pinMode(counterRead, INPUT_PULLUP);
  pinMode(counterLed, OUTPUT);

  pinMode(shutterRead, INPUT_PULLUP);
  pinMode(firePin, OUTPUT);

  pinMode(encoderSwitchPin, INPUT);
  digitalWrite(encoderSwitchPin, HIGH); //turn pullup resistor on

  pinMode(motorPWM, OUTPUT);
  pinMode(motorEnable, OUTPUT);
  pinMode(motorDir, OUTPUT);
  
  digitalWrite(motorPWM, LOW);
  digitalWrite(motorEnable, LOW);
  digitalWrite(motorDir, LOW);


}

void loop() {
  // put your main code here, to run repeatedly:
  if (counter != lastCounter) {
    shutterSet();
  }

  if (digitalRead(shutterRead) == LOW) {            // checks if shutter button is pressed, if yes runs shoot and then advance

    shoot();                                        // triggers the shutter
    delay(500);                                     // delay
    advance();                                      // advances the film to the next frame

  }

  rewindState = digitalRead(encoderSwitchPin);

  if (rewindState == 0 && rewindState != lastRewindState) { //checks whether the button has been pressed, and whether is has changed since last time it checked
    rewindTimer();
  }
  lastRewindState = rewindState;

  if (onTime > 0 && millis() - onTime > 1500){   // checks that the button has be pressed for 1.5s or longer 
    rewindroll();                                // rewinds roll, sets timer and frame count to 0
    onTime = 0;
    frame = 0;
    EEPROM.write(addr, frame);
  }

  else {
    // button is not being pushed.

  }


}

void rewindTimer() {

  onTime = millis(); // starts a timer to check that you're not pressing it by mistake

}

void rotate() {                                               // rotary encoder counter
  unsigned char result = rotary.process();
  if (result == DIR_CW) {
    counter++;
    //    Serial.println(counter);
  } else if (result == DIR_CCW) {
    counter--;
    //    Serial.println(counter);
  }

}


void shoot() {                                              

  digitalWrite(firePin, HIGH);                                    //opens the shutter
  delay(shutterSpeed);                                            // delays the duration of the shutter speed
  digitalWrite(firePin, LOW);                                     // closes the shutter

}

void led_Detect() {                                               //frame counter incrementing
  counter_incr++;
  // Serial.println("detect");
  //Serial.println(counter_incr);

}

void advance() {


  int IRState = 0;                                            // current state of the IR sensor
  int lastIRState = 0;                                        // last state of the IR sensor



  digitalWrite(counterLed, HIGH);                           //turns the IR LED in the back on
  digitalWrite(motorPWM, HIGH);                             // turns on motor outputs
  digitalWrite(motorEnable, LOW);
  digitalWrite(motorDir, HIGH);


  while (counter_incr <= 8) {// check if the frame has been fully advanced or not, if not then continue winding

    IRState = digitalRead(counterRead);


    
    if (IRState != lastIRState) {                                                   // compare the buttonState to its previous state
                                                                                    // if the state has changed, increment the counter
      if (IRState == LOW) {
                                                                                    // if the current state is LOW then the IR sensor was activated:
        counter_incr++;
      }
      else {
                                                                                   // if the current state is HIGH then the IR Sensor is not activated, so we can ignore :
      }
                                                                                   // Delay a little bit to avoid bouncing
      delay(100);
    }
                                                                                   // save the current state as the last state, for next time through the loop
    lastIRState = IRState;

  }
                                                                                   //count_incr over 6, stop winding and count the frame.
  digitalWrite(motorPWM, LOW);
  digitalWrite(motorEnable, LOW);
  digitalWrite(motorDir, LOW);
  digitalWrite(counterLed, LOW);
  counter_incr = 0;
  frame++;

  //Serial.println("frame:");
  //Serial.print(frame);

  EEPROM.write(addr, frame);
  frameDisplay();

}

void rewindroll () {


  while (digitalRead(encoderSwitchPin) == 0) {

    digitalWrite(motorPWM, HIGH);
    digitalWrite(motorEnable, LOW);
    digitalWrite(motorDir, LOW);
    delay(300);                                                                   // delay seens to help avoid noise
  }
 
  delay(300);
  digitalWrite(motorPWM, LOW);
  digitalWrite(motorEnable, LOW);
  digitalWrite(motorDir, LOW);


}


void frameDisplay(void) {

  display.fillRect(0, 0, 40, 14, SSD1306_BLACK); // clears the frame section of the display without impacting the rest
  display.display();
  delay(1);

  display.setTextSize(2);             //  2:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0, 0);            // Start at top-left corner
  if (frame <= 9) {                             // if the frame is a single digit number a 0 needs to be printed before the frame value.
    display.print(0);
    display.print(frame);

  }
  else {
    display.print(frame);                       // if it's double digits then you can just print the number
  }

  display.fillRect(27, 0, 4, 2, SSD1306_WHITE);          // draws a little canister
  display.fillRect(25, 2, 8, 12,  SSD1306_WHITE);
  display.drawRect(33, 3, 3, 10, SSD1306_WHITE);



  display.display();
  delay(250); // updates the display
  return;
}

void shutterSet(void) {

  display.fillRect(50, 0, 40, 14, SSD1306_BLACK);           //clears the shutterspeed side of the display
  display.display();


  display.setCursor(50, 0);             // Cursor at x50 y0
  display.setTextSize(2);             //  2:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text


  counter = constrain (counter, 0, 14);                   //limit the counter the the number of values in the shutter table array

  shutterDisplay = SHUTTER_TABLE[counter];
  invShutterDisplay = 1 / shutterDisplay;                 // invShutterDisplay is the reciprocal of the shutterDisplay, this gives the needed shutter speed as a float


  if (counter >= 6) {                                       // if the counter is in the fraction section of the shutter speed array
    display.print(shutterDisplay, 0);

    shutterSpeed = (int)(invShutterDisplay * 1000);   // invShutterDisplay is multiplied by a thousand and made an integer to be used as a delay value



  }
  else {
    display.print(shutterDisplay, 0);
    display.print(F("s"));
    shutterSpeed = (int)(shutterDisplay * 1000);  

  }

  display.display();
  lastCounter = counter;

  // updates the display


}

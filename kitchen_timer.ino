// kitchen timer https://github.com/ldijkman/Arduino_CountDown_Timer_rotary_encoder_TM1637_7_Segment_LED
// kitchen timer https://github.com/ldijkman/Arduino_CountDown_Timer_rotary_encoder_TM1637_7_Segment_LED
// kitchen timer https://github.com/ldijkman/Arduino_CountDown_Timer_rotary_encoder_TM1637_7_Segment_LED
// only TM1637 7segment  rotary encoder and buzzer (NOT with 2x16 LCD)
// version 15 - may - 2021
// https://www.youtube.com/watch?v=B5O9bT54BzI
// TM1637 7segment https://www.youtube.com/watch?v=a7cO0Zcwmvw
// original with seperate start button https://www.youtube.com/watch?v=rzxisoU9D6c

// TM1637 4 digit 7segment
// rotary encoder
// buzzer

#include <Arduino.h>
#include <TM1637Display.h> //https://github.com/avishorp/TM1637
#include <Wire.h>

// Module connection pins (Digital Pins)
#define CLK 8           // D8 to  TM1637 4 character 7 segment LED Display
#define DIO 9            // D9 to  TM1637 4 character 7 segment LED Display

// The amount of time (in milliseconds) between tests
#define TEST_DELAY   2000

const uint8_t SEG_DONE[] = {
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
  SEG_C | SEG_E | SEG_G,                           // n
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G            // E
};


TM1637Display display(CLK, DIO);


#define Start 4               // start stop button D4
// possible without button
// use encoder longpress to start
#define BuzzerPin 11          // buzzer D11 
#define BACKLIGHT_PIN 13


int hours = 0;
int minutes = 0;
int seconds = 0;
int sethours = 0;
int setminutes = 0;
int setseconds = 0;
boolean timeState = false;
#define encoderPinA 2        // encoder  CLK right
#define encoderPinB 3        // encoder  DT  left
#define encoderButton 5      // encoder  SW  switch
int HMS = 1;
int encoderPos = 0;               // a counter for the dial
unsigned int lastReportedPos = 1; // change management
static boolean rotating = false;  // debounce management
boolean A_set = false;
boolean B_set = false;
int cld;


void setup() {


  pinMode(Start, INPUT_PULLUP);
  pinMode(encoderPinA, INPUT_PULLUP); //enabling pullups
  pinMode(encoderPinB, INPUT_PULLUP);
  pinMode(encoderButton, INPUT_PULLUP);
  attachInterrupt(0, doEncoderA, CHANGE); //pin 2
  attachInterrupt(1, doEncoderB, CHANGE); //pin 3

  Serial.begin(9600); // output

  HMS = 1;
}


void loop() {


  if (digitalRead(encoderButton) == LOW)
  {
    HMS = HMS + 1;
    if (HMS == 3)
    {
      HMS = 1;
    }
    
    tone(BuzzerPin, 1000, 100);
    int t = 80;
    while (digitalRead(encoderButton) == LOW) {
      delay(11);
      t = t - 1;
      //lcd.setCursor(0, 0); lcd.print(t); lcd.print("          ");
      if (t == 60) {
        tone(BuzzerPin, 200, 50); // beep
      }
      if (t == 40) {
        tone(BuzzerPin, 200, 50); // beep
      }
      if (t == 20) {
        tone(BuzzerPin, 200, 50); // beep
      }
      if (t <= 1) {
        t = 1;
        timeState = true;                       // start countdown
        sethours = hours;
        setminutes = minutes;
        setseconds = seconds;
        tone(BuzzerPin, 2000, 100); // piep
      }
    }
  }




  rotating = true; // reset the debouncer
  encoderPos = constrain(encoderPos, -1, 1);
  if (lastReportedPos != encoderPos) {


    if (HMS == 3) {
      hours = hours + encoderPos;
      hours = constrain(hours, 0, 48);

    }
    else if (HMS == 2) {
      minutes = minutes + encoderPos;
      minutes = constrain(minutes, 0, 99);

    }
    else if (HMS == 1) {
      seconds = seconds + encoderPos;
      if (seconds == 60) {
        seconds = 0;
        minutes = minutes + 1;
      }
      if (seconds < 0) {
        seconds = 59;
        minutes = minutes - 1;
      }
      seconds = constrain(seconds, 0, 60);

    }





    encoderPos = 0;
    lastReportedPos = encoderPos;


    display.setBrightness(0x0f);
    if (minutes == 0)display.clear();
    if (minutes > 0)display.showNumberDec(seconds, true, 2, 2); // Expect:  1:04
    if (minutes == 0)display.showNumberDec(seconds, false, 2, 2); // Expect: no leading 0
    if (minutes > 0) display.showNumberDecEx(minutes, (0b01000000), false, 2, 0);


  }




  if (digitalRead(Start) == LOW || timeState == true) { //start count down timer
    
    timeState = true;
    delay(100);
    

    while (timeState == true) {

      int period = 968;        // 965 maintime delay adjust if your timer goes to fast or slow
      unsigned long time_now = 0;
      time_now = millis();
      while (millis() < time_now + period) {
        //wait approx. [period] ms}
        if (digitalRead(encoderButton) == LOW) {
          timeState = false;
          tone(BuzzerPin, 2000, 100); // piep
        }
      }
      seconds = seconds - 1;

      if (minutes == 0 && hours >= 1) {
        minutes = 60;
        hours = hours - 1;
      }
      if (seconds <= 0 && minutes >= 1) {
        seconds = 60;
        minutes = minutes - 1;
      }
      else if (minutes == 0 && hours == 0 && seconds == 0) { //count down alarm
        while (timeState == true) {

          display.setSegments(SEG_DONE);  // display text => done

          tone(BuzzerPin, 600, 250);
          delay(250);
          tone(BuzzerPin, 800, 250);
          delay(250);
          if (digitalRead(Start) == LOW) { // turn alarm off with switch
            timeState = false;
            seconds = 1;

            hours = sethours;
            minutes = setminutes;
            seconds = setseconds;
            break;
          }


          if (digitalRead(encoderButton) == LOW) { // turn alarm off with rotary encoder switch
            timeState = false;

            while (digitalRead(encoderButton) == LOW) {
              delay(1000); //wait for release switch
            }
            hours = sethours;
            minutes = setminutes;
            seconds = setseconds;
            break;
          }


        }
      }


    Serial.println(seconds);

      display.setBrightness(0x0f);
      if (minutes == 0)display.clear();
      if (minutes > 0)display.showNumberDec(seconds, true, 2, 2); // Expect:  1:04 leading 0 on seconds 
      if (minutes == 0)display.showNumberDec(seconds, false, 2, 2); // Expect: no leading 0 on seconds 0 to 9 
      if (minutes > 0) display.showNumberDecEx(minutes, (0b01000000), false, 2, 0);

      tone(BuzzerPin, 50, 50); // second buz



      if (digitalRead(encoderButton) == LOW) {    // pauze countdown with encoder button
        delay(50);
        tone(BuzzerPin, 500, 100);
        timeState = false;
        break;
      }
    }
  }
}







// Interrupt on A changing state
void doEncoderA() {
  // debounce
  if ( rotating ) delay (1); // wait a little until the bouncing is done

  // Test transition, did things really change?
  if ( digitalRead(encoderPinA) != A_set ) { // debounce once more
    A_set = !A_set;

    // adjust counter + if A leads B
    if ( A_set && !B_set )
      encoderPos = 1;

    rotating = false; // no more debouncing until loop() hits again
  }
}


// Interrupt on B changing state
void doEncoderB() {
  if ( rotating ) delay (1);
  if ( digitalRead(encoderPinB) != B_set ) {
    B_set = !B_set;
    // adjust counter – 1 if B leads A
    if ( B_set && !A_set ) encoderPos = -1;

    rotating = false;
  }
}

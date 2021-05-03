/*

Aarya Gupta
1 year ago (edited)
 @Luberth Dijkman  second is going in negative

I have corrected it,thankq so much for the code
  
haha 
if i remember right when start is for example 1:00 2:00

think i add line +/-260       if (seconds<0) seconds=0;


maybe try this     codeis not perfect maybe u can use it
*/


#include <Arduino.h>
#include <TM1637Display.h> //https://github.com/avishorp/TM1637   download ZIP and install library from ZIP in arduino


// Module connection pins (Digital Pins)
#define CLK 8           // D8 to  TM1637 4 character 7 segment LED Display
#define DIO 9            // D9 to  TM1637 4 character 7 segment LED Display

// i2c 16x2 LCD   to  SCL SDA or A4 A5




// The amount of time (in milliseconds) between tests
#define TEST_DELAY   2000


const uint8_t SEG_DONE[] = {
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
  SEG_C | SEG_E | SEG_G,                           // n
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G            // E
  };


TM1637Display display(CLK, DIO);




#include <Wire.h>
#include <LiquidCrystal_I2C.h>  // do not know dfrobot.com i2c lib should change this to a github link
//#include <LCD.h>

//version 29-2-2020
//https://www.youtube.com/watch?v=B5O9bT54BzI
// lcd and TM1637 7segmnent https://www.youtube.com/watch?v=a7cO0Zcwmvw
//original with seperate start button https://www.youtube.com/watch?v=rzxisoU9D6c
LiquidCrystal_I2C lcd(0x27, 16, 2);
//LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
//LiquidCrystal_I2C lcd(0x3f, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // Set the LCD I2C address
//LiquidCrystal_I2C lcd(0x38);  // Set the LCD I2C address


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


void setup() {


  pinMode(Start, INPUT_PULLUP);
  pinMode(encoderPinA, INPUT_PULLUP); //enabling pullups
  pinMode(encoderPinB, INPUT_PULLUP);
  pinMode(encoderButton, INPUT_PULLUP);
  attachInterrupt(0, doEncoderA, CHANGE); //pin 2
  attachInterrupt(1, doEncoderB, CHANGE); //pin 3
  Serial.begin(9600); // output
  lcd.begin(); // initialize the lcd for 16 chars 2 lines, turn on backlight
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("COUNT DOWN TIMER");
  lcd.setCursor(4, 1);
  lcd.print("00:00:00");
  delay(2500);


}


void loop() {


  if (digitalRead(encoderButton) == LOW)
  {
    HMS = HMS + 1;
    if (HMS == 4)
    {
      HMS = 1;
    }
    tone(BuzzerPin, 1000, 100);
    int t = 80;
    while (digitalRead(encoderButton) == LOW) {   
      delay(1); t = t - 1; lcd.setCursor(0, 0); lcd.print(t); lcd.print("          ");
      if (t == 60) {tone(BuzzerPin, 200, 50);}
        if (t == 40) {tone(BuzzerPin, 200, 50);}
          if (t == 20) {tone(BuzzerPin, 200, 50);}
      if (t <= 1) {
        t = 1; timeState = true;
            sethours = hours;
            setminutes=minutes;
            setseconds=seconds;
        lcd.setCursor(0, 0); lcd.print("Get Set Go      ");
        tone(BuzzerPin, 2000, 100);
      }
    }
  }
  //  lcd.setCursor(0, 0);
  //  lcd.print("COUNT DOWN TIMER");
  //  delay(500);
  if (HMS == 1) {
    lcd.setCursor(0, 0);
    lcd.print("set hours       ");
  }
  if (HMS == 2) {
    lcd.setCursor(0, 0);
    lcd.print("set minutes     ");
  }
  if (HMS == 3) {
    lcd.setCursor(0, 0);
    lcd.print("set seconds     ");
  }
  //delay(500);


  rotating = true; // reset the debouncer
  encoderPos = constrain(encoderPos, -1, 1);
  if (lastReportedPos != encoderPos) {


    if (HMS == 1) {
      hours = hours + encoderPos;
      hours = constrain(hours, 0, 48);
     
    }
    else if (HMS == 2) {
      minutes = minutes + encoderPos;
      minutes = constrain(minutes, 0, 60);
     
    }
    else if (HMS == 3) {
      seconds = seconds + encoderPos;
      seconds = constrain(seconds, 0, 60);
      
    }


    // Serial.println(encoderPos); // for testing
    // Serial.print('hours ');
    // Serial.println(hours);
    // Serial.print('minutes ');
    // Serial.println(minutes);
    // Serial.print('seconds ');
    // Serial.println(seconds);
    // Serial.println(' ');








    lcd.setCursor(4, 1);
    if (hours <= 9)
    {
      lcd.print("0");
    }
    lcd.print(hours);
    lcd.print(":");
    if (minutes <= 9)
    {
      lcd.print("0");
    }
    lcd.print(minutes);
    lcd.print(":");
    if (seconds <= 9)
    {
      lcd.print("0");
    }
    lcd.print(seconds);
    encoderPos = 0;
    lastReportedPos = encoderPos;
    
  
  display.setBrightness(0x0f);
 if (minutes == 0)display.clear();
 if (minutes>0)display.showNumberDec(seconds, true, 2, 2);  // Expect:  1:04
 if (minutes==0)display.showNumberDec(seconds, false, 2, 2);  // Expect: no leading 0
 if (minutes > 0) display.showNumberDecEx(minutes, (0b01000000), false,2,0); 
   
   
  }




  if (digitalRead(Start) == LOW || timeState == true) { //start count down timer
    lcd.setCursor(0, 0); lcd.print("Counting Down!  ");
    timeState = true;
    delay(500);
    
    while (timeState == true) {
       
    //delay(992);           //maintime delay
    int period = 1000;
    unsigned long time_now = 0;
    time_now = millis();
    while(millis() < time_now + period){ //wait approx. [period] ms}


      
    }
       seconds = seconds - 1;
    // if (seconds<0) seconds=0;
       
      if (minutes == 0 && hours >= 1) {
        minutes = 60;
        hours = hours - 1;
      }
      if (seconds == 0 && minutes >= 1) {
        seconds = 60;
        minutes = minutes - 1;
      }
      else if (minutes == 0 && hours == 0 && seconds == 0) { //count down alarm
        while (timeState == true) {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("COUNT DOWN TIMER");
            lcd.setCursor(5, 1);
            lcd.print("Alarm!");


 display.setSegments(SEG_DONE);
            
          tone(BuzzerPin, 600, 250);
          delay(250);
          tone(BuzzerPin, 800, 250);
          delay(250);
          if (digitalRead(Start) == LOW) { // turn alarm off with switch
            timeState = false;
            seconds = 1;
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("COUNT DOWN TIMER");
            lcd.setCursor(4, 1);
            lcd.print("00:00:00");
            
            hours = sethours;
            minutes=setminutes;
            seconds=setseconds;


            break;
          }


          if (digitalRead(encoderButton) == LOW) { // turn alarm off with rotary encoder switch
            timeState = false;
            //seconds = 1;
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("COUNT DOWN TIMER");
            lcd.setCursor(4, 1);
            lcd.print("Done!");
            while(digitalRead(encoderButton) == LOW){delay(1000);}//wait for release switch
            hours = sethours;
            minutes=setminutes;
            seconds=setseconds;
            break;
          }


        }
      }
    // delay(992);




  //    seconds = seconds - 1;


      //Serial.print('hours '); // for testing
      //Serial.println(hours);
      //Serial.print('minutes ');
      //Serial.println(minutes);
      //Serial.print('seconds ');
      // Serial.println(seconds);
      //Serial.println(' ');


  int k;
  uint8_t data[] = { 0xff, 0xff, 0xff, 0xff };
  uint8_t blank[] = { 0x00, 0x00, 0x00, 0x00 };


 
  //display.clear();
  //display.showNumberDecEx(14, (0b01000000), false,2,0);


  
  //display.showNumberDec(14, true, 2, 0); // Expect: __14
  //delay(TEST_DELAY);  
  
  display.setBrightness(0x0f);
  if (minutes == 0)display.clear();
 if (minutes>0)display.showNumberDec(seconds, true, 2, 2);  // Expect:  1:04
 if (minutes==0)display.showNumberDec(seconds, false, 2, 2);  // Expect: no leading 0
 if (minutes > 0) display.showNumberDecEx(minutes, (0b01000000), false,2,0); 
   
  


 




      


      lcd.setCursor(4, 1);
      if (hours <= 9)
      {
        lcd.print("0");
      }
      lcd.print(hours);
      lcd.print(":");
      if (minutes <= 9)
      {
        lcd.print("0");
      }
      lcd.print(minutes);
      lcd.print(":");
      if (seconds <= 9)
      {
        lcd.print("0");
      }
      lcd.print(seconds);


      if (digitalRead(Start) == LOW) {           // pauze countdown with start button
        delay(1000);
        timeState = false;
        break;
      }
      if (digitalRead(encoderButton) == LOW) {    // pauze countdown with encoder button
        delay(1000);
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
    if ( B_set && !A_set )
      encoderPos = -1;


    rotating = false;
  }
}

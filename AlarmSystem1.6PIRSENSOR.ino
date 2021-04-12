/*Basic alarm system.  On/Off switch is connected to pin 8 with a pull up resistor
 * Zone 1 is connected to pin 3 with a pull up resistor, the entry exit zone is 
 * connected to interrupt pin 2 with a pull up resistor.  The output is an LED
 * connected to pin 7 which times out after 5 flashes and resets the variables.
 */

 
/*************************KEYPAD/ PASSWORD VARIABLES***************/

#include <Keypad.h>
#include <Password.h>
 
String newPasswordString; //hold the new password
char newPassword[6]; //character string of newPasswordString
 
//initialize password to 1234
//you can use password.set(newPassword) to overwrite it
Password password = Password("1234");
 
byte maxPasswordLength = 6; 
byte currentPasswordLength = 0;
const byte ROWS = 4; // Four rows
const byte COLS = 4; // Four columns
 
//Define the keymap
char keys[ROWS][COLS] = {
{'1','2','3','A'},
{'4','5','6','B'},
{'7','8','9','C'},
{'*','0','#','D'}
};
 
//// Connect keypad ROW0, ROW1, ROW2 and ROW3 to these Arduino pins.

byte rowPins[ROWS] = {30, 29, 28, 27}; //connect to row pinouts


// Connect keypad COL0, COL1, COL2 and COL3 to these Arduino pins.

byte colPins[COLS] = {26, 25, 24, 23}; //connect to row pinouts

 
// Create the Keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
 

int x = 0;
unsigned long time_now = 0;
int period = 1000;


/********************LCD VARIABLES**********************************/

#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

int lcdperiod = 500;


/***********************RGB LED VARIABLES**************************/

// Define Pins
#define BLUE 3
#define GREEN 5
#define RED 6

int redValue;
int greenValue;
int blueValue;

int Sirenperiod = 1;

/**************************BUZZER VARIABLES*************************/

int buzzer = 13;//the pin of the active buzzer

/*************************ALARM VARIABLES**************************/

volatile int entry_exit = 0;  //entry/exit zone connected to interrupt
volatile int on_off = 0;  // on/off
volatile int zone_1 = 0;
int a=0, z;


/**************************PIR VARIABLES*******************************/

int ledPin = 50;                // choose the pin for the LED
int inputPin = 40;               // choose the input pin (for PIR sensor)
int pirState = LOW;             // we start, assuming no motion detected
int val = 0;                    // variable for reading the pin status


/*************************RGB LED FUNCTIONS******************************/

void Siren_Delay()  {
    time_now = millis();
    while(millis() < time_now + Sirenperiod )  {
    Read_key();
    //Serial.print("millis()");
    //Serial.println(millis());
// wait appox. [period] ms
}
}

void Siren(){

  digitalWrite(RED, HIGH);
  digitalWrite(GREEN, LOW);
  digitalWrite(BLUE, LOW);

  redValue = 255;
  greenValue = 0;
  blueValue = 0;


for(int i = 0; i < 255; i += 1) // fades out red
{
redValue -= 1;

analogWrite(RED, redValue);
digitalWrite(buzzer,HIGH);
Siren_Delay();
}

redValue = 0;
greenValue = 0;
blueValue = 0;

for(int i = 0; i < 255; i += 1) // fades in blue
{
blueValue += 1;
digitalWrite(buzzer,LOW);
analogWrite(BLUE, blueValue);
Siren_Delay();
}

redValue = 0;
greenValue = 0;
blueValue = 255;

for(int i = 0; i < 255; i += 1) // fades out blue
{
blueValue -= 1;
digitalWrite(buzzer,HIGH);
analogWrite(BLUE, blueValue);
Siren_Delay();
}

redValue = 0;
greenValue = 0;
blueValue = 0;

for(int i = 0; i < 255; i += 1) // fades in red
{
redValue += 1;
digitalWrite(buzzer,LOW);
analogWrite(RED, redValue);
Siren_Delay();
}

redValue = 255; 
greenValue = 0;
blueValue = 0;
}


/************************KEYPAD/LCD FUNCTIONS****************************/

void Read_key(){
  char key = keypad.getKey();
   if (key != NO_KEY){
      delay(60); 
      switch (key){
      case 'A': break; 
      case 'B': break; 
      case 'C': break; 
      case 'D': break; 
      case '#': checkPassword(); break;
      case '*': resetPassword(); break;
      default: processNumberKey(key);
      }
   }
}
 
void processNumberKey(char key) {
   Serial.print(key);
   lcd.print("*");
   currentPasswordLength++;
   password.append(key);
   if (currentPasswordLength == maxPasswordLength) {
      checkPassword();
   } 
}

void LCD_Scroll_Delay()  {
    time_now = millis();
    while(millis() < time_now + lcdperiod)  {
    Read_key();
    //Serial.print("millis()");
    //Serial.println(millis());
// wait appox. [period] ms
}
}

void checkPassword() {
   if (password.evaluate()){
      Serial.println("PASSWORD ENTERED");
      lcd.clear();
      lcd.print("PASSWORD ENTERED" );

      on_off = on_off^1; //toggle on_off
      
   } else {
      Serial.println("WRONG PASSWORD!");
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("WRONG PASSWORD!" );
      for(int x=0; x < 3; x++) { // loop 3 times
    lcd.noDisplay(); // turn display off
    LCD_Scroll_Delay();
    lcd.display(); // turn it back on again
    LCD_Scroll_Delay();
    
    }
    lcd.setCursor(6,1);

   } 
   
   resetPassword();
}

void resetPassword() {
   password.reset(); 
   currentPasswordLength = 0; 
}


void My_Delay()  {
    time_now = millis();
    while(millis() < time_now + period)  {
    Read_key();
    //Serial.print("millis()");
    //Serial.println(millis());
// wait appox. [period] ms
}
  
    }

    /*********************ALARM FUNCTIONS**************************/

   void pulse() {
  for(a=0;a<5;a++){
    digitalWrite(13,HIGH);
    My_Delay();
    digitalWrite(13,LOW);
    My_Delay();
    if(on_off==0)
    return;
  }
  entry_exit = 0;
  zone_1 = 0;
  // on_off = 0; No longer needed with keypad attached
}

void read_on_off(){
  Read_key();
}

void read_zone_1(){
  zone_1 = !digitalRead(4);
}

void read_zone_PIR(){
  
  }

void countdown(){
  lcd.clear();
  lcd.print("ALARM ARMED IN ");
  Serial.print("ALARM ARMED IN ");
  for(z=0;z<5;z++){
    Serial.println(z);
    lcd.setCursor(0,1);
    lcd.print(z);
    My_Delay();
  }
 entry_exit = 0;
}

void ISR_pin2(){
  entry_exit = 1; 
  
}

/****************************MAIN SETUP*************************/

void setup()
{
  pinMode(ledPin, OUTPUT); //initialise PIR led as output
  pinMode(inputPin, INPUT); //initialise pir sensor as input
  digitalWrite(ledPin, HIGH);
  delay(1000);
  digitalWrite(ledPin, LOW);


  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  digitalWrite(RED, HIGH);
  digitalWrite(GREEN, LOW);
  digitalWrite(BLUE, LOW);

  pinMode(buzzer,OUTPUT);//initialize the buzzer pin as an output

  
  pinMode(2, INPUT); //entry exit zone
  //pinMode(8, INPUT); // pin 8 is no longer on off with the keypad
  pinMode(4, INPUT); // pin 4 is zone 1
  
  lcd.begin(16,2);
 
  
  Serial.begin(9600);
  Serial.println("READY TO TURN ON THE ALARM");
  lcd.print("READY TO TURN ON");
  lcd.setCursor(0,1);
  lcd.print("THE ALARM");
  lcd.setCursor(10,1);
  
  
  
  
  attachInterrupt(digitalPinToInterrupt(2), ISR_pin2, FALLING);
  
  //pin 2 is the interrupt, when this is triggered, and pin 2 goes from logic 1 to 0
  //when triggered ISR pin 2 is ran which sets entry exit to 1.
  //While the on/off switch = 0 stay in the while loop reading the switch
  
  while(!on_off){   //
                    //will loop through this until alarm is turned on as on off is 0.
    read_on_off(); //reads status of on_off.
    
    
  }
  //countdown to allow you to leave the house
  //after 0n_off switch is on, exit while loop, and count down giving you a chance to leave the house
  countdown();
 
 }

/*******************MAIN LOOP***********************/

void loop()
{
      read_on_off();
      read_zone_1();
      
    //  Serial.print("zone_1   ");
    //  Serial.println(zone_1);
      //we dont read entry exit zone because it is set as an interrupt routine above.
      
      if(a == 1){   //This allows the alarm to be reset

            digitalWrite(RED, HIGH);
            digitalWrite(GREEN, LOW);
            digitalWrite(BLUE, LOW);
          Serial.println("TURN ON ALARM");
          lcd.clear();
          lcd.print("TURN ON ALARM " );
          lcd.setCursor(6,1);
          entry_exit = 0;
          zone_1 = 0;
          on_off = 0;
          a = 0;
      }

    if(on_off == 1){   //This alows the alarm to be reset
          countdown();
          Serial.println("ALARM ARMED");
          lcd.clear();
          lcd.print("ALARM ARMED " );
          lcd.setCursor(6,1);
          digitalWrite(RED, LOW);
          digitalWrite(GREEN, HIGH);
          digitalWrite(BLUE, LOW);
          
    }
            
    while (on_off){      // While on_off switch = 1 check other sensors
      
        read_zone_1();
        read_on_off();

        val = digitalRead(inputPin);  // read input value
  if (val == HIGH) {            // check if the input is HIGH
    digitalWrite(ledPin, HIGH);  // turn LED ON
    if (pirState == LOW) {
      // we have just turned on
      Serial.println("Motion detected!");
      // We only want to print on the output change, not state
      pirState = HIGH;
    }
  } else {
    digitalWrite(ledPin, LOW); // turn LED OFF
  }
    
        if(entry_exit == 1){
        
        
          
          Serial.println("TURN OFF ALARM");
          lcd.clear();
          lcd.print("TURN OFF ALARM " );
          lcd.setCursor(7,1);
          
          for(int x=0; x < 5; x++) { // 5 seconds to turn off alarm
            My_Delay();
            read_on_off();
          }

          

          
          Serial.println("ENTRY EXIT ZONE BREACHED");
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("ENTRY EXIT ZONE" );
          lcd.setCursor(0,1);
          lcd.print("BREACHED" );
          lcd.setCursor(10,1);
          

         a = 1;
          
          read_on_off();
        
          while(on_off&&entry_exit){
            
            read_on_off();
            Siren();
            a = 1;
            
          }
        }

        
        //Serial.print("zone_1   "); //printed these to figure out why zone 1 wasnt triggering the alarm.
        //Serial.print(zone_1);
        //Serial.print("  on_off  ");
        //Serial.println(on_off); 
        
        while(zone_1&&on_off){ //while zone1 is on (triggered) and the alarm is on
          //if zone_1 is triggered keep alarm going until it is turned off
          read_on_off();
          
          a = 1;
          Serial.println("ZONE 1 BREACHED");
          lcd.clear();
          lcd.print("ZONE 1 BREACHED" );
          lcd.setCursor(6,1);
          
          Siren();

          
        }

       while(on_off&&val==HIGH){

          read_on_off();
          
          a = 1;

          Serial.println("PIR ZONE BREACHED");
          lcd.clear();
          lcd.print("MOTION SENSOR" );
          lcd.setCursor(0,1);
          lcd.print("TRIGGERED");
          lcd.setCursor(11,1);
          
          Siren();
       }
   
}
}

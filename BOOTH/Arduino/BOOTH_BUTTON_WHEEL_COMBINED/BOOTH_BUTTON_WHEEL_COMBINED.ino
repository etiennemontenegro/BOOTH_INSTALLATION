#include <Chrono.h>
#include <Bounce2.h>

#define BUTTON_PIN 8//pin connected to button
#define LED_PIN 9 // pin connected to LED button
#define reed A0//pin connected to read switch

//VARIABLE BUTTON
int ledState = LOW;
int buttonState = 0;


//VARIABLE CHRONO
Chrono refreshRate;
Bounce debouncer = Bounce(); // Instantiate a Bounce object
int interval = 25;

//VARIABLE SPEEDDOMETRE
//storage variables
int reedVal;
long timer;// time between one full rotation (in ms)
float mph;
float radius = 12;// tire radius (in inches)
float circumference;

int maxReedCounter = 100;//min time (in ms) of one rotation (for debouncing)
int reedCounter;


void setup() {

   Serial.begin(9600); 


 //SETUP BUTTON//////////////////////////////////////////////////////////////////////////////////////////////////
  debouncer.attach(BUTTON_PIN,INPUT_PULLUP); // Attach the debouncer to a pin with INPUT_PULLUP mode
  debouncer.interval(25); // Use a debounce interval of 25 milliseconds 
  pinMode(LED_PIN,OUTPUT); // Setup the LED
  digitalWrite(LED_PIN,ledState);


//SETuP SPEEDOMETER/////////////////////////////////////////////////////////////////////////////////////////////
  reedCounter = maxReedCounter;
  circumference = 2*3.14*radius;
  pinMode(reed, INPUT);
  
  // TIMER SETUP- the timer interrupt allows precise timed measurements of the reed switch
  //for more info about configuration of arduino timers see http://arduino.cc/playground/Code/Timer1
  cli();//stop interrupts

  //set timer1 interrupt at 1kHz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;
  // set timer count for 1khz increments
  OCR1A = 1999;// = (1/1000) / ((1/(16*10^6))*8) - 1
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS11 bit for 8 prescaler
  TCCR1B |= (1 << CS11);   
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  
  sei();//allow interrupts
  //END TIMER SETUP
}


//SPEEDOMETER FUNCTION
ISR(TIMER1_COMPA_vect) {//Interrupt at freq of 1kHz to measure reed switch
  reedVal = digitalRead(reed);//get val of A0
  if (reedVal){//if reed switch is closed
    if (reedCounter == 0){//min time between pulses has passed
      mph = (56.8*float(circumference))/float(timer);//calculate miles per hour
      timer = 0;//reset timer
      reedCounter = maxReedCounter;//reset reedCounter
    }
    else{
      if (reedCounter > 0){//don't let reedCounter go negative
        reedCounter -= 1;//decrement reedCounter
      }
    }
  }
  else{//if reed switch is open
    if (reedCounter > 0){//don't let reedCounter go negative
      reedCounter -= 1;//decrement reedCounter
    }
  }
  if (timer > 2000){
    mph = 0;//if no new pulses from reed switch- tire is still, set mph to 0
  }
  else{
    timer += 1;//increment timer
  } 
}

void loop() {

  if (refreshRate.hasPassed(interval) ) { // elapsed(1000) returns 1 if 1000ms have passed.
      refreshRate.restart();  // restart the Chrono 



     debouncer.update(); // Update the Bounce instance
   
     if ( debouncer.fell() ) {  // Call code if button transitions from HIGH to LOW
      ledState = !ledState; // Toggle LED state
      digitalWrite(LED_PIN,ledState); // Apply new LED state
      buttonState = 1;
     }
    else if (debouncer.rose())
     {
       buttonState = 0;
     }
  String  message_button = "button " + String(buttonState);
  String  message_wheel = "wheel " + String(mph);
   Serial.print(message_button);
   Serial.print("\n");
   Serial.print(message_wheel);
   Serial.print("\n");



    
  }
 
}

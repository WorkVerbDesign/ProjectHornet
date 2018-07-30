/* Project Hornet: Test Hardware 2.0
   Commissioned originally by Jack Packard and Rich Evans
   https://www.twitch.tv/previouslyrecorded_live
   based on "lump of clay" rapid prototyping technology
   This is the first physcial version of Project Hornet
   Including 3d printed enclosure and internal teensy 3.6

   Created by MarioMario42 (hardware) and oh_bother (Jesse, electronics + software)
   catch my streams at  https://www.twitch.tv/oh_bother
   
   Buttons pull down. Joystick is 5k? 3.4 on each side?
   down to 10ohm, needs calibration.
   
   Pinout:
   D2 - RightLow Trigger
   D3 - RightUpper Trigger
   D4 - LeftUpper Trigger
   D5 - LeftLower Trigger

   D6 - Dpad Down
   D7 - Dpad Up
   D8 - Dpad Right
   D9 - Dpad Left

   D10 - Select
   D11 - Start

   D12 - Stick (button)
   A0 - Yaxis (positive = Up)
   A1 - Xaxis (positive = Right)

   D20 - A (inner button)
   D21 - B (outer button)

   D13 - Status LED (teensy only)

   Code uses ResponsiveAnalog Read
   Might use extreme Joystick for 16 bit
 */
 
#include <ResponsiveAnalogRead.h>
#include <Bounce2.h>

//troubleshoot
#define SERIALOK 0
#define FILTEROK 0
#define INVERTY 0
#define INVERTX 0

//timings
#define LDEL 20 //loop delay in ms
#define BNC_INT 10 //debounce interval on all buttons
#define FLASHY //led flash delay

//triggers
#define T_RL 2
#define T_RU 3
#define T_LU 4
#define T_LL 5

//D pad
#define D_DN 6
#define D_UP 7
#define D_LE 8
#define D_RI 9

//Select (left) start (right)
#define B_SEL 10
#define B_STA 11

//A and B
#define B_A 20
#define B_B 21

//Analog stick
#define B_ANA 12
#define X_ANA A0
#define Y_ANA A1

//status LED
#define LED 13

//analog objects
ResponsiveAnalogRead xaxis(X_ANA, true);
ResponsiveAnalogRead yaxis(Y_ANA, true);

//snapMultiplier 0.01 default

//Bounce objects
Bounce bn0 = Bounce();
Bounce bn1 = Bounce();
Bounce bn2 = Bounce();
Bounce bn3 = Bounce();
Bounce bn4 = Bounce();
Bounce bn5 = Bounce();
Bounce bn6 = Bounce();
Bounce bn7 = Bounce();
Bounce bn8 = Bounce();
Bounce bn9 = Bounce();
Bounce bn10 = Bounce();
Bounce bn11 = Bounce();
Bounce bn12 = Bounce();

//globals
int xVal = 537;
int yVal = 537;

//constants
const float deg45 = 0.70716;

void setup() {
  if(SERIALOK){
    Serial.begin(9600);
  }
  //buttons
  pinMode(T_RL, INPUT_PULLUP);
  pinMode(T_RU, INPUT_PULLUP);
  pinMode(T_LU, INPUT_PULLUP);
  pinMode(T_LL, INPUT_PULLUP);
  pinMode(D_DN, INPUT_PULLUP);
  pinMode(D_UP, INPUT_PULLUP);
  pinMode(D_LE, INPUT_PULLUP);
  pinMode(D_RI, INPUT_PULLUP);
  pinMode(B_SEL, INPUT_PULLUP);
  pinMode(B_STA, INPUT_PULLUP);
  pinMode(B_ANA, INPUT_PULLUP);
  pinMode(B_A, INPUT_PULLUP);
  pinMode(B_B, INPUT_PULLUP);

  //attach bounces
  bn0.attach(T_RL);
  bn1.attach(T_RU);
  bn2.attach(T_LU);
  bn3.attach(T_LL);
  bn4.attach(D_DN);
  bn5.attach(D_UP);
  bn6.attach(D_LE);
  bn7.attach(D_RI);
  bn8.attach(B_SEL);
  bn9.attach(B_STA);
  bn10.attach(B_ANA);
  bn11.attach(B_A);
  bn12.attach(B_B);

  //intervals
  bn0.interval(BNC_INT);
  bn1.interval(BNC_INT);
  bn2.interval(BNC_INT);
  bn3.interval(BNC_INT);
  bn4.interval(BNC_INT);
  bn5.interval(BNC_INT);
  bn6.interval(BNC_INT);
  bn7.interval(BNC_INT);
  bn8.interval(BNC_INT);
  bn9.interval(BNC_INT);
  bn10.interval(BNC_INT);
  bn11.interval(BNC_INT);
  bn12.interval(BNC_INT);
  
  //axiseses
  pinMode(X_ANA, INPUT);
  pinMode(Y_ANA, INPUT);

  //LEDs
  pinMode(LED, OUTPUT);

  //status
  for(int i = 0; i<=5 ; i++){
    digitalWrite(LED, HIGH);
    delay(FLASHY);
    digitalWrite(LED, LOW);
    delay(FLASHY);
  }

}

void loop() {
  //read
  analogs();
  buttons();

  //change
  translate();

  //troubleshoot outs
  serialOuts();

  //joystick outs
  joyWrite();

  //delay
  delay(LDEL);
}

//get analog values
void analogs(){
  xaxis.update();
  yaxis.update();

  xVal = xaxis.getValue();
  yVal = yaxis.getValue();
}

//print troubleshooting stuff
void serialOuts(){
  if(SERIALOK){
  //  Serial.print(xaxis.getRawValue());
  //  Serial.print(" ");
    Serial.print(analogRead(X_ANA));
    Serial.print(" ");
    Serial.print(analogRead(Y_ANA));
    Serial.print(" filter: ");
    Serial.print(xVal);
    Serial.print(" ");
    Serial.print(yVal);
  //  if(xaxis.hasChanged()) {
  //    Serial.print(" changed");
  //  }
    Serial.println("");
  }
}

//do any modifications to the axis globals
void translate(){
  if(INVERTX){
    xVal = 1024 - xVal;
  }
  if(INVERTY){
    yVal = 1024 - yVal;
  }
  if(FILTEROK){
    //sin and cos 45 is 0.70709
    //so x' =  x*cos(45) + y*sin(45)
    //   y' = -x*sin(45) + y*cos(45)
    //   x' = ( x + y)*(0.70709)
    //   y' = (-x + y)*(0.70709)
    //https://www.siggraph.org/education/materials/HyperGraph/modeling/mod_tran/2drota.htm
  
    // map to center first though
    float xPrime = float(xVal);
    float yPrime = float(yVal);
    float xPrimePrime = 0;
    float yPrimePrime = 0;
  
    //map to origin
    xPrime -= 512;
    yPrime -= 512;
  
    //calc rotation
    xPrimePrime = (xPrime * deg45) - (yPrime * deg45);
    yPrimePrime = (xPrime * deg45) + (yPrime * deg45);
  
    //offset back to coordinates
    xPrime = xPrimePrime + 512;
    yPrime = yPrimePrime + 512;
  
    //cheating using map function, idk
    xPrime = map(xPrime, -300, 1230, 0, 1024);
    yPrime = map(yPrime, -300, 1230, 1024, 0);
  
  
    //cast back into int
    xVal = int(xPrime);
    yVal = int(yPrime);
  }
}

//update all the buttons
void buttons(){
  bn0.update();
  bn1.update();
  bn2.update();
  bn3.update();
  bn4.update();
  bn5.update();
  bn6.update();
  bn7.update();
  bn8.update();
  bn9.update();
  bn10.update();
  bn11.update();
  bn12.update();
}

void joyWrite(){
  //update Analog values
  Joystick.X(xVal);
  Joystick.Y(yVal);

  //update joystick buttons
  //falling edge
  if (bn0.fallingEdge()) { Joystick.button(1, 1); }
  if (bn1.fallingEdge()) { Joystick.button(2, 1); }
  if (bn2.fallingEdge()) { Joystick.button(3, 1); }
  if (bn3.fallingEdge()) { Joystick.button(4, 1); }
  if (bn4.fallingEdge()) { Joystick.button(5, 1); }
  if (bn5.fallingEdge()) { Joystick.button(6, 1); }
  if (bn6.fallingEdge()) { Joystick.button(7, 1); }
  if (bn7.fallingEdge()) { Joystick.button(8, 1); }
  if (bn8.fallingEdge()) { Joystick.button(9, 1); }
  if (bn9.fallingEdge()) { Joystick.button(10, 1); }
  if (bn10.fallingEdge()) { Joystick.button(11, 1); }
  if (bn11.fallingEdge()) { Joystick.button(12, 1); }
  if (bn12.fallingEdge()) { Joystick.button(13, 1); }

  //rising edge
  if (bn0.risingEdge()) { Joystick.button(1, 0); }
  if (bn1.risingEdge()) { Joystick.button(2, 0); }
  if (bn2.risingEdge()) { Joystick.button(3, 0); }
  if (bn3.risingEdge()) { Joystick.button(4, 0); }
  if (bn4.risingEdge()) { Joystick.button(5, 0); }
  if (bn5.risingEdge()) { Joystick.button(6, 0); }
  if (bn6.risingEdge()) { Joystick.button(7, 0); }
  if (bn7.risingEdge()) { Joystick.button(8, 0); }
  if (bn8.risingEdge()) { Joystick.button(9, 0); }
  if (bn9.risingEdge()) { Joystick.button(10, 0); }
  if (bn10.risingEdge()) { Joystick.button(11, 0); }
  if (bn11.risingEdge()) { Joystick.button(12, 0); }
  if (bn12.risingEdge()) { Joystick.button(13, 0); }

  //now write the goober
  Joystick.send_now();
}


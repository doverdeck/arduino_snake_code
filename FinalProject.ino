#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
#ifndef _BV
#define _BV(bit) (1<<(bit))
#endif

Adafruit_LEDBackpack matrix = Adafruit_LEDBackpack();
int start = 0;
int orientation=3;
int size=2;
class Snake {       // The class
  public:             // Access specifier
    int posx;        // Attribute (int variable)
    int posy;  // Attribute (string variable)
    int oldposx;
    int oldposy;
};
class Fruit {       // The class
  public:             // Access specifier
    int posx;        // Attribute (int variable)
    int posy;  // Attribute (string variable)
};
Fruit fruit;
Snake snakearr[64];
Snake emptyarr[1];
// LEDs - color/purpose/pin number
int const green8 = 8;    // led3  LED3   blueLED3
int const yellow9=9;    // led4 LED4 greenLED4
int const red10=10;     // led5 LED5 greenLED5
// Buttons - type/purpose/pin number
int const button3 = 3;
const int SW_pin = 4; // digital pin connected to switch output
const int X_pin = A3; // analog pin connected to X output
const int Y_pin = A2; // analog pin connected to Y output
const int potPin = A1; 
int switchState=0;
int lock=1;
void setup() {
  // declare the LED pins as outputs
  pinMode( green8 , OUTPUT );
  pinMode( yellow9 , OUTPUT );
  pinMode( red10 , OUTPUT );
  pinMode(SW_pin, INPUT);
  digitalWrite(SW_pin, HIGH);
  // declare the switch pin as an input
  pinMode( button3 , INPUT );
  matrix.begin(0x70);  // pass in the address
  matrix.clear();
  randomSeed(analogRead(0));
  Serial.begin(9600);   // Needed to do print statements
}

int difficulty(){
  int potVal=analogRead(potPin);
  Serial.println("READ");
  int diff=1;
  if (potVal<333){
    diff=1;
  }
  if((potVal>333)&&(potVal<667)){
    diff=2;
  }
  if(potVal>667){
    diff=3;
  }
  return diff;
}

//Runs the code that turns the lights on constantly and the code that checks if the button is pressed
void loop() {
  int potVal=analogRead(potPin);
  Serial.println("potVal" + (String)potVal);
  //runs all the methods
  // printDebug();
  // controlLEDs(0); 
  int switchState = digitalRead( button3 ) ;
  if (switchState==1){
    lock=0;
  }
  if (lock==0){
    if (start==0){
      Serial.println("start");
      Snake a;
      a.posx=3;
      a.posy=3;
      snakearr[0]=a;
      Snake b;
      b.posx=3;
      b.posy=4;
      snakearr[1]=b;
      spawnFruit();
      start=1;
      size=2;
      paint();
      digitalWrite( red10, HIGH ); 
      delay(1000);
      digitalWrite( red10, LOW ); 
      digitalWrite( yellow9, HIGH ); 
      delay(1000);
      digitalWrite( yellow9, LOW ); 
      digitalWrite(  green8, HIGH ); 
      delay(1000);
      digitalWrite(  green8, LOW ); 
    }
    joystick();
    Serial.println("Orientation "+ (String)orientation);
    move();
  }
  int diff=difficulty();
  if (diff==1){
  delay(2000);
  }
  else if (diff==2){
  delay(1000);
  }
  else if (diff==3){
  delay(500);
  }
}

int joystick(){
  if ((analogRead(Y_pin)>650) && (abs(analogRead(Y_pin)-540)>abs(analogRead(X_pin)-540)) && (orientation!=1)){
    orientation=3; //down
  }
  else if ((analogRead(X_pin)>650) && ((abs(analogRead(Y_pin)-540))<abs(analogRead(X_pin)-540)) && (orientation!=4)){
    orientation=2; //right
  }
  else if ((analogRead(Y_pin)<450) && (abs(analogRead(Y_pin)-540)>abs(analogRead(X_pin)-540))&& (orientation!=3)){
    orientation=1; //up
  }
  else if ((analogRead(X_pin)<450) && ((abs(analogRead(Y_pin)-540))<abs(analogRead(X_pin)-540))&& (orientation!=2)){
    orientation=4; //left
  }
  return orientation;
}
void paint(){
//paint updates the matrix 
//clears the matrix
  matrix.clear();
//for loop for code that uses recursion to save pixels in same row and traverses all snake objects
  for (uint8_t i=0; i<size; i++){
  matrix.displaybuffer[snakearr[i].posx] = matrix.displaybuffer[snakearr[i].posx]|_BV(snakearr[i].posy);
  }
  //same idea with fruit object
  matrix.displaybuffer[fruit.posx] =matrix.displaybuffer[fruit.posx]| _BV(fruit.posy);
  //displays the code
  matrix.writeDisplay();
}
void endGame(){
  orientation=3;
  Serial.println("DEAD");
  matrix.clear();
  matrix.writeDisplay();
  lock=1;
  start=0;
}
void checkCollide(){
  for (int i=1; i<size; i++){
  if (snakearr[0].posx==snakearr[i].posx && snakearr[0].posy==snakearr[i].posy){
    Serial.println("COLLIDE");
    endGame();
  }
}
}
void spawnFruit()
{
  fruit.posx=(int)random(7);
  fruit.posy=(int)random(7);
  for (int i=0; i<size; i++){
  if (snakearr[i].posx==fruit.posx && snakearr[i].posy==fruit.posy){
  spawnFruit();
  }
  }
  Serial.println("Fruit posx" + (String)fruit.posx);
  Serial.println("Fruit posy" + (String)fruit.posy);
}
void checkFruit(){
  if (snakearr[0].posx==fruit.posx && snakearr[0].posy==fruit.posy){
    Snake c;
    c.posx=snakearr[size-1].oldposx;
    c.posy=snakearr[size-1].oldposy;
    snakearr[size]=c;
    size=size+1;
    Serial.println("Size "+(String)size);
    spawnFruit();
  }
}
void move(){
  for (int i=0; i<size; i++){
    //Serial.println("i "+(String)i);
    if(i==0){
    snakearr[0].oldposx=snakearr[0].posx;
    snakearr[0].oldposy=snakearr[0].posy;
    if (orientation==2){
    snakearr[0].posx=snakearr[0].posx+1;
    if (snakearr[0].posx==8){
      endGame();
    }
    }
    if (orientation==4){
    snakearr[0].posx=snakearr[0].posx-1;
    if (snakearr[0].posx==-1){
      endGame();
    }
    }
    if (orientation==1){
    snakearr[0].posy=snakearr[0].posy+1;
    if (snakearr[0].posy==7){
    Serial.println("WHYYYY");
    endGame();
    }
    if (snakearr[0].posy==8){
    snakearr[0].posy=0;
    }
    }
    if (orientation==3){
    snakearr[0].posy=snakearr[0].posy-1;
    if ((snakearr[0].posy)==6){
    endGame();
    }
    if ((snakearr[0].posy)==-1){
    snakearr[0].posy=7;
    }
    }
    checkCollide();
    checkFruit();
    }
    else {
    snakearr[i].oldposx=snakearr[i].posx;
    snakearr[i].oldposy=snakearr[i].posy;
    snakearr[i].posx=snakearr[i-1].oldposx;
    snakearr[i].posy=snakearr[i-1].oldposy;
    }
  }
  Serial.println("Posx"+ (String)snakearr[0].posx);
  Serial.println("Posy"+ (String)snakearr[0].posy);
  paint();
}
/*
* This function manages all the debug statements
*/
void printDebug()
{
  // read the value of the switch
  // digitalRead() checks to see if there is voltage on the pin or not
  //prints out all the debugs for the button, the joystick, and the potentiometer
  switchState = digitalRead( button3 ) ;
  Serial.print("Button: ");
  Serial.print(switchState);
  Serial.print("\n");
  Serial.print("Switch:  ");
  Serial.print(digitalRead(SW_pin));
  Serial.print("\n");
  Serial.print("X-axis: ");
  Serial.print(analogRead(X_pin));
  Serial.print("\n");
  Serial.print("Y-axis: ");
  Serial.println(analogRead(Y_pin));
  int potVal=analogRead(potPin);
  Serial.println("potVal = " + (String)potVal); 
}
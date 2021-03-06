#include <Adafruit_NeoPixel.h>
#include <avr/power.h>

#define PIN_EYE_LEFT 6
#define PIN_EYE_RIGHT 7
#define PIN_HEART 8

#define pinPix 6                       // Pin driving NeoPixel Ring or String
#define numPix 7                       // Number of NeoPixels in the Ring or Strip
#define numEyePix 7
#define numHeartPix 16


enum States{
  SLEEP,
  SLEEPING,
  WAKEUP,
  NORMAL,
  FEEL_POSITIVE,
  FEEL_NEGATIVE,
  FEEL_EXCITED,
  HRM_MODE
};

States eye_state = SLEEPING;
States heart_state = NORMAL;



// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel right_eye_strip = Adafruit_NeoPixel(numEyePix, PIN_EYE_RIGHT, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel left_eye_strip = Adafruit_NeoPixel(numEyePix, PIN_EYE_LEFT, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel heart_strip = Adafruit_NeoPixel(numHeartPix, PIN_HEART, NEO_GRB + NEO_KHZ800);


// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.


String inputString = "";         // a string to hold incoming data
boolean toggleComplete = false;

int bright_level;
int hrm_level=60;
int heartColorIndex=0;

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code

  // initial serial
  Serial.begin(9600);

  right_eye_strip.begin();
  right_eye_strip.show(); // Initialize all pixels to 'off'
  left_eye_strip.begin();
  left_eye_strip.show(); // Initialize all pixels to 'off'
  
  heart_strip.begin();
  heart_strip.show();
}

void loop() {


  // Check for new command 
  while (Serial.available() && toggleComplete == false ) {
    char inChar = (char)Serial.read();
    if(inChar == 'E'){ // end character for toggle LED
      toggleComplete = true;
    }   
    else{
      inputString += inChar;
    }
  }

  if(!Serial.available() && toggleComplete == true)
    {
      // convert String to int.
      parseInput();
      toggleComplete = false;
        
    }


  // Local State Transition
  
  if(eye_state == NORMAL){


    if(heart_state == NORMAL){ 
      heartRainbowCycle();
     // heartChaseRainbow();
     // grow_color(50,20, 255,255,0);
    }else if(heart_state == FEEL_EXCITED){

      heartExcited();

      heart_state = NORMAL;
      
    }else if(heart_state == HRM_MODE){

      heartBeat(hrm_level);
      heart_state = NORMAL;        
    }

    
  }else if(eye_state == WAKEUP){

    wakeup();
    
    eye_state = NORMAL;
    heart_state = NORMAL;

  }else if(eye_state == FEEL_POSITIVE){

   
    for (int i=0; i<numEyePix; i++) {
      right_eye_strip.setPixelColor(i,0,255,0);
      left_eye_strip.setPixelColor(i,0,255,0);
      right_eye_strip.setBrightness(bright_level);
      left_eye_strip.setBrightness(bright_level);     
      
    }
    right_eye_strip.show();
    left_eye_strip.show();
    eye_state = NORMAL;
      
  }else if (eye_state == FEEL_NEGATIVE){

    
    for (int i=0; i<numEyePix; i++) {
      right_eye_strip.setPixelColor(i,0,0,255);
      left_eye_strip.setPixelColor(i,0,0,255);
      right_eye_strip.setBrightness(bright_level);
      left_eye_strip.setBrightness(bright_level);
          
    }

     right_eye_strip.show();
     left_eye_strip.show();
     eye_state = NORMAL;
      
  }else if (eye_state == SLEEP){
    
    sleep();
    eye_state = SLEEPING;
    heart_state = SLEEPING;
  }
  
   
  delay(50);

}

void parseInput()
{
  char charHolder[inputString.length()+1];
  
  inputString.toCharArray(charHolder,inputString.length()+1);
  
  if(inputString.startsWith("FP")){

    eye_state = FEEL_POSITIVE;

    bright_level = atoi(charHolder+2);
 
  }else if (inputString.startsWith("FN")){

    eye_state = FEEL_NEGATIVE; 
    bright_level = atoi(charHolder+2);
 
  }else if (inputString.startsWith("WK")){

    // wake up
    eye_state = WAKEUP;
    
  }else if (inputString.startsWith("XX")){

    heart_state = FEEL_EXCITED;
    
  }else if (inputString.startsWith("SL")){
    eye_state = SLEEP;

  }else if (inputString.startsWith("HR")){

    heart_state = HRM_MODE;
    hrm_level = atoi(charHolder+2);
  }
    
  inputString = "";
  
}

void grow_color(int pause, int steps, byte R, byte G, byte B) { 

  int tmpR, tmpG, tmpB;         // Temp values
  
  // Fade up
  for (int s=1; s<=steps; s++) {
    tmpR = (R * s) / steps;     // Multiply first to avoid truncation errors  
    tmpG = (G * s) / steps;
    tmpB = (B * s) / steps;
    
    for (int i=0; i<numEyePix; i++) {
      right_eye_strip.setPixelColor(i,tmpR,tmpG,tmpB);
      left_eye_strip.setPixelColor(i,tmpR,tmpG,tmpB);
    }
    right_eye_strip.show();
    left_eye_strip.show();
    delay(pause);
  }    

  // Fade down
  for (int s=steps; s>0; s--) {
    tmpR = (R * s) / steps;     // Multiply first to avoid truncation errors  
    tmpG = (G * s) / steps;
    tmpB = (B * s) / steps;
    
    for (int i=0; i<numEyePix; i++) {
      right_eye_strip.setPixelColor(i,tmpR,tmpG,tmpB);
      left_eye_strip.setPixelColor(i,tmpR,tmpG,tmpB);
    }
    
    right_eye_strip.show();
    left_eye_strip.show();
    delay(pause);
  }    

  delay(pause * 10);
}


void heartBeat (int rate){

  int beat_delay = (int) (60000 / rate);

  int count = 20;

  while(count--){

    for (volatile int i=0; i<heart_strip.numPixels(); i++){
      heart_strip.setPixelColor(i, heart_strip.Color(255, 0, 0));
            
    }
    heart_strip.show();
    delay(150);


    for (volatile int i=0; i<heart_strip.numPixels(); i++){
      heart_strip.setPixelColor(i, heart_strip.Color(20, 0, 0));
            
    }
    heart_strip.show();
    delay(beat_delay);

        
  }
  
}

void heartExcited(){


  for(int j=0;j<20;j++){
    
    for (int i=0; i<numHeartPix; i++) {
      heart_strip.setPixelColor(i,255,0,0);
    }

    heart_strip.show();

    delay(200);

    for (int i=0; i<numHeartPix; i++) {
      heart_strip.setPixelColor(i,0);
    }

    heart_strip.show();
    delay(200);
  }
  
}

void heartRainbowCycle() {

  uint16_t i, j;
  //uint8_t wait = 1;
  heartColorIndex++;
  
  if( heartColorIndex > 256 ){
     heartColorIndex = 0;
  }
  
  j = heartColorIndex;

  for(i=0; i< heart_strip.numPixels(); i++) {
    heart_strip.setPixelColor(i, Wheel(((i * 256 / heart_strip.numPixels()) + j) & 255));
  }
  
  heart_strip.show();
  //delay(wait);
  
}



void heartChaseRainbow() {
  
  uint8_t wait = 50;
   
  heartColorIndex++;
  if( heartColorIndex > 256 ){
     heartColorIndex = 0;
  }
  
  int j = heartColorIndex;
  
  for (int q=0; q < 3; q++) {
      for (int i=0; i < heart_strip.numPixels(); i=i+3) {
        heart_strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      heart_strip.show();
         
      delay(wait);
         
      for (int i=0; i < heart_strip.numPixels(); i=i+3) {
          heart_strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
  }
 
}




void sleep() {

  int pause = 80;
  int steps = 50;

  int tmpR, tmpG, tmpB;         // Temp values


  for (int s=steps; s>=0; s--) {
    tmpR = (255 * s) / steps;     // Multiply first to avoid truncation errors  
    tmpG = (255 * s) / steps;
    tmpB = (0 * s) / steps;
    
    for (int i=0; i<numEyePix; i++) {
      right_eye_strip.setPixelColor(i,tmpR,tmpG,tmpB);
      left_eye_strip.setPixelColor(i,tmpR,tmpG,tmpB);
    }
    right_eye_strip.show();
    left_eye_strip.show();
    delay(pause);
  }   

  for (int i=0; i<numHeartPix; i++) {
      heart_strip.setPixelColor(i,0,0,0);
  }

  heart_strip.show();
  heart_state = SLEEPING;
    
}

void wakeup(){

  int pause = 80;
  int steps = 50;

  int tmpR, tmpG, tmpB;         // Temp values


  for (int s=1; s<=steps; s++) {
    tmpR = (255 * s) / steps;     // Multiply first to avoid truncation errors  
    tmpG = (255 * s) / steps;
    tmpB = (0 * s) / steps;
    
    for (int i=0; i<numEyePix; i++) {
      right_eye_strip.setPixelColor(i,tmpR,tmpG,tmpB);
      left_eye_strip.setPixelColor(i,tmpR,tmpG,tmpB);
    }
    right_eye_strip.show();
    left_eye_strip.show();
    delay(pause);
  }          
}


// Helper Functions

void setFixColor(uint32_t c, uint8_t wait) {

  for(uint16_t i=0; i<right_eye_strip.numPixels(); i++) {
      right_eye_strip.setPixelColor(i, c);
      left_eye_strip.setPixelColor(i, c);
      //strip.setBrightness(bright_level);
      //strip2.setBrightness(bright_level);
      right_eye_strip.show();
      left_eye_strip.show();
      delay(wait);
  }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return heart_strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return heart_strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return heart_strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}


/*
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip2.setPixelColor(i, c);
      strip.setBrightness(bright_level);
      strip2.setBrightness(bright_level);
      strip.show();
      delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}



//Theatre-style crawling lights with rainbow effect

*/

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
/*

*/


// Pause = delay between transitions
// Steps = number of steps
// R, G, B = Full-on RGB values


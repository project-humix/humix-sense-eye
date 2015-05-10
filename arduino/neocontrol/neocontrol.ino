#include <Adafruit_NeoPixel.h>
#include <avr/power.h>

#define PIN 6
#define PIN2 7

#define pinPix 6                       // Pin driving NeoPixel Ring or String
#define numPix 7                       // Number of NeoPixels in the Ring or Strip

enum States{
  SLEEP,
  WAKEUP,
  NORMAL,
  FEEL_POSITIVE,
  FEEL_NEGATIVE,
  FEEL_EXCITED 
};

States eye_state = SLEEP;
States heart_state = NORMAL;


// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(7, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(7, PIN2, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.


String inputString = "";         // a string to hold incoming data
boolean toggleComplete = false;

int bright_level;


void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code

  // initial serial
  Serial.begin(9600);

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  strip2.begin();
  strip2.show(); // Initialize all pixels to 'off'
}

void loop() {



  while (Serial.available() && toggleComplete == false ) {
    char inChar = (char)Serial.read();
    if(inChar == 'E'){ // end character for toggle LED
      toggleComplete = true;
    }   
    else{
      inputString += inChar;
    }

  }


 

  // Toggle LED 13
  if(!Serial.available() && toggleComplete == true)
    {
      // convert String to int.
      parseInput();

      toggleComplete = false;
        
    }


  if(eye_state == WAKEUP){

    // TODO
    wakeup();
    
    //letsDoIt(30, 50, 255,255,0);     
    eye_state == NORMAL;

  }else if(eye_state == FEEL_POSITIVE){
 
    colorWipe(strip.Color(0, 255, 0), 50); // Green
      
    //strip.setBrightness(bright_level);    
      
  }else if (eye_state == FEEL_NEGATIVE){
    
    colorWipe(strip.Color(0, 0, 255), 50); // Green
    //strip.setBrightness(bright_level);
      
  }else{
    letsDoIt(30, 50, 255,255,0);     
  }
   
  delay(100);

// different control effects
 /*
  colorWipe(strip.Color(255, 0, 0), 50); // Red
  colorWipe(strip.Color(0, 255, 0), 50); // Green
  colorWipe(strip.Color(0, 0, 255), 50); // Blue
  // Send a theater pixel chase in...
  theaterChase(strip.Color(127, 127, 127), 50); // White
*/
  //theaterChase(strip.Color(127,   0,   0), 50); // Red
  //theaterChase(strip.Color(  0,   0, 127), 50); // Blue

  //rainbow(20);
  //rainbowCycle(20);
  //theaterChaseRainbow(50);
  //letsDoIt(30, 50, 255,255,0); 

}


void parseInput()
{
  char charHolder[inputString.length()+1];
  
  inputString.toCharArray(charHolder,inputString.length()+1);
  
  if(inputString.startsWith("FP")){

    eye_state = FEEL_POSITIVE;

  }else if (inputString.startsWith("FN")){

    eye_state = FEEL_NEGATIVE; 

  }else if (inputString.startsWith("WK")){

    // wake up
    eye_state = WAKEUP;
    
  }else if (inputString.startsWith("XX")){

    // getting exciting

    
  }
  
  
  
  inputString = "";
  
  bright_level = atoi(charHolder+2);
  
}

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

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
     
      delay(wait);
     
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
        }
        strip.show();
       
        delay(wait);
       
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, 0);        //turn every third pixel off
        }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}


// Pause = delay between transitions
// Steps = number of steps
// R, G, B = Full-on RGB values
void letsDoIt(int pause, int steps, byte R, byte G, byte B) { 

  int tmpR, tmpG, tmpB;         // Temp values
  
  // Fade up
  for (int s=1; s<=steps; s++) {
    tmpR = (R * s) / steps;     // Multiply first to avoid truncation errors  
    tmpG = (G * s) / steps;
    tmpB = (B * s) / steps;
    
    for (int i=0; i<numPix; i++) {
      strip.setPixelColor(i,tmpR,tmpG,tmpB);
      strip2.setPixelColor(i,tmpR,tmpG,tmpB);
    }
    strip.show();
    delay(pause);
  }    

  // Fade down
  for (int s=steps; s>0; s--) {
    tmpR = (R * s) / steps;     // Multiply first to avoid truncation errors  
    tmpG = (G * s) / steps;
    tmpB = (B * s) / steps;
    
    for (int i=0; i<numPix; i++) {
      strip.setPixelColor(i,tmpR,tmpG,tmpB);
      strip2.setPixelColor(i,tmpR,tmpG,tmpB);
    }
    strip.show();
    delay(pause);
  }    

  delay(pause * 10);
}


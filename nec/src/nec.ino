


///////REMEMBER TO SET THIS FUCKING VALUE

// #define ArduinoB

#define STRIP_PIN_L 2
#define STRIP_PIN_R 9
#define NUMBEROFPIXELS 8
#define NUMSTRIPS 2
#define NUMBEROFTIMERS 6

#include <Adafruit_NeoPixel.h>
#include "wrapper_class.h"

// // Parameter 1 = number of pixels in strip
// // Parameter 2 = pin number (most are valid)
// // Parameter 3 = pixel type flags, add together as needed:
// //   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
// //   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
// //   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
// //   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
// // Adafruit_NeoPixel stripLB = Adafruit_NeoPixel(NUMBEROFPIXELSCORNER, STRIP_PIN_LB, NEO_GRB + NEO_KHZ800);
// Adafruit_NeoPixel stripRobot = Adafruit_NeoPixel(NUMBEROFPIXELSROBOT, STRIP_PIN_ROBOT, NEO_GRB + NEO_KHZ800);
// Adafruit_NeoPixel stripCenter = Adafruit_NeoPixel(NUMBEROFPIXELSCENTER, STRIP_PIN_CENTER, NEO_GRB + NEO_KHZ800);
// Adafruit_NeoPixel stripRF = Adafruit_NeoPixel(NUMBEROFPIXELSCORNER, STRIP_PIN_RF, NEO_GRB + NEO_KHZ800);
// Adafruit_NeoPixel stripLF = Adafruit_NeoPixel(NUMBEROFPIXELSCORNER, STRIP_PIN_LF, NEO_GRB + NEO_KHZ800);
// // Adafruit_NeoPixel stripRB = Adafruit_NeoPixel(NUMBEROFPIXELSCORNER, STRIP_PIN_RB, NEO_GRB + NEO_KHZ800);
#include <Brain.h>

// Set up the brain parser, pass it the hardware serial object you want to listen on.
Brain brain(Serial);

uint8_t fadeSpeed             = 1;
int     attentionLevel        = 0;
int     attentionLevelOld     = 0;
int     meditationLevel       = 0;
int     meditationLevelOld    = 0;
int     signalStrength        = 0;
uint32_t  delta               = 0;
uint32_t  deltaOld            = 0;
uint32_t  theta               = 0;
uint32_t  thetaOld            = 0;
bool    setStrip              = true;
bool    aGoodSignalStreak     = false;
bool    colorReached          = false;
bool    started               = false;
bool    isMeditation          = false;
bool    isAttenttion          = false;
bool    isNeutral             = false;
bool    isTheta               = false;
bool    isDelta               = false;
bool    isNeutralDT           = false;
uint8_t end                   = 10;

uint32_t waitTime[NUMBEROFTIMERS];
uint32_t currentTime[NUMBEROFTIMERS];

String inByte;

Wrapper_class strips[] = {

  Wrapper_class(NUMBEROFPIXELS, STRIP_PIN_R),
  Wrapper_class(NUMBEROFPIXELS, STRIP_PIN_L),
};


void setup() {
  Serial.begin(57600);
  for (int i = 0; i < NUMSTRIPS; ++i)
  {
    strips[i].init();
    strips[i].setStripColor(0,255,0);
    strips[i].show();
  }
  delay(2000);
  setTargetColor(255,255,255);

  //wait Nr 1:
  wait(3000,0);
  //wait Nr 2: Checks
  wait(5000,1);
  //wait Nr3: Checks Signal Quality
  wait(500,2);
  Serial.print("Setup");
}

void loop() {


  if (Serial.available() > 0){

    if (brain.update()) {

      // Check for good connection and preCalc Data
      if(checkTimers(2)){
        signalStrength = brain.readSignalQuality();
        // Serial.println(signalStrength);
        // if(signalStrength == 0){
        //   aGoodSignalStreak = true;
        // }
        // else{
        //     aGoodSignalStreak = false;
        // }
        //
        aGoodSignalStreak = signalStrength == 0 ? true : false;
        // Serial.print("good: ");
        // Serial.println(aGoodSignalStreak);
      }

      // brain.printCSV();
      attentionLevel = brain.readAttention();
      meditationLevel = brain.readMeditation();

      delta = brain.eegPower[0]%100; // * (1.8/4096) ) / 2000; //voltage conversion
      theta = brain.eegPower[1]%100; // * (1.8/4096) ) / 2000;


      if(aGoodSignalStreak && colorReached) {
        aGoodSignalStreak = false;
        if(attentionLevel != attentionLevelOld || meditationLevel != meditationLevelOld){
          // Serial.println("attentionLevel: ");
          // Serial.println(attentionLevel);
          // Serial.println("meditationLevel: ");
          // Serial.println(meditationLevel);
          // Serial.println("In eSense before timer");
          if(checkTimers(0)){
            // Serial.println("In eSense");
            checkColorEsense();
            attentionLevelOld = attentionLevel;
            meditationLevelOld = meditationLevel;
          }
        }

      }else if (brain.readSignalQuality() > 0 && colorReached) {
         if (delta != deltaOld && theta != thetaOld){
            if(delta > 0 || theta > 0){
              // if(checkTimers(0)){
              //   Serial.print("In raw");
              // }
              // Serial.print("delta: ");
              // Serial.println(delta);
              // Serial.print("theta: ");
              // Serial.println(theta);
            }
            if(checkTimers(1)){
              checkColorRaw();
            }
          }
          deltaOld = delta;
          thetaOld = theta;
      }
    }
  }


  if(!colorReached){
    if(fadeSpeed > 0){
     for (int i = 0; i < NUMSTRIPS; ++i)
      {
        fadeOut(i);
      }
      // Serial.println("In fade");
    }
  }

  if(colorReached && !started){
    started = true;
  }

}


void showStrips(){
  for(int i = 0; i < NUMSTRIPS; i++){
    strips[i].show();
  }
  setStrip = true;
}


int setColor(uint8_t r, uint8_t g, uint8_t b){

   for(int i = 0; i < NUMSTRIPS; i++){
   strips[i].setStripColor(r,g,b);
  }

}

int setTargetColor(uint8_t r, uint8_t g, uint8_t b){

  for(int i = 0; i < NUMSTRIPS; i++){
    strips[i].targetColorR = r;
    strips[i].targetColorG = g;
    strips[i].targetColorB = b;
  }

}

void checkColorEsense(){

   if(attentionLevel >= 60 && meditationLevel < 60 && colorReached && !isAttenttion){
      setTargetColor(255,0,0);
      colorReached = false;
      // Serial.println("In attention");
      isMeditation = false;
      isAttenttion = true;
      isNeutral = false;
      Serial.println('1');
    }

    if (meditationLevel >= 60 && attentionLevel < 60 && colorReached && !isMeditation){
      setTargetColor(0,255,0);
      colorReached = false;
      // Serial.println("In meditation");
      isMeditation = true;
      isAttenttion = false;
      isNeutral = false;
      Serial.println('2');
    }

    if (meditationLevel < 60 && attentionLevel < 60 && colorReached && !isNeutral){
      setTargetColor(255,160,0);
      colorReached = false;
      // Serial.println("In neutral");
      isMeditation = false;
      isAttenttion = false;
      isNeutral = true;
      Serial.println('3');
    }

}

void checkColorRaw(){

   if(delta >= 70 && theta < 70 && colorReached && !isDelta){
      setTargetColor(255,0,0);
      colorReached = false;
      // Serial.println("In attention");
      isTheta = false;
      isDelta = true;
      isNeutralDT = false;
      Serial.println('4');
    }

    if (theta >= 70 && delta < 70 && colorReached && !isTheta){
      setTargetColor(0,255,0);
      colorReached = false;
      // Serial.println("In meditation");
      isTheta = true;
      isDelta = false;
      isNeutralDT = false;
      Serial.println('5');
    }

    if (theta < 70 && delta < 70 && colorReached && !isNeutralDT){
      setTargetColor(255,160,0);
      colorReached = false;
      // Serial.println("In neutral");
      isTheta = false;
      isDelta = false;
      isNeutralDT = true;
      Serial.println('6');
    }
}

//set up to 6 Timers:
void wait(int _waitTime, int _numberOfIndex){
  waitTime[_numberOfIndex] = _waitTime;
  currentTime[_numberOfIndex] = millis();

}

//check Time:
bool checkTimers(int _index){
  if(millis() - currentTime[_index] >= waitTime[_index]){
    // Serial.print("TimeStamp: ");
    // Serial.println(currentTime[_index]);
    currentTime[_index] = millis();
    return true;
  }else{
    return false;
  }
}



void fadeOut(int k){
    for(int i = 0; i < strips[k].getNumPixels(); i++){
      uint32_t color = strips[k].getPixelColor(i);

      int
      r = (uint8_t)(color >> 16),
      g = (uint8_t)(color >>  8),
      b = (uint8_t)color;

      r = (fadeSpeed >= abs(strips[k].targetColorR - r))?strips[k].targetColorR:r<strips[k].targetColorR?r+fadeSpeed:r-fadeSpeed;
      g = (fadeSpeed >= abs(strips[k].targetColorG - g))?strips[k].targetColorG:g<strips[k].targetColorG?g+fadeSpeed:g-fadeSpeed;
      b = (fadeSpeed >= abs(strips[k].targetColorB - b))?strips[k].targetColorB:b<strips[k].targetColorB?b+fadeSpeed:b-fadeSpeed;

      if(r == strips[k].targetColorR && g == strips[k].targetColorG && b == strips[k].targetColorB && !colorReached){
          // Serial.println("Fade done");
          colorReached = true;
      }


      strips[k].setPixelColor(i, r, g, b);
    }
    // delay(20);
}

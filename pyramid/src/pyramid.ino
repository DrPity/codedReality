///////REMEMBER TO SET THIS FUCKING VALUES
#define STRIP_PIN_1 2
#define STRIP_PIN_2 3
#define STRIP_PIN_3 4
#define NUMBEROFPIXELS 34
#define STRIPSEGMENTS 3
#define NUMSTRIPS 1

#include <Adafruit_NeoPixel.h>
#include "wrapper_class.h"
#include "stripSegments.h"


char end                    = '\n';

bool positionIsNotRequested = false;
bool shallFlickerInFade     = false;
bool watchdogActive         = false;
bool colorReached           = true;
bool shallFlicker           = false;
bool startRainbow           = false;
bool isSegments             = false;
bool ledsReady              = true;
bool setStrip               = false;

int parameterArray[6];
int splitArray[6];

int connectionTimeOut =  10;
int fadeOutSpeed      = 2;

long currentTime[8];
long waitTime[8];


int rc = 0;
int gc = 0;
int bc = 0;
int rt = 0;
int gt = 0;
int bt = 0;
int fade = 0;


unsigned long loopTime;

uint8_t fadeSpeed        = 1;

String inByte;

Wrapper_class strips[] = {
  Wrapper_class(NUMBEROFPIXELS, STRIP_PIN_1),
  // Wrapper_class(NUMBEROFPIXELS, STRIP_PIN_2),
  // Wrapper_class(NUMBEROFPIXELS, STRIP_PIN_3),
};

StripSegments segments[] = {
  StripSegments(0,20),
  StripSegments(21,30),
  StripSegments(31,34)
  // Wrapper_class(NUMBEROFPIXELS, STRIP_PIN_2),
  // Wrapper_class(NUMBEROFPIXELS, STRIP_PIN_3),
};

//------------------------------------------------------------------------------

void setup() {
  Serial.begin(57600);
  Serial.println("Before Init");
  for (int i = 0; i < NUMSTRIPS; ++i)
  {
    strips[i].init();
  }
  Serial.println("After");
  establishContact();
  wait(1,0);
  wait(1,1);
}


//------------------------------------------------------------------------------

int d = 0;
void loop() {

  if (Serial.available() > 0){
    // Serial.println("Serial Recieved");
    inByte = Serial.readStringUntil(end);
    inByte.trim();
    if(inByte.indexOf('T') == 0 && inByte.indexOf('t') == 1){
      setTargetColor(inByte);
    }
    if(inByte.indexOf('C') == 0 && inByte.indexOf('c') == 1){
      setColor(inByte);
    }
    if(inByte.indexOf('S') == 0 && inByte.indexOf('s') == 1){
      for (int i = 0; i < STRIPSEGMENTS; i++) {
        Serial.print(segments[i].lowerBound);
        Serial.println(" / ");
        Serial.println(segments[i].higherBound);
      }
    }


  }

  if(startRainbow){
    if(millis() - loopTime >= 60){
      for (int i = 0; i < NUMSTRIPS; i++){
        strips[i].rainbowCycle(20);
      }
      loopTime = millis();
    }
  }


  if(fadeSpeed > 0 && !colorReached && checkTimers(0)){
    // Serial.print("In fade Speed");
    shallFlicker = false;
    setStrip = true;
    for(int i = 0; i < NUMSTRIPS; i++){
      if(!isSegments){
        fadeOut(i);
      }else{
        fadeOutSegments(i);
      }
    }
  }
  // if(shallFlicker && checkTimers(1)){
  //   setStrip = true;
  //   // Serial.println("shallFlicker");
  //   for(int i = 0; i < NUMSTRIPS; i++){
  //     flicker(i);
  //     wait(random(50,150),1);
  //   }
  // }



  if(setStrip){
    showStrips();
    setStrip = false;
  }

}

//------------------------------------------------------------------------------

void showStrips(){
  for(int i = 0; i < NUMSTRIPS; i++){
    strips[i].show();
  }
}

//------------------------------------------------------------------------------

void setColor(String inByte){

  for (int i = 0; i < 5; i++){
    if(i == 0){
      splitArray[i] = inByte.indexOf(',');
      parameterArray[i] = inByte.substring(2,splitArray[i]).toInt();
    }else if(i > 0){
      splitArray[i] = inByte.indexOf(',', splitArray[i-1] + 1);
      parameterArray[i] = inByte.substring(splitArray[i-1] +1,splitArray[i]).toInt();
    }
  }

  int strip      = parameterArray[0];
  rc             = parameterArray[1];
  gc             = parameterArray[2];
  bc             = parameterArray[3];
  // colorReached   = parameterArray[4];

  // Serial.println(strip);
  // Serial.println(rc);
  // Serial.println(gc);
  // Serial.println(bc);
  // Serial.println(colorReached);

  positionIsNotRequested = true;
  if(strip == 0){
    for(int i = 0; i < NUMSTRIPS; i++){
      strips[i].setStripColor(rc,gc,bc);
    }
  }

  if(strip > 0 && strip < 9){
    strips[strip - 1].setStripColor(rc,gc,bc);
  }

  if(strip == 9){
    // Serial.print("Rainbow set to true");
    startRainbow = true;
    loopTime = millis();
  }

  if(strip == 10){
    startRainbow = false;
    for(int i = 0; i < NUMSTRIPS; i++){
      strips[i].setStripColor(rc,gc,bc);
    }
  }

  setStrip = true;
  // positionIsNotRequested = true;
}

//------------------------------------------------------------------------------

void setTargetColor(String inByte){

  for (int i = 0; i < 6; i++){
    if(i == 0){
      splitArray[i] = inByte.indexOf(',');
      parameterArray[i] = inByte.substring(2,splitArray[i]).toInt();
    }else if(i > 0){
      splitArray[i] = inByte.indexOf(',', splitArray[i-1] + 1);
      parameterArray[i] = inByte.substring(splitArray[i-1] +1,splitArray[i]).toInt();
    }
  }


  int strip      = parameterArray[0];
  int segment    = parameterArray[1];
  rt             = parameterArray[2];
  gt             = parameterArray[3];
  bt             = parameterArray[4];
  fade           = parameterArray[5];


  // Serial.println("Splitted Strings: ");
  // Serial.println(strip);
  // Serial.println(segment);
  // Serial.println(rt);
  // Serial.println(gt);
  // Serial.println(bt);
  // Serial.println(fade);

  isSegments = false;

  if(strip == 0){
    for(int i = 0; i < NUMSTRIPS; i++){
      strips[i].targetColorR = rt;
      strips[i].targetColorG = gt;
      strips[i].targetColorB = bt;
    }
  }else if(segment > 0){
    segments[segment - 1].targetColorR = rt;
    segments[segment - 1].targetColorG = gt;
    segments[segment - 1].targetColorB = bt;
    isSegments = true;
    segments[segment - 1].colorReached = false;
  }else{
    strips[strip - 1].targetColorR = rt;
    strips[strip - 1].targetColorG = gt;
    strips[strip - 1].targetColorB = bt;
  }

  colorReached = false;
  // positionIsNotRequested = true;

}


// -----------------------------------------------------------------------------

void establishContact() {
  delay(1000);  // do not print too fast!
  while (Serial.available() <= 0) {
    Serial.print("master");   // send a capital A
    Serial.println();
    ledsReady = true;
    delay(300);
  }
}

//------------------------------------------------------------------------------

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

    if(r == strips[k].targetColorR && g == strips[k].targetColorG && b == strips[k].targetColorB){
      colorReached = true;
      shallFlicker = true;
    }

    if(shallFlickerInFade && checkTimers(1)){
      strips[k].setBrightness(random(50,255));
    }
    strips[k].setPixelColor(i, r, g, b);
    wait(fade,0);
  }
  // wait(random(0,fade),1);
}

//------------------------------------------------------------------------------

void fadeOutSegments(int k){
  for(int s = 0; s < STRIPSEGMENTS; s++){
    for(int i = segments[s].lowerBound; i < segments[s].higherBound; i++){
      uint32_t color = strips[k].getPixelColor(i);

      int
      r = (uint8_t)(color >> 16),
      g = (uint8_t)(color >>  8),
      b = (uint8_t)color;

      r = (fadeSpeed >= abs(segments[s].targetColorR - r))?segments[s].targetColorR:r<segments[s].targetColorR?r+fadeSpeed:r-fadeSpeed;
      g = (fadeSpeed >= abs(segments[s].targetColorG - g))?segments[s].targetColorG:g<segments[s].targetColorG?g+fadeSpeed:g-fadeSpeed;
      b = (fadeSpeed >= abs(segments[s].targetColorB - b))?segments[s].targetColorB:b<segments[s].targetColorB?b+fadeSpeed:b-fadeSpeed;

      if(r == segments[s].targetColorR && g == segments[s].targetColorG && b == segments[s].targetColorB){
        segments[s].colorReached = true;
        shallFlicker = true;
      }

      if(shallFlickerInFade && checkTimers(1)){
        strips[k].setBrightness(random(50,255));
      }
      strips[k].setPixelColor(i, r, g, b);
      wait(fade,0);
    }
  }

  if(checkSegColor()){
    // Serial.println("Truuue");
    colorReached = true;
    isSegments = false;
  }
  // wait(random(0,fade),1);
}

//------------------------------------------------------------------------------

bool checkSegColor()
{
  for(int s = 0; s < STRIPSEGMENTS-1; s++){
    if(segments[s].colorReached != segments[s + 1].colorReached){
      return false;
    }else if(!segments[s].colorReached && !segments[s + 1].colorReached){
      return false;
    }
  }
  return true;
}


//------------------------------------------------------------------------------

void flicker(int k){
  for(int i = 0; i <strips[k].getNumPixels(); i++)
  {

    int flicker = random(20,100);

    int r1 = rt-flicker;
    int g1 = gt-flicker;
    int b1 = bt-flicker;

    if(g1<0) g1=0;
    if(r1<0) r1=0;
    if(b1<0) b1=0;

    strips[k].setPixelColor(i,r1,g1,b1);
  }
}

//------------------------------------------------------------------------------

void wait(long _waitTime, long _numberOfIndex)
{
  waitTime[_numberOfIndex] = _waitTime;
  currentTime[_numberOfIndex] = millis();
}

//------------------------------------------------------------------------------

bool checkTimers(int _index)
{
  if(millis() - currentTime[_index] >= waitTime[_index]){
    currentTime[_index] = millis();
    return true;
  }else
  {
    return false;
  }
}

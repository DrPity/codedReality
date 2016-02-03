#ifndef WRAPPER_H
#define WRAPPER_H
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "MAPPING.h"

class Wrapper_class{

private:
	int _numberOfPixels;
	int _stripPin;
	Adafruit_NeoPixel* _strip;
	uint8_t _flicker[NUMBEROFPIXELS];
	uint8_t _targetColorR[NUMBEROFPIXELS];
	uint8_t _targetColorG[NUMBEROFPIXELS];
	uint8_t _targetColorB[NUMBEROFPIXELS];
	bool _colorReached[NUMBEROFPIXELS];
	bool _ignorePixel[NUMBEROFPIXELS];

public:	
	void setStripColor();

	Wrapper_class(int numberOfPixels, int stripPin): _numberOfPixels(numberOfPixels), _stripPin(stripPin){
		_strip = new Adafruit_NeoPixel(_numberOfPixels, _stripPin, NEO_GRB + NEO_KHZ800);

		for (int i = 0; i < _numberOfPixels; i++) {
			_flicker[i] = 0;
			_ignorePixel[i] = false;
			_targetColorR[i] = 0;
			_targetColorG[i] = 0;
			_targetColorB[i] = 0;
			_colorReached[i] = false;
		}
	}


	void init(){
	_strip->show();
	_strip->begin();
	_strip->setBrightness(255);

	}

	void setStripColor(uint8_t r, uint8_t g, uint8_t b){
		for(int i = 0; i<_numberOfPixels; i++){
			_strip->setPixelColor(i, r, g, b);
		}
		// _strip->show();
	}

	void setBrightness(uint8_t brightness){
		_strip->setBrightness(brightness);
		// _strip->show();
	}

	int getNumPixels(){
		return _strip->numPixels();
	}

	uint32_t getPixelColor(int i){
		return _strip->getPixelColor(i);
	}

	void setPixelColor(int i, uint8_t r, uint8_t g, uint8_t b){
		_strip->setPixelColor(i, r, g, b);
		// _strip->show();
	}

	void setPixelColor(int i, uint32_t color){
		_strip->setPixelColor(i, color);
	}

	void show(){
		_strip->show();
	}

	void setIgnorePixel(int i, bool ignore){
		_ignorePixel[i] = ignore;
	}

	bool getIgnorePixel(int i){
		return _ignorePixel[i];
	}

	void setTargetColorR(int i, uint8_t red){
		_targetColorR[i] = red;
	}

	uint8_t getTargetColorR(int i){
		return _targetColorR[i];
	}

	void setTargetColorG(int i, uint8_t green){
		_targetColorG[i] = green;
	}

	uint8_t getTargetColorG(int i){
		return _targetColorG[i];
	}

	void setTargetColorB(int i, uint8_t blue){
		_targetColorB[i] = blue;
	}

	uint8_t getTargetColorB(int i){
		return _targetColorB[i];
	}

	void setNewFlickerValue(int i){
		_flicker[i] = (uint8_t) random(0,200);
	}

	uint8_t getCurrentFlickerValue(int i) {
		return _flicker[i];
	}

	void setColorReached(int i, bool status){
		_colorReached[i] = status;
	}

	bool getColorReached(int i) {
		return _colorReached[i];
	}

	void rainbowCycle(int wait) {
	  int j = (millis()/wait)%(256*5);
	  for(int i=0; i < _strip->numPixels(); i++) {
	    _strip->setPixelColor(i, Wheel(((i * 256 / _strip->numPixels()) + j) & 255));
	  }
	  _strip->show();
	}

	// Input a value 0 to 255 to get a color value.
	// The colours are a transition r - g - b - back to r.
	uint32_t Wheel(byte WheelPos) {
	  if(WheelPos < 85) {
	   return _strip->Color(WheelPos * 3, 255 - WheelPos * 3, 0);
	  } else if(WheelPos < 170) {
	   WheelPos -= 85;
	   return _strip->Color(255 - WheelPos * 3, 0, WheelPos * 3);
	  } else {
	   WheelPos -= 170;
	   return _strip->Color(0, WheelPos * 3, 255 - WheelPos * 3);
	  }
	}

};
#endif

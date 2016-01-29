#ifndef WRAPPER_H
#define WRAPPER_H
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

class Wrapper_class{

private:
	int _numberOfPixels;
	int _stripPin;
	Adafruit_NeoPixel* _strip;

public:
	uint8_t targetColorR;
	uint8_t targetColorG;
	uint8_t targetColorB;


	void setStripColor();

	Wrapper_class(int numberOfPixels, int stripPin): _numberOfPixels(numberOfPixels), _stripPin(stripPin){
		_strip = new Adafruit_NeoPixel(_numberOfPixels, _stripPin, NEO_GRB + NEO_KHZ800);
		targetColorR, targetColorG, targetColorB = 0;
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
		// _strip->show();
	}

	void show(){
		_strip->show();
	}


	void rainbowCycle(int wait) {
	  int j = (millis()/wait)%(256*5);
	  for(int i=0; i < _strip->numPixels(); i++) {
	    _strip->setPixelColor(i, Wheel(((i * 256 / 2) + j) & 255));
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

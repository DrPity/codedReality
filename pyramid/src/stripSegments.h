class StripSegments
{
   public:
      int lowerBound;
      int upperBound;

      bool colorReached;

      uint8_t targetColorR;
    	uint8_t targetColorG;
    	uint8_t targetColorB;

  StripSegments(int _lowerBound, int _upperBound){
    lowerBound = _lowerBound;
    upperBound = _upperBound;
    colorReached = false;
    targetColorR, targetColorG, targetColorB = 255;
  }

};

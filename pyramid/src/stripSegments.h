class StripSegments
{
   public:
      int lowerBound;
      int higherBound;

      bool colorReached;

      uint8_t targetColorR;
    	uint8_t targetColorG;
    	uint8_t targetColorB;

  StripSegments(int _lowerBound, int _higherBound){
    lowerBound = _lowerBound;
    higherBound = _higherBound;
    colorReached = false;
    targetColorR, targetColorG, targetColorB = 0;
  }

};

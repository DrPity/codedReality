//Mockup-fire effect

int r = 255;
int g = r-40;
int b = 40;

for(int x = 8; x <99; x++)
{
int flicker = random(0,150);
int r1 = r-flicker;
int g1 = g-flicker;
int b1 = b-flicker;
if(g1<0) g1=0;
if(r1<0) r1=0;
if(b1<0) b1=0;
strip.setPixelColor(x,r1,g1, b1);
}
strip.show();
delay(random(50,150));

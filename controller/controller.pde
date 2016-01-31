import controlP5.*;
import processing.serial.*;
import java.util.*;

ControlP5 cp5;
WatchDog wA;
Textarea myTextarea;
Helpers help;


long[] waitTime         = new long [8];
long[] currentTime      = new long [8];

PFont fontSmall;

List<WatchDog> serialDevices = new ArrayList<WatchDog>();
List<Toggle> deviceList = new ArrayList<Toggle>();
List serialList= new ArrayList();
int[] topList = {1,2,3};

float togglePos = 0.03;
float easing    = 0.05;
float[] y       = new float [8];

int fadeSpeed = 10;
int slave = -1;
int idx = 0;
int lf  = 10;

boolean[] headSetPopulated  = {false,false,false,false,false,false};
boolean toplistNewPopulated = false;
boolean isHashtrue          = false;
boolean newEvent            = false;

Println console;

static int numberOfSegments = 3;
color [] topListColor = new color [numberOfSegments];

//-------------------------------------------------------------------------------

void setup()
{
  size(700, 400, FX2D);
  surface.setResizable(false);
  background(128);
  smooth();
  frameRate(60);
  fontSmall = createFont("OpenSans-Semibold.ttf",10);

  checkSerialPorts(true);

  help = new Helpers();
  help.movingAverage(1);

  cp5 = new ControlP5(this);
  cp5.setFont(fontSmall);
  cp5.addFrameRate().setInterval(5).setColor(0).setPosition(round(width*0.01),round(height * 0.95)).setFont(fontSmall);

  // create the console txt field
  myTextarea = cp5.addTextarea("txt")
  .setPosition(round(width*0.45),round(height * 0.015))
  .setSize(380, round(height * 0.97))
  .setFont(createFont("", 10))
  .setLineHeight(14)
  .setColor(color(200))
  .setColorBackground(color(0, 100))
  .setColorForeground(color(255, 100));
  ;

  cp5.addSlider("fading")
   .setPosition(round(width*0.01),round(height * 0.89))
   .setSize(300,12)
   .setRange(0,100) // values can range from big to small as well
   .setValue(10)
   .setNumberOfTickMarks(15)
   .setSliderMode(Slider.FLEXIBLE)
   ;

  cp5.getController("fading").getCaptionLabel().align(ControlP5.RIGHT, ControlP5.BOTTOM_OUTSIDE).setPaddingY(11);

  console = cp5.addConsole(myTextarea);//
  console.setMax(30);


  initList();

  //Bug in controlP5 when using alt-tab
  unregisterMethod("keyEvent", cp5);
  unregisterMethod("keyEvent", cp5.getWindow());
  wait(1,0);
  wait(5000,1);
  wait(5000,2);
}

//-------------------------------------------------------------------------------

void draw()
{

  background(128);
  text("Select a port from the list and connect to the device", round(width*0.01),round(height * 0.11));
  if(checkTimers(0) && slave >= 0 && !serialDevices.get(slave).paused && newEvent){
    try
    {
      help.shiftArray(topList);
      topList[0] = ceil(help.getAverage());
      //maybe taking the segements lenght would be better --> best if segment == topList.length
      for(int i = 0; i < topList.length; i++){
        switch (topList[i]) {
          case 1:  topListColor[i] = color(255,160,0);
                   break;
          case 2:  topListColor[i] = color(0,255,0);
                   break;
          case 3:  topListColor[i] = color(255,0,0);
                   break;
          case 4:  topListColor[i] = color(255,160,0);
                  break;
          case 5:  topListColor[i] = color(0,255,0);
                  break;
          case 6:  topListColor[i] = color(0,255,0);
                  break;
          default: topListColor[i] = color(255,160,0);
        }
        println("current Toplist values: " + topList[i] + "color: " + topListColor[i]);
      }
      toplistNewPopulated = true;
    }
    catch ( Exception e )
    {
     println(e);
    }
    wait(2000,0);
    newEvent = false;
  }


  if(checkTimers(1))
  {
    checkSerialPorts(false);
    wait(5000,1);
  }

  if(checkTimers(2) && toplistNewPopulated){
    // if((idx+1)%(numberOfSegments+1) != 0){
      // println("Tt1,"+ (idx+1) + "," + topListColor[idx] +"," + fadeSpeed);
      serialDevices.get(slave).port.write("Tt1,1," + topListColor[0] + "," + topListColor[1] + "," + topListColor[2] + "," + fadeSpeed);
      // idx++;
      // wait(3000,2);
    // }else if (idx%numberOfSegments == 0){
      println("In idx = 0");
      toplistNewPopulated = false;
      // idx = 0;
      wait(1000,2);
    // }
  }
}


//-------------------------------------------------------------------------------

void wait(int _waitTime, int _numberOfIndex)
{
  waitTime[_numberOfIndex] = _waitTime;
  currentTime[_numberOfIndex] = millis();
}

//-------------------------------------------------------------------------------

boolean checkTimers(int _index)
{
  if(millis() - currentTime[_index] >= waitTime[_index]){
    currentTime[_index] = millis();
    return true;
  }else
  {
    return false;
  }
}


//-------------------------------------------------------------------------------

void serialEvent(Serial thisPort)
{
  try
  {
    for(int i=0; i < deviceList.size(); i++){
      if (thisPort == serialDevices.get(i).port && serialDevices.get(i).deviceInstanciated)
      {
        while (serialDevices.get(i).port.available() > 0)
        {
          // byte[] = wA.port.readBytesUntil(end);
          String inByte = serialDevices.get(i).port.readStringUntil(lf).trim();

          if (inByte != null)
          {
            // println("inByte: " + inByte);
            // String [] s = split(inByte, ',');
            if(inByte.equals("master")){
              slave = i;
              serialDevices.get(slave).port.write("Cc1,0,0,255");
            }

            if(i != slave && slave != -1){
              int state = Integer.parseInt(inByte);
              help.add(state);
              newEvent = true;
            }
          }
        }
      }
    }
  }
  catch (Exception e)
  {
    // println("Exepction: " + e);
  }
}

//-------------------------------------------------------------------------------

void manageSerial(String inChar)
{

}

//-------------------------------------------------------------------------------

void checkSerialPorts(boolean init)
{

  if(!init){
    serialList.clear();
  }

  for (int i = 0; i < Serial.list().length; i++)
  {
    // println("[" + i + "] " + Serial.list()[i]);
    serialList.add(Serial.list()[i]);
  }

  if(!init){
    cp5.get(ScrollableList.class, "deviceList").setItems(serialList);
    for(int k = 0; k < deviceList.size(); k++){
      // println("toogle id: " + deviceList.get(k).getName());
      CColor c = new CColor();
      c.setBackground(color(255,0,0));
      cp5.get(ScrollableList.class, "deviceList").getItem(Integer.parseInt(deviceList.get(k).getName())).put("color", c);
    }
  }
}

//-------------------------------------------------------------------------------

void initList()
{
  CallbackListener toFront = new CallbackListener() {
    public void controlEvent(CallbackEvent theEvent) {
      theEvent.getController().bringToFront();
      ((ScrollableList)theEvent.getController()).open();
    }
  };

  CallbackListener close = new CallbackListener() {
    public void controlEvent(CallbackEvent theEvent) {
      ((ScrollableList)theEvent.getController()).close();
    }
  };

  cp5.addScrollableList("deviceList")
  .setSize(400, 300)
  .addItems(serialList)
  .setPosition(round(width*0.01),round(height * 0.015))
  .onEnter(toFront)
  .onLeave(close)
  .setWidth(300)
  .setItemHeight(20)
  .setBarHeight(20)
  .setBackgroundColor(color(128))
  .close()
  ;
}


//-------------------------------------------------------------------------------

void deviceList(int n) {
  /* request the selected item based on index n */
  // println(cp5.get(ScrollableList.class, "deviceList").getItem(n).get("value"));
  /* here an item is stored as a Map  with the following key-value pairs:
  * name, the given name of the item
  * text, the given text of the item by default the same as name
  * value, the given value of the item, can be changed by using .getItem(n).put("value", "abc"); a value here is of type Object therefore can be anything
  * color, the given color of the item, how to change, see below
  * view, a customizable view, is of type CDrawable
  */
  String value = Integer.toString(n);
  CColor c = new CColor();
  c.setBackground(color(255,0,0));
  cp5.get(ScrollableList.class, "deviceList").getItem(n).put("color", c);
  togglePos += 0.1;
  addToggleButton(n, togglePos);
}

//-------------------------------------------------------------------------------

void addToggleButton(int id, float yPos)
{
  Toggle tg = cp5.addToggle(Integer.toString(id))
  .setPosition(round(width*0.01),round(height * yPos))
  .setSize(300,20)
  .setValue(true)
  .setColorLabel(0)
  .setColorActive(color(0,255,0))
  .setColorForeground(color(0,127,0))
  .setValue(false)
  ;

  deviceList.add(tg);
};

//------------------------------------------------------------------------------

void controlEvent(ControlEvent theEvent)
{
  for (int i=0;i<deviceList.size();i++) {
    if (theEvent.isFrom(deviceList.get(i))) {
      if(!headSetPopulated[i]){
        int id = Integer.parseInt(deviceList.get(i).getName());
        registerSerialDevice(deviceList.get(i).getName(), (String) serialList.get(id), i);
      }else if(headSetPopulated[i]){
        if(!serialDevices.get(i).paused){
          println("Stoping device: " + serialDevices.get(i).id);
          serialDevices.get(i).port.stop();
          serialDevices.get(i).paused = true;
        }else if(serialDevices.get(i).paused){
          println("Connecting device: " + serialDevices.get(i).id);
          serialDevices.get(i).initSerialPort();
          serialDevices.get(i).paused = false;
        }
      }
    }
  }
}

//------------------------------------------------------------------------------

void registerSerialDevice(String id, String port, int index)
{
  println("ID: " + id + " port: " + port + "index: " + index);
  headSetPopulated[index] = true;
  WatchDog sd = new WatchDog(1,id, port, true, headSetPopulated[index], 57600, true, false, this);
  sd.start();
  serialDevices.add(sd);
  // println(serialDevices.get(0).id);
}

//------------------------------------------------------------------------------

void fading(float speed){
  fadeSpeed = floor(speed);
  println("Fade Speed: " + fadeSpeed);
}

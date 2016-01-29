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

float togglePos = 0.03;
float easing    = 0.05;
float[] y       = new float [8];

int slave = -1;
int lf  = 10;

boolean[] headSetPopulated  = {false,false,false,false,false,false};
boolean isHashtrue          = false;
boolean newEvent            = false;

Println console;

color [] c = new color [10];

//-------------------------------------------------------------------------------

void setup()
{
  size(700, 400, FX2D);
  background(128);
  smooth();
  frameRate(120);
  fontSmall = createFont("OpenSans-Semibold.ttf",10);

  checkSerialPorts();

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

  console = cp5.addConsole(myTextarea);//
  console.setMax(30);


  initList();

  //Bug in controlP5 when using alt-tab
  unregisterMethod("keyEvent", cp5);
  unregisterMethod("keyEvent", cp5.getWindow());
  wait(1,0);
}

//-------------------------------------------------------------------------------

void draw()
{

  background(128);
  text("Select a port from the list and connect to the device", round(width*0.01),round(height * 0.11));
  if(checkTimers(0) && slave >= 0 && !serialDevices.get(slave).paused && newEvent){
    String c = "";
    switch (help.getAverage()) {
      case 1:  c = "255,0,0";
               break;
      case 2:  c = "0,255,0";
               break;
      case 3:  c = "255,160,0";
               break;
      case 4:  c = "255,0,0";
              break;
      case 5:  c = "0,255,0";
              break;
      case 6:  c = "255,160,0";
              break;
      default: c = "255,160,0";
    }

    serialDevices.get(slave).port.write("Tt1,"+c+",10");
    wait(500,0);
    newEvent = false;
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
            println("inByte: " + inByte, "i: " + i);
            // String [] s = split(inByte, ',');
            if(inByte.equals("master")){
              slave = i;
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

void checkSerialPorts()
{
  for (int i = 0; i < Serial.list().length; i++)
  {
    println("[" + i + "] " + Serial.list()[i]);
    serialList.add(Serial.list()[i]);
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

  cp5.addScrollableList("dropdown")
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

void dropdown(int n) {
  /* request the selected item based on index n */
  // println(cp5.get(ScrollableList.class, "dropdown").getItem(n).get("value"));
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
  cp5.get(ScrollableList.class, "dropdown").getItem(n).put("color", c);
  togglePos += 0.1;
  addToggleButton(n, togglePos);
}

//-------------------------------------------------------------------------------

void addToggleButton(int id, float yPos)
{
  Toggle tg = cp5.addToggle(Integer.toString(id))
  .setPosition(round(width*0.01),round(height * yPos))
  .setSize(50,20)
  .setValue(true)
  .setColorLabel(0)
  .setColorActive(color(0,255,0))
  .setColorForeground(color(0,127,0))
  .setValue(false)
  ;

  deviceList.add(tg);
};

//-------------------------------------------------------------------------------

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

//-------------------------------------------------------------------------------

void registerSerialDevice(String id, String port, int index)
{
  println("ID: " + id + " port: " + port + "index: " + index);
  headSetPopulated[index] = true;
  WatchDog sd = new WatchDog(1,id, port, true, headSetPopulated[index], 57600, true, false, this);
  sd.start();
  serialDevices.add(sd);
  // println(serialDevices.get(0).id);
}

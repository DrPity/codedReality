import processing.core.*; 
import processing.data.*; 
import processing.event.*; 
import processing.opengl.*; 

import controlP5.*; 
import processing.serial.*; 
import java.util.*; 

import java.util.HashMap; 
import java.util.ArrayList; 
import java.io.File; 
import java.io.BufferedReader; 
import java.io.PrintWriter; 
import java.io.InputStream; 
import java.io.OutputStream; 
import java.io.IOException; 

public class controller extends PApplet {





ControlP5 cp5;
WatchDog wA;
Textarea myTextarea;

long[] waitTime         = new long [8];
long[] currentTime      = new long [8];

PFont fontSmall;

List serialList= new ArrayList();
List<Toggle> deviceList = new ArrayList<Toggle>();
List<WatchDog> serialDevices = new ArrayList<WatchDog>();

float[] y               = new float [8];
float   easing          = 0.05f;
float togglePos         = 0.03f;

int lf                 = 10;
int slave              = -1;

boolean isHashtrue          = false;
boolean[] headSetPopulated  = {false,false,false,false,false,false};

Println console;

int [] c = new int [10];

//-------------------------------------------------------------------------------

public void setup()
{
  
  background(128);
  
  frameRate(120);
  fontSmall = createFont("OpenSans-Semibold.ttf",10);

  checkSerialPorts();

  cp5 = new ControlP5(this);
  cp5.setFont(fontSmall);
  cp5.addFrameRate().setInterval(5).setColor(0).setPosition(round(width*0.01f),round(height * 0.95f)).setFont(fontSmall);


  // create the console txt field
  myTextarea = cp5.addTextarea("txt")
  .setPosition(round(width*0.45f),round(height * 0.015f))
  .setSize(380, round(height * 0.97f))
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

public void draw()
{

  background(128);
  text("Select a port from the list and connect to the device", round(width*0.01f),round(height * 0.11f));
  if(checkTimers(0) && slave >= 0 && !serialDevices.get(slave).paused){
    serialDevices.get(slave).port.write("Tt1,"+random(50,250) +",50"+","+random(50,250)+",10");
    wait(3000,0);
  }
}


//-------------------------------------------------------------------------------

public void wait(int _waitTime, int _numberOfIndex)
{
  waitTime[_numberOfIndex] = _waitTime;
  currentTime[_numberOfIndex] = millis();
}

//-------------------------------------------------------------------------------

public boolean checkTimers(int _index)
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

public void serialEvent(Serial thisPort)
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
            println("value: " + inByte);
            // String [] s = split(inByte, ',');
            if(inByte.equals("master")){
              // serialDevices.get(i).port.write("Cc1,0,100,2");
              // serialDevices.get(i).port.write("Tt1,230,90,0,150");
              slave = i;
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

public void manageSerial(String inChar)
{

}

//-------------------------------------------------------------------------------

public void checkSerialPorts()
{
  for (int i = 0; i < Serial.list().length; i++)
  {
    println("[" + i + "] " + Serial.list()[i]);
    serialList.add(Serial.list()[i]);
  }
}

//-------------------------------------------------------------------------------

public void initList()
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
  .setPosition(round(width*0.01f),round(height * 0.015f))
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

public void dropdown(int n) {
  /* request the selected item based on index n */
  println(cp5.get(ScrollableList.class, "dropdown").getItem(n).get("value"));
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
  togglePos += 0.1f;
  addToggleButton(n, togglePos);
}

//-------------------------------------------------------------------------------

public void addToggleButton(int id, float yPos)
{
  Toggle tg = cp5.addToggle(Integer.toString(id))
  .setPosition(round(width*0.01f),round(height * yPos))
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

public void controlEvent(ControlEvent theEvent)
{
  for (int i=0;i<deviceList.size();i++) {
    if (theEvent.isFrom(deviceList.get(i))) {
      if(!headSetPopulated[i]){
        int id = Integer.parseInt(deviceList.get(i).getName());
        print("Incoming Event from Toggle Button Nr: "+deviceList.get(i).getName()+"\t\n");
        println(serialList.get(id));
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

public void registerSerialDevice(String id, String port, int index)
{
  println("ID: " + id + " port: " + port + "index: " + index);
  headSetPopulated[index] = true;
  WatchDog sd = new WatchDog(1,id, port, true, headSetPopulated[index], 57600, true, false, this);
  sd.start();
  serialDevices.add(sd);
  println(serialDevices.get(0).id);
}
class WatchDog extends Thread
{

  public Serial   port;
  public boolean  deviceInstanciated;
  public boolean  paused;
  public String   id;



  private PApplet   p;
  private boolean   running;
  private boolean   deviceLost;
  private boolean   buffer;
  private boolean   isPort;
  private boolean   isFirstContact;
  private boolean   isArduino;
  private boolean   testingLeonardo;

  private long      heartBeat;

  private int       bautRate;
  private int       conValue;
  private int       wait;

  private String    devicePort;

  // ------------------------------------------------------------------------------------

  WatchDog ( int _wait, String _id, String _devicePort, boolean _buffer, boolean _isPort, int _bautRate, boolean _isArduino, boolean _testingLeonardo, PApplet _p )
  {
    wait                = _wait;
    p                   = _p;
    running             = false;
    deviceInstanciated  = false;
    deviceLost          = false;
    devicePort          = _devicePort;
    buffer              = _buffer;
    isPort              = _isPort;
    bautRate            = _bautRate;
    isArduino           = _isArduino;
    testingLeonardo     = _testingLeonardo;
    id                  = _id;
    conValue            = 255;
    heartBeat           = millis();
  }

  // ------------------------------------------------------------------------------------

  // Overriding "start()"
  public void start ()
  {
    running = true;
    println("Starting thread (will execute every " + wait + " milliseconds.)");
    super.start();
  }

  // ------------------------------------------------------------------------------------

  // We must implement run, this gets triggered by start()
  public void run ()
  {
    // sleep(2000);
    println(id + " " + conValue);
    deviceInit();
    sleep(300);
    while (running)
    {
      check();
      checkHeartBeat();
      sleep(wait);
    }
    System.out.println(id + " thread is done!");  // The thread is done when we get to the end of run()
    quit();
  }

  // ------------------------------------------------------------------------------------

  public void check()
  {

    if ( !deviceInstanciated )
    {
      sleep(3000);
      deviceInit();
    }else if( deviceInstanciated && deviceLost )
    {
      sleep(3000);
      port.stop();
      // checkIfLeonardo() do here;
      deviceInit();
    }
  }

  // ------------------------------------------------------------------------------------

  // Our method that quits the thread
  public void quit()
  {
    System.out.println("Quitting.");
    running = false;
    port.stop();
    // In case the thread is waiting. . .
    interrupt();
  }

  // ------------------------------------------------------------------------------------

  public void sleep( int sleepTime )
  {
    try {
      sleep( (long)(sleepTime) );
    } catch ( Exception e )
    {
    }

  }

  // ----------------------------------------------------------------------------------

  public void checkHeartBeat()
  {
    if ( isArduino )
    {
      if ( deviceInstanciated )
      {
        if ( millis() -  heartBeat >= 4000 && deviceInstanciated )
        {
          boolean checkSerial = checkSerial();
          if(!checkSerial){
            isFirstContact = false;
            deviceLost = true;
            // testingLeonardo = true;
            println(id + " heartBeat lost");
            conValue = 100;
            sleep(1000);
          }
        }
      }
    }
  }

// ------------------------------------------------------------------------------------


  public boolean checkSerial(){
    for (int i = 0; i < Serial.list().length; i++)
    {
      if(Serial.list()[i].equals(devicePort)){
        heartBeat = millis();
        return true;
      }
    }
    return false;
  }

  // ------------------------------------------------------------------------------------

  public void deviceInit()
  {
    if(isPort)
    {
      println("In is Port: " + id );
      try
      {
        initSerialPort();
        deviceInstanciated = true;
        deviceLost = false;
        // testingLeonardo = false;
        if(isArduino)
        {
          isFirstContact = false;
        }
      }
      catch ( Exception e )
      {
        deviceInstanciated = false;
        deviceLost = true;
        println(id + " port received an exepction: " + e);
      }
    }
  }

  // ------------------------------------------------------------------------------------

  public void initSerialPort(){
    port = new Serial(p, devicePort, bautRate);
    if(buffer)
    {
      port.bufferUntil(lf);
    }
    port.clear();
  }
}
public class Helpers {

  private int size;
  private int total;
  private int index;
  private int samples[];

  public Helpers() {
    this.total = 0;
    this.index = 0;
  }

  //Moving average stuff

  public void movingAverage(int size){
    this.size = size;
    samples = new int[size];
    for (int i = 0; i < size; i++) samples[i] = 0;
  }

  public void add(int x) {
      total -= samples[index];
      samples[index] = x;
      total += x;
      if (++index == size) index = 0; // cheaper than modulus
  }

  public int getAverage() {
      return total / size;
  }

}
  public void settings() {  size(700, 400, FX2D);  smooth(); }
  static public void main(String[] passedArgs) {
    String[] appletArgs = new String[] { "--present", "--window-color=#666666", "--stop-color=#CCCCCC", "controller" };
    if (passedArgs != null) {
      PApplet.main(concat(appletArgs, passedArgs));
    } else {
      PApplet.main(appletArgs);
    }
  }
}

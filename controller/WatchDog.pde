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
  void start ()
  {
    running = true;
    println("Starting thread (will execute every " + wait + " milliseconds.)");
    super.start();
  }

  // ------------------------------------------------------------------------------------

  // We must implement run, this gets triggered by start()
  void run ()
  {
    // sleep(2000);
    // println(id + " " + conValue);
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

  void check()
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
  void quit()
  {
    System.out.println("Quitting.");
    running = false;
    port.stop();
    // In case the thread is waiting. . .
    interrupt();
  }

  // ------------------------------------------------------------------------------------

  void sleep( int sleepTime )
  {
    try {
      sleep( (long)(sleepTime) );
    } catch ( Exception e )
    {
    }

  }

  // ----------------------------------------------------------------------------------

  void checkHeartBeat()
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


  boolean checkSerial(){
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

  void deviceInit()
  {
    if(isPort)
    {
      // println("In is Port: " + id );
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

  void initSerialPort(){
    port = new Serial(p, devicePort, bautRate);
    if(buffer)
    {
      port.bufferUntil(lf);
    }
    port.clear();
  }
}

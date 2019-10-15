
// USE leds library
#include <Adafruit_NeoPixel.h>


#define LEDPIN 9
#define NBLEDS 8
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NBLEDS, LEDPIN, NEO_GRB + NEO_KHZ800);

char *msgTab[] =
{
  "stop",
  "reset",
  "run",
  "star",
  "fill",
  "slide",
  "empty",
  "help",
  "TAKE THE BOBST CHALLENGE TO CONTROL ME !"
};

int MAX_CMD_LENGTH = 42;
char cmd[42];
int cmdIndex;
char incomingByte;
bool stringComplete = false;
bool isRunning = false;
int functionRun = 0 ;
int fill = 0 ;
unsigned long previousTime;
unsigned long actualTime;


// Use the MD_MAX72XX library to Display scrolling text

#include <MD_MAX72xx.h>
#include <SPI.h>
#define DEMO_DELAY  10  // time to show each demo element in seconds


#define DEBUG 0   // Enable or disable (default) debugging output

#if DEBUG
#define PRINT(s, v)   { Serial.print(F(s)); Serial.print(v); }        // Print a string followed by a value (decimal)
#define PRINTX(s, v)  { Serial.print(F(s)); Serial.print(v, HEX); }   // Print a string followed by a value (hex)
#define PRINTB(s, v)  { Serial.print(F(s)); Serial.print(v, BIN); }   // Print a string followed by a value (binary)
#define PRINTC(s, v)  { Serial.print(F(s)); Serial.print((char)v); }  // Print a string followed by a value (char)
#define PRINTS(s)     { Serial.print(F(s)); }                         // Print a string
#else
#define PRINT(s, v)   // Print a string followed by a value (decimal)
#define PRINTX(s, v)  // Print a string followed by a value (hex)
#define PRINTB(s, v)  // Print a string followed by a value (binary)
#define PRINTC(s, v)  // Print a string followed by a value (char)
#define PRINTS(s)     // Print a string
#endif

// --------------------
// MD_MAX72xx hardware definitions and object
// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
//
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CLK_PIN   52  // or SCK
#define DATA_PIN  51  // or MOSI
#define CS_PIN    53  // or SS

MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);                      // SPI hardware interface
//MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES); // Arbitrary pins


// --------------------
// Constant parameters
//
// Various delays in milliseconds
#define UNIT_DELAY      25
#define SCROLL_DELAY    (1 * UNIT_DELAY)

#define CHAR_SPACING     1  // pixels between characters
#define BUF_SIZE        75  // character buffer size

// ========== General Variables ===========
//
uint32_t prevTimeAnim = 0;    // Used for remembering the millis() value in animations

uint32_t prevTimeDemo = 0;      //  Used for remembering the millis() time in demo loop
uint8_t  timeDemo = DEMO_DELAY; // number of seconds left in this demo loop
bool bInit = true ;

// ========== Text routines ===========
//
// Text Message Table
// To change messages simply reorder, add to, or delete from, this table

void setup() 
{
  Serial.begin(9600);
  strip.begin();
      for (int i =0 ; i < NBLEDS ; i = i+2) 
      {
        //if ( i%2 == 0)
          strip.setPixelColor(i,255,0,0);
        }    
        
  strip.show(); // Initialize all pixels to 'off'
  strip.setBrightness(255);

  // initialize scroll screen
  mx.begin();
  prevTimeAnim = millis();
  prevTimeDemo = millis();
  strcpy (cmd , msgTab[8]);
}

void Stop()
{
    for (int i =0 ; i < NBLEDS ; i++)
      strip.setPixelColor(i, 255,0,0);
    strip.show(); 
}

void Reset()
{
    for (int i =0 ; i < NBLEDS ; i++)
      strip.setPixelColor(i, 0,0,255);
    strip.show();
    delay(1000);
    for (int i =0 ; i < NBLEDS ; i++)
      strip.setPixelColor(i, 0,0,0);
    strip.show();
}

void Run()
{
    for (int i =0 ; i < NBLEDS ; i++)
      strip.setPixelColor(i, 0,255,0);
    strip.show();
}

void Fill()
{
    actualTime = millis();
    if (isRunning &&(functionRun == 2) && ((actualTime - previousTime) > 250))
    { 
     for (int i =0 ; i < NBLEDS ; i++)
      strip.setPixelColor(i, 0,0,0);
     for (int i =0 ; i <= fill ; i++)
      strip.setPixelColor(i, 255,0,0);
    strip.show();
    fill = (fill + 1 ) % NBLEDS;
    previousTime = actualTime;   
    } 
}

void Slide()
{
    actualTime = millis();
    if (isRunning &&(functionRun == 3) && ((actualTime - previousTime) > 100))
    { 
              for (int i =0 ; i < NBLEDS ; i++)
    strip.setPixelColor(i, 0,0,0);
        strip.setPixelColor(fill -2 , 0,50,0);
    strip.setPixelColor(fill, 0,255,0);
        strip.setPixelColor(fill + 2, 0,50,0);
    strip.show();
    fill = (fill + 2 ) % (NBLEDS+6) ;
    previousTime = actualTime;   
    }
  
}

void Empty()
{
    actualTime = millis();
    if (isRunning &&(functionRun == 4) && ((actualTime - previousTime) > 250))
    { 
     for (int i =0 ; i < NBLEDS ; i++)
      strip.setPixelColor(i, 255,0,0);
     for (int i =0 ; i <= fill ; i++)
      strip.setPixelColor(i, 0,0,0);
    strip.show();
    fill = (fill + 1 ) % NBLEDS;
    previousTime = actualTime;   
    }  
}
  
void Star()
{
    actualTime = millis();
    if (isRunning && (functionRun == 1) && ((actualTime - previousTime) > 150))
    {
        for (int i =0 ; i < NBLEDS ; i++)
    strip.setPixelColor(i, random(0,15)*16, random(0,15)*16, random(0,15)*16);
    strip.show();
    previousTime = actualTime;   
    }
  }
 
void serialEvent() 
{
    if (incomingByte=Serial.available()>0) 
    {
      
      char byteIn = Serial.read();
      cmd[cmdIndex] = byteIn;
      
      if(byteIn=='\n'){
        //command finished
        isRunning = false;
        cmd[cmdIndex] = '\0';
        cmdIndex = 0;
        stringComplete = true; 
      }
      else
      {
        if(cmdIndex++ >= MAX_CMD_LENGTH)
        {
          cmdIndex = 0;
        }
      }
    }
}

bool scrollText(bool bInit, char *pmsg)
// Callback function for data that is required for scrolling into the display
{
  static char   curMessage[BUF_SIZE];
  static char   *p = curMessage;
  static uint8_t  state = 0;
  static uint8_t  curLen, showLen;
  static uint8_t  cBuf[8];
  uint8_t         colData;

  // are we initializing?
  if (bInit)
  {
    PRINTS("\n--- Initializing ScrollText");
    resetMatrix();
    strcpy(curMessage, pmsg);
    state = 0;
    p = curMessage;
    bInit = false;
  }

  // Is it time to scroll the text?
  if (millis()-prevTimeAnim < SCROLL_DELAY)
    return(bInit);

  // scroll the display
  mx.transform(MD_MAX72XX::TSL);  // scroll along
  prevTimeAnim = millis();        // starting point for next time

  // now run the finite state machine to control what we do
  PRINT("\nScroll FSM S:", state);
  switch (state)
  {
    case 0: // Load the next character from the font table
      PRINTC("\nLoading ", *p);
      showLen = mx.getChar(*p++, sizeof(cBuf)/sizeof(cBuf[0]), cBuf);
      curLen = 0;
      state = 1;

    // !! deliberately fall through to next state to start displaying

    case 1: // display the next part of the character
      colData = cBuf[curLen++];
      mx.setColumn(0, colData);
      if (curLen == showLen)
      {
        showLen = ((*p != '\0') ? CHAR_SPACING : mx.getColumnCount()-1);
        curLen = 0;
        state = 2;
      }
      break;

    case 2: // display inter-character spacing (blank column) or scroll off the display
      mx.setColumn(0, 0);
      if (++curLen == showLen)
      {
        state = 0;
        bInit = (*p == '\0');
      }
      break;

    default:
      state = 0;
  }
  return(bInit);
}

// ========== Control routines ===========
//
void resetMatrix(void)
{
  mx.control(MD_MAX72XX::INTENSITY, MAX_INTENSITY);
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
  mx.clear();
  prevTimeAnim = 0;
}

//void runMatrixAnimation(void)
//{
//  static  uint8_t state = 0;
//  static  uint8_t mesg = 0;
//  static  boolean bRestart = true;
//  static boolean  bInMessages = false;
//  boolean changeState = false;
//
//  // check if one second has passed and then count down the demo timer. Once this
//  // gets to zero, change the state.
//  if (millis()-prevTimeDemo >= 1000)
//  {
//    prevTimeDemo = millis();
//    if (--timeDemo == 0)
//    {
//      timeDemo = DEMO_DELAY;
//      changeState = true;
//    }
//  }
//
//  if (changeState)
//  {
//    if (bInMessages) // the message display state
//    {
//      mesg++;
//      if (mesg >= sizeof(msgTab)/sizeof(msgTab[0]))
//      {
//        mesg = 0;
//      bInMessages = false;
//        state++;
//      }
//    }
//    else
//      state++;
//
//    bRestart = true;
//  };
//
//  // now do whatever we do in the current state
//  switch(state)
//  {
//    case  0: bInMessages = true; bRestart = scrollText(bRestart, msgTab[mesg]); break;
//
//    default: state = 0;
//  }
//}

void loop() 
{
  if ( stringComplete )
  {
    Serial.println(cmd);
  
    if(strcmp(cmd, msgTab[0])  == 0)
    {
      Serial.println("Command received: stop");
      isRunning = false;
      bInit = true ;
      Stop();
    }
    else if (strcmp(cmd, msgTab[1])  == 0) 
    {
      Serial.println("Command received: reset");
      isRunning = false;
      bInit = true ;
      Reset();
    }
    else if (strcmp(cmd, msgTab[2])  == 0) 
    {
      Serial.println("Command received: run");
      isRunning = false;
      bInit = true ;
      Run();
    }
    else if (strcmp(cmd, msgTab[3])  == 0) 
    {
      Serial.println("Command received: star");
      functionRun = 1 ;
      isRunning = true;
      bInit = true ;
    }
    else if (strcmp(cmd, msgTab[4])  == 0) 
    {
      Serial.println("Command received: fill");
      functionRun = 2 ;
      isRunning = true;
      bInit = true ;
    }
    else if (strcmp(cmd, msgTab[5])  == 0) 
    {
      Serial.println("Command received: strip");
      functionRun = 3 ;
      isRunning = true;
      bInit = true ;
    }
    else if (strcmp(cmd, msgTab[6])  == 0)
    {
      Serial.println("Command received: empty");
      functionRun = 4 ;
      isRunning = true;
      bInit = true ;
    }
    else if (strcmp(cmd, msgTab[7])  == 0) 
    {
      Serial.println("Command received: help");
      for (int i = 0 ; i < sizeof(msgTab)/sizeof(msgTab[0]); i++ )
        Serial.println(msgTab[i]);
       isRunning = false;
       bInit = true ;
    }
    else
    {
      Serial.println("Command received: unknown!");
    }
  
  stringComplete = false;
  }
  Star();
  Fill();
  Slide();
  Empty();
  bInit = scrollText(bInit, cmd);
}

//
//#include "led_display.h";
//
//int MAX_CMD_LENGTH = 10;
//char cmd[10];
//int cmdIndex;
//char incomingByte;
//bool stringComplete = false;
//bool isRunning = false;
//bool res = false;
//unsigned long previousTime;
//unsigned long actualTime;
//
//class led_display;
//
//led_display ledDisplay;
//
//void setup() {
//  Serial.begin(9600);
//  res = ledDisplay.Init();
//}
//
// 
//void loop() {
//
//if ( stringComplete )
//{
//Serial.println(cmd);
//ledDisplay.ScrollText(cmd);
////        if(strcmp(cmd, "stop")  == 0){
////          Serial.println("Command received: stop");
////                        isRunning = false;
////            Stop();
////
////        }else if (strcmp(cmd, "reset")  == 0) {
////          Serial.println("Command received: reset");
////              isRunning = false;
////  Reset();
////
////        }
////        else if (strcmp(cmd, "run")  == 0) {
////          Serial.println("Command received: run");
////              isRunning = false;
////  Run();
////
////        }
////        else if (strcmp(cmd, "star")  == 0) {
////          Serial.println("Command received: star");
////            isRunning = true;
////
////        }else{
////          Serial.println("Command received: unknown!");
////        }
//
//stringComplete = false;
//}
//
//}
//
//
//
//void serialEvent() 
//{
//  if (incomingByte=Serial.available()>0) 
//  {
//    char byteIn = Serial.read();
//    cmd[cmdIndex] = byteIn; 
//    if(byteIn=='\n')
//    {
//      //command finished
//      isRunning = false;
//      cmd[cmdIndex] = '\0';
//      cmdIndex = 0;
//      stringComplete = true;
//        
//
//        
//     }
//      else
//     {
//      if(cmdIndex++ >= MAX_CMD_LENGTH)
//      {
//        cmdIndex = 0;
//      }
//     }
//
//   }
//}

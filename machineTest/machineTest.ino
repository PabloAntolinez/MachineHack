
// USE leds library
#include <Adafruit_NeoPixel.h>


#define LEDPIN 2
#define NBLEDS 52
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NBLEDS, LEDPIN, NEO_GRBW + NEO_KHZ800);

uint32_t red = strip.Color(255, 0, 0, 0);
uint32_t green = strip.Color(0, 255, 0, 0);
uint32_t blue = strip.Color(0, 0, 255, 0);
uint32_t white = strip.Color(0, 0, 0, 255);
uint32_t black = strip.Color(0, 0, 0, 0);

// declare led index tables for animations
int feederPos[] =
{
  7,6,5,4,3,2,1,0
  };

int feederStatusPos[]=
{
  8,9,10
  };
int chainPathPos[] =
{
  11,12,13,14,  15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,40,41,42,43
  };
int cutStatusPos[] =
{
  31,32,33
  };
int ejectionStatusPos[] =
{
  34,35,36
  };
int receptionStatusPos[] =
{
  37,38,39
  };
int receptionPos[] =
{
  44,45,46,47,48,49,50,51
  };

#define FEEDER_NBSHEET_MAX 150
#define RECEPTION_NBSHEET_MAX 150
   
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
bool isResetOn = false;
int functionRun = 0 ;
int chainStep = 0 ;
int fillStep = 0;
int emptyStep = 0;
unsigned long previousTimeChain;
unsigned long previousTimeFill;
unsigned long previousTimeEmpty;
unsigned long previousTimeStar;
unsigned long previousTimeReset;
unsigned long actualTime;
bool statusToggle = false;
int boxCounter = 0 ;


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
#define MAX_DEVICES 8
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
      for (int i =0 ; i < NBLEDS ; i ++) 
      {
        //if ( i%2 == 0)
          strip.setPixelColor(i,0,0,0,128);
        }    
        
  strip.show(); // Initialize all pixels to 'off'
  strip.setBrightness(255);

  // initialize scroll screen
  mx.begin();
  prevTimeAnim = millis();
  prevTimeDemo = millis();
  strcpy (cmd , msgTab[8]);
}

void SetFeeder(int sheetNumberRemaining)
{
  // 125 because feeder got 8 steps
  int ratio = (int)((sheetNumberRemaining*1000/FEEDER_NBSHEET_MAX)/125);
  int sizeleds = (sizeof(feederPos)/sizeof(feederPos[0]));
  for ( int i = 0 ; i < sizeleds ; i++)
  {
    if ( (i+1)<= ratio)
      strip.setPixelColor(feederPos[i], red);
    else
      strip.setPixelColor(feederPos[i], black);
    }
}

void SetReception(int sheetNumberRemaining)
{
  int ratio = (int)((sheetNumberRemaining*1000/RECEPTION_NBSHEET_MAX)/125); 
  int sizeleds = (sizeof(receptionPos)/sizeof(receptionPos[0])); 
  for ( int i = 0 ; i < sizeleds ; i++)
  {
    if ( (i+1)<= ratio)
      strip.setPixelColor(receptionPos[i], red);
    else
      strip.setPixelColor(receptionPos[i], black);
    }
}

void SetStatusFeeder(int statusFeeder)
{
  int sizeleds = (sizeof(feederStatusPos) / sizeof(feederStatusPos[0]));
    switch (statusFeeder)
  {
  case 0:
          SetAllLedsArray(feederStatusPos,sizeleds, black);
    break;
  case 1:
          SetAllLedsArray(feederStatusPos,sizeleds, green);
    break;
  case 2:
          SetAllLedsArray(feederStatusPos,sizeleds, red);
    break;
  case 3:
          SetAllLedsArray(feederStatusPos,sizeleds, blue);
    break;
  default:
    break;
  }
};

void SetStatusReception(int statusReception)
{
  int sizeleds = (sizeof(receptionStatusPos) / sizeof(receptionStatusPos[0]));
  switch (statusReception)
  {
  case 0:
    SetAllLedsArray(receptionStatusPos,sizeleds, black);
    break;
  case 1:
    SetAllLedsArray(receptionStatusPos,sizeleds, green);
    break;
  case 2:
    SetAllLedsArray(receptionStatusPos,sizeleds, red);
    break;
  case 3:
    SetAllLedsArray(receptionStatusPos,sizeleds, blue);
    break;
  default:
    break;
  }
};

void SetStatusCut(int statusCut)
{
  int sizeleds = (sizeof(cutStatusPos) / sizeof(cutStatusPos[0]));
  switch (statusCut)
  {
  case 0:
    SetAllLedsArray(cutStatusPos,sizeleds, black);
    break;
  case 1:
    SetAllLedsArray(cutStatusPos,sizeleds, green);
    break;
  case 2:
    SetAllLedsArray(cutStatusPos,sizeleds, red);
    break;
  case 3:
    SetAllLedsArray(cutStatusPos,sizeleds, blue);
    break;
  default:
    break;
  }
};

void SetStatusEjection(int statusEjection)
{
  int sizeleds = (sizeof(ejectionStatusPos) / sizeof(ejectionStatusPos[0]));
  switch (statusEjection)
  {
  case 0:
    SetAllLedsArray(ejectionStatusPos,sizeleds, black);
    break;
  case 1:
    SetAllLedsArray(ejectionStatusPos,sizeleds, green);
    break;
  case 2:
    SetAllLedsArray(ejectionStatusPos,sizeleds, red);
    break;
  case 3:
    SetAllLedsArray(ejectionStatusPos,sizeleds, blue);
    break;
  default:
    break;
  }
};

void SetAllLedsArray ( int * arrayLed, int sizeArray, uint32_t color )
{
  for (size_t iLed = 0; iLed < sizeArray; iLed++)
  {
    strip.setPixelColor( arrayLed [iLed], color);
  }
  strip.show();
}

void SetAllStatus( int statusAll )
{
  SetStatusCut ( statusAll) ;
  SetStatusEjection ( statusAll);
  SetStatusFeeder ( statusAll);
  SetStatusReception ( statusAll);
}

void Stop()
{
  SetAllStatus (2);
}

void Reset()
{
  actualTime = millis();
  if (isResetOn && ((actualTime - previousTimeReset) < 1000))
  {
    SetAllStatus(3);
  }
  else
  {
    SetAllStatus(0);
    isResetOn = false;
  }
  previousTimeReset = actualTime;

}

void Run()
{
    SetAllStatus(1);
}

void Fill()
{
    actualTime = millis();
    int sizeleds = (sizeof(feederPos)/sizeof(feederPos[0]));
    if (isRunning &&(functionRun == 3 || functionRun == 2) && ((actualTime - previousTimeFill) > 250))
    { 
     for (int i =0 ; i < sizeleds ; i++)
      strip.setPixelColor(feederPos[i], 0,0,0);
     for (int i =0 ; i <= fillStep ; i++)
      strip.setPixelColor(feederPos[i], 255,0,0);
    fillStep = (fillStep + 1 ) % sizeleds;
    previousTimeFill = actualTime;   
    } 
}

void Slide()
{
    actualTime = millis();
    int sizeleds = (sizeof(chainPathPos)/sizeof(chainPathPos[0]));
    if (isRunning &&(functionRun == 3) && ((actualTime - previousTimeChain) > 25))
    { 
        for (int i =0 ; i < sizeleds ; i++)
            strip.setPixelColor(chainPathPos[i], 0,0,0,0);
        strip.setPixelColor(chainPathPos[chainStep], 0,0,0,255);
    chainStep = (chainStep + 1 ) % (sizeleds) ;
    previousTimeChain = actualTime;   
    }
  
}

void Empty()
{
    actualTime = millis();
    int sizeleds = (sizeof(receptionPos)/sizeof(receptionPos[0]));
    if (isRunning &&(functionRun == 3 ||functionRun == 4 ) && ((actualTime - previousTimeEmpty) > 250))
    { 
     for (int i =0 ; i < sizeleds ; i++)
      strip.setPixelColor(receptionPos[i], 255,0,0);
     for (int i =0 ; i <= emptyStep ; i++)
      strip.setPixelColor(receptionPos[i], 0,0,0);
    emptyStep = (emptyStep + 1 ) % sizeleds;
    previousTimeEmpty = actualTime;   
    }  
}
  
void Star()
{
    actualTime = millis();
    if (isRunning && (functionRun == 1) && ((actualTime - previousTimeStar) > 150))
    {
        for (int i =0 ; i < NBLEDS ; i++)
    strip.setPixelColor(i, random(0,15)*16, random(0,15)*16, random(0,15)*16, 0);
    strip.show();
    previousTimeStar = actualTime;   
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

void SetBoxCounter()
{



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

void loop() 
{
  if ( stringComplete )
  {
    Serial.println(cmd);
  
    if(strcmp(cmd, msgTab[0])  == 0)//stop
    {
      Serial.println("Command received: stop");
      isRunning = false;
      bInit = true ;
      Stop();
    }
    else if (strcmp(cmd, msgTab[1])  == 0)//reset
    {
      Serial.println("Command received: reset");
      isResetOn = true;
      bInit = true ;
    }
    else if (strcmp(cmd, msgTab[2])  == 0)//run
    {
      Serial.println("Command received: run");
      isRunning = false;
      bInit = true ;
      Run();
    }
    else if (strcmp(cmd, msgTab[3])  == 0)//star
    {
      Serial.println("Command received: star");
      functionRun = 1 ;
      isRunning = true;
      bInit = true ;
    }
    else if (strcmp(cmd, msgTab[4])  == 0)//fill
    {
      Serial.println("Command received: fill");
      functionRun = 2 ;
      isRunning = true;
      bInit = true ;
    }
    else if (strcmp(cmd, msgTab[5])  == 0)//slide
    {
      Serial.println("Command received: slide");
      functionRun = 3 ;
      isRunning = true;
      bInit = true ;
    }
    else if (strcmp(cmd, msgTab[6])  == 0)//empty
    {
      Serial.println("Command received: empty");
      functionRun = 4 ;
      isRunning = true;
      bInit = true ;
    }
    else if (strcmp(cmd, msgTab[7])  == 0)//help
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

  Reset();
  Star();
  Fill();
  Slide();
  Empty();
  strip.show();
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

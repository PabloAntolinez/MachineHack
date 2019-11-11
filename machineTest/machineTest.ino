
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
uint32_t orange = strip.Color(255,105,0,0);
uint32_t whiteMax = strip.Color(255, 255, 255, 255);

// declare led index tables for animations
int feederPos[] =
    {
        7, 6, 5, 4, 3, 2, 1, 0
    };

int feederStatusPos[] =
    {
        8, 9, 10
    };
int chainPathPos[] =
    {
        /*11,12,13,14,*/ 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, //15, 16, 17, 18, 19, 20, 21 /*,40,41,42,43*/
    };

int feederBeltPos[] =
    {11, 12, 13, 14};

int ejectionBeltPos[] =
    {40, 41, 42, 43};

int cutStatusPos[] =
    {
        31, 32, 33};
int ejectionStatusPos[] =
    {
        34, 35, 36};
int receptionStatusPos[] =
    {
        37, 38, 39};
int receptionPos[] =
    {
        44, 45, 46, 47, 48, 49, 50, 51};

#define FEEDER_NBSHEET_MAX 100
#define RECEPTION_NBSHEET_MAX 100

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
        "TAKE THE BOBST CHALLENGE TO CONTROL ME !"};

const int MAX_CMD_LENGTH = 512;
char cmd[MAX_CMD_LENGTH];
int cmdIndex;
char incomingByte;
bool stringComplete = false;
bool isRunning = false;
bool isResetOn = false;
bool isResetToggle = false;
bool isBoxOnFeeder = false;
bool isBoxOnEjection = false;
int functionRun = 0;
int chainStep = 0;
int feederStep = 0;
int ejectionStep = 0;
int receptionStep = 0;
int feederBeltStep = 0;
unsigned long previousTimeChain;
unsigned long previousTimeReception;
unsigned long previousTimeFeeder;
unsigned long previousTimeStar;
unsigned long previousTimeReset;
unsigned long previousTimeFeederBelt;
unsigned long previousTimeEjectionBelt;
unsigned long actualTime;
bool statusToggle = false;
int boxCounter = 0;
int machineSpeed = 3;

// Use the MD_MAX72XX library to Display scrolling text

#include <MD_MAX72xx.h>
#include <SPI.h>
#define DEMO_DELAY 5 // time to show each demo element in seconds

// --------------------
// MD_MAX72xx hardware definitions and object
// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
//
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 8
#define CLK_PIN 52  // or SCK
#define DATA_PIN 51 // or MOSI
#define CS_PIN 53   // or SS

MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES); // SPI hardware interface

// --------------------
// Constant parameters
//
// Various delays in milliseconds
#define UNIT_DELAY 12
#define SCROLL_DELAY (1 * UNIT_DELAY)

#define CHAR_SPACING 1 // pixels between characters
#define BUF_SIZE 512    // character buffer size

// ========== General Variables ===========
//
uint32_t prevTimeAnim = 0;     // Used for remembering the millis() value in animations
uint32_t prevTimeDemo = 0;     //  Used for remembering the millis() time in demo loop
uint8_t timeDemo = DEMO_DELAY; // number of seconds left in this demo loop
bool bInit = true;

void setup()
{
  Serial.begin(115200);
  strip.begin();
  for (int i = 0; i < NBLEDS; i++)
  {
    //if ( i%2 == 0)
    strip.setPixelColor(i, 0, 0, 0, 128);
  }

  strip.show(); // Initialize all pixels to 'off'
  strip.setBrightness(255);

  // initialize scroll screen
  mx.begin();
  prevTimeAnim = millis();
  prevTimeDemo = millis();
  strcpy(cmd, msgTab[8]);
  Run();
  SetAllStatus(4);
  SetFeeder(5);
  for (int i = 0; i < 100; i++)
  {
    SetStateText(208 + i % 4);
    SetBoxCounter(i);
    delay(10);
  }
  Run();
    isRunning = true;
  functionRun = 3;
}

void SetFeeder(int sheetNumberRemaining)
{
  // 12.5 because feeder got 8 steps
  int ratio = (int)(((double)sheetNumberRemaining / FEEDER_NBSHEET_MAX) * 8);
  int sizeLeds = (sizeof(feederPos) / sizeof(feederPos[0]));
  for (int i = 0; i < sizeLeds; i++)
    strip.setPixelColor(feederPos[i], black);
  for (int i = 0; i  < ratio; i++)
    strip.setPixelColor(feederPos[i], whiteMax);
  if (ratio <= 1) // warning
    SetStatusFeeder(4);
  else
    SetStatusFeeder(1);
}

void SetReception(int sheetNumber)
{
  int ratio = (int)(((double)sheetNumber / FEEDER_NBSHEET_MAX) * 8);
  int sizeLeds = (sizeof(receptionPos) / sizeof(receptionPos[0]));
    for (int i = 0; i < sizeLeds; i++)
    strip.setPixelColor(receptionPos[i], black);
  for (int i = 0; i  < ratio; i++)
    strip.setPixelColor(receptionPos[i], whiteMax);
    if (ratio >= 7) // warning
    SetStatusReception(4);
  else
    SetStatusReception(1);
}

void SetStatusFeeder(int statusFeeder)
{
  int sizeLeds = (sizeof(feederStatusPos) / sizeof(feederStatusPos[0]));
  switch (statusFeeder)
  {
  case 0:
    SetAllLedsArray(feederStatusPos, sizeLeds, black);
    break;
  case 1:
    SetAllLedsArray(feederStatusPos, sizeLeds, green);
    break;
  case 2:
    SetAllLedsArray(feederStatusPos, sizeLeds, red);
    break;
  case 3:
    SetAllLedsArray(feederStatusPos, sizeLeds, blue);
    break;
      case 4:
    SetAllLedsArray(feederStatusPos, sizeLeds, orange);
    break;
  default:
    break;
  }
};

void SetStatusReception(int statusReception)
{
  int sizeLeds = (sizeof(receptionStatusPos) / sizeof(receptionStatusPos[0]));
  switch (statusReception)
  {
  case 0:
    SetAllLedsArray(receptionStatusPos, sizeLeds, black);
    break;
  case 1:
    SetAllLedsArray(receptionStatusPos, sizeLeds, green);
    break;
  case 2:
    SetAllLedsArray(receptionStatusPos, sizeLeds, red);
    break;
  case 3:
    SetAllLedsArray(receptionStatusPos, sizeLeds, blue);
    break;
  case 4:
    SetAllLedsArray(receptionStatusPos, sizeLeds, orange);
    break;
  default:
    break;
  }
};

void SetStatusCut(int statusCut)
{
  int sizeLeds = (sizeof(cutStatusPos) / sizeof(cutStatusPos[0]));
  switch (statusCut)
  {
  case 0:
    SetAllLedsArray(cutStatusPos, sizeLeds, black);
    break;
  case 1:
    SetAllLedsArray(cutStatusPos, sizeLeds, green);
    break;
  case 2:
    SetAllLedsArray(cutStatusPos, sizeLeds, red);
    break;
  case 3:
    SetAllLedsArray(cutStatusPos, sizeLeds, blue);
    break;
  case 4:
    SetAllLedsArray(cutStatusPos, sizeLeds, orange);
    break;
  default:
    break;
  }
};

void SetStatusEjection(int statusEjection)
{
  int sizeLeds = (sizeof(ejectionStatusPos) / sizeof(ejectionStatusPos[0]));
  switch (statusEjection)
  {
  case 0:
    SetAllLedsArray(ejectionStatusPos, sizeLeds, black);
    break;
  case 1:
    SetAllLedsArray(ejectionStatusPos, sizeLeds, green);
    break;
  case 2:
    SetAllLedsArray(ejectionStatusPos, sizeLeds, red);
    break;
  case 3:
    SetAllLedsArray(ejectionStatusPos, sizeLeds, blue);
    break;
  case 4:
    SetAllLedsArray(ejectionStatusPos, sizeLeds, orange);
    break;
  default:
    break;
  }
};

void SetAllLedsArray(int *arrayLed, int sizeArray, uint32_t color)
{
  for (size_t iLed = 0; iLed < sizeArray; iLed++)
  {
    strip.setPixelColor(arrayLed[iLed], color);
  }
  strip.show();
}

void SetAllStatus(int statusAll)
{
  SetStatusCut(statusAll);
  SetStatusEjection(statusAll);
  SetStatusFeeder(statusAll);
  SetStatusReception(statusAll);
}

void Stop()
{
  SetAllStatus(2);
}

void Reset()
{
  actualTime = millis();
  if (isResetOn)
  {
    SetAllStatus(3);
    isResetOn = false;
    isResetToggle = true;
    previousTimeReset = actualTime;
  }
  if (isResetToggle && ((actualTime - previousTimeReset) > 1000))
  {
    SetAllStatus(0);
    isResetToggle = false;
  }
}

void Run()
{
  SetAllStatus(1);
}

void Feeder()
{
  actualTime = millis();
  int sizeLeds = (sizeof(feederPos) / sizeof(feederPos[0]));
  if (isRunning && (functionRun == 3 || functionRun == 2) && (chainStep == 0||chainStep == 8) && ((actualTime - previousTimeFeeder) > ((12/machineSpeed)*25)))
  {
    SetFeeder((sizeLeds-feederStep)*125/10 +1);
    feederStep = (feederStep + 1) % sizeLeds;
    previousTimeFeeder = actualTime;
  }
}

void FeederBelt()
{
  actualTime = millis();
  int sizeLeds = (sizeof(feederBeltPos) / sizeof(feederBeltPos[0]));
  if (feederBeltStep == sizeLeds)
  {
    feederBeltStep = 0;
    isBoxOnFeeder = false;
    strip.setPixelColor(feederBeltPos[sizeLeds - 1], black);
  }
  if (isRunning && (functionRun == 3)  && ((actualTime - previousTimeFeederBelt) > ((6 / machineSpeed) * 10)) && isBoxOnFeeder)
  {
    for (int i = 0; i < sizeLeds; i++)
      strip.setPixelColor(feederBeltPos[i], black);
    strip.setPixelColor(feederBeltPos[feederBeltStep], whiteMax);
    feederBeltStep = (feederBeltStep + 1);
    previousTimeFeederBelt = actualTime;
  }
}

void Chain()
{
  actualTime = millis();
  int sizeLeds = (sizeof(chainPathPos) / sizeof(chainPathPos[0]));
  if (isRunning && (functionRun == 3) && ((actualTime - previousTimeChain) > ((6/machineSpeed)*25)))
  {
    for (int i = 0; i < sizeLeds; i++)
      strip.setPixelColor(chainPathPos[i], black);
    strip.setPixelColor(chainPathPos[chainStep], (chainStep >= 0 && chainStep <= 6)? whiteMax : green);
    strip.setPixelColor(chainPathPos[(chainStep+sizeLeds/2)%sizeLeds],(((chainStep+sizeLeds/2)%sizeLeds) >= 0 && ((chainStep+sizeLeds/2)%sizeLeds) <= 6)? whiteMax : green);
    chainStep = (chainStep + 1) % (sizeLeds);
    if (chainStep == 15||chainStep==7)
    {
      isBoxOnFeeder = true;
    }
        if (chainStep == 6||chainStep==14)
    {
      isBoxOnEjection = true;
    }

    previousTimeChain = actualTime;
  }
}

void Reception()
{
  actualTime = millis();
  int sizeLeds = (sizeof(receptionPos) / sizeof(receptionPos[0]));
  if (isRunning && (functionRun == 3 || functionRun == 2) && (chainStep == 6||chainStep == 14) && ((actualTime - previousTimeReception) > ((12/machineSpeed)*25)))
  {
    SetReception(receptionStep*125/10 + 1);
    receptionStep = (receptionStep + 1) % sizeLeds;
    previousTimeReception = actualTime;
  }
}

void EjectionBelt()
{
  actualTime = millis();
  int sizeLeds = (sizeof(ejectionBeltPos) / sizeof(ejectionBeltPos[0]));
  if (ejectionStep == sizeLeds)
  {
    ejectionStep = 0;
    isBoxOnEjection = false;
    strip.setPixelColor(ejectionBeltPos[sizeLeds - 1], black);
  }
  
  if (isRunning && (functionRun == 3) && ((actualTime - previousTimeEjectionBelt) > ((6/machineSpeed)*10))&&isBoxOnEjection)
  {
    for (int i = 0; i < sizeLeds; i++)
      strip.setPixelColor(ejectionBeltPos[i], black);
    strip.setPixelColor(ejectionBeltPos[ejectionStep], whiteMax);
    ejectionStep = (ejectionStep +1);
    previousTimeEjectionBelt = actualTime;
  }
}

void Star()
{
  actualTime = millis();
  if (isRunning && (functionRun == 1) && ((actualTime - previousTimeStar) > 150))
  {
    for (int i = 0; i < NBLEDS; i++)
      strip.setPixelColor(i, random(0, 15) * 16, random(0, 15) * 16, random(0, 15) * 16, 0);
    strip.show();
    previousTimeStar = actualTime;
  }
}

void serialEvent()
{
  if (incomingByte = Serial.available() > 0)
  {
    char byteIn = Serial.read();
    cmd[cmdIndex] = byteIn;
    if (byteIn == '\n')
    {
      //command finished
      isRunning = false;
      cmd[cmdIndex] = '\0';
      cmdIndex = 0;
      stringComplete = true;
    }
    else
    {
      if (cmdIndex++ >= MAX_CMD_LENGTH)
      {
        cmdIndex = 0;
      }
    }
  }
}

void SetStateText(int state)
{
  //char buf[6];
  //buf = "Hello";
  //sprintf(buf, "%s", state);
  PrintText(4, 7, "Hello");
}

void SetBoxCounter(int counter)
{
  char buf[6];
  sprintf(buf, "%04d", counter); // modified Ò font to display nothing
  //Serial.println(buf);
  PrintText(0, 2, buf);
}

void PrintText(uint8_t modStart, uint8_t modEnd, char *pMsg)
// Print the text string to the LED matrix modules specified.
// Message area is padded with blank columns after printing.
{
  uint8_t state = 0;
  uint8_t curLen;
  uint16_t showLen;
  uint8_t cBuf[8];
  int16_t col = ((modEnd + 1) * COL_SIZE) - 1;
  //mx.clear();
  mx.control(modStart, modEnd, MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

  do // finite state machine to print the characters in the space available
  {
    switch (state)
    {
    case 0: // Load the next character from the font table
      // if we reached end of message, reset the message pointer
      if (*pMsg == '\0')
      {
        showLen = col - (modEnd * COL_SIZE); // padding characters
        state = 2;
        break;
      }

      // retrieve the next character form the font file
      showLen = mx.getChar(*pMsg++, sizeof(cBuf) / sizeof(cBuf[0]), cBuf);
      curLen = 0;
      state++;
      // !! deliberately fall through to next state to start displaying

    case 1: // display the next part of the character
      mx.setColumn(col--, cBuf[curLen++]);

      // done with font character, now display the space between chars
      if (curLen == showLen)
      {
        showLen = CHAR_SPACING;
        state = 2;
      }
      break;

    case 2: // initialize state for displaying empty columns
      curLen = 0;
      state++;
      // fall through

    case 3: // display inter-character spacing or end of message padding (blank columns)
      mx.setColumn(col--, 0);
      curLen++;
      if (curLen == showLen)
        state = 0;
      break;

    default:
      col = -1; // this definitely ends the do loop
    }
  } while (col >= (modStart * COL_SIZE));

  mx.control(modStart, modEnd, MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
}

bool scrollText(bool bInit, char *pmsg)
// Callback function for data that is required for scrolling into the display
{
  static char curMessage[BUF_SIZE];
  static char *p = curMessage;
  static uint8_t state = 0;
  static uint8_t curLen, showLen;
  static uint8_t cBuf[8];
  uint8_t colData;

  // are we initializing?
  if (bInit)
  {
    resetMatrix();
    strcpy(curMessage, pmsg);
    state = 0;
    p = curMessage;
    bInit = false;
  }

  // Is it time to scroll the text?
  if (millis() - prevTimeAnim < SCROLL_DELAY)
    return (bInit);

  // scroll the display
  mx.transform(MD_MAX72XX::TSL); // scroll along
  prevTimeAnim = millis();       // starting point for next time

  // now run the finite state machine to control what we do
  switch (state)
  {
  case 0: // Load the next character from the font table
    showLen = mx.getChar(*p++, sizeof(cBuf) / sizeof(cBuf[0]), cBuf);
    curLen = 0;
    state = 1;

    // !! deliberately fall through to next state to start displaying

  case 1: // display the next part of the character
    colData = cBuf[curLen++];
    mx.setColumn(0, colData);
    if (curLen == showLen)
    {
      showLen = ((*p != '\0') ? CHAR_SPACING : mx.getColumnCount() - 1);
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
  return (bInit);
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
  if (stringComplete)
  {
    Serial.println(cmd);

    if (strcmp(cmd, msgTab[0]) == 0) //stop
    {
      Serial.println("Command received: stop");
      isRunning = false;
      bInit = true;
      Stop();
    }
    else if (strcmp(cmd, msgTab[1]) == 0) //reset
    {
      Serial.println("Command received: reset");
      isResetOn = true;
      bInit = true;
    }
    else if (strcmp(cmd, msgTab[2]) == 0) //run
    {
      Serial.println("Command received: run");
      isRunning = false;
      bInit = true;
      Run();
    }
    else if (strcmp(cmd, msgTab[3]) == 0) //star
    {
      Serial.println("Command received: star");
      functionRun = 1;
      isRunning = true;
      bInit = true;
      strcpy(cmd,"Yippie !!!!");
    }
    else if (strcmp(cmd, msgTab[4]) == 0) //fill
    {
      Serial.println("Command received: fill");
      functionRun = 2;
      isRunning = true;
      bInit = true;
    }
    else if (strcmp(cmd, msgTab[5]) == 0) //slide
    {
      Serial.println("Command received: slide");
      functionRun = 3;
      isRunning = true;
      bInit = true;
    }
    else if (strcmp(cmd, msgTab[6]) == 0) //empty
    {
      Serial.println("Command received: empty");
      functionRun = 4;
      isRunning = true;
      bInit = true;
    }
    else if (strcmp(cmd, msgTab[7]) == 0) //help
    {
      Serial.println("Command received: help");
      for (int i = 0; i < sizeof(msgTab) / sizeof(msgTab[0]); i++)
        Serial.println(msgTab[i]);
      isRunning = false;
      bInit = true;
    }
    else
    {
      Serial.println(cmd);
    }

    stringComplete = false;
  }

  Reset();
  Star();
  Feeder();
  FeederBelt();
  Chain();
  Reception();
  EjectionBelt();
  strip.show();
  bInit = scrollText(bInit, cmd);
}

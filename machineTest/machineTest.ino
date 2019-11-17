
// *************************************************** USE leds library ***************************************************
#include <Adafruit_NeoPixel.h>

#define LEDPIN 2
#define NBLEDS 52
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NBLEDS, LEDPIN, NEO_GRBW + NEO_KHZ800);

uint32_t red = strip.Color(255, 0, 0, 0);
uint32_t green = strip.Color(0, 255, 0, 0);
uint32_t darkGreen = strip.Color(0, 40, 0, 0);
uint32_t blue = strip.Color(0, 0, 255, 0);
uint32_t white = strip.Color(0, 0, 0, 255);
uint32_t black = strip.Color(0, 0, 0, 0);
uint32_t orange = strip.Color(255, 105, 0, 0);
uint32_t whiteMax = strip.Color(255, 255, 255, 255);

// declare led index tables for animations
int feederPos[] =
    {
        7, 6, 5, 4, 3, 2, 1, 0};

int feederStatusPos[] =
    {
        8, 9, 10};
int chainPathPos[] =
    {
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30};

int feederBeltPos[] =
    {11, 12, 13, 14};

int ejectionBeltPos[] =
    {40, 41, 42, 43};

int cutStatusPos[] =
    {31, 32, 33};
int ejectionStatusPos[] =
    {34, 35, 36};
int receptionStatusPos[] =
    {37, 38, 39};
int receptionPos[] =
    {44, 45, 46, 47, 48, 49, 50, 51};

char *msgTab[] =
    {
        "run",
        "prod",
        "sat",
        "star",
        "speed",
        "state",
        "blocked",
        "help",
        "TAKE THE BOBST CHALLENGE TO CONTROL ME !"};
char *msgTab2[] =
    {
        "FS", //feederStack
        "FM", //feederMax
        "DS", //deliveryStack
        "DM", //deliveryMax
        "BW", //boxwasted
        "WM", //wastedMax
        "BP", //boxProduced
        "FR", //feederRunning
        "CS", //CurrentSpeed
        "MS", // machineSpeed
        "PR", //press
        "EJ"  //ejection
};

const int MAX_CMD_LENGTH = 512;
char cmd[MAX_CMD_LENGTH];
char messageDisp[MAX_CMD_LENGTH] = "0";
int cmdIndex = 0;
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
int statusStep = 0;
unsigned long previousTimeChain;
unsigned long previousTimeReception;
unsigned long previousTimeFeeder;
unsigned long previousTimeStar;
unsigned long previousTimeReset;
unsigned long previousTimeFeederBelt;
unsigned long previousTimeEjectionBelt;
unsigned long previousTimeStatus;
unsigned long actualTime;
bool statusToggle = false;
int statusPress = 0;
int statusEjection = 0;
int boxCounter = 235;
int wasteCounter = 0;
int feederCounter = 0;
int receptionCounter = 0;
int feederSheetMax = 100;
int receptionSheetMax = 100;
int wasteSheetMax = 100;
int machineSpeed = 1;
int currentState = 0;
String cmdString = "";
int valueCmd = 0;
// *************************************************** Use the MD_MAX72XX library to Display scrolling text ***************************************************

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
#define BUF_SIZE 512   // character buffer size

// ========== General Variables ===========
//
uint32_t prevTimeAnim = 0;     // Used for remembering the millis() value in animations
uint32_t prevTimeDemo = 0;     //  Used for remembering the millis() time in demo loop
uint8_t timeDemo = DEMO_DELAY; // number of seconds left in this demo loop
bool bInit = true;

// // *************************************************** Json ***************************************************

// #include <ArduinoJson.h>

// //const char *json = "{\"NbBoxFeederStack\":0,\"NbBoxFeederStackMax\":10000,\"NbBoxDeliveryStack\":0,\"NbBoxDeliveryStackMax\":10000,\"NbBoxWasted\":0,\"NbBoxWastedTotal\":0,\"NbBoxWastedMax\":600,\"NbBoxProduced\":0,\"FeederRunning\":false,\"NbOfIssues\":0,\"IssueType\":\"tbd\",\"CurrentSpeed\":0,\"CurrentMachineState\":\"running\",\"MachineTime\":640,\"ProductionTime\":0}";
// StaticJsonDocument<500> doc;

// int NbBoxFeederStack = 0;
// int NbBoxFeederStackMax = 10000;
// int NbBoxDeliveryStack = 0;
// int NbBoxDeliveryStackMax = 10000;     // 10000
// int NbBoxWasted = 0;                   // 0
// int NbBoxWastedTotal = 0;              // 0
// int NbBoxWastedMax = 600;              // 600
// int NbBoxProduced = 0;                 // 0
// bool FeederRunning = false;            // false
// int NbOfIssues = 0;                    // 0
// char *IssueType = "tbd";               // "tbd"
// int CurrentSpeed = 0;                  // 0
// char *CurrentMachineState = "running"; // "running"
// int MachineTime = 640;                 // 640
// int ProductionTime = 0;                // 0

// *************************************************** Setup ***************************************************

void setup()
{
  Serial.begin(9600);
  Serial.setTimeout(2000);
  // strip init
  strip.begin();
  strip.setBrightness(255);
  // for (int i = 0; i < NBLEDS; i++)
  // {
  //   strip.setPixelColor(i, 0, 0, 0, 128);
  // }
  // strip.show(); // Initialize all pixels to 'off'

  // initialize scroll screen
  mx.begin();
  prevTimeAnim = millis();
  prevTimeDemo = millis();
  Init();
  // // init demo mode
  // strcpy(cmd, msgTab[8]);
  // SetStateText(0);
  // for (int i = 0; i < 100; i++)
  // {
  //   SetBoxCounterDisp(i);
  //   delay(10);
  // }
  // AllBlack();
}

// *************************************************** functions ***************************************************

void Init()
{
  mx.clear();
  for (int i = 0; i < NBLEDS; i++)
  {
    strip.setPixelColor(i, 0, 0, 0, 128);
  }
  strip.show(); // Initialize all pixels to 'off'
  // init demo mode
  strcpy(messageDisp, msgTab[8]);
  currentState = 0;
  SetStateText(currentState);
  for (int i = 0; i < 100; i++)
  {
    SetBoxCounterDisp(i);
    delay(5);
  }
  AllBlack();
  mx.clear();
}

void SetFeeder(int sheetNumberRemaining)
{
  feederCounter = sheetNumberRemaining;
  if (feederCounter > feederSheetMax)
  {
    feederCounter = feederSheetMax;
  }
  int ratio = (int)ceil(((double)sheetNumberRemaining / feederSheetMax) * 8);
  int sizeLeds = (sizeof(feederPos) / sizeof(feederPos[0]));
  for (int i = 0; i < sizeLeds; i++)
    strip.setPixelColor(feederPos[i], black);
  for (int i = 0; i < ratio; i++)
    strip.setPixelColor(feederPos[i], whiteMax);
  if (ratio == 1)
  {
    SetStatusFeeder(orange);
  }
  if (sheetNumberRemaining == 0) // warning
    SetStatusFeeder(red);
}

void SetReception(int sheetNumber)
{
  receptionCounter = sheetNumber;
  if (sheetNumber > receptionSheetMax)
  {
    sheetNumber = receptionSheetMax;
  }

  int ratio = (int)(((double)sheetNumber / receptionSheetMax) * 8);
  int sizeLeds = (sizeof(receptionPos) / sizeof(receptionPos[0]));
  for (int i = 0; i < sizeLeds; i++)
    strip.setPixelColor(receptionPos[i], black);
  for (int i = 0; i < ratio; i++)
    strip.setPixelColor(receptionPos[i], whiteMax);
  if (ratio == 8)
  {
    SetStatusFeeder(orange);
  }

  if (sheetNumber == receptionSheetMax) // warning
    SetStatusReception(red);
}

void SetStatusFeeder(uint32_t statusFeederColor)
{
  int sizeLeds = (sizeof(feederStatusPos) / sizeof(feederStatusPos[0]));
  SetAllLedsArray(feederStatusPos, sizeLeds, statusFeederColor);
};

void SetStatusReception(uint32_t statusReceptionColor)
{
  int sizeLeds = (sizeof(receptionStatusPos) / sizeof(receptionStatusPos[0]));
  SetAllLedsArray(receptionStatusPos, sizeLeds, statusReceptionColor);
};

void SetStatusCut(uint32_t statusCutColor)
{
  int sizeLeds = (sizeof(cutStatusPos) / sizeof(cutStatusPos[0]));
  SetAllLedsArray(cutStatusPos, sizeLeds, statusCutColor);
};

void SetStatusEjection(uint32_t statusEjectionColor)
{
  int sizeLeds = (sizeof(ejectionStatusPos) / sizeof(ejectionStatusPos[0]));
  SetAllLedsArray(ejectionStatusPos, sizeLeds, statusEjectionColor);
};

void SetAllLedsArray(int *arrayLed, int sizeArray, uint32_t color)
{
  for (size_t iLed = 0; iLed < sizeArray; iLed++)
  {
    strip.setPixelColor(arrayLed[iLed], color);
  }
}

void SetAllStatus(uint32_t statusAllColor)
{
  SetStatusCut(statusAllColor);
  SetStatusEjection(statusAllColor);
  SetStatusFeeder(statusAllColor);
  SetStatusReception(statusAllColor);
}

void Stop()
{
  //SetAllStatus(red);
}

void Reset()
{
  actualTime = millis();
  if (isResetOn)
  {
    SetAllStatus(blue);
    isResetOn = false;
    isResetToggle = true;
    previousTimeReset = actualTime;
  }
  if (isResetToggle && ((actualTime - previousTimeReset) > 1000))
  {
    SetAllStatus(black);
    isResetToggle = false;
  }
}

void Run()
{
  SetAllStatus(green);
}

void Feeder()
{
  actualTime = millis();
  int sizeLeds = (sizeof(feederPos) / sizeof(feederPos[0]));
  if ((currentState == 0) && (chainStep == 0 || chainStep == 8) && ((actualTime - previousTimeFeeder) > ((12 / machineSpeed) * 25)))
  {
    SetFeeder((sizeLeds - feederStep) * 125 / 10 + 1);
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
  if ((currentState == 0 || currentState == 2 || currentState == 3) && ((actualTime - previousTimeFeederBelt) > ((6 / machineSpeed) * 10)) && isBoxOnFeeder)
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
  if ((currentState == 0 || currentState == 1 || currentState == 2 || currentState == 3) && ((actualTime - previousTimeChain) > ((6 / machineSpeed) * 25)))
  {
    for (int i = 0; i < sizeLeds; i++)
      strip.setPixelColor(chainPathPos[i], black);
    strip.setPixelColor(chainPathPos[chainStep], (chainStep >= 0 && chainStep <= 6 && (currentState == 0 || currentState == 2 || currentState == 3)) ? whiteMax : green);
    strip.setPixelColor(chainPathPos[(chainStep + sizeLeds / 2) % sizeLeds], (((chainStep + sizeLeds / 2) % sizeLeds) >= 0 && ((chainStep + sizeLeds / 2) % sizeLeds) <= 6 && (currentState == 0 || currentState == 2 || currentState == 3)) ? whiteMax : green);
    chainStep = (chainStep + 1) % (sizeLeds);
    if (chainStep == 15 || chainStep == 7)
    {
      isBoxOnFeeder = true;
      isBoxOnEjection = true;
    }
    if (currentState == 0 || currentState == 2 || currentState == 3)
    {
      SetAllStatus(darkGreen);
      if (chainStep == 7 || (chainStep + 8) % 16 == 7)
      {
        SetStatusFeeder(green);
      }
      else if (chainStep == 2 || (chainStep + 8) % 16 == 2)
      {
        SetStatusCut(green);
      }
      else if (chainStep == 4 || (chainStep + 8) % 16 == 4)
      {
        SetStatusEjection(green);
      }
      else if (chainStep == 6 || (chainStep + 8) % 16 == 6)
      {
        SetStatusReception(green);
      }
      if (currentState == 3)
      {
        SetStatusReception(orange);
      }
    }

    previousTimeChain = actualTime;
  }
}

void Reception()
{
  actualTime = millis();
  int sizeLeds = (sizeof(receptionPos) / sizeof(receptionPos[0]));
  if ((currentState == 0) && (chainStep == 6 || chainStep == 14) && ((actualTime - previousTimeReception) > ((12 / machineSpeed) * 25)))
  {
    SetReception(receptionStep * 125 / 10 + 1);
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

  if ((currentState == 0 || currentState == 2 || currentState == 3) && ((actualTime - previousTimeEjectionBelt) > ((6 / machineSpeed) * 10)) && isBoxOnEjection)
  {
    for (int i = 0; i < sizeLeds; i++)
      strip.setPixelColor(ejectionBeltPos[i], black);
    strip.setPixelColor(ejectionBeltPos[ejectionStep], whiteMax);
    ejectionStep = (ejectionStep + 1);
    previousTimeEjectionBelt = actualTime;
  }
}

void Status()
{
  actualTime = millis();
  switch (currentState)
  {
  case 1:
    SetStatusCut(darkGreen);
    SetStatusEjection(darkGreen);
    SetStatusReception(darkGreen);
    SetStatusFeeder(black);
    break;

  case 4:
    SetAllStatus(red);
    break;

  default:
    break;
  }
}
void Star()
{
  actualTime = millis();
  if (currentState == 5 && ((actualTime - previousTimeStar) > 150))
  {
    for (int i = 0; i < NBLEDS; i++)
      strip.setPixelColor(i, random(0, 15) * 16, random(0, 15) * 16, random(0, 15) * 16, 0);
    strip.show();
    previousTimeStar = actualTime;
  }
}

void AllBlack()
{
  for (int i = 0; i < NBLEDS; i++)
    strip.setPixelColor(i, black);
  strip.show();
}

// void serialEvent()
// {
//   char byteIn = 0;
//   int cmdIndex = 0;

//   bool finished = false;
//   while (Serial.available() > 0 && !finished)
//   {
//     byteIn = Serial.read();
//     if (byteIn == '\n')
//     {
//       stringComplete = true;
//       finished = true;
//       // strcpy(cmd,cmdString.c_str());
//     }
//     else
//     {
//       cmd[cmdIndex++] = byteIn;
//     }

//     // cmdString.concat(byteIn);

//   }
// }

void Ejection()
{
  switch (statusEjection)
  {
  case 0:
    SetStatusEjection(green);
    break;
  case 1:
    SetStatusEjection(orange);
    break;
  case 2:
    SetStatusEjection(red);
    break;
  default:
    break;
  }
}

void Cut()
{
  switch (statusPress)
  {
  case 0:
    SetStatusCut(green);
    break;
  case 1:
    SetStatusCut(orange);
    break;
  case 2:
    SetStatusCut(red);
    break;
  default:
    break;
  }
}

void SetStateText(int state)
{
  char buf[12];
  switch (state)
  {
  case 0:
    sprintf(buf, "Hello !");
    break;
  case 1:
    sprintf(buf, "RUN");
    break;

  case 2:
    sprintf(buf, "PROD");
    break;

  case 3:
    sprintf(buf, "SAT");
    break;

  case 4:
    sprintf(buf, "BLK");
    break;
  case 6:
    sprintf(buf, "LOG");
    break;
  default:
    break;
  }
  PrintText(4, 7, buf);
}

void SetCurrentState(int state)
{
  currentState = state;
}
void SetBoxCounter(int counter)
{
  boxCounter = counter;
}

void SetWastedCounter(int counter)
{
  wasteCounter = counter;
}

void Waste()
{
  if (wasteCounter > wasteSheetMax)
  {
    wasteCounter = wasteSheetMax;
  }

  int ratio = (int)ceil(((double)wasteCounter / wasteSheetMax) * 4);
  int sizeLeds = (sizeof(ejectionBeltPos) / sizeof(ejectionBeltPos[0]));
  for (int i = 0; i < sizeLeds; i++)
    strip.setPixelColor(ejectionBeltPos[i], black);
  for (int i = 0; i < ratio; i++)
    strip.setPixelColor(ejectionBeltPos[sizeLeds - i - 1], whiteMax);
  if (ratio == 4)
  {
    for (int i = 0; i < sizeLeds; i++)
      strip.setPixelColor(ejectionBeltPos[i], orange);
  }
  if (wasteCounter == wasteSheetMax) // warning
    for (int i = 0; i < sizeLeds; i++)
      strip.setPixelColor(ejectionBeltPos[i], red);
}

void SetBoxCounterDisp(int counter)
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

// void JsonRead()
// {
//   DeserializationError error = deserializeJson(doc, cmd);
//   // Test if parsing succeeds.
//   if (error)
//   {
//     Serial.print(F("deserializeJson() failed: "));
//     Serial.println(error.c_str());
//     return;
//   }

//   Serial.print("Json said :");
//   Serial.println (cmd);

//   const int MAX_CMD_LENGTH = 512;
//   char cmd[MAX_CMD_LENGTH];
//   int cmdIndex;
//   char incomingByte;
//   bool stringComplete = false;
//   bool isRunning = false;
//   bool isResetOn = false;
//   bool isResetToggle = false;
//   bool isBoxOnFeeder = false;
//   bool isBoxOnEjection = false;
//   int functionRun = 0;
//   int chainStep = 0;
//   int feederStep = 0;
//   int ejectionStep = 0;
//   int receptionStep = 0;
//   int feederBeltStep = 0;
//   int statusStep = 0;
//   unsigned long previousTimeChain;
//   unsigned long previousTimeReception;
//   unsigned long previousTimeFeeder;
//   unsigned long previousTimeStar;
//   unsigned long previousTimeReset;
//   unsigned long previousTimeFeederBelt;
//   unsigned long previousTimeEjectionBelt;
//   unsigned long previousTimeStatus;
//   unsigned long actualTime;
//   bool statusToggle = false;
//   int boxCounter = 235;
//   int wasteCounter = 0;
//   int feederCounter = 0;
//   int receptionCounter = 0;
//   int feederSheetMax = 100;
//   int receptionSheetMax = 100;
//   int wasteSheetMax = 100;
//   int machineSpeed = 1;
//   int currentState = 0;

//   feederCounter = doc["NbBoxFeederStack"].as<int>();          // 0
//   feederSheetMax = doc["NbBoxFeederStackMax"].as<int>();      // 10000
//   receptionCounter = doc["NbBoxDeliveryStack"].as<int>();     // 0
//   receptionSheetMax = doc["NbBoxDeliveryStackMax"].as<int>(); // 10000
//   wasteCounter = doc["NbBoxWasted"].as<int>();                // 0
//   NbBoxWastedTotal = doc["NbBoxWastedTotal"].as<int>();       // 0
//   wasteSheetMax = doc["NbBoxWastedMax"].as<int>();            // 600
//   boxCounter = doc["NbBoxProduced"].as<int>();                // 0
//   FeederRunning = doc["FeederRunning"].as<bool>();            // false
//   NbOfIssues = doc["NbOfIssues"].as<int>();                   // 0
//   IssueType = doc["IssueType"];                               // "tbd"
//   machineSpeed = doc["CurrentSpeed"].as<int>();               // 0

//   CurrentMachineState = doc["CurrentMachineState"]; // "running"

//   int oldState = currentState;
//   if (strcmp(CurrentMachineState, "running") == 0)
//   {
//     currentState = 1;
//   }
//   else if (strcmp(CurrentMachineState, "production") == 0)
//   {
//     currentState = 2;
//   }
//   else if (strcmp(CurrentMachineState, "saturated") == 0)
//   {
//     currentState = 3;
//   }
//   else if (strcmp(CurrentMachineState, "blocked") == 0)
//   {
//     currentState = 4;
//   }
//   else
//   {
//     currentState = oldState;
//     Serial.println(" state not understood");
//   }

//   MachineTime = doc["MachineTime"];       // 640
//   ProductionTime = doc["ProductionTime"]; // 0

// }

// ========== Control routines ===========
//
void resetMatrix(void)
{
  mx.control(MD_MAX72XX::INTENSITY, MAX_INTENSITY);
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
  mx.clear();
  prevTimeAnim = 0;
}

// char *msgTab[] =
//     {
//         "run",
//         "prod",
//         "sat",
//         "star",
//         "speed",
//         "state",
//         "blocked",
//         "help",
//         "TAKE THE BOBST CHALLENGE TO CONTROL ME !"};

// char *msgTab2[] =
//     {
//         "FS",//feederStack
//         "FM",//feederMax
//         "DS",//deliveryStack
//         "DM",//deliveryMax
//         "BW",//boxwasted
//         "WM",//wastedMax
//         "BP",//boxProduced
//         "FR",//feederRunning
//         "CS", //CurrentSpeed
//         "MS" // machineSpeed
//         };

// *************************************************** Loop ***************************************************
void loop()
{
  // char byteIn = 0;
  // int cmdIndex = 0;
  // int valueCmd = 0;
  // while (Serial.available() > 0)
  // {
  //   byteIn = Serial.read();
  //   if (byteIn == '\n')
  //   {
  //     stringComplete = true;
  //     strcpy(cmd,cmdString.c_str());
  //   }
  //   cmdString.concat(byteIn);
  //   //Serial.print("out :");
  //   //Serial.println(cmdString);

  // }
  char byteIn = 0;
  bool finished = false;
  while (Serial.available() > 0 && !finished)
  {
    byteIn = Serial.read();
    if (byteIn == '\n')
    {
      stringComplete = true;
      finished = true;
      cmd[cmdIndex++] = '\0';
      cmdIndex = 0;
      // strcpy(cmd,cmdString.c_str());
    }
    else
    {
      cmd[cmdIndex++] = byteIn; //cmdIndex + 48;
    }

    // cmdString.concat(byteIn);
  }

  if (stringComplete == true)
  {
    // Serial.print("cmd : ");
    // Serial.println( cmd );
    stringComplete = false;

    //***********************************************command machine ***************************************************

    if (strncmp(cmd, msgTab2[0], 2) == 0) //         "FS"//feederStack
    {
      sscanf(cmd + 3, "%i", &valueCmd);
      feederCounter = valueCmd;
    }
    else if (strncmp(cmd, msgTab2[1], 2) == 0) //         "FM",//feederMax
    {
      sscanf(cmd + 3, "%i", &valueCmd);
      feederSheetMax = valueCmd;
    }
    else if (strncmp(cmd, msgTab2[2], 2) == 0) //         "DS",//deliveryStack
    {
      sscanf(cmd + 3, "%i", &valueCmd);
      receptionCounter = valueCmd;
    }
    else if (strncmp(cmd, msgTab2[3], 2) == 0) //         "DM",//deliveryMax
    {
      sscanf(cmd + 3, "%i", &valueCmd);
      receptionSheetMax = valueCmd;
    }
    else if (strncmp(cmd, msgTab2[4], 2) == 0) //         "BW",//boxwasted
    {
      sscanf(cmd + 3, "%i", &valueCmd);
      wasteCounter = valueCmd;
    }
    else if (strncmp(cmd, msgTab2[5], 2) == 0) //         "WM",//wastedMax
    {
      sscanf(cmd + 3, "%i", &valueCmd);
      wasteSheetMax = valueCmd;
    }
    else if (strncmp(cmd, msgTab2[6], 2) == 0) //         "BP",//boxProduced
    {
      sscanf(cmd + 3, "%i", &valueCmd);
      boxCounter = valueCmd;
    }
    else if (strncmp(cmd, msgTab2[7], 2) == 0) //         "FR",//feederRunning
    {
      sscanf(cmd + 3, "%i", &valueCmd);
      //Serial.println("not implemented");
    }
    else if (strncmp(cmd, msgTab2[8], 2) == 0) //         "CS", //CurrentSpeed
    {
      sscanf(cmd + 3, "%i", &valueCmd);
      machineSpeed = valueCmd;
    }
    else if (strncmp(cmd, msgTab2[9], 2) == 0) //         "MS" // machineState
    {
      sscanf(cmd + 3, "%i", &valueCmd);
      mx.clear();
      currentState = valueCmd;
    }
    else if (strncmp(cmd, msgTab2[10], 2) == 0) //         "PR" // press
    {
      sscanf(cmd + 3, "%i", &valueCmd);
      statusPress = valueCmd;
    }
    else if (strncmp(cmd, msgTab2[11], 2) == 0) //         "EJ" // ejection
    {
      sscanf(cmd + 3, "%i", &valueCmd);
      statusEjection = valueCmd;
    }
    else if (strcmp(cmd, msgTab[0]) == 0) //run
    {
      currentState = 1;
      Serial.println("Command received: run");
      mx.clear();
    }
    else if (strcmp(cmd, msgTab[1]) == 0) //prod
    {
      Serial.println("Command received: prod");
      currentState = 2;
      AllBlack();
      mx.clear();
    }
    else if (strcmp(cmd, msgTab[2]) == 0) //sat
    {
      Serial.println("Command received: sat");
      currentState = 3;
      AllBlack();
      mx.clear();
    }
    else if (strcmp(cmd, msgTab[3]) == 0) //star
    {
      Serial.println("Command received: star");
      currentState = 5;
      AllBlack();
      mx.clear();
      bInit = true;
      strcpy(messageDisp, "Yippie !!!! Thank you and welcome !!!");
    }
    else if (strcmp(cmd, msgTab[4]) == 0) //speed
    {
      Serial.println("Command received: speed");
      machineSpeed = (machineSpeed + 1) % 4;
      bInit = true;
    }
    else if (strcmp(cmd, msgTab[5]) == 0) //state
    {
      Serial.println("Command received: state");
      currentState = (currentState + 1) % 5;
      if (currentState == 0)
        strcpy(messageDisp, msgTab[8]);
      mx.clear();
      bInit = true;
    }
    else if (strcmp(cmd, msgTab[6]) == 0) //blocked
    {
      Serial.println("Command received: blocked");
      currentState = 4;
      mx.clear();
      bInit = true;
    }
    else if (strcmp(cmd, msgTab[7]) == 0) //help
    {
      Serial.println("Command received: help");
      for (int i = 0; i < sizeof(msgTab) / sizeof(msgTab[0]); i++)
        Serial.println(msgTab[i]);
      bInit = true;
    }
    else if (strcmp(cmd, "init") == 0) //init
    {
      Serial.println("Command received: init");
      Init();
    }

    else if (strcmp(cmd, "") != 0)
    {
      if (currentState == 0 || currentState == 5)
      {

        strcpy(messageDisp, cmd);
      }
    }
    // cmdString = "";
  }
  Reset();
  Star();
  FeederBelt();
  Status();
  Chain();
  Feeder();
  Reception();
  Ejection();
  Cut();

  if (currentState != 0 && currentState != 5)
  {
    Waste();
  }
  if (currentState == 0 || currentState == 3)
    EjectionBelt();

  if (currentState == 0 || currentState == 5)
  {
    bInit = scrollText(bInit, messageDisp);
  }
  else
  {
    SetStateText(currentState);
    SetBoxCounterDisp(boxCounter);
    SetFeeder(feederCounter);
    SetReception(receptionCounter);
    Ejection();
    Cut();
  }
  if (currentState == 6)
  {
    AllBlack();
  }
  strip.show();
}

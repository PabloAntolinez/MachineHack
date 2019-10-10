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
  "help"
};

int MAX_CMD_LENGTH = 10;
char cmd[10];
int cmdIndex;
char incomingByte;
bool stringComplete = false;
bool isRunning = false;
int functionRun = 0 ;
int fill = 0 ;
unsigned long previousTime;
unsigned long actualTime;

void setup() {
  Serial.begin(9600);
  strip.begin();
      for (int i =0 ; i < NBLEDS ; i = i+2) 
      {
        //if ( i%2 == 0)
          strip.setPixelColor(i,255,0,0);
        }    
        
  strip.show(); // Initialize all pixels to 'off'
  strip.setBrightness(255);
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
 
void loop() {

if ( stringComplete )
{
Serial.println(cmd);

        if(strcmp(cmd, msgTab[0])  == 0){
          Serial.println("Command received: stop");
                        isRunning = false;
            Stop();

        }else if (strcmp(cmd, msgTab[1])  == 0) {
          Serial.println("Command received: reset");
              isRunning = false;
  Reset();

        }
        else if (strcmp(cmd, msgTab[2])  == 0) {
          Serial.println("Command received: run");
              isRunning = false;
  Run();

        }
        else if (strcmp(cmd, msgTab[3])  == 0) {
          Serial.println("Command received: star");
            functionRun = 1 ;
            isRunning = true;

        }
        else if (strcmp(cmd, msgTab[4])  == 0) {
          Serial.println("Command received: fill");
            functionRun = 2 ;
            isRunning = true;
        }
        else if (strcmp(cmd, msgTab[5])  == 0) {
          Serial.println("Command received: strip");
            functionRun = 3 ;
            isRunning = true;
        }
        else if (strcmp(cmd, msgTab[6])  == 0) {
          Serial.println("Command received: empty");
            functionRun = 4 ;
            isRunning = true;
        }
        else if (strcmp(cmd, msgTab[7])  == 0) {
          Serial.println("Command received: help");
          for (int i = 0 ; i < sizeof(msgTab)/sizeof(msgTab[0]); i++ )
           Serial.println(msgTab[i]);
            isRunning = false;
        }else{
          Serial.println("Command received: unknown!");
        }

stringComplete = false;
}

Star();
Fill();
Slide();
Empty();
}



void serialEvent() {
    if (incomingByte=Serial.available()>0) {
      
      char byteIn = Serial.read();
      cmd[cmdIndex] = byteIn;
      
      if(byteIn=='\n'){
        //command finished
        isRunning = false;
        cmd[cmdIndex] = '\0';
        cmdIndex = 0;
        stringComplete = true;
        

        
      }else{
        if(cmdIndex++ >= MAX_CMD_LENGTH){
          cmdIndex = 0;
        }
  }

    }
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

#include <Adafruit_NeoPixel.h>


#define LEDPIN 9
#define NBLEDS 8
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NBLEDS, LEDPIN, NEO_GRB + NEO_KHZ800);


int MAX_CMD_LENGTH = 10;
char cmd[10];
int cmdIndex;
char incomingByte;
bool stringComplete = false;
bool isRunning = false;
unsigned long previousTime;
unsigned long actualTime;

void setup() {
  Serial.begin(9600);
  strip.begin();
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


void Star()
{
    actualTime = millis();
    if (isRunning && ((actualTime - previousTime) > 150))
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

        if(strcmp(cmd, "stop")  == 0){
          Serial.println("Command received: stop");
                        isRunning = false;
            Stop();

        }else if (strcmp(cmd, "reset")  == 0) {
          Serial.println("Command received: reset");
              isRunning = false;
  Reset();

        }
        else if (strcmp(cmd, "run")  == 0) {
          Serial.println("Command received: run");
              isRunning = false;
  Run();

        }
        else if (strcmp(cmd, "star")  == 0) {
          Serial.println("Command received: star");
            isRunning = true;

        }else{
          Serial.println("Command received: unknown!");
        }

stringComplete = false;
}

Star();

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


#include "led_display.h";

int MAX_CMD_LENGTH = 10;
char cmd[10];
int cmdIndex;
char incomingByte;
bool stringComplete = false;
bool isRunning = false;
bool res = false;
unsigned long previousTime;
unsigned long actualTime;

class led_display;

led_display ledDisplay;

void setup() {
  Serial.begin(9600);
  res = ledDisplay.Init();
}

 
void loop() {

if ( stringComplete )
{
Serial.println(cmd);
ledDisplay.ScrollText(cmd);
//        if(strcmp(cmd, "stop")  == 0){
//          Serial.println("Command received: stop");
//                        isRunning = false;
//            Stop();
//
//        }else if (strcmp(cmd, "reset")  == 0) {
//          Serial.println("Command received: reset");
//              isRunning = false;
//  Reset();
//
//        }
//        else if (strcmp(cmd, "run")  == 0) {
//          Serial.println("Command received: run");
//              isRunning = false;
//  Run();
//
//        }
//        else if (strcmp(cmd, "star")  == 0) {
//          Serial.println("Command received: star");
//            isRunning = true;
//
//        }else{
//          Serial.println("Command received: unknown!");
//        }

stringComplete = false;
}

}



void serialEvent() 
{
  if (incomingByte=Serial.available()>0) 
  {
    char byteIn = Serial.read();
    cmd[cmdIndex] = byteIn; 
    if(byteIn=='\n')
    {
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

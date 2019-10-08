//#include <Adafruit_NeoPixel.h>
//
//
//#define LEDPIN 9
//#define NBLEDS 8
//Adafruit_NeoPixel strip = Adafruit_NeoPixel(NBLEDS, LEDPIN, NEO_GRB + NEO_KHZ800);
//
//class led_machine ()
//{
//  public
//  led_machine(){};
//  bool init ()
//  {
//    strip = Adafruit_NeoPixel(NBLEDS, LEDPIN, NEO_GRB + NEO_KHZ800);
//    strip.begin();
//    strip.show(); // Initialize all pixels to 'off'
//    strip.setBrightness(128);
//    return true ;
//    }
//  void Stop()
//{
//    for (int i =0 ; i < NBLEDS ; i++)
//      strip.setPixelColor(i, 255,0,0);
//    strip.show(); 
//}
//
//void Reset()
//{
//    for (int i =0 ; i < NBLEDS ; i++)
//      strip.setPixelColor(i, 0,0,255);
//    strip.show();
//    delay(1000);
//    for (int i =0 ; i < NBLEDS ; i++)
//      strip.setPixelColor(i, 0,0,0);
//    strip.show();
//}
//
//void Run()
//{
//      for (int i =0 ; i < NBLEDS ; i++)
//      strip.setPixelColor(i, 0,255,0);
//    strip.show();
//  
//  
//  }
//
//
//void Star()
//{
//    actualTime = millis();
//    if (isRunning && ((actualTime - previousTime) > 150))
//    {
//        for (int i =0 ; i < NBLEDS ; i++)
//    strip.setPixelColor(i, random(0,15)*16, random(0,15)*16, random(0,15)*16);
//    strip.show();
//    previousTime = actualTime;   
//    }
//  
//  }
//  private :
//
//  Adafruit_NeoPixel strip;
//int MAX_CMD_LENGTH = 10;
//char cmd[10];
//int cmdIndex;
//char incomingByte;
//bool stringComplete = false;
//bool isRunning = false;
//unsigned long previousTime;
//unsigned long actualTime;
//  
//  }
//
//
//

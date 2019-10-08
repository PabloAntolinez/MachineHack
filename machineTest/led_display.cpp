#include "led_display.h"

 led_display::led_display()
 {
  };
 
  bool led_display::Init() 
  {
    led_display::mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
    led_display::Clear_Display();
    return true ;
  }
     
  void led_display::ScrollText(char *p)
  {
    uint8_t charWidth;
    uint8_t cBuf[8];  // this should be ok for all built-in fonts
    mx.clear();

    while (*p != '\0')
    {
      charWidth = mx.getChar(*p++, sizeof(cBuf) / sizeof(cBuf[0]), cBuf);
      for (uint8_t i=0; i<=charWidth; i++)  // allow space between characters
      {
        mx.transform(MD_MAX72XX::TSL);
        if (i < charWidth)
          mx.setColumn(0, cBuf[i]);
        delay(DELAYTIME);
      }
    }
  }
  
  void led_display::Update_Display()
  {
    
    
  }

   
  void led_display::Clear_Display()
  {
    mx.control(MD_MAX72XX::INTENSITY, MAX_INTENSITY/2);
    mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
    mx.clear();
    led_display::prevTimeAnim = 0;
   }

//#include <MD_MAX72xx.h>
//
//// Turn on debug statements to the serial output
//#define  DEBUG  1
//#define PACMAN_DELAY 100
//
//
//// Define the number of devices we have in the chain and the hardware interface
//// NOTE: These pin numbers will probably not work with your hardware and may
//// need to be adapted
//#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
//#define MAX_DEVICES 4
//
//#define CLK_PIN   52  // or SCK
//#define DATA_PIN  51  // or MOSI
//#define CS_PIN    53  // or SS
//
//// SPI hardware interface
//
//// We always wait a bit between updates of the display
//#define  DELAYTIME  25  // in milliseconds
//
//class led_display 
//{  
//  public :
//  led_display() ;
//
//  bool Init() ;
//     
//  void ScrollText(char *p);
//  
//  void Update_Display();
//  private :
//   
//  void Clear_Display();
//   
//  uint32_t prevTimeAnim;
//  MD_MAX72XX mx ;
//};

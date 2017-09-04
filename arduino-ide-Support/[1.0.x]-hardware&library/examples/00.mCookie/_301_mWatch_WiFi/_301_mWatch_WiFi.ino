#include "U8glib.h"
#include "ESP8266.h"
#include "I2Cdev.h"
#include <Wire.h>
#include <Rtc_Pcf8563.h>
//#include <AM2321.h>
#include "rtc.h"


#define SSID        "xxxxxx"
#define PASSWORD    "xxxxxx"
#define HOST_NAME   "pool.ntp.org"
#define HOST_PORT   (123)

#define INTERVAL_NET             20000 
#define INTERVAL_LCD             200             //定义OLED刷新时间间隔  
#define INTERVAL_SENSOR          1000  

ESP8266 wifi(Serial1);

String wifiSetInfo="wifi ready...";

unsigned long net_time = millis();  
unsigned long lcd_time = millis();                 //OLED刷新时间计时器  

uint8_t buffer[128] = {0};
static uint8_t upd_id = 0;
uint32_t len=0;


U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);     //设置OLED型号  

//-------字体设置，大、中、小
#define setFont_L u8g.setFont(u8g_font_7x13)
#define setFont_M u8g.setFont(u8g_font_fixed_v0r)
#define setFont_S u8g.setFont(u8g_font_fixed_v0r)

#define setFont_SS u8g.setFont(u8g_font_fub25n)

const unsigned char bmp_logo0[] U8G_PROGMEM = 
{
  0x00,0x00,0x00,0x00,0x1E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0xFF,0xFF,0x01,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0xFC,0xFF,0xFF,0x01,0x87,0x9F,0x1D,0x00,0x00,0x00,0x00,0xC0,0xFF,0xFF,0xFF,0xC0,0x87,0x0F,0x7C,0x00,
  0x00,0x00,0x00,0xF0,0xFF,0xFF,0xFF,0xC0,0x03,0x38,0xE0,0x01,0x00,0x00,0x00,0xF8,0xFF,0xFE,0xFF,0x00,0x00,0x3C,0xFC,0xC1,0x07,0x00,0x00,0xF8,0xFF,0xE0,0x7F,0x00,0x00,0x6F,0xFF,0x9F,0x03,0x00,0x1C,0xF8,0xFF,0xE3,0x7F,0x00,0x0E,0xF7,
  0xFF,0xFF,0x1F,0x06,0xFF,0xFF,0xFF,0xE7,0x7F,0x80,0xBF,0xFF,0xFF,0xFF,0xFF,0x07,0xFF,0xFF,0xFF,0xEF,0xFF,0x81,0xFF,0xFF,0xFF,0xFF,0xFF,0x0F,0xFF,0xFF,0xFF,0xEF,0xE7,0xC1,0xFF,0xFF,0xFF,0xFF,0xFF,0x0F,0xFF,0xFF,0xFF,0xC7,0xC3,0xF0,
  0xFF,0xFF,0xFF,0xFF,0xFF,0x0B,0xFF,0xFF,0x8F,0x8F,0x01,0xF6,0xFF,0xFF,0xFF,0xFF,0xFF,0x01,0x1E,0xFF,0x9F,0x0F,0x00,0xEC,0xFF,0xFF,0xFF,0xFF,0x3C,0x00,0x07,0xFE,0xFF,0x3F,0x00,0xEF,0xFF,0xFF,0xFF,0xFF,0x39,0x00,0x01,0xFE,0xFF,0x3F,
  0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x19,0x00,0x00,0xFC,0xFF,0x7F,0x00,0xFC,0xFF,0xFF,0xFF,0xFF,0x05,0x00,0x00,0xF8,0xFF,0x7F,0x00,0xFC,0xFF,0xFF,0xFF,0xFF,0x07,0x00,0x00,0xF8,0xFF,0x0F,0x00,0xFE,0xFF,0xFF,0xFF,0xFF,0x03,0x00,0x00,0xF8,
  0xFF,0x01,0x00,0xEE,0xFF,0xFF,0xFF,0x9F,0x01,0x00,0x00,0xF8,0xFF,0x01,0x00,0xFE,0xFE,0xFF,0xFF,0xFF,0x00,0x00,0x00,0xF0,0xFF,0x00,0x00,0xFE,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0xE0,0x7F,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x17,0x00,0x00,
  0x00,0xC0,0xC7,0x00,0x80,0xFF,0xFF,0xFF,0xFF,0x17,0x00,0x00,0x08,0xC0,0xF7,0x03,0x80,0xFF,0xFF,0xE7,0xFF,0x07,0x00,0x00,0x08,0x00,0xBF,0x07,0x80,0xFF,0xFF,0xC7,0xFB,0x04,0x00,0x00,0x00,0x00,0x7C,0x00,0x80,0xFF,0xFF,0xC1,0xF9,0x0E,
  0x00,0x00,0x00,0x00,0xE0,0x0F,0x80,0xFF,0xFF,0xC1,0xF1,0x0C,0x00,0x00,0x00,0x00,0xC0,0x7F,0x00,0xFF,0xFF,0x81,0x39,0x0F,0x00,0x00,0x00,0x00,0x80,0x7F,0x00,0xEE,0xFF,0x00,0xB8,0x1F,0x00,0x00,0x00,0x00,0xC0,0xFF,0x01,0xE0,0x7F,0x00,
  0xF0,0xFF,0x00,0x00,0x00,0x00,0xC0,0xFF,0x07,0xC0,0x3F,0x00,0xE0,0xF7,0x0F,0x00,0x00,0x00,0xC0,0xFF,0x07,0xC0,0x3F,0x00,0xC0,0xCF,0x1F,0x00,0x00,0x00,0x80,0xFF,0x07,0xC0,0xBF,0x01,0x00,0xFE,0x17,0x00,0x00,0x00,0x80,0xFF,0x03,0xC0,
  0xFF,0x01,0x00,0xFC,0x01,0x03,0x00,0x00,0x00,0xFE,0x03,0xC0,0xDF,0x01,0x00,0xFF,0x63,0x03,0x00,0x00,0x00,0xFE,0x01,0x80,0xDF,0x00,0x00,0xFF,0x67,0x00,0x00,0x00,0x00,0xFE,0x00,0x80,0xDF,0x00,0x00,0xFF,0x07,0x00,0x00,0x00,0x00,0x7F,
  0x00,0x80,0x0F,0x00,0x00,0xFF,0x07,0x00,0x00,0x00,0x00,0x7F,0x00,0x00,0x07,0x00,0x00,0xEF,0x87,0x00,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x00,0x00,0x80,0x83,0x03,0x00,0x00,0x00,0x0F,0x00,0x00,0x00,0x00,0x00,0x00,0x83,0x03,0x00,0x00,
  0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0xC3,0x01,0x00,0x00,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0x00,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

void setup(void)
{
  Serial.begin(115200);


  showLogo();
  //while (!Serial); // wait for Leonardo enumeration, others continue immediately
  Serial.print("setup begin\r\n");

  Serial.print("FW Version:");
  Serial.println(wifi.getVersion().c_str());

  if (wifi.setOprToStationSoftAP()) {
    Serial.print("to station + softap ok\r\n");
    wifiSetInfo="softap ok";
    showLogo();
  } else {
    Serial.print("to station + softap err\r\n");
    wifiSetInfo="softap err";
    showLogo();
  }

  if (wifi.joinAP(SSID, PASSWORD)) {
    Serial.print("Join AP success\r\n");
    Serial.print("IP: ");
    Serial.println(wifi.getLocalIP().c_str());
    wifiSetInfo="Join AP success";
    showLogo();
  } else {
    Serial.print("Join AP failure\r\n");
    wifiSetInfo="Join AP failure";
    showLogo();
  }

  if (wifi.enableMUX()) {
    Serial.print("multiple ok\r\n");
  } else {
    Serial.print("multiple err\r\n");
  }

  Serial.print("setup end\r\n");

  wifiSetInfo="Getting Time...";
  showLogo();
  updateTimeData();


}

void loop(void)
{

  if (lcd_time > millis()) lcd_time = millis();
  if (millis() - lcd_time > INTERVAL_LCD) {
    ret = getRtcTimeString();

    //getRTC();
    volcd();                       //调用显示库

    //serialShowDateTime();
    lcd_time = millis();
  }

}





void updateTimeData() {
  do {
      delay(200);
      registerUDPAndSendRecvData();
      if(len>0) {
        getTimeStampAndSetRTC();
        unregisterUDP();
      } else {
        unregisterUDP();
      }
  } while(!len);
}



void registerUDPAndSendRecvData() {
  if (wifi.registerUDP(upd_id, HOST_NAME, HOST_PORT)) {
    Serial.print("register udp ");
    Serial.print(upd_id);
    Serial.println(" ok");
  } else {
    Serial.print("register udp ");
    Serial.print(upd_id);
    Serial.println(" err");
  }

  static const char PROGMEM
  timeReqA[] = { 227,  0,  6, 236 }, timeReqB[] = {  49, 78, 49,  52 };
  // Assemble and issue request packet
  uint8_t       buf[48];
  memset(buf, 0, sizeof(buf));
  memcpy_P( buf    , timeReqA, sizeof(timeReqA));
  memcpy_P(&buf[12], timeReqB, sizeof(timeReqB));

  wifi.send(upd_id, (const uint8_t*)buf, 48);
  //uint32_t len = wifi.recv(upd_id, buffer, sizeof(buffer), 10000);
  len = wifi.recv(upd_id, buffer, sizeof(buffer), 10000);
}

void getTimeStampAndSetRTC() {
      Serial.print("Received:[");

    unsigned long t = (((unsigned long)buffer[40] << 24) |
                       ((unsigned long)buffer[41] << 16) |
                       ((unsigned long)buffer[42] <<  8) |
                       (unsigned long)buffer[43]) - 2208988800UL;

    Serial.print("Unix timestamp:");
    Serial.print(t);
    Serial.print("]\r\n");

    getDateStamp(t);
    setRTC();

}


void unregisterUDP() {
    if (wifi.unregisterUDP(upd_id)) {
      Serial.print("unregister udp ");
      Serial.print(upd_id);
      Serial.println(" ok");
    } else {
      Serial.print("unregister udp ");
      Serial.print(upd_id);
      Serial.println(" err");
    }
}


//显示函数 
void volcd() {
  //pkj-=4;
  u8g.firstPage();
  do {
      setFont_L;
      u8g.setPrintPos(4, 16);
      u8g.print(rtc.formatDate());
      u8g.print("    ");
      switch (rtc.getWeekday()) {
        case 1:
          u8g.print("Mon");
          break;
        case 2:
          u8g.print("Tue");
          break;
        case 3:
          u8g.print("Wed");
          break;
        case 4:
          u8g.print("Thu");
          break;
        case 5:
          u8g.print("Fri");
          break;
        case 6:
          u8g.print("Sat");
          break;
        //case 7:
        case 0:
          u8g.print("Sun");
          break;
      }

      setFont_SS;
      //u8g.setPrintPos(18, 49);
      u8g.setPrintPos(18, 60);

      //u8g.print(rtc.getHour());

      if (rtc.getHour() < 10)
      {
        u8g.print("0");
      } 

      u8g.print(rtc.getHour());

      //u8g.setPrintPos(55, 46);
      u8g.setPrintPos(55, 55);
      if (rtc.getSecond() % 2 == 0)
        u8g.print(":");
      else
        u8g.print(" ");
      //u8g.setPrintPos(68, 48);
      u8g.setPrintPos(68, 60);
      if (rtc.getMinute() < 10)
      {
        u8g.print("0");
        u8g.print(rtc.getMinute());
      }
      else
        u8g.print(rtc.getMinute());
  }
  while( u8g.nextPage() );
} 



void showLogo() {
  u8g.firstPage();
  do {
    u8g.setDefaultForegroundColor();
    u8g.drawXBMP( 10, 1, 92, 46, bmp_logo0);

    setFont_L;
      u8g.setPrintPos(10, 60);
      //u8g.print("wifi ready...");
      u8g.print(wifiSetInfo);
  }
  while( u8g.nextPage() );
} 

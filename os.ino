/*
  OS
  
   SD card attached to SPI bus as follows:
 ** CS - pin 10
 ** The rest go to the ICSP pins

LCD pin outs
//-- Arduino UNO or Mega 2560 Plugged as OPEN_SMART shield 
  LCD_RST A4  //unused
  LCD_CS A3
  LCD_RS A2
  LCD_WR A1
  LCD_RD A0

  LCD_D0 8
  LCD_D1 9
  LCD_D2 10 //2
  LCD_D3 11 //3
  LCD_D4 4
  LCD_D5 13 //5
  LCD_D6 6
  LCD_D7 7

  created 27 Jun 2021
  by samuelh2005
  modified 3 Aug 2021
  by samuelh2005

*/

#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
#include <TouchScreen.h>
#define MINPRESSURE 100
#define MAXPRESSURE 1000

const int XP = 6, XM = A2, YP = A1, YM = 7; 
const int TS_LEFT = 907, TS_RT = 136, TS_TOP = 942, TS_BOT = 139;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

Adafruit_GFX_Button on_btn, off_btn;

int pixel_x, pixel_y;     //Touch_getXY() updates global vars
bool Touch_getXY(void)
{
    TSPoint p = ts.getPoint();
    pinMode(YP, OUTPUT);      //restore shared pins
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH);   //because TFT control pins
    digitalWrite(XM, HIGH);
    bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
    if (pressed) {
        pixel_x = map(p.x, TS_LEFT, TS_RT, 0, tft.width()); //.kbv makes sense to me
        pixel_y = map(p.y, TS_TOP, TS_BOT, 0, tft.height());
    }
    return pressed;
}

#define BLACK       0x0000
#define BLUE        0x001F
#define RED         0xF800
#define GREEN       0x07E0
#define LIGHT_BLUE  0x07FF
#define CYAN        0x0BBF
#define MAGENTA     0xF81F
#define YELLOW      0xFFE0
#define WHITE       0xFFFF

void setup(void)
{
    Serial.begin(9600);
    uint16_t ID = tft.readID();
    Serial.print("TFT ID = 0x");
    Serial.println(ID, HEX);
    Serial.println("Calibrate for your Touch Panel");
    if (ID == 0xD3D3) ID = 0x9486; // write-only shield
    tft.begin(ID);
    tft.setRotation(0);            //PORTRAIT
    tft.fillScreen(CYAN);
    on_btn.initButton(&tft,  60, 200, 100, 40, WHITE, LIGHT_BLUE, BLACK, "ON", 2);
    off_btn.initButton(&tft, 180, 200, 100, 40, WHITE, LIGHT_BLUE, BLACK, "OFF", 2);
    on_btn.drawButton(false);
    off_btn.drawButton(false);
    tft.fillRect(40, 80, 160, 80, RED);
//    tft.setCursor(40+strlen("hello")*10, 80+(80/2));
//    tft.setTextColor(WHITE);
//    tft.print("hello");
}

/* two buttons are quite simple
 */
void loop(void)
{
    bool down = Touch_getXY();
    on_btn.press(down && on_btn.contains(pixel_x, pixel_y));
    off_btn.press(down && off_btn.contains(pixel_x, pixel_y));
    if (on_btn.justReleased())
        on_btn.drawButton();
    if (off_btn.justReleased())
        off_btn.drawButton();
    if (on_btn.justPressed()) {
        on_btn.drawButton(true);
        tft.fillRect(40, 80, 160, 80, GREEN);
    }
    if (off_btn.justPressed()) {
        off_btn.drawButton(true);
        tft.fillRect(40, 80, 160, 80, RED);
    }

    tft.fillRect(0, 0, tft.width(), 30, BLUE);
}

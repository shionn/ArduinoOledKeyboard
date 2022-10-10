#include <Arduino.h>
#include <ssd1309.h>
#include <Keyboard.h>
#include <Timer.h>
#include "icon.h"

/** sur un promicro
 * digital pin 16 is MOSI
 * digital pin 14 is MISO
 * digital pin 15 is SCLK (or SCK)
 * digital pin 17 is SS
 */

#define SHIFT_SELECT 5
#define SHIFT_CLOCK 4
#define SHIFT_DATA 6

#define LCD_CS 8
#define LCD_RW 9
#define LCD_RS 10

#define BUTTONS 2
#define MENU_BUTTON 3

#define MENU_1_MODE 0
#define OPTION_MODE 1

Ssd1309 lcd = Ssd1309(LCD_CS, LCD_RW, LCD_RS);
Timer timer = Timer();

uint8_t buttons[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
uint8_t mode = MENU_1_MODE;
boolean afkEnable = false;

void updateAfk()
{
  if (afkEnable)
  {
    Keyboard.press(KEY_LEFT_ALT);
    delay(1);
    Keyboard.release(KEY_LEFT_ALT);
  }
}

void setup()
{
  lcd.init();

  pinMode(SHIFT_SELECT, OUTPUT);
  pinMode(SHIFT_CLOCK, OUTPUT);
  pinMode(SHIFT_DATA, OUTPUT);

  pinMode(BUTTONS, INPUT_PULLUP);
  pinMode(MENU_BUTTON, INPUT_PULLUP);
  digitalWrite(SHIFT_SELECT, HIGH);
  timer.every(10000, updateAfk);
}

void display()
{
  switch (mode)
  {
  case MENU_1_MODE:
    lcd.spritePgm(0, 0, 32, 32, firefox);
    lcd.spritePgm(32, 0, 32, 32, spotify);
    lcd.spritePgm(64, 0, 32, 32, gimp);
    lcd.spritePgm(96, 0, 32, 32, blender);
    lcd.spritePgm(0, 32, 32, 32, shell);
    lcd.spritePgm(32, 32, 32, 32, eclipse);
    lcd.spritePgm(64, 32, 32, 32, vscode);
    lcd.spritePgm(96, 32, 32, 32, arduino);
    break;
  case OPTION_MODE:
    if (afkEnable)
      lcd.print(3, 12, F("NoAFK"));
    else
      lcd.print(8, 12, F("AFK"));
    break;
  default:
    break;
  }
}

void commande(const String command)
{
  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('r');
  Keyboard.releaseAll();
  delay(20);
  Keyboard.println(command);
}

void press(uint8_t button)
{
  switch (button + mode * 10)
  {
    /* MENU_1_MODE */
  case 0:
    commande(F("blender"));
    break;
  case 1:
    commande(F("gimp"));
    break;
  case 2:
    commande(F("spotify"));
    break;
  case 3:
    commande(F("firefox"));
    break;
  case 4:
    commande(F("arduino"));
    break;
  case 5:
    commande(F("code"));
    break;
  case 6:
    commande(F("eclipse"));
    break;
  case 7:
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.press(KEY_RETURN);
    Keyboard.releaseAll();
    break;
  case 8:
    mode = OPTION_MODE;
    break;
  /* OPTION_MODE */
  case 13:
    afkEnable = !afkEnable;
    break;
  case 18:
    mode = MENU_1_MODE;
    break;
  }
}

void readInputs()
{
  for (uint8_t i = 0; i < 8; i++)
  {
    digitalWrite(SHIFT_SELECT, LOW);
    shiftOut(SHIFT_DATA, SHIFT_CLOCK, LSBFIRST, 0xFF ^ (1 << i));
    digitalWrite(SHIFT_SELECT, HIGH);
    uint8_t button = digitalRead(BUTTONS);
    if (buttons[i] && !button)
    {
      press(i);
    }
    buttons[i] = button;
  }
  uint8_t button = digitalRead(MENU_BUTTON);
  if (button && !buttons[8])
  {
    press(8);
  }
  buttons[8] = button;
}

void loop()
{
  lcd.clearBuffer();

  lcd.hline(0, 127, 0);
  lcd.hline(0, 127, 63);
  lcd.vline(0, 0, 63);
  lcd.vline(127, 0, 63);

  display();

  lcd.display();

  readInputs();
  timer.update();
}
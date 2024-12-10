#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SLAVE_ADDRESS 4
LiquidCrystal_I2C lcd(0x27, 16, 2);

int threshold = 0;
int distance = 0;

void setup() {
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveData);

  lcd.init();
  lcd.backlight();
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print("Thresh: ");
  lcd.print(threshold);
  lcd.print(" cm   ");

  lcd.setCursor(0, 1);
  lcd.print("Dist: ");
  lcd.print(distance);
  lcd.print(" cm   ");

  delay(200);
}

/*
  Lux sensor
  W.J. Hoogervorst april 2021
  Based on example of BH1750 library.

*/

#include <U8x8lib.h>
#include <Wire.h>
#include <BH1750.h>

U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);

BH1750 lightMeter;

void setup() {

  Serial.begin(9600);
  u8x8.begin();
  u8x8.setPowerSave(0);
  u8x8.setFont(u8x8_font_inb21_2x4_f);
  // Initialize the I2C bus (BH1750 library doesn't do this automatically)
  Wire.begin();

  lightMeter.begin();

  //measure battery voltage
  // 3.6 - 4.2 is 0 % - 100 %
  float voltage;
  // A0 can read 0 - 3.3V as 0 - 1023, battery voltage is max 4.2V, so use a voltage divider.
  // using voltage divider of 19.8K and 10K, a voltage 3.3V at the analog pin refers to 4.96V, so 4.96 V at the input of the voltage divider results in a reading of 1023
  for (int i = 0; i < 10; i++) //take 10 measurements
    voltage += map(analogRead(A0), 0, 1023, 0, 496); // analog value 0 - 1023 and map to 496 (4.96V * 100)
  voltage = voltage / (float)1000;      // divide by 100 for converting to value to a float with 2 digits, divide by 10 for averaging measurements
  Serial.println(voltage, 2);
  int percentage;
  if (voltage < 3.6) // voltage to low, prevent negative percentages when using map function
    percentage = 2;
  else if (voltage > 4.2) // voltage to high, prevent percentages > 100% (like 101% or 102%)
    percentage = 100;
  else
    percentage = map(voltage * 100, 360, 420, 20, 100); // 3.6 - 4.2 is 0 % - 100 %
  Serial.println(percentage);
  String Perc_s = "Bat:";
  Perc_s +=  String(percentage);
  Perc_s += " %";
  char buf_volt[10];
  Perc_s.toCharArray(buf_volt, Perc_s.length() + 1);
  u8x8.clear();
  u8x8.drawString(0, 0, buf_volt);
  delay(2000);
}


void loop() {
  u8x8.clear();
  float lux = lightMeter.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");
  if (lux < 10)
  {
    String lux_s = String(lux);
    lux_s += " Lx";
    char buf[10];
    lux_s.toCharArray(buf, lux_s.length() + 1);
    u8x8.drawString(2 * (1 - (int)log10(lux)), 0, buf);
  }
  else if (lux < 100)
  {
    String lux_s = String(lux);
    String short_lux_s = lux_s.substring(0, lux_s.length() - 1);
    short_lux_s += " Lx";
    char buf[10];
    short_lux_s.toCharArray(buf, short_lux_s.length() + 1);
    u8x8.drawString(2 * (2 - (int)log10(lux)), 0, buf);
  }
  else {
    long lux_rounded0 = lux * (int)100 / 100;
    char buf2[20];
    sprintf(buf2, "%ld", lux_rounded0);
    char buf3[20] = " Lx";
    strcat (buf2, buf3);
    u8x8.drawString(2 * (4 - (int)log10(lux)), 0, buf2);
  }
  delay(1000);
  // The screen is 32 x 128 pixels, in the 8*8 library this results in 32/4 = 4 lines (0 - 3) and 128/8 = 16 columns (0 - 15).
  // The font uses 2*4 squares of 8*8 pixels. This results in 1 line (32/4/8 = 1) and 8 characters (128/2/8 = 8)
  // The 8 characters are sufficient for showing the maximum value of "65535_Lx".
  // The string is positioned at the right of the screen. The indent depends on the amount of figures in the value
  // The number of figures is derrived from the 10Log of the value (0 - 65535), which can have 1 - 5 figures, 10Log is 0 - 4.
  // The font has 2 positions for one character, that's why the outcome is multiplied by 2.
  // A value of 5 figures starts at column 0 (most left), a value of 2 figures starts at column 3*2 = column 6
}

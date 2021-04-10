/*
  Light meter using a BH1750 Lux sensor
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
  int lux = lightMeter.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");

  char buf2[20];
  sprintf(buf2, "%d", lux);

  char buf3[20] = " Lx";
  strcat (buf2, buf3);

  u8x8.clear();
  u8x8.drawString(1 + 2 * (3 - (int)log10(lux)), 0, buf2);
  delay(1000);
}

#include <Wire.h>
#include <Adafruit_INA219.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define AVG_LENGTH 100
#define REFRESH_FREQ_MS 100

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_MOSI   11
#define OLED_CLK   13
#define OLED_DC    8
#define OLED_CS    10
#define OLED_RESET 6

Adafruit_INA219 ina219;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

double shuntvoltage_avg = 0;
double busvoltage_avg = 0;
double current_mA_avg = 0;
double loadvoltage_avg = 0;
double power_mW_avg = 0;

void setup(void) 
{
  Serial.begin(115200);
  while (!Serial) {
      delay(1);
  }
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    while(1);
  }
  Serial.println("Hello!");
  if (! ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) { delay(10); }
  }
  ina219.setCalibration_16V_400mA(); // highest precision on current
  Serial.println("Measuring voltage and current with INA219 ...");
}

void loop(void) 
{
  for(int j=0; j<AVG_LENGTH; j++){
    current_mA_avg += ina219.getCurrent_mA();
    power_mW_avg += ina219.getPower_mW();
    delay(REFRESH_FREQ_MS);
  }

  current_mA_avg = current_mA_avg / AVG_LENGTH;
  power_mW_avg = power_mW_avg / AVG_LENGTH;

  Serial.print("Current:       "); Serial.print(current_mA_avg); Serial.println(" mA");
  Serial.print("Power:         "); Serial.print(power_mW_avg); Serial.println(" mW");
  Serial.println("");

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Current: ");display.print(current_mA_avg);display.println(" mA");
  display.print("Power: ");display.print(power_mW_avg);display.println(" mW");
  display.display();
}

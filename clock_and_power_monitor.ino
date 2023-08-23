#include <Wire.h>
#include <Adafruit_INA219.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define AVG_LENGTH 10
#define REFRESH_FREQ_MS 200

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_MOSI   11
#define OLED_CLK   13
#define OLED_DC    8
#define OLED_CS    10
#define OLED_RESET 6

Adafruit_INA219 ina219;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

int i = 0;
float shuntvoltage[AVG_LENGTH] = {0};
float busvoltage[AVG_LENGTH] = {0};
float current_mA[AVG_LENGTH] = {0};
float loadvoltage[AVG_LENGTH] = {0};
float power_mW[AVG_LENGTH] = {0};

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
  shuntvoltage[i] = ina219.getShuntVoltage_mV();
  busvoltage[i] = ina219.getBusVoltage_V();
  current_mA[i] = ina219.getCurrent_mA();
  power_mW[i] = ina219.getPower_mW();
  loadvoltage[i] = busvoltage[i] + (shuntvoltage[i] / 1000);
  
  float shuntvoltage_avg = 0;
  float busvoltage_avg = 0;
  float current_mA_avg = 0;
  float loadvoltage_avg = 0;
  float power_mW_avg = 0;

  for(int j=0; j<AVG_LENGTH; j++){
    shuntvoltage_avg += shuntvoltage[i];
    busvoltage_avg = busvoltage[i];
    current_mA_avg = current_mA[i];
    loadvoltage_avg = loadvoltage[i];
    power_mW_avg = power_mW[i];
  }

  shuntvoltage_avg += shuntvoltage_avg / AVG_LENGTH;
  busvoltage_avg = busvoltage_avg / AVG_LENGTH;
  current_mA_avg = current_mA_avg / AVG_LENGTH;
  loadvoltage_avg = current_mA_avg / AVG_LENGTH;
  power_mW_avg = power_mW_avg / AVG_LENGTH;

  Serial.print("Bus Voltage:   "); Serial.print(busvoltage_avg); Serial.println(" V");
  Serial.print("Shunt Voltage: "); Serial.print(shuntvoltage_avg); Serial.println(" mV");
  Serial.print("Load Voltage:  "); Serial.print(loadvoltage_avg); Serial.println(" V");
  Serial.print("Current:       "); Serial.print(current_mA_avg); Serial.println(" mA");
  Serial.print("Power:         "); Serial.print(power_mW_avg); Serial.println(" mW");
  Serial.println("");

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Bus: ");display.print(busvoltage_avg);display.println(" V");
  display.print("Shunt: ");display.print(shuntvoltage_avg);display.println(" mV");
  display.print("Current: ");display.print(current_mA_avg);display.println(" mA");
  display.print("Power: ");display.print(power_mW_avg);display.println(" mW");
  display.display();

  i++;
  if(i == AVG_LENGTH){
    i = 0;
  }
  
  delay(REFRESH_FREQ_MS);
}

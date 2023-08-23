#include <WiFi.h>
#include "time.h"
#include <NTPClient.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW

#define MAX_DEVICES 4
#define CLK_PIN 18
#define DATA_PIN 23
#define CS_PIN 17

#define TRANSITION_TIME 5000

const char ssid[] = "TIM-28371915";        // your network SSID (name)
const char password[] = "Mar1annaCarl0An1taS3bast1an01r3n3";    // your network password (use for WPA, or use as key for WEP)
const char* serverName = "https://third.zcsazzurroportal.com:19003/";
String thingKey = "ZH3ES160NCA187";
DynamicJsonDocument jsonBuffer(2048);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

int fuso = 2;
unsigned long t1;

unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

MD_Parola display = MD_Parola(HARDWARE_TYPE,CS_PIN, MAX_DEVICES);


void setup() { 
  Serial.begin(115200);
  display.begin();
  display.setIntensity(0);

	// Clear the display
	display.displayClear();
 
  int i = 0;
  while (!Serial) {
    delay(100);
    i++;
    if (i >= 10) break;
  }
  
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  timeClient.begin();
}
 
void loop() {
  
  static bool fpoint;

  /* TIME */
  if(WiFi.status()== WL_CONNECTED){
    timeClient.update();
    int hh = timeClient.getHours();
    hh += fuso;
    if (hh >= 24) hh - 24;
    int mm = timeClient.getMinutes();
    String time_string = "";
    time_string = time_string + String(hh);
    time_string = time_string + ":";
    time_string = time_string + String(mm);
    display.setTextAlignment(PA_CENTER);
    display.displayClear();
    display.print(time_string);
  }else {
    Serial.println("WiFi Disconnected");
  }

  delay(TRANSITION_TIME);

  /* POWER */
  if(WiFi.status()== WL_CONNECTED){
    HTTPClient http;
    // Your Domain name with URL path or IP address with path
    http.begin(serverName);
    // Specify content-type header
    // If you need an HTTP request with a content type: application/json, use the following:
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Zcs eHWAeEq0aYO0");
    http.addHeader("client", "CarloMonti");
    // JSON data to send with HTTP POST
    String httpRequestData = "{\"realtimeData\": {\"command\": \"realtimeData\",\"params\": {\"thingKey\": \"ZH3ES160NCA187\",\"requiredValues\": \"*\"}}}";           
    // Send HTTP POST reques

    int httpResponseCode = http.POST(httpRequestData);
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    if(httpResponseCode == 200)
    {
      //Expected output:{"realtimeData": {"params": {"value": [{"ZH3ES160NCA187": {"energyGenerating": 21.33, "thingFind": "2023-05-19T15:29:41Z", "lastUpdate": "2023-08-17T12:48:01Z", "energyGeneratingTotal": 2851.3, "powerGenerating": 4120}}]}, "success": true}}
        String json = http.getString();
        Serial.println(json);
        Serial.println("NOW parsing");
        DeserializationError err = deserializeJson(jsonBuffer, json);
        if(err){
          Serial.println("ERROR: ");
          Serial.println(err.c_str());
          return;
        }else{
          Serial.println("Deserialization OK: ");
        }
        float powerGenerating = jsonBuffer["realtimeData"]["params"]["value"][0]["ZH3ES160NCA187"]["powerGenerating"];
        String power_string = "";
        if(powerGenerating < 1){
          power_string = String(powerGenerating);
        }else{
          power_string = String(int(powerGenerating));
        }
        power_string = power_string + "kW";
        Serial.println("powerGenerating: ");
        Serial.println(powerGenerating);
        display.setTextAlignment(PA_CENTER);
        display.displayClear();
        display.print(power_string);
    }
    else
    {
      Serial.println("Error in response");
    }
    // Free resources
    http.end();
  }else {
    Serial.println("WiFi Disconnected");
  }

  delay(TRANSITION_TIME);
  
  /* METEO */
  if(WiFi.status()== WL_CONNECTED){
    HTTPClient http;
    // Your Domain name with URL path or IP address with path
    http.begin("https://avwx.rest/api/metar/LIMC");
    // Specify content-type header
    // If you need an HTTP request with a content type: application/json, use the following:
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "BEARER hSJvRXMUoCPDcY0khi9-u1B_rdfJzm8cUepgM0RVUCI");
    int httpResponseCode = http.GET();
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    if(httpResponseCode == 200)
    {
      //Expected output:{"realtimeData": {"params": {"value": [{"ZH3ES160NCA187": {"energyGenerating": 21.33, "thingFind": "2023-05-19T15:29:41Z", "lastUpdate": "2023-08-17T12:48:01Z", "energyGeneratingTotal": 2851.3, "powerGenerating": 4120}}]}, "success": true}}
        String json = http.getString();
        Serial.println(json);
        Serial.println("NOW parsing");
        DeserializationError err = deserializeJson(jsonBuffer, json);
        if(err){
          Serial.println("ERROR: ");
          Serial.println(err.c_str());
          return;
        }else{
          Serial.println("Deserialization OK: ");
        }

        /*{
            "meta": {
                "timestamp": "2023-08-17T19:20:53.943348Z",
                "stations_updated": "2023-08-05",
                "cache-timestamp": "2023-08-17T19:20:07.161000Z"
            },
            "altimeter": {
                "repr": "Q1016",
                "value": 1016,
                "spoken": "one zero one six"
            },
            "clouds": [],
            "flight_rules": "VFR",
            "other": [],
            "visibility": {
                "repr": "CAVOK",
                "value": 9999,
                "spoken": "ceiling and visibility ok"
            },
            "wind_direction": {
                "repr": "010",
                "value": 10,
                "spoken": "zero one zero"
            },
            "wind_gust": null,
            "wind_speed": {
                "repr": "04",
                "value": 4,
                "spoken": "four"
            },
            "wx_codes": [],
            "raw": "LIMC 171850Z 01004KT CAVOK 25/14 Q1016 NOSIG",
            "sanitized": "LIMC 171850Z 01004KT CAVOK 25/14 Q1016 NOSIG",
            "station": "LIMC",
            "time": {
                "repr": "171850Z",
                "dt": "2023-08-17T18:50:00Z"
            },
            "remarks": "NOSIG",
            "dewpoint": {
                "repr": "14",
                "value": 14,
                "spoken": "one four"
            },
            "relative_humidity": 0.5043159272445457,
            "remarks_info": {
                "maximum_temperature_6": null,
                "minimum_temperature_6": null,
                "pressure_tendency": null,
                "precip_36_hours": null,
                "precip_24_hours": null,
                "sunshine_minutes": null,
                "codes": [],
                "dewpoint_decimal": null,
                "maximum_temperature_24": null,
                "minimum_temperature_24": null,
                "precip_hourly": null,
                "sea_level_pressure": null,
                "snow_depth": null,
                "temperature_decimal": null
            },
            "runway_visibility": [],
            "temperature": {
                "repr": "25",
                "value": 25,
                "spoken": "two five"
            },
            "wind_variable_direction": [],
            "density_altitude": 2070,
            "pressure_altitude": 686,
            "units": {
                "accumulation": "in",
                "altimeter": "hPa",
                "altitude": "ft",
                "temperature": "C",
                "visibility": "m",
                "wind_speed": "kt"
            }
        }*/
        int temperature = jsonBuffer["temperature"]["value"];
        String meteo_string = "" + String(temperature) + "°C";
        Serial.println("temperature: ");
        Serial.println(temperature);
        display.setTextAlignment(PA_CENTER);
        display.displayClear();
        display.print(meteo_string);
    }
    else
    {
      Serial.println("Error in response");
    }
    // Free resources
    http.end();
  }else {
    Serial.println("WiFi Disconnected");
  }

  delay(TRANSITION_TIME);
}
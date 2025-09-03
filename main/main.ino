#include <WiFi.h>
#include <time.h>
#include <TFT_eSPI.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

// Defines constants
#define CONNECTION_TIMEOUT 10

// Add your wifi information here
const char* ssid = "";
const char* password = "";

TFT_eSPI tft = TFT_eSPI();

WiFiClientSecure client; 
HTTPClient http;

// Network based time
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -14400;
const int   daylightOffset_sec = 0;

// Weather Information
int temperature = 0;
const char * TEMPUNIT = "F";
int percentPrecip = 0;

void queryWeather() {
    client.setInsecure();
    client.setTimeout(60000);

    // Send request
    http.useHTTP10(true);
    http.begin(client, "https://api.weather.gov/gridpoints/FFC/88,99/forecast");
    http.GET();

    // Parse response
    DynamicJsonDocument doc(12288);
    DeserializationError error = deserializeJson(doc, http.getStream());

    if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
    }

    // Read values
    temperature = doc["properties"]["periods"][0]["temperature"].as<int>();
    percentPrecip = doc["properties"]["periods"][0]["probabilityOfPrecipitation"]["value"].as<int>();
    const char * shortForecast = doc["properties"]["periods"][0]["shortForecast"];
    const char * shortForecastTmr = doc["properties"]["periods"][0]["shortForecast"];
    const char * icon = doc["properties"]["periods"][1]["icon"];

    // Disconnect
    http.end();

    // Displaying scrapped data
    tft.print("TODAY:    ");
    tft.println(shortForecast);
    tft.print("TOMORROW: ");
    tft.println(shortForecastTmr);

}


void setup() {
    // Creates serial monitor page
    Serial.begin(115200);
    delay(1000);

    // Initializes the tft screen
    tft.begin();
    tft.setRotation(1);

    // Sets the screen color
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    delay(1000);

    // Initializes the wifi
    WiFi.mode(WIFI_STA); //Optional
    WiFi.begin(SSID, PASSWORD);
    tft.println("\nConnecting");
    uint8_t timeout_counter = 0;

    // Waits until connected
    while(WiFi.status() != WL_CONNECTED){
        //Serial.print(".");
        tft.print(".");
        delay(200);
        timeout_counter++;

        // If there is an issue connecting, the ESP restarts
        if(timeout_counter >= CONNECTION_TIMEOUT*5){
            ESP.restart();
        }
    }

    tft.println("\nConnected to the WiFi network");
    tft.print("Local ESP32 IP: ");
    tft.println(WiFi.localIP()); 

    delay(1000);
}

void loop() {
    // Resets the screen
    tft.setCursor(0,0);
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(3);


    // Loading message
    tft.println("Loading...");
    tft.setCursor(0, 0);
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(3);

    // Getting the time
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
        Serial.println("Failed to obtain time");
        return;
    }

    // Checks if AM or PM
    char * endFormat;
    char timeHour[3];
    strftime(timeHour,3, "%H", &timeinfo);
    if (0 == atoi(timeHour)) {
        endFormat = " AM";
    } else if (atoi(timeHour) < 12) {
        endFormat = " AM";
    } else {
        endFormat = " PM";
    }

    // Location Content
    tft.println("Athens, GA");

    tft.print(&timeinfo, "%I");
    tft.print(":");
    tft.print(&timeinfo, "%M");
    tft.println(endFormat);

    // Gets the latest weather data
    tft.println("");
    queryWeather();

    // Weather Content
    tft.println("");
    tft.print("Temperature: ");
    tft.print(temperature);
    tft.println(TEMPUNIT);

    tft.print("Precipitation: ");
    tft.print(percentPrecip);
    tft.println("%");

    delay(60000);

}
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
int temperature;

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
    Serial.println(temperature);

    // Disconnect
    http.end();
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
    WiFi.begin(ssid, password);
    //Serial.println("\nConnecting");
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

    // Wifi connection information
    //Serial.println("\nConnected to the WiFi network");
    //Serial.print("Local ESP32 IP: ");
    //Serial.println(WiFi.localIP());

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
    tft.setCursor(0,0);
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(3);

    uint8_t timeout_counter = 0;

    // Gets the latest weather data
    queryWeather();

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
        endFormat = " PM";
    } else if (atoi(timeHour) < 12) {
        endFormat = " AM";
    } else {
        endFormat = " PM";
    }

    // Content
    tft.println("");
    tft.println("Athens, GA");
    tft.print(&timeinfo, "%I");
    tft.print(":");
    tft.print(&timeinfo, "%M");
    tft.println(endFormat);
    tft.println("");
    tft.println("Temperature: ");
    tft.println("Humidity: ");

    delay(60000);

}
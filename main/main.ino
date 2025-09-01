#include <WiFi.h>
#include <TFT_eSPI.h>

// Defines constants
#define CONNECTION_TIMEOUT 10

// Add your wifi information here
const char* ssid = "";
const char* password = "";

TFT_eSPI tft = TFT_eSPI();


void setup() {
    // Creates serial monitor page
    Serial.begin(115200);
    delay(1000);

    // Initializes the tft screen
    tft.begin();
    tft.setRotation(1);
    delay(1000);

    // Initializes the wifi
    WiFi.mode(WIFI_STA); //Optional
    WiFi.begin(ssid, password);
    Serial.println("\nConnecting");
    int timeout_counter = 0;

    // Waits until connected
    while(WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(200);
        timeout_counter++;

        // If there is an issue connecting, the ESP restarts
        if(timeout_counter >= CONNECTION_TIMEOUT*5){
            ESP.restart();
        }
    }

    Serial.println("\nConnected to the WiFi network");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());

    // Sets the screen color
    tft.setTextColor(TFT_WHITE, TFT_BLUE, true); 
    tft.println("Hello World");
}

void loop() {


}
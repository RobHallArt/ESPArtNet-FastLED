// Based on https://github.com/rstephan/ArtnetWifi/tree/master/examples/ArtnetWifiFastLED
// WS2812 LEDs via the FastLED library: https://github.com/FastLED/FastLED

#if defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <WiFiUdp.h>
#include <ArtnetWifi.h>
#include <FastLED.h>

// Wifi settings
const char* ssid = "showAP";
const char* password = "4000HZ-AP";

// LED settings
const int numLeds = 120; // Set Number of LEDs which need to be controlled
const int numberOfChannels = numLeds * 3; // Total number of channels you want to receive (1 led = 3 channels)
const byte dataPin = 2;
CRGB leds[numLeds];

// Art-Net settings
ArtnetWifi artnet;
const int startUniverse = 0;
const int modeChannel = 7; // Set channel to be listened to.

// Check if we got all universes
const int maxUniverses = numberOfChannels / 512 + ((numberOfChannels % 512) ? 1 : 0);
bool universesReceived[maxUniverses];
bool sendFrame = 1;
int previousDataLength = 0;

// Initialise int to track mode we are in.
int currentMode;


// connect to wifi – returns true if successful or false if not
boolean ConnectWifi(void)
{
  boolean state = true;
  int i = 0;

  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 20){
      state = false;
      break;
    }
    i++;
  }
  // Print different messages to serial output depending on connection success.
  if (state){
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("");
    Serial.println("Connection failed.");
  }

  return state;
}

// Flash LEDs different colours to indicate successful connection to network.
void initTest()
{
  for (int i = 0 ; i < numLeds ; i++) {
    leds[i] = CRGB(127, 0, 0);
  }
  FastLED.show();
  delay(500);
  for (int i = 0 ; i < numLeds ; i++) {
    leds[i] = CRGB(0, 127, 0);
  }
  FastLED.show();
  delay(500);
  for (int i = 0 ; i < numLeds ; i++) {
    leds[i] = CRGB(0, 0, 127);
  }
  FastLED.show();
  delay(500);
  for (int i = 0 ; i < numLeds ; i++) {
    leds[i] = CRGB(0, 0, 0);
  }
  FastLED.show();
}

// Fade LEDs in a pattern simmilar to that on the Amazon Alexa device.
void alexaStyle(){
  // Loop Through LEDs
  for (int i = 0 ; i < numLeds ; i++) {
    // Calculate brightness of each LED based on time elapsed an LED position.
    leds[i] = CRGB(0, 0, (sin((millis()+(i*60))*0.003)+1)*110);
  }
  FastLED.show();
  
}
// Turn all LEDs on and off for a strobe effect.
void strobeWhite(){
  // Turn on all LEDs
  for (int i = 0 ; i < numLeds ; i++) {
    leds[i] = CRGB(255,255,255);
  }
  FastLED.show();
  // Wait 30ms
  delay(30);
  // Turn them all off
  for (int i = 0 ; i < numLeds ; i++) {
    leds[i] = CRGB(0,0,0);
  }
  FastLED.show();
  // Wait 30ms
  delay(30);
  
}

// Runs every time a DMX packet is revieved
void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
{
  // For debugging print the values recieved on the specified channel to serial.
  Serial.print(" Value : ");
  Serial.print(data[modeChannel]);
  Serial.println();

  // Set which lighting mode to use based on DMX value of specified channel.
  if(data[modeChannel]<30){
    currentMode = 0;
  }

  if(data[modeChannel]>30 && data[modeChannel]<60){
    currentMode = 1;
  }

  if(data[modeChannel]>60 && data[modeChannel]<90){
    currentMode = 2;
  }

  if(data[modeChannel]>90 && data[modeChannel]<120){
    currentMode = 3;
  }

  if(data[modeChannel]>120 && data[modeChannel]<160){
    currentMode = 4;
  }

}


void setup()
{
  Serial.begin(115200);
  ConnectWifi();
  artnet.begin();
  FastLED.addLeds<WS2812, dataPin, GRB>(leds, numLeds);
  initTest();

  // This will be called for each packet received
  artnet.setArtDmxCallback(onDmxFrame);
}

void loop()
{
  // We call the read function inside the loop to check for new packets.
  artnet.read();

  // Set of if statements to decide which data to send to the LEDs.
  if(currentMode == 0){
    for (int i = 0 ; i < numLeds ; i++) {
      // All Off
      leds[i] = CRGB(0, 0, 0);
    }
    FastLED.show();
  }

  if(currentMode == 1){
    alexaStyle();
  }

  if(currentMode == 2){
    for (int i = 0 ; i < numLeds ; i++) {
      // All Red
      leds[i] = CRGB(255, 0, 0);
    }
    FastLED.show();
  }

  if(currentMode == 3){
    for (int i = 0 ; i < numLeds ; i++) {
      // All Green
      leds[i] = CRGB(0, 255, 0);
    }
    FastLED.show();
  }

  if(currentMode == 4){
    strobeWhite();
  }
}

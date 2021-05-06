/*  Digital Alarmclock Project
 *  
 *  Gets the current weather and time of Houston, Texas
 *  Also allows for functionality for an Alarm Clock. Currently must be hard-coded
 *  
 *  Sometimes the NTP server refreshes and you get a different server & time. Edit the value in calculateTime() to fix this
 *  
 *  Board used: Arduino Uno Wifi Rev 2
 *  Parts used: LCD1602, Full Breadboard, Potentiometer 10K, Passive Buzzer
 * 
 *  This was done as a project for ELET 2303 (Digital Systems)
 *  
 *  Time Service Used: Network Time Protocol
 *  Weather Web Service Used: https://openweathermap.org/
 *  
 *  Setup
 *    Wire up the board correctly
 *    Configure ssid and pass with your wifi settings
 *    Set alarm time if needed in loop()
 *    Configure NTP settings if needed in calculateTime()
 *    Get API key from openweathermap and paste it in httpRequest()
*/

// Wifi modules needed
#include <WiFiNINA.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// LCD display modules
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

// Wifi info
char ssid[] = ""; //WiFi name
char pass[] = ""; //WiFi password

// Other wifi variable initialization
int status = WL_IDLE_STATUS;
unsigned long lastConnectionTime = 0;            // last time you are connected to the server, in milliseconds
const unsigned long postingInterval = 30L * 1000L; // delay between updates, in milliseconds

// NTP stuff for time calculation
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Initialize the Wifi client library
WiFiSSLClient client;

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int Second; 
int Minute; 
int Hour; 
int BuzzerID;
float weatherTemperature;

// Connect to wifi, get time from NTP server, and assign buzzer id
void setup(){
  
  Serial.begin(9600);

  // Wifi setup
  while(status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.print(ssid);
    Serial.print(" ");
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }

  String ssid = WiFi.SSID();
  IPAddress ip = WiFi.localIP();
  IPAddress subnet = WiFi.subnetMask();
  IPAddress gateway = WiFi.gatewayIP();


  Serial.print("SSID: ");
  Serial.println(ssid);

  Serial.print("IP: ");
  Serial.println(ip);


  // Time setup
  timeClient.update();
  Serial.println(timeClient.getFormattedTime());
  Second = timeClient.getSeconds(); 
  Minute = timeClient.getMinutes(); 
  Hour = timeClient.getHours();

  // Digital Port which the buzzer is attached to
  BuzzerID = 8;

  pinMode(BuzzerID, OUTPUT);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("hello, world!");

  Serial.println("Ready");
  Serial.println("");

}

// Clear screen, get time, connect to weather and get temp, check alarm, and write to screen
void loop(){
  lcd.clear();
  lcd.setCursor(4, 0);
  calculateTime();
  lcd.setCursor(0, 1);
  if (millis() - lastConnectionTime > postingInterval) {
    httpRequest();
  }

  lcd.print("Temp: ");
  lcd.print(weatherTemperature);
  lcd.print(" F");

  // Not enought space on the lcd :(
  //lcd.print(" Run-time:");
  //lcd.print(millis() / 1000);
  
  //Alarm Time
  //These values are currently hardcoded
  if(Hour== 10 && Minute==10){
    BuzzerOn();
  }
  delay(1000);
  if(Hour== 10 && Minute==10){
    BuzzerOff();
  }
}

void calculateTime(){
  Second = timeClient.getSeconds(); 
  Minute = timeClient.getMinutes(); 
  Hour = timeClient.getHours();

  Hour -= 2; // I subtracted x hours because my ntp was off x hours
  if(Hour < 0){
    Hour = 24 + Hour; //(Because it'd be negative)
  }
  if(Hour < 10){
    lcd.print("0");
    lcd.print(Hour);
  } else {
    lcd.print(Hour);
  }
  lcd.print(":");
  if(Minute < 10){
    lcd.print("0");
    lcd.print(Minute);
  } else {
    lcd.print(Minute);
  }
  lcd.print(":");
  if(Second < 10){
    lcd.print("0");
    lcd.print(Second);
  } else {
  lcd.print(Second);
  }

  // Had some manual time calculation but removed it due to ntp service

}

void BuzzerOn(){
  digitalWrite(BuzzerID, HIGH);
}
void BuzzerOff(){
  digitalWrite(BuzzerID, LOW);
}

void httpRequest(){
  // if there's a successful connection:
  if (client.connect("api.openweathermap.org", 443)) {
    Serial.println("connecting...");
    // send the HTTP PUT request:
    client.println("GET /data/2.5/weather?q=houston,us&units=imperial&APPID=<API KEY> HTTP/1.1"); // Need to set API key here!
    client.println("Host: api.openweathermap.org");
    client.println("Connection: close");
    client.println();

    // Check HTTP status
    char status[32] = {0};
    client.readBytesUntil('\r', status, sizeof(status));
    // It should be "HTTP/1.0 200 OK" or "HTTP/1.1 200 OK"
    if (strcmp(status + 9, "200 OK") != 0) {
      Serial.print(F("Unexpected response: "));
      Serial.println(status);
      return;
    }

    // Skip HTTP headers
    char endOfHeaders[] = "\r\n\r\n";
    if (!client.find(endOfHeaders)) {
      Serial.println(F("Invalid response"));
      return;
    }

    // Allocate the JSON document
    // Use arduinojson.org/v6/assistant to compute the capacity.
    const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(0) + 2*JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + 2*JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(14) + 280;
    DynamicJsonDocument doc(capacity);
    
    // Parse JSON object
    DeserializationError error = deserializeJson(doc, client);
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      return;
    }
        
    int weatherId = doc["weather"][0]["id"].as<int>();
    weatherTemperature = doc["main"]["temp"].as<float>();
    int weatherHumidity = doc["main"]["humidity"].as<int>();
    
    //Disconnect
    client.stop();
    
    Serial.println(F("Response:"));
    Serial.print("Weather: ");
    Serial.println(weatherId);
    Serial.print("Temperature: ");
    Serial.println(weatherTemperature);
    Serial.print("Humidity: ");
    Serial.println(weatherHumidity);
    Serial.println();

    // note the time that the connection was made:
    lastConnectionTime = millis();
   }else{
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }
    
}

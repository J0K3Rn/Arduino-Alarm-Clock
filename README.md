# Arduino Alarm Clock
  
Gets the current weather and time of Houston, Texas
Also allows for functionality for an Alarm Clock. Currently must be hard-coded
 
Board used: Arduino Uno Wifi Rev 2
Parts used: LCD1602, Full Breadboard, Potentiometer 10K, Passive Buzzer
  
Time Service Used: Network Time Protocol
Weather Web Service Used: https://openweathermap.org/
 
Setup
 - Wire up the board correctly
 - Configure ssid and pass with your wifi settings
 - Set alarm time if needed in loop()
 - Configure NTP settings if needed in calculateTime()
 - Get API key from openweathermap and paste it in httpRequest(). You can also adjust the city if needed
 
 Alarm Clock in use:
 ![alt text](https://github.com/J0K3Rn/Arduino-Alarm-Clock/blob/main/example.jpg?raw=true)
# Arduino Alarm Clock

Features:
 - Gets the current weather and time of Houston, Texas
 - Uses Network Time Protocol (NTP) to get the current time
 - Uses a buzzer for a alarm. Alarm time currently must be hard-coded
 - Writes time & temperature to LCD screen. More information could be presented, but need more space
 - Potentiometer acts as a bightness adjustment to the LCD screen

Board used: Arduino Uno Wifi Rev 2
Parts used: LCD1602, Potentiometer 10K, Active Buzzer

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
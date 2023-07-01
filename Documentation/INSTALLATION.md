# Cryologger Automatic Weather Station (AWS) - Installation
This guide provides step-by-step instructions on installing the Arduino IDE and required board definitions and libraries, which are necessary for uploading code to the Cryologger AWS.

## Step 1: Download Arduino IDE  
* Navigate to https://www.arduino.cc/en/software and download the most recent version of the Arduino IDE

![image](https://github.com/adamgarbo/cryologger-ice-tracking-beacon/assets/22924092/323c74a1-2ce1-4e39-a8eb-d39ad760a9a6)

## Step 2: Add Additional Boards Manager URL:
* In the Arduino IDE navigate to: Preferences
* Add the following "Additional Boards Manager URL" as shown in the screenshot below:  
```https://adafruit.github.io/arduino-board-index/package_adafruit_index.json```
* Also during this step, check the "compile" and "upload" boxes for "Show verbose output during" and change "Compiler warnings" to "All"
![image](https://github.com/adamgarbo/cryologger-ice-tracking-beacon/assets/22924092/f0db6767-526f-45cb-b0a7-c750295eeb83)

## Step 3: Install Board Definitions
* Navigate to: Tools > Boards > Boards Manager
* Search for: SAMD Boards
* Select and install version: Arduino SAMD Boards v1.8.13 
* Select and install version: Adafruit SAMD Boards v1.7.13
![image](https://github.com/adamgarbo/cryologger-ice-tracking-beacon/assets/22924092/f4d9af80-c4ba-4fab-958b-c35e1e4ac1e3)

## Step 4: Install Libraries
Several libraries are required by the Cryologger ice tracking beacons. These can be downloaded using the Arduino IDE's Library Manager (requires an Internet connection).

* Navigate to: Tools > Manage Libraries
* Search for and install the following libraries:

**Table 1.** Libraries required by Cryologger AWS. Last updated 2023-07-01.
| Library                                         | Version | GitHub Repository                                                   |
|-------------------------------------------------|:-------:|---------------------------------------------------------------------|
| Adafruit BME280 Library                         |  2.2.2  | https://github.com/adafruit/Adafruit_BME280                         |
| Adafruit LSM303 Accelerometer Library           |  1.1.6  | https://github.com/adafruit/Adafruit_LSM303_Accel                   |
| Adafruit Unified Sensor Driver                  |  1.1.9  | https://github.com/adafruit/Adafruit_Sensor                         |
| Arduino Low Power Library                       |  1.2.2  | https://github.com/arduino-libraries/ArduinoLowPower                |
| SparkFun IridiumSBD I2C Arduino Library         |  3.0.6  | https://github.com/sparkfun/SparkFun_IridiumSBD_I2C_Arduino_Library |
| RTCZero Library                                 |  1.6.0  | https://github.com/arduino-libraries/RTCZero                        |
| SdFat                                           |  2.2.2  | https://github.com/greiman/SdFat                                    |
| Sensirion Temperature / Humidity Sensor Library |         | https://github.com/HydroSense/sensirion                             |
| Statistic Library                               |  1.0.4  | https://github.com/RobTillaart/Statistic                            |
| Arduino Time Library                            |  1.6.1  | https://github.com/PaulStoffregen/Time                              |
| TinyGPSPlus                                     |  1.0.3  | https://github.com/mikalhart/TinyGPSPlus                            |

![image](https://github.com/adamgarbo/cryologger-ice-tracking-beacon/assets/22924092/dadb37fe-46c7-48af-ad3d-cbb4061d01a6)

## Step 5: Download Software
Code for the Cryologger AWS is made available on the following GitHub repository:
* [https://github.com/adamgarbo/Cryologger Automatic Weather Station](https://github.com/adamgarbo/Cryologger_Automatic_Weather_Station)
* Click on "Releases" and download the v1.2.0 .zip file:
![image](https://github.com/adamgarbo/cryologger-automatic-weather-station/assets/22924092/7f80eec5-b06e-466e-aa5a-90dcd1c81258)

## Step 6: Test Program Compilation
* Navigate to the /Software/Arduino/cryologger_aws folder of the downloaded repository
* Double click on the `cryologger_itb.aws` file
* Click on the checkmark in the upper left corner of the Arduino IDE program window
* Watch debugging window for compilation errors
![image](https://github.com/adamgarbo/cryologger-automatic-weather-station/assets/22924092/99570272-b388-4752-a01e-235f2eb6a84b)

## Step 7: Connect Hardware
* Connect to the Cryologger ITB using a micro USB cable.
* In the Arduino IDE click on "Select Board" and then "Adafruit Feather M0 (SAMD)
![image](https://github.com/adamgarbo/cryologger-ice-tracking-beacon/assets/22924092/0199d2f2-ca16-42ae-bb7f-c8cd82348479)
* If the board is not auto-populated, click on "Select other board and port..." and search for "Adafruit Feather M0":
* Be sure to select the appropriate serial port that is connected to the Arduino
![image](https://github.com/adamgarbo/cryologger-ice-tracking-beacon/assets/22924092/dc0b3bcc-e7c9-4635-941f-46600e63a128)

## Step 8: Upload Program
* Once the code has compiled successfully, click on the right-pointed arrow to upload the code
* Watch the output window for compilation errors and/or success messages
* If no errors are presented, the code has now been successfully uploaded! 
![image](https://github.com/adamgarbo/cryologger-automatic-weather-station/assets/22924092/7dc17bf4-c6a8-4e82-bd01-af80899bb6d0)

## Step 9: Next Steps
* The detailed operation of the Cryologger AWS will be covered next in OPERATION.md

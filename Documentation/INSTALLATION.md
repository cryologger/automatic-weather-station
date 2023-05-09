# Cryologger - Automatic Weather Station Configuration

## Step 1: Download Arduino IDE
Download the latest release of the Arduino IDE v1.8.x from the following URL: 
* https://www.arduino.cc/en/software
* Note: v2.x of the IDE is highly unreliable. Download at your own risk.

## Step 2: Add Additional Boards Manager URL:
* In the Arduino IDE navigate to: Preferences
* Add the following Additional Boards Manager URL as shown in the sceenshot below:
  * `https://adafruit.github.io/arduino-board-index/package_adafruit_index.json`
<img width="720" alt="image" src="https://user-images.githubusercontent.com/22924092/178781840-8e82d6f8-babb-4437-b90f-de9070d21310.png">

## Step 3: Install Board Definition
* Navigate to: Tools > Boards > Boards Manager
* Search for: Adafruit SAMD Boards
* Select and install the following two board definitions:
  * Arduino SAMD Boards v1.8.13
  * Adafruit SAMD Boards v1.7.11
<img width="720" alt="image" src="https://user-images.githubusercontent.com/22924092/178782962-b1719459-208c-4823-9fa6-06f69197af2a.png">

## Step 4: Install Libraries
The Cryologger automatic weather stations requires several external libraries. These can either be downloaded using the Arduino IDE's Library Manager (requires Internet connection), or directly from GitHub.

* Navigate to: Tools > Manage Libraries
* Search for and install the following libraries:

| Library                                         | Version | GitHub URL                                                          | Comments                       |
|-------------------------------------------------|---------|---------------------------------------------------------------------|--------------------------------|
| Adafruit BME280 Library                         | 2.2.2   | https://github.com/adafruit/Adafruit_BME280                         |                                |
| Adafruit LSM303 Accelerometer Library           | 1.1.4   | https://github.com/adafruit/Adafruit_LSM303_Accel                   |                                |
| Adafruit Unified Sensor Driver                  | 1.1.9   | https://github.com/adafruit/Adafruit_Sensor                         |                                |
| Arduino Low Power Library                       | 1.2.2   | https://github.com/arduino-libraries/ArduinoLowPower                |                                |
| SparkFun IridiumSBD I2C Arduino Library         | 3.0.5   | https://github.com/sparkfun/SparkFun_IridiumSBD_I2C_Arduino_Library |                                |
| RTCZero Library                                 | 1.6.0   | https://github.com/arduino-libraries/RTCZero                        |                                |
| SdFat                                           | 2.2.0   | https://github.com/greiman/SdFat                                    |                                |
| Sensirion Temperature / Humidity Sensor Library |         | https://github.com/HydroSense/sensirion                             | Download from GitHub           |
| Statistic Library                               | 1.0.0   | https://github.com/RobTillaart/Statistic                            |                                |
| Arduino Time Library                            | 1.6.1   | https://github.com/PaulStoffregen/Time                              |                                |
| TinyGPSPlus                                     | 1.0.3   | https://github.com/mikalhart/TinyGPSPlus                            |                                |

## Step 5: Select Harware & Configure Port Settings
*  Navigate to Tools > Board > Adafruit SAMD Boards

<img width="377" alt="image" src="https://user-images.githubusercontent.com/22924092/178784887-24cdacb0-873f-40f0-aaa0-9549235a5816.png"><img width="503" alt="image" src="https://user-images.githubusercontent.com/22924092/178784943-1ce56d81-3cba-4b58-93e5-b5df031debfc.png">

*  Select Adafruit Feather M0
<img width="366" alt="image" src="https://user-images.githubusercontent.com/22924092/178784990-380931f8-7d47-4fb0-bde5-7b0f8bc7f536.png">

## Step 6: Download Automatic Weather Station Program
Code for the Cryologger AWS is made available on the following GitHubrepository:
* [https://github.com/adamgarbo/Cryologger Automatic Weather Station](https://github.com/adamgarbo/Cryologger_Automatic_Weather_Station)
* Click on "Releases" and download the v1.0.0 .zip file:

## Step 7: Test Program Compiliation
* Navigate to the /Software/cryologger_aws folder of the downloaded repository
* Double click on the `cryologger_aws.ino` file
* Click on the checkmark in the upper left corner of the Arduino IDE program window
* Watch debugging window for compilation errors
<img width="720" alt="image" src="https://github.com/adamgarbo/cryologger-automatic-weather-station/assets/22924092/b86e3e6c-f294-46a8-9646-5754488ec4a9.png">

## Step 7: Upload Program
* Once the code has compiled successfully, connect the Adafruit Feather M0 Adalogger via micro USB cable. 
* The board should appear under "Port" as a USB or COM device
* Make any necessary changes to the code
* Click on the right pointed arrow to upload the code
* Watch debugging window for compilation errors and/or success messages

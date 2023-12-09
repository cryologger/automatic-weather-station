<p align="left">
<img alt="GitHub" src="https://img.shields.io/github/license/adamgarbo/Cryologger_Automatic_Weather_Station">
<img alt="GitHub release (latest by date)" src="https://img.shields.io/github/v/release/adamgarbo/Cryologger_Automatic_Weather_Station">
<img alt="GitHub issues" src="https://img.shields.io/github/issues/adamgarbo/Cryologger_Automatic_Weather_Station">
</p>

# Cryologger - Automatic Weather Station (AWS)

### Note: This project is under active development.

![DSC_7941](https://github.com/cryologger/automatic-weather-station/assets/22924092/5b3acadf-4ccf-4cc4-9847-c8cd8aa003ba)

## 1.0 Introduction
Polar environments are undergoing rapid environmental change, yet there is a lack of high spatial and temporal monitoring data available in these remote regions. Reliance on expensive and proprietary commercial data acquisition and telemetry systems can inhibit both researchers and citizen scientists and present a significant barrier to the establishment of widespread monitoring networks. The development of low-cost, open-source instrumentation can greatly reduce the cost of cryospheric research, improve the spatial density and coverage of collected data, and produce new ways to observe and monitor the cryosphere.

The Cryologger Automatic Weather Station (AWS), is an open-source datalogger and telemeter comprised of low-cost, off the shelf components that is capable of supporting the physical measurement needs of a variety of cryospheric scientific applications when configured as an automatic weather station (AWS).

## 2.0 Methods

### 2.1 Design
The Cryologger AWS is based on the open-source Arduino platform (www.arduino.cc) and built using low-cost, do-it-yourself electronics that can be easily modified to meet the needs of the end-user. Code was written using the Arduino Integrated Development Environment (IDE) and benefits from the availability of community-generated libraries.

Planned for extended deployments in harsh Arctic conditions, the Cryologger AWS provides long-term measurements of weather variables such as temperature, humidity, wind speed, wind direction, solar irradiance and snow depth, as well as system diagnostic variables including pitch, roll, battery voltage. Data are transmitted over the Iridium satellite network at specified intervals and can be remotely updated based on the desired sampling frequency. Collected data are made available in near-real time at https://cryologger.org.

<img src="https://user-images.githubusercontent.com/22924092/224180959-e6673df1-275e-406b-9564-ec4f626f1293.png" width="720">

#### 2.1.1. Bill of Materials (BOM)

For a complete bill of materials, including requirements for power and mounting of a complete deployable system, please see ASSEMBLY.md.

**Table 1.** Bill of materials and associated costs for the main components used in the Cryologger AWS v1.0. Prices are listed in USD and are current as of December 2023. Taxes and shipping not included. <sup>1</sup>Denotes optional component.
| Component             | Product                                                                                                                      | Quantity | Cost (USD) |
|-----------------------|------------------------------------------------------------------------------------------------------------------------------|:--------:|:----------:|
| PCB                   | [Custom Cryologger Printed Circuit Board](https://jlcpcb.com)                                                                |     1    |    $5.00   |
| Satellite transceiver | [Rock7 RockBLOCK 9603](http://www.rock7mobile.com/products-rockblock-9603)                                                   |     1    |   $267.50  |
| Satellite antenna     | [Maxtena M1621HCT-P-SMA](https://maxtena.com/products/f-passive/m1621hct-p-sma-iridium-passive-antenna/)                     |     1    |   $54.00   |
| Processor             | [Adafruit Feather M0 Adalogger](https://www.adafruit.com/product/2796)                                                       |     1    |   $19.95   |
| GNSS                  | [Adafruit Ultimate GPS FeatherWing](https://www.adafruit.com/product/3133)                                                   |     1    |   $24.95   |
| IMU                   | [Adafruit LSM303AGR Accelerometer Magnetometer](https://www.adafruit.com/product/4413)                                       |     1    |   $12.50   |
| Sensor                | [Adafruit BME280 Temperature Humidity Pressure Sensor ](https://www.adafruit.com/product/2652)                               |     1    |   $14.95   |
| Voltage regulator     | [Pololu D36V6F3 3.3V 600mA Step-Down Voltage Regulator](https://www.pololu.com/product/3791)                                 |     1    |   $16.95   |
| Voltage regulator     | [Pololu D36V6F5 5V 600mA Step-Down Voltage Regulator](https://www.pololu.com/product/3792)                                   |     1    |   $16.95   |
| Voltage regulator     | [Pololu U3V40F12 12V Step-Up Voltage Regulator ](https://www.pololu.com/product/4016)                                        |     1    |    $9.95   |
| Resistor              | [10 M 1% 0.6 W resistor](https://www.mouser.ca/ProductDetail/594-MBB02070C1005FCT)                                           |     1    |    $0.44   |
| Resistor              | [1 M 1% 0.6 W resistor](https://www.mouser.ca/ProductDetail/594-B0207C1M000F5T)                                              |     1    |    $0.40   |
| Capacitor             | [0.1 uF Capacitor](https://www.mouser.ca/ProductDetail/Vishay-BC-Components/K104K15X7RF53L2?qs=mWFvmKOfYW8KbAXlf9eSQA%3D%3D) |     1    |    $0.29   |
| Power connector       |                                                                                                                              |     1    |            |
| Power connector       |                                                                                                                              |     1    |            |
| Connector             |                                                                                                                              |     1    |            |

**Table 2.** Environmental sensors used in Cryologger AWS v1.0. Prices are listed in USD and are current as of December 2023. Taxes and shipping not included.
| Environment Variable          | Sensor                                                                                                                       | Cost (USD) |
|-------------------------------|------------------------------------------------------------------------------------------------------------------------------|:----------:|
| Wind Speed & Direction        | [RM Young Wind Monitor 5103L Anemometer](https://www.youngusa.com/product/wind-monitor/)                                     | $1432      |
| Temperature/Relative Humidity | [Vaisala HMP60 Humidity and Temperature Probe](https://store.vaisala.com/en/products/HMP60)                                  | $330       |
| Solar Irradiance              | [SP-212-SS: Amplified 0-2.5 Volt Pyranometer](https://www.apogeeinstruments.com/sp-212-ss-amplified-0-2-5-volt-pyranometer/) | $325       |
| Snow Depth                    | [Maxboxtix MB7374 HRXL-MaxSonar-WRST7](https://maxbotix.com/products/mb7374)                                                 | $290       |


### 2.2 Operation
When initially powered on, the Cryologger AWS will attempt to establish a GNSS fix and sychronize the real-time clock (RTC) for up to 5 minutes. Once the RTC is synchronized, the system will set an alarm to wake at the user-specified time and then enter a low-power deep sleep. Nominally, the system will wake every 5 minutes, obtain measurements from all sensors, and log data to a microSD card. Measurements are averaged each hour and transmitted  

While the system is in deep sleep, a Watchdog Timer (WDT) will wake the system to check the program has not frozen every 8 seconds and perform a reset, if necessary. This helps to ensure the long-term reliability of the system.

#### 2.2.1 Measurements

**Table 2.** List of Cryologger AWS v1.0 data variables that can be transmitted and/or logged with variable sizes.

| Type     | Variable          | Description                        | Size (bytes) |
|----------|-------------------|------------------------------------|:------------:|
| uint32_t | unixtime          | UNIX Epoch time                    |       4      |
| int16_t  | temperatureInt    | Internal temperature (°C)          |       2      |
| uint16_t | humidityInt       | Internal humidity (%)              |       2      |
| uint16_t | pressureInt       | Internal pressure (hPa)            |       2      |
| int16_t  | temperatureExt    | External temperature (°C)          |       2      |
| uint16_t | humidityExt       | External humidity (%)              |       2      |
| int16_t  | pitch             | Pitch (°)                          |       2      |
| int16_t  | roll              | Roll (°)                           |       2      |
| uint16_t | solar             | Solar irradiance (W m-2)           |       2      |
| uint16_t | windSpeed         | Mean wind speed (m/s)              |       2      |
| uint16_t | windDirection     | Mean wind direction (°)            |       2      |
| uint16_t | windGustSpeed     | Wind gust speed (m/s)              |       2      |
| uint16_t | windGustDirection | Wind gust direction (°)            |       2      |
| int32_t  | latitude          | Latitude (DD)                      |       4      |
| int32_t  | longitude         | Longitude (DD)                     |       4      |
| uint8_t  | satellites        | Number of satellites               |       1      |
| uint16_t | hdop              | HDOP                               |       2      |
| uint16_t | voltage           | Battery voltage (V)                |       2      |
| uint16_t | transmitDuration  | Previous transmission duration (s) |       2      |
| uint8_t  | transmitStatus    | Iridium return code                |       1      |
| uint16_t | iterationCounter  | Message counter                    |       2      |

#### 2.2.2 Data Transmission and Processing

Sensor measurements and GNSS position are recorded hourly and stored in memory until the desired transmission interval is reached. Data are compressed into a binary message (340 bytes maximum) to minimize the cost and total number of transmissions required. Data are transmitted via the Iridium Short Burst Data (SBD) satellite network at user-specified intervals, which can be remotely updated based on the desired sampling frequency. SBD data messages are received by an Iridium ground station and sent to Ground Control's server. The data is then forwarded to an Amazon Web Services (AWS) SQS queue, decoded using an AWS Lambda Python function and stored in a database using the Amazon Relational Database Service (RDS). Data can be viewed in near-real time at https://cryologger.org.

## 3.0 Deployments

<p align="center"><img src="https://github.com/cryologger/automatic-weather-station/assets/22924092/1f591af9-fa7d-48b0-97dc-aee63c6f5255" width="640"></p>
<p align="center"><b>Figure 3.</b> Map of Cryologger AWS deployments made between 2019 and 2023.</p>

## 4.0 Results


## 5.0 Conclusion


## Repository Contents

* **/Documentation** - Assembly, deployment and troubleshoting guides, as well as information of components used and associated costs.

* **/Hardware** - KiCad PCB schematic and design files.

* **/Software** - Arduino code and Python data analysis scripts.

## License Information
This project is released under the GNU General Public License v3.0 (https://www.gnu.org/licenses/gpl-3.0.en.html).

Cheers,

**Adam**

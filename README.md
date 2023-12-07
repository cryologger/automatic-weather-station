<p align="left">
<img alt="GitHub" src="https://img.shields.io/github/license/adamgarbo/Cryologger_Automatic_Weather_Station">
<img alt="GitHub release (latest by date)" src="https://img.shields.io/github/v/release/adamgarbo/Cryologger_Automatic_Weather_Station">
<img alt="GitHub issues" src="https://img.shields.io/github/issues/adamgarbo/Cryologger_Automatic_Weather_Station">
</p>

# Cryologger - Automatic Weather Station (AWS)

![DSC_7941](https://github.com/cryologger/automatic-weather-station/assets/22924092/5b3acadf-4ccf-4cc4-9847-c8cd8aa003ba)

## Background
Polar environments are undergoing rapid environmental change, yet there is a lack of high spatial and temporal monitoring data available in these remote regions. Reliance on expensive and proprietary commercial data acquisition and telemetry systems can inhibit both researchers and citizen scientists and present a significant barrier to the establishment of widespread monitoring networks. 

The development of low-cost, open-source instrumentation can greatly reduce the cost of cryospheric research, improve the spatial density and coverage of collected data, and produce new ways to observe and monitor the cryosphere.

## Objective
The goal of this project is to determine if the Cryologger, an open-source datalogger and telemeter comprised of low-cost, off the shelf components, is capable of supporting the physical measurement needs of a variety of cryospheric scientific applications when configured as an automatic weather station (AWS).

## Design
The Cryologger design is based on the open-source Arduino platform (www.arduino.cc) and built using easy-to-use, do-it-yourself electronics. Code was written using the open-source Arduino integrated development environment (IDE) and compiled from community generated libraries. Programming logic is highly focused on power optimization.

<img width="720" alt="image" src="https://user-images.githubusercontent.com/22924092/224180959-e6673df1-275e-406b-9564-ec4f626f1293.png">

### Bill of Materials

**Table 1.** Bill of materials used in automatic weather station deployments.
| Component             | Product                                                               | Cost (USD) |
|-----------------------|-----------------------------------------------------------| :---: |
| Satellite transceiver | [Rock7 RockBLOCK 9603](http://www.rock7mobile.com/products-rockblock-9603) | $267.50
| Satellite antenna*    | [Maxtena M1621HCT-P-SMA](https://maxtena.com/products/f-passive/m1621hct-p-sma-iridium-passive-antenna/) | $50.00 
| Processor             | [Adafruit Feather M0 Adalogger](https://www.adafruit.com/product/2796) | $19.95
| GNSS                  | [Adafruit Ultimate GPS FeatherWing](https://www.adafruit.com/product/3133) | $24.95
| IMU                   | [Adafruit LSM303AGR Accelerometer Magnetometer](https://www.adafruit.com/product/4413) | $12.50
| Sensor                | [Adafruit BME280 Temperature Humidity Pressure Sensor ](https://www.adafruit.com/product/2652) | $14.95
| Voltage regulator     | [Pololu D36V6F3 3.3V 600mA Step-Down Voltage Regulator](https://www.pololu.com/product/3791) | $16.95 
| Voltage regulator     | [Pololu D36V6F5 5V 600mA Step-Down Voltage Regulator](https://www.pololu.com/product/3792) | $16.95 
| Voltage regulator     | [Pololu U3V40F12 12V Step-Up Voltage Regulator ](https://www.pololu.com/product/4016) | $9.95 
| Resistor              | [10 M 1% 0.6 W resistor](https://www.mouser.ca/ProductDetail/594-MBB02070C1005FCT) | $0.44 |
| Resistor              | [1 M 1% 0.6 W resistor](https://www.mouser.ca/ProductDetail/594-B0207C1M000F5T) | $0.40 
| Capacitor             | [0.1 uF Capacitor](https://www.mouser.ca/ProductDetail/Vishay-BC-Components/K104K15X7RF53L2?qs=mWFvmKOfYW8KbAXlf9eSQA%3D%3D) | $0.29 
| Power connector       |	[Phoenix Contact MSTB 2,5/ 2-ST-5,08 - 1757019](https://www.mouser.ca/ProductDetail/Phoenix-Contact/1757242?qs=%2Fha2pyFadugVjodGKkrF4xNq%252BZEVHysqCHlL2cTnJ%252B8%3D) | $0.67 
| Power connector	      | [Phoenix Contact MSTBA 2,5/ 2-G-5,08 - 1757242](https://www.mouser.ca/ProductDetail/Phoenix-Contact/1757019?qs=sGAEpiMZZMvlX3nhDDO4AGmxTE5dWGQY3FmaBdjJUN0%3D) | $2.08 
| Connector             |	[Molex PicoBlade PCB Header](https://www.mouser.ca/datasheet/2/276/0530471010_PCB_HEADERS-171035.pdf) | $0.50 

**Table x.** Power and mounting bill of materials
| Component           | Product | Cost (USD) |
|---------------------|-------- | :---: |
| Solar Panel         | [Ameresco Solar 20 Watt Solar Panel 20J](https://www.amerescosolar.com/20j-ameresco-solar-20-watt-solar-panel) | $99 |
| Solar Panel Bracket | [Ameresco Solar HPM18-30 Solar Panel Pole Mount (optional)](https://www.amerescosolar.com/hpm18-30solar-panel-pole-mount) | $ |
| Charge Controller   | [Genasun GV-4 50 W 4 A Solar charge controller with MPPT](https://sunforgellc.com/gv-4/) | $63 |
| Battery             | [Deka SOLAR Gel 36 Ah 8GUIH](https://www.amerescosolar.com/deka-solar-8gu1-deka-gel-deep-cycle-battery-12v-36-ah) | $ |
| Enclosure           | [nVent Hoffman HJ1412HWPL2LG](https://hoffman.nvent.com/en-us/products/enchj1412hwpl2lg) | $ |
| Pole Mount          | L-Com Enclosure Pole Mounting Kit | $ |
| Tripod              | Onset HOBO Weather Station m 3m Tripod |  $ | 

**Table x.** Sensors
| Environment Variable          | Sensor                                                                                         | Cost (USD) |
|-------------------------------|------------------------------------------------------------------------------------------------|:----------:|
| Wind Speed & Direction        | [RM Young Wind Monitor 5103L Anemometer](https://www.youngusa.com/product/wind-monitor/)       | $1432      |
| Temperature/Relative Humidity | [Vaisala HMP60 Humidity and Temperature Probe](https://store.vaisala.com/en/products/HMP60)    | $330       |
| Solar Irradiance              | [SP-212-SS: Amplified 0-2.5 Volt Pyranometer](https://www.apogeeinstruments.com/sp-212-ss-amplified-0-2-5-volt-pyranometer/) | $325       |
| Snow Depth                    | [Maxboxtix MB7374 HRXL-MaxSonar-WRST7](https://maxbotix.com/products/mb7374)                   | $290       |

## Measurements

**Table 2.** List of all available data variables that can be transmitted and/or logged with associated sizes and encoding schemes.

| Type     | Variable          | Description                         | Bytes | Encoding formula | 
|----------|-------------------|-------------------------------------|:-----:|------------------|
| uint32_t | unixtime          | UNIX Epoch time                     | 4     |                  |
| int16_t  | temperatureInt    | Internal temperature (°C)           | 2     | * 100            |
| uint16_t | humidityInt       | Internal humidity (%)               | 2     | * 100            |
| uint16_t | pressureInt       | Internal pressure (hPa)             | 2     | (- 850) * 100    |
| int16_t  | temperatureExt    | External temperature (°C)           | 2     | * 100            |
| uint16_t | humidityExt       | External humidity (%)               | 2     | * 10             |
| int16_t  | pitch             | Pitch (°)                           | 2     | * 100            |
| int16_t  | roll              | Roll (°)                            | 2     | * 100            |
| uint16_t | solar             | Solar irradiance (W m-2)            | 2     | * 100            |
| uint16_t | windSpeed         | Mean wind speed (m/s)               | 2     | * 100            |
| uint16_t | windDirection     | Mean wind direction (°)             | 2     | * 10             |
| uint16_t | windGustSpeed     | Wind gust speed (m/s)               | 2     | * 100            |
| uint16_t | windGustDirection | Wind gust direction (°)             | 2     | * 10             |
| int32_t  | latitude          | Latitude (DD)                       | 4     | * 1000000        |
| int32_t  | longitude         | Longitude (DD)                      | 4     | * 1000000        |
| uint8_t  | satellites        | Number of satellites                | 1     |                  |
| uint16_t | hdop              | HDOP                                | 2     |                  |
| uint16_t | voltage           | Battery voltage (V)                 | 2     | * 100            |
| uint16_t | transmitDuration  | Previous transmission duration (s)  | 2     |                  |
| uint8_t  | transmitStatus    | Iridium return code                 | 1     |                  |
| uint16_t | iterationCounter  | Message counter                     | 2     |                  |

### Data transmission and processing
Sensor measurements and GPS position are recorded hourly and stored in memory until the desired transmission interval is reached. Data are compressed into a binary message (340 bytes maximum) to minimize the cost and total number of transmissions required. Data are transmitted via the Iridium Short Burst Data (SBD) satellite network at user-specified intervals, which can be remotely updated based on the desired sampling frequency. SBD data messages are received by an Iridium ground station and sent to Rock7's server. The data is then forwarded to an Amazon Web Services (AWS) SQS queue, decoded using an AWS Lambda Python function and stored in a database using the Amazon Relational Database Service (RDS). Data is made freely available and can be viewed in near-real time at https://cryologger.org.


## Repository Contents

* **/Archive** - Contains information on previous versions of the Cryologger design.

* **/Software** - Contains the Arduino code.

* **/Bill of Materials** - Information on all components used construction and their associated costs.

* **/Documentation** - All project documents

* **/Hardware** - Autodesk EAGLE schematics and Fusion 360 design files.

## Documentation
* Currently in progress.

## License Information
This project is distributed under the GNU General Public License v3.0

Cheers,

**Adam**

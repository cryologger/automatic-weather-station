# Cryologger - Automatic Weather Station (AWS)
![Image](https://github.com/adamgarbo/Cryologger_Automatic_Weather_Station/blob/main/Images/DSC_1877.JPG)

## Background
Polar environments are undergoing rapid environmental change, yet there is a lack of high spatial and temporal monitoring data available in these remote regions. Reliance on expensive and proprietary commercial data acquisition and telemetry systems can inhibit both researchers and citizen scientists and present a significant barrier to the establishment of widespread monitoring networks. 

The development of low-cost, open-source instrumentation can greatly reduce the cost of cryospheric research, improve the spatial density and coverage of collected data, and produce new ways to observe and monitor the cryosphere.

## Objective
The goal of this project is to determine if the Cryologger, an open-source datalogger and telemeter comprised of low-cost, off the shelf components, is capable of supporting the physical measurement needs of a variety of cryospheric scientific applications when configured as an automatic weather station (AWS).

## Design
The Cryologger design is based on the open-source Arduino platform (www.arduino.cc) and built using easy-to-use, do-it-yourself electronics. Code was written using the open-source Arduino integrated development environment (IDE) and compiled from community generated libraries. Programming logic is highly focused on power optimization.

The Cryologger relies on industrial rated components, IP67 rated enclosures and lithium thionyl chloride (LiSOCl2) batteries to withstand harsh the Arctic environment and ensure operation in temperatures as low as -50°C.

### Bill of Materials

**Table 1.** Bill of materials used in automatic weather station deployments
| Component | Product | Cost (USD) | URL |
| --- | --- | :---: | --- |
| Satellite transceiver | Rock7 RockBLOCK 9603 | $249.95 | https://www.rock7.com/shop-product-detail?productId=51 |
| Microcontroller | Adafruit Feather M0 Adalogger | $19.95 | https://www.adafruit.com/product/2796 |
| Real-time clock | Adafruit DS3231 Precision RTC FeatherWing | $13.95 | https://www.adafruit.com/product/3028 |
| Real-time clock battery | CR1220 12mm Diameter - 3V Lithium Coin Cell Battery | $2.50 | https://www.adafruit.com/product/380 |
| Protoboard | Adafruit FeatherWing Tripler Mini Kit | $8.50 | https://www.adafruit.com/product/3417 |
| Terminal block | Adafruit Terminal Block kit for Feather - 0.1" Pitch | $5.95 | https://www.adafruit.com/product/3173 |
| Voltage Regulator | Pololu D36V6F3 3.3V 600 mA Step-Down Voltage Regulator | $4.95 | https://www.pololu.com/product/3791 |
| Power connector | Male DC Power adapter - 2.1mm plug to screw terminal block | $2.00 | https://www.adafruit.com/product/369 |
| Power connector | Breadboard-friendly 2.1mm DC barrel jack | $0.95 | https://www.adafruit.com/product/373 |
| Anemometer / Wind Vane | Davis Instruments Anemometer (#7911) | $155.00 | https://www.davisinstruments.com/product/anemometer-for-weather-monitor-or-wizard/ |
| Temperature / Relative Humidity Sensor | Davis Instruments Temperature/Humidity Sensor (#6834) | $70.00 | https://www.davisinstruments.com/product/temperature-humidity-sensor/ |
| Radiation Shield | Davis Instruments Radiation Shield (#7714) | $85.00 | https://www.davisinstruments.com/product/radiation-shield/ |
| Battery |	Tadiran	Lithium Pulses Plus 7.2V 38Ah 273.6Wh (TLP-93131/B/AL1) | $150 | http://www.tadiranbat.com/assets/tlp-93121-b-al1.pdf |
| Enclosure | Pelican 1120 | $31.95 | https://www.pelican.com/us/en/product/cases/protector/1120 |
| Total |  | $ | |

### Data transmission and processing
Sensor measurements and GPS position are recorded hourly and stored in memory until the desired transmission interval is reached. Data are compressed into a binary message (340 bytes maximum) to minimize the cost and total number of transmissions required. Data are transmitted via the Iridium Short Burst Data (SBD) satellite network at user-specified intervals, which can be remotely updated based on the desired sampling frequency. SBD data messages are received by an Iridium ground station and sent to Rock7's server. The data is then forwarded to an Amazon Web Services (AWS) SQS queue, decoded using an AWS Lambda Python function and stored in a database using the Amazon Relational Database Service (RDS). Data is made freely available and can be viewed in near-real time at https://cryologger.org.

## Deployments
Three different configurations of the Cryologger AWS were deployed in 2019.

### Nain, Newfoundland and Labrador
* Data: https://cryologger.org/nain/
* Deployed: February 7, 2019
* Coordinates: 56.43 N,	-61.46 W

### Recorded Measurements
| Variable  | Unit | Comments |
| --- | :---: | --- |
| Datetime  |   | YYYY-MM-DD hh:mm:ss |
| Temperature (ext) | °C  |  |
| Temperature (int) | °C  |  |
| Humidity | % |  |
| Wind speed | m s<sup>-1</sup> | Wind speed at 3 m height |
| Wind direction | ° | Wind direction at 3 m height |
| Wind gust | m s<sup>-1</sup>  | Wind gust speed at 3 m height |
| Wind gust Direction | ° | Wind gust direction at 3 m height |
| Voltage | V | Battery voltage |
| Transmit duration  | s | Length of Iridium transmission  |
| Message counter |  | Number of transmitted messages |


### Arctic Bay, Nunavut
* Data: https://cryologger.org/arctic-bay-2019/
* Deployed: May 27, 2019
* Coordinates: 73.31 N, -86.2 5W
<img src="https://github.com/adamgarbo/Cryologger_Automatic_Weather_Station/blob/main/Images/DSC_0440.JPG" height="480">

#### Recorded Measurements
| Variable  | Unit | Comments |
| --- | :---: | --- |
| Datetime  |   | YYYY-MM-DD hh:mm:ss |
| Temperature  | °C  |  |
| Humidity | % |  |
| Wind speed 1 | m s<sup>-1</sup> | Wind speed at 1 m height |
| Wind direction 1 | ° | Wind direction at 1 m height |
| Wind gust 1 | m s<sup>-1</sup>  | Wind gust speed at 1 m height |
| Wind gust Direction 1 | ° | Wind gust direction at 1 m height |
| Wind speed 2 | m s<sup>-1</sup> | Wind speed at 3 m height |
| Wind direction 2 | ° | Wind direction at 3 m height |
| Wind gust 2 | m s<sup>-1</sup>  | Wind gust speed at 3 m height |
| Wind gust Direction 2 | ° | Wind gust direction at 3 m height |
| Voltage | V | Battery voltage |
| Transmit duration  | s | Length of Iridium transmission  |
| Message counter |  | Number of transmitted messages |

### Milne Glacier, Nunavut
* Data: https://cryologger.org/milne-glacier-2019/
* Deployed: July 18, 2019
* Coordinates: 82.45 N,	-80.40 W

<img src="https://github.com/adamgarbo/Cryologger_Automatic_Weather_Station/blob/main/Images/DSC_1873.JPG" width="720">

#### Recorded Measurements
| Variable  | Unit | Comments |
| --- | :---: | --- |
| Datetime  |   | YYYY-MM-DD hh:mm:ss |
| Temperature (ext) | °C  | External air temperature |
| Temperature (int) | °C  | Internal air temperature |
| Humidity | % | External humidity  |
| Wind speed | m s<sup>-1</sup> | Wind speed at 3 m height |
| Wind direction | ° | Wind direction at 3 m height |
| Wind gust | m s<sup>-1</sup>  | Wind gust speed at 3 m height |
| Wind gust Direction | ° | Wind gust direction at 3 m height |
| Voltage | V | Battery voltage |
| Transmit duration  | s | Length of Iridium transmission  |
| Message counter |  | Number of transmitted messages |

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

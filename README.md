# Cryologger - Automatic Weather Station (AWS)
![Image](https://github.com/adamgarbo/Cryologger_Automatic_Weather_Station/blob/master/Images/DSC_1877.JPG)

## Background
Polar environments are undergoing rapid environmental change, yet there is a lack of high spatial and temporal monitoring data available in these remote regions. Reliance on expensive and proprietary commercial data acquisition and telemetry systems can inhibit both researchers and citizen scientists and present a significant barrier to the establishment of widespread monitoring networks. 

The development of low-cost, open-source instrumentation can greatly reduce the cost of cryospheric research, improve the spatial density and coverage of collected data, and produce new ways to observe and monitor the cryosphere.

## Objective
The goal of this project is to determine if the Cryologger, an open-source datalogger and telemeter comprised of low-cost, off the shelf components, is capable of supporting the physical measurement needs of a variety of cryospheric scientific applications when configured as an automatic weather station (AWS).

## Design
The Cryologger design is based on the open-source Arduino platform (www.arduino.cc) and built using easy-to-use, do-it- yourself electronics. Code was written using the open-source Arduino integrated development environment (IDE) and compiled from community generated libraries. Programming logic is highly focused on power optimization.

The Cryologger relies on industrial rated components, IP67 rated enclosures and lithium thionyl chloride (LiSOCl2) batteries to withstand harsh the Arctic environment and ensure operation in temperatures as low as -50°C.

Data transmitted by the Cryologger is automatically displayed on the https://cryologger.org website.

## Deployments
Three different configurations of the Cryologger AWS were deployed in 2019.

## Nain, Newfoundland and Labrador
* Date: February, 2019
* Coordinates:

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


## Arctic Bay, Nunavut
* Data: https://cryologger.org/arctic-bay-2019/
* Date: May 27, 2019
* Coordinates: 73.31 N, -86.2 5W
<img src="https://github.com/adamgarbo/Cryologger_Automatic_Weather_Station/blob/master/Images/DSC_0440.JPG" height="480">

### Recorded Measurements
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

## Milne Glacier, Nunavut
* Data: https://cryologger.org/milne-glacier-2019/
* Date: July, 2019
* Coordinates: 82.45 N,	-80.40 W

<img src="https://github.com/adamgarbo/Cryologger_Automatic_Weather_Station/blob/master/Images/DSC_1873.JPG" width="720">

### Recorded Measurements
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
* **/Arduino**
* **/Bill of Materials**
* **/Images**

## Documentation
* Work in progress.

## License Information
This project is distributed under the GNU General Public License v3.0.

Cheers,

**Adam**

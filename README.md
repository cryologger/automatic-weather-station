<p align="left">
<img alt="GitHub" src="https://img.shields.io/github/license/adamgarbo/Cryologger_Automatic_Weather_Station">
<img alt="GitHub release (latest by date)" src="https://img.shields.io/github/v/release/adamgarbo/Cryologger_Automatic_Weather_Station">
<img alt="GitHub issues" src="https://img.shields.io/github/issues/adamgarbo/Cryologger_Automatic_Weather_Station">
</p>

# Cryologger - Automatic Weather Station (AWS)
![Image](https://github.com/adamgarbo/Cryologger_Automatic_Weather_Station/blob/main/Images/DSC_1877.JPG)

## Background
Polar environments are undergoing rapid environmental change, yet there is a lack of high spatial and temporal monitoring data available in these remote regions. Reliance on expensive and proprietary commercial data acquisition and telemetry systems can inhibit both researchers and citizen scientists and present a significant barrier to the establishment of widespread monitoring networks. 

The development of low-cost, open-source instrumentation can greatly reduce the cost of cryospheric research, improve the spatial density and coverage of collected data, and produce new ways to observe and monitor the cryosphere.

## Objective
The goal of this project is to determine if the Cryologger, an open-source datalogger and telemeter comprised of low-cost, off the shelf components, is capable of supporting the physical measurement needs of a variety of cryospheric scientific applications when configured as an automatic weather station (AWS).

## Design
The Cryologger design is based on the open-source Arduino platform (www.arduino.cc) and built using easy-to-use, do-it-yourself electronics. Code was written using the open-source Arduino integrated development environment (IDE) and compiled from community generated libraries. Programming logic is highly focused on power optimization.

### Bill of Materials

**Table 1.** Bill of materials used in automatic weather station deployments
| Component | Product | Cost (USD) | URL |
| --- | --- | :---: | --- |

### Data transmission and processing
Sensor measurements and GPS position are recorded hourly and stored in memory until the desired transmission interval is reached. Data are compressed into a binary message (340 bytes maximum) to minimize the cost and total number of transmissions required. Data are transmitted via the Iridium Short Burst Data (SBD) satellite network at user-specified intervals, which can be remotely updated based on the desired sampling frequency. SBD data messages are received by an Iridium ground station and sent to Rock7's server. The data is then forwarded to an Amazon Web Services (AWS) SQS queue, decoded using an AWS Lambda Python function and stored in a database using the Amazon Relational Database Service (RDS). Data is made freely available and can be viewed in near-real time at https://cryologger.org.

## Deployments

#### Recorded Measurements

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

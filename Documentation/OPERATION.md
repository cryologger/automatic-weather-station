# Cryologger Automatic Weather Station (AWS) - Operation



## Wiring Diagram

**Table 1.** Davis Instruments 7911 anemometer.
| Wire   | Description    | Pin |
|--------|----------------|-----|
| Black  | Wind speed     | A1  |
| Red    | Ground         | GND |
| Green  | Wind direction | A2  |
| Yellow | Power          | U3  |

**Table 2.** Davis Instruments temperature relative humidity sensor (Sensiron SHT31-LSS).
| Wire   | Description | Pin |
|--------|-------------|-----|
| Blue   | Data        | SDA |
| Yellow | Power       | 3.3 |
| Green  | Ground      | GND |
| White  | Clock       | SCL |

**Table 3.** Wiring diagram for Apogee SP-212 pyranometer.
| Wire   | Description | Pin |
|--------|-------------|-----|
| Black  | Ground      | GND |
| White  | Sensor      | A3  |
| Red    | Power       | 5V  |
| Shield | Shield      | GND |

**Table 4.** Wiring diagram for MaxBotix MB7354 HRXL-MaxSonar-WRS5 ultrasonic sensor.
| Wire Colour |      Description      |      Pin      |
|:-----------:|:---------------------:|:-------------:|
|    White    |   Temperature Sensor  | Not connected |
|    Orange   |   Pulse Width Output  |       A3      |
|    Brown    | Analog Voltage Output | Not connected |
|    Green    |   Ranging Start/Stop  | Not connected |
|     Blue    |     Serial Output     | Not connected |
|     Red     |          Vcc          |       5V      |
|    Black    |         Ground        |      GND      |

## LED Blink Patterns

When initially powered on, the system will attempt to establish a GNSS fix and synchronize the real-time clock (RTC) for up to 2 minutes. During this time, the LED will blink every second. Once the RTC is synchronized, the system will set an alarm to wake at the user-specified time and then enter a low-power deep sleep. While the system is in deep sleep, the Watchdog Timer (WDT) will wake the system to check the program has not frozen approximately every 8 seconds. The LED will blink briefly (100 ms) during this time.

Table x. LED blink patterns and associated description and troubleshooting guide.
| Number | Duration (ms) |    Colour   |   Pattern   | Description                     | Troubleshooting Steps                     |
|:------:|:-------------:|:-----------:|:-----------:|---------------------------------|-------------------------------------------|
|    4   |      500      |     Red     |   Single    | System start up                 | None                                      |
|   10   |      100      | Green + Red | Alternating | Initialization complete         | None                                      |
|    2   |      250      |     Red     |  Continuous | microSD failure                 | Reseat microSD card and reboot system     |
|    1   |      1000     |     Red     |  Continuous | GNSS signal acquisition         | None                                      |
|    5   |      100      |    Green    |   Single    | GNSS fix found                  | None                                      |
|    5   |      100      |     Red     |   Single    | GNSS fix not found              | Ensure good view of sky                   |
|   10   |      250      |    Green    |   Single    | Iridium transmission successful | None                                      |
|   10   |      250      |     Red     |   Single    | Iridium transmission failure    | Ensure good view of sky and reboot system |
|    1   |      100      |     Red     |   Single    | Watchdog Timer every 8 seconds  | None                                      |


## Tools
| Component                             	| Type                       	| Tool(s)                             	|
|---------------------------------------	|----------------------------	|-------------------------------------	|
| Enclosure panel                       	| 10/32" 3/8" Pan Head Screw 	| Phillips Head Screwdriver (large)   	|
| Enclosure pole mount bracket          	| 1/4"-20 Pan Head Screw     	| Phillips Head Screwdriver (large)   	|
| Enclosure pole mount bracket          	| 1/4"-20 Hex Nut            	| 7/16" socket/wrench                 	|
| Enclosure pole mount bracket u-bolt   	| 1/4"-20 Hex Nut            	| 7/16" socket/wrench                 	|
| Radiation shield                      	| 1/4"-20 Locknut            	| 7/16" socket                        	|
| Solar panel 1 3/4" u-bolt             	| 5/16"-18 Hex Nut           	| 1/2" wrench/socket                  	|
| Polar Wire cable                      	| 14 AWG                     	| Wire stripper                       	|
| Genasun GV-4 Solar charge controller  	| Screw terminal             	| Flat head (small)                   	|
| Deka 8GUIH battery terminals          	| 1/4"-20 Hex Nut            	| 7/16" socket/wrench                 	|
| Anemometer 1-3/4" & 1-5/8" u-bolts    	| 5/16"-18 Hex Nut           	| 1/2" socket/wrench                  	|
| Anemometer stainless steel hose clamp 	| Screw                      	| Phillips Head Screwdriver  (medium) 	|
| Anemometer stainless steel hose clamp 	| Nut                        	| 5/16" socket (optional)             	|
| Tripod 1-3/4" u-bolt                  	| 5/16"-18 Hex Nut           	| 1/2" socket/wrench                  	|
| Tripod 1-5/8" u-bolt                  	| 1/4"-20 Hex Nut            	| 7/16" socket/wrench                 	|
| Tripod guy wire kit                   	| Cable                      	| Lineman Cutters                     	|
| Tripod guy wire kit                   	| 10/32" 3/8" Pan Head Screw 	| Phillips Head Screwdriver (medium)  	|
| Tripod guy wire kit                   	| 1/4"-20 Hex Nut            	| 7/16" socket                        	|
| Tripod guy wire kit                   	| 5/16"-18 Hex Nut           	| 1/2" wrench                         	|
| PCB Screw terminals                   	| Screw terminal             	| Flat head (small)                   	|

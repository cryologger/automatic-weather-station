# Cryologger Automatic Weather Station (AWS) - Assembly

## 1.0 Introduction
This guide provides detailed instructions on the assembly of a Cryologger AWS v1.0.

## 2.0 Bill of Materials (BOM)
The components required to assemble a Cryologger GVT system are split into two groups: 
1) Components required for the assembly of the Cryologger AWS (e.g., PCB, GNSS receiver, microcontroller, etc.)
2) Components required for the mounting, power and deployment of the complete system (e.g., tripod, solar panel, battery, etc.).

**Table 1.** Recommended tools required for the asssembly of a Cryologger AWS v1.0.
| Tool | Recommended Product |
| --- | --- | 
| Soldering iron | [Hakko FX-888D Digital Soldering Station](https://hakkousa.com/fx-888d-digital-soldering-station.html) |
| Solder | [SparkFun Solder - 1/4lb Spool (0.020") Special Blend](https://www.sparkfun.com/products/10242) |
| Side cutter | [Hakko CHP-170 Micro Cutter](https://hakkousa.com/chp-170-micro-cutter.html) |
| Pliers | [Hakko CHP PN-2001 Pointed Nose Pliers](https://hakkousa.com/products/chp-tools/chp-hand-tools/chp-pn-2001-pointed-nose-pliers.html) |
| Hot-air rework station | [Quick 957DW+](https://www.adafruit.com/product/1869) |
| Screw driver | Phillips head screw driver |
| PCB cleaning brush | [ESD-Safe PCB Cleaning Brush](https://www.adafruit.com/product/1209) |
| PCB cleaning solution | Deionized (DI) water |

**Table 1.** Bill of materials and associated costs for components used in the Cryologger AWS v1.0. Prices are listed in USD and are current as of December 2023. Taxes and shipping not included. <sup>1</sup>Denotes optional component. Please note the BOM is a work in progress.
| Component             | Product                                                               | Cost (USD) |
|-----------------------|-----------------------------------------------------------| :---: |
| Satellite transceiver | [Rock7 RockBLOCK 9603](http://www.rock7mobile.com/products-rockblock-9603) | $267.50
| Satellite antenna*    | [Maxtena M1621HCT-P-SMA](https://maxtena.com/products/f-passive/m1621hct-p-sma-iridium-passive-antenna/) | $54.00 
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


**Table x.** Sensors
| Environment Variable          | Sensor                                                                                         | Cost (USD) |
|-------------------------------|------------------------------------------------------------------------------------------------|:----------:|
| Wind Speed & Direction        | [RM Young Wind Monitor 5103L Anemometer](https://www.youngusa.com/product/wind-monitor/)       | $1432      |
| Temperature/Relative Humidity | [Vaisala HMP60 Humidity and Temperature Probe](https://store.vaisala.com/en/products/HMP60)    | $330       |
| Solar Irradiance              | [SP-212-SS: Amplified 0-2.5 Volt Pyranometer](https://www.apogeeinstruments.com/sp-212-ss-amplified-0-2-5-volt-pyranometer/) | $325       |
| Snow Depth                    | [Maxboxtix MB7374 HRXL-MaxSonar-WRST7](https://maxbotix.com/products/mb7374)                   | $290       |



**Table 3.** Components making up the enclosure, mounting and power systems for Cryologger AWS v1.0. <sup>1</sup>Denotes optional component. <sup>2</sup>Denotes educational pricing.
| Component | Product | Quantity | Cost (USD) | 
| --- | --- | :---: | :---: | 
| Solar Panel | [Ameresco Solar 20 Watt Solar Panel 20J<sup>2</sup>](https://www.amerescosolar.com/20j-ameresco-solar-20-watt-solar-panel) | 1 |  $99.00 |
| Solar Panel Bracket | [Ameresco Solar HPM18-30 Solar Panel Pole Mount<sup>2</sup>](https://www.amerescosolar.com/hpm18-30solar-panel-pole-mount) | 1 |  $39.00 
| Solar Charge Controller | [Genasun GV-4 4A MPPT Solar Charge Controller](https://www.altestore.com/store/charge-controllers/solar-charge-controllers/mppt-solar-charge-controllers/genasun-gv-4-pb-12v-4a-mppt-controller-for-12v-lead-acid-batteries-p10622/) | 1 |  $63.00 |
| Solar Panel Power Cable | [Polarwire Arctic Ultraflex Blue Twin Trailer Cable AUF-TC162](https://store.polarwire.com/arctic-trailer-cable-16-2-gray-16ga-2-conductors/) | Variable | $0.55/ft |
| Solar Panel Cable Gland | [Heyco M4340 M20 Cable Gland](https://octopart.com/m4340-heyco-2298364?r=sp) | 2 | $5.94 |
| Battery Ring Terminal | [Molex Perma-Seal Ring Terminal 14-16 AWG Wire 1/4" (M6) 19164-0026](https://octopart.com/19164-0026-molex-99569?r=sp) | 2 | $1.00 |
| Battery | Deka SOLAR Gel 48 Ah 8G40C | 1 | ~$150.00 |
| Antenna Bulkhead Adapter | [Amphenol TNC Jk to TNC Jk 50 Ohm Str Bkhd IP67 AD-TNCJTNCJ-1](https://octopart.com/ad-tncjtncj-1-amphenol+rf-108109770?r=sp) | 1 | $42.79 |
| Antenna Cable | [GPAC GPS Antenna Cable TNC to TNC (3 m)](https://www.gpac-llc.com/product.php?product=1) | 1 | $56.00 |
| Enclosure | [L-Com 14x12x7 Inch Weatherproof NEMA Enclosure](https://www.l-com.com/nema-enclosures-14x12x7-inch-weatherproof-nema-enclosure-only) | 1 |  $161.99 |
| Enclosure Panel | [L-Com Blank Aluminum Mounting Plate](https://www.l-com.com/nema-enclosures-blank-aluminum-mounting-plate-for-1412xx-series-enclosures) | 1 |  $32.39 |
| Pole Mount | [L-Com Enclosure Pole Mounting Kit](https://www.l-com.com/nema-enclosures-enclosure-pole-mounting-kit-pole-diameters-1-1-4-to-2-inches) | 1 | $75.99 |
| Tripod | [Onset HOBO Weather Station m 3m Tripod M-TPA](https://www.onsetcomp.com/products/mounting/m-tpa/) | 1 |  $200.00 | 


## 3.0 Assembly

### 3.1 Cryologger AWS PCB Assembly
**1) Collect all required components**
* It is suggested to first gather all of the components listed in the bill of materials (Table 2), required for the assembly of the Cryologger. Remove the components from their packaging and inspect them for any signs of damage.

**2) Solder through-hole resistors and capacitor**
* The first components recommended to be soldered to the Cryologger PCB are the through-hole resistors and capacitor (Figure 1). 
* Both the capacitor and resistors can be inserted into the PCB in any orientation.
* It is important to pay attention to the placement of the resistors, as they differ in resistance. 
  * R1 = 10 MOhm. Colour code: Brown Black Black Green Brown
  * R2 = 1 MOhm. Colour code: Brown Black Black Yellow Brown
* For more information on resistance colour codes, please see: https://www.digikey.com/en/resources/conversion-calculators/conversion-calculator-resistor-color-code



## 4.0 Calibration

## 5.0 Testing


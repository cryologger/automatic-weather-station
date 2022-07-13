# Cryologger - Automatic Weather Station (AWS)


## Wiring

**Table 1.** Davis Instruments 7911 anemometer.
| Wire   | Description    | Pin |
|--------|----------------|-----|
| Black  | Wind speed     | A1  |
| Red    | Ground         | GND |
| Green  | Wind direction | A2  |
| Yellow | Power          | A4  |

**Table 2.** Davis Instruments temperature relative humidity sensor (Sensiron SHT31-LSS).
| Wire   | Description | Pin |
|--------|-------------|-----|
| Blue   | Data        | SDA |
| Yellow | Power       | 5V  |
| Green  | Grou nd     | GND |
| White  | Clock       | SCL |

**Table 3.** Apogee SP-212 pyranometer.
| Wire   | Description | Pin |
|--------|-------------|-----|
| Black  | Ground      | GND |
| White  | Sensor      | A3  |
| Red    | Power       | 5V  |
| Shield | Shield      | GND |


## LED Blink Patterns

| Number | Duration (ms) |    Colour   |   Pattern   | Description                     | Troubleshooting Steps                     |
|:------:|:-------------:|:-----------:|:-----------:|---------------------------------|-------------------------------------------|
|    4   |      500      |     Red     |   Single    | System start up                 | None                                      |
|   10   |      100      | Green + Red | Alternating | Initialization complete         | None                                      |
|    2   |      250      |     Red     |  Continuous | microSD failure                 | Reseat microSD card and reboot system     |
|    1   |      1000     |     Red     |  Continuous | GNSS signal acquisition         | None                                      |
|    5   |      100      |    Green    |   Single    | GNSS fix found                  | None                                      |
|    5   |      100      |     Red     |   Single    | GNSS fix not found              | Ensure good view of sky                   |
|   10   |      100      |    Green    |   Single    | Iridium transmission successful | None                                      |
|   10   |      100      |     Red     |   Single    | Iridium transmission failure    | Ensure good view of sky and reboot system |

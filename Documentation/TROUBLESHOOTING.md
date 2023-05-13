

## RM Young 5103L Wind Monitor

### Wiring

| Pin | Wire   | Description                         | Connector |
|:---:|:------:|-------------------------------------|:---------:|
| 1   | White  | 12-30 VDC Input (Wind direction +)  | 12V       |
| 2   | Green  | 4-20 mA Output  (Wind direction -)  | A2        |
| 3   | Black  | 12-30 VDC Input (Wind speed +)      | 12V       |
| 4   | Red    | 4-20 mA Output  (Wind speed -)      | A1        |
| 5   | Shield | Earth ground                        | GND       |

### Troubleshooting

| Scenario       | Wind Speed (Analog) | Wind Direction (Analog) | Wind Speed (m/s) | Wind Direction (°) | Comments      |
|----------------|:-------------------:|:-----------------------:|:----------------:|:------------------:|---------------|
| Disconnect 12V |          4          |            3            |      -25.21      |       -90.89       | Floating      |
| Disconnect WD+ |          0          |            -            |      -25.21      |          -         | Floating      |
| Disconnect WD- |          -          |            0            |         -        |       -90.89       | Floating      |
| Unplug battery |          0          |            0            |      -25.21      |       -90.89       | Floating      |

## Vaisala HMP60

### Wiring
| Pin | Wire  | Description   | Description                    | Connector |
|:---:|:-----:|---------------|--------------------------------|:---------:|
| 1   | Brown | VDC           | Voltage Input (5-28V)          | 12V       |
| 2   | White | Channel 1: RH | Relative Humidity (%) (0-2.5V) | A3        |
| 3   | Blue  | GND           | Ground                         | GND       |
| 4   | Black | Channel 2: T  | Temperature (°C) (0-2.5V)      | A4        |
| 5   | Shield | Earth ground |                                | GND       |

### Troubleshooting

| Scenario                           | Temperature (Analog) | Humidity (Analog) | Temperature (°C) | Humidity (%) | Comments      |
|------------------------------------|:--------------------:|:-----------------:|:----------------:|:------------:|---------------|
| Disconnect 12V                     |        287-416       |      288-417      |       ~-55       |      ~10     | Floating      |
| Disconnect GND                     |         3286         |        3241       |        >40       |     >100     | Max ADC value |
| Disconnect Channel 1 (Humidity)    |           -          |        1550       |         -        |      50      | Floating      |
| Disconnect Channel 2 (Temperature) |         1550         |         -         |        -10       |       -      | Floating      |
| Unplug M12 connector               |         ~1550        |       ~1550       |        -10       |      50      | Floating      |
| Unplug battery                     |          10          |         10        |        -60       |       0      | Min ADC value |

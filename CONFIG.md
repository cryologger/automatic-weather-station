
## Sensor Configuration

### Vaisala HMP60


| Pin | Wire  | Description   | Description                  | Connector |
|-----|-------|---------------|------------------------------|-----------|
| 1   | Brown | VDC           | Voltage Input (5-28V)        | 12V       |
| 2   | White | Channel 1: RH | Relative Humidity (%) (0-1V) | A4        |
| 3   | Blue  | GND           | Ground                       | A3        |
| 4   | Black | Channel 2: T  | Temperature (°C) (0-1V)      | GND       |

<img width="240" alt="image" src="https://user-images.githubusercontent.com/22924092/166289749-5f874318-572a-4545-b4d9-ad0d5a73522e.png">


### RM Young 5103L Wind Monitor
| Pin | Pin   | Wire  | Description     | Connector |
|-----|-------|-------|-----------------|-----------|
| 1   | WD+   | White | 12-30 VDC Input | 12V       |
| 2   | WD-   | Green | 4-20 mA Output  | A2        |
| 3   | WS+   | Black | 12-30 VDC Input | 12V       |
| 4   | WS-   | Red   | 4-20 mA Output  | A1        |
| 5   | Earth |       |                 | GND       |

### LSM303AGR
Orientation of sensor, when viewing enclosure from the front:

**Pitch:**
* Positive values (+) = Upwards (↑)
* Negative values (-) = Downwards (↓)

**Roll:**
* Positive values (+) = Left (←)
* Negative values (-) = Right (→)



## Amazon Web Services (AWS) Configuration


### Data Transmission Format


| Type     | Variable          | Description                    | Bytes | Encoding formula |
|----------|-------------------|--------------------------------|-------|------------------|
| uint32_t | unixtime          | UNIX Epoch time                | 4     |                  |
| int16_t  | intTemperature    | Internal temperature (°C)      | 2     | * 100            |
| int16_t  | extTemperature    | External temperature (°C)      | 2     | * 100            |
| uint16_t | extHumidity       | External humidity (%)          | 2     | * 10             |
| int16_t  | pitch             | Pitch (°)                      | 2     | * 100            |
| int16_t  | roll              | Roll (°)                       | 2     | * 100            |
| uint16_t | windSpeed         | Wind speed (m/s)               | 2     | * 100            |
| uint16_t | windDirection     | Wind direction (°)             | 2     |                  |
| uint16_t | windGust          | Wind gust speed (m/s)          | 2     | * 100            |
| uint16_t | windGustDirection | Wind gust direction (°)        | 2     |                  |
| uint16_t | voltage           | Battery voltage (V)            | 2     | * 100            |
| uint16_t | transmitDuration  | Previous transmission duration | 2     |                  |
| uint8_t  | transmitStatus    | Iridium return code            | 1     |                  |
| uint16_t | iterationCounter  | Message counter                | 2     |                  |
|          |                   |                      **Total** | **29**|                  |

### MariaDB 

```
CREATE TABLE 2022_aws_arctic_bay (id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, imei BIGINT UNSIGNED, momsn INT UNSIGNED, transmit_time TEXT(24), iridium_latitude DOUBLE, iridium_longitude DOUBLE, iridium_cep DOUBLE, data TEXT(340), unixtime INT UNSIGNED, temperature_int DOUBLE, temperature_ext DOUBLE, humidity_ext DOUBLE, pitch DOUBLE, roll DOUBLE, wind_speed DOUBLE, wind_direction DOUBLE, wind_gust_speed DOUBLE, wind_gust_direction DOUBLE, voltage DOUBLE, transmit_duration SMALLINT, transmit_status TINYINT, message_counter INT UNSIGNED);
```

```
CREATE TABLE 2022_aws_pond_inlet (id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, imei BIGINT UNSIGNED, momsn INT UNSIGNED, transmit_time TEXT(24), iridium_latitude DOUBLE, iridium_longitude DOUBLE, iridium_cep DOUBLE, data TEXT(340), unixtime INT UNSIGNED, temperature_int DOUBLE, temperature_ext DOUBLE, humidity_ext DOUBLE, pitch DOUBLE, roll DOUBLE, wind_speed DOUBLE, wind_direction DOUBLE, wind_gust_speed DOUBLE, wind_gust_direction DOUBLE, voltage DOUBLE, transmit_duration SMALLINT, transmit_status TINYINT, message_counter INT UNSIGNED);
```

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


### MariaDB 

```
CREATE TABLE 2022_aws_arctic_bay (id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, imei BIGINT UNSIGNED, momsn INT UNSIGNED, transmit_time TEXT(24), iridium_latitude DOUBLE, iridium_longitude DOUBLE, iridium_cep DOUBLE, data TEXT(340), unixtime INT UNSIGNED, temperature_int DOUBLE, temperature_ext DOUBLE, humidity_ext DOUBLE, pitch DOUBLE, roll DOUBLE, wind_speed DOUBLE, wind_direction DOUBLE, wind_gust_speed DOUBLE, wind_gust_direction DOUBLE, voltage DOUBLE, transmit_duration SMALLINT, transmit_status TINYINT, message_counter INT UNSIGNED);
```

```
CREATE TABLE 2022_aws_pond_inlet (id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, imei BIGINT UNSIGNED, momsn INT UNSIGNED, transmit_time TEXT(24), iridium_latitude DOUBLE, iridium_longitude DOUBLE, iridium_cep DOUBLE, data TEXT(340), unixtime INT UNSIGNED, temperature_int DOUBLE, temperature_ext DOUBLE, humidity_ext DOUBLE, pitch DOUBLE, roll DOUBLE, wind_speed DOUBLE, wind_direction DOUBLE, wind_gust_speed DOUBLE, wind_gust_direction DOUBLE, voltage DOUBLE, transmit_duration SMALLINT, transmit_status TINYINT, message_counter INT UNSIGNED);
```

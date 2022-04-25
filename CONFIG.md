## Amazon Web Services (AWS) Configuration

### MariaDB 

```
CREATE TABLE 2022_aws_arctic_bay (id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, imei BIGINT UNSIGNED, momsn INT UNSIGNED, transmit_time TEXT(24), iridium_latitude DOUBLE, iridium_longitude DOUBLE, iridium_cep DOUBLE, data TEXT(340), unixtime INT UNSIGNED, temperature_int DOUBLE, temperature_ext DOUBLE, humidity_ext DOUBLE, pitch DOUBLE, roll DOUBLE, wind_speed DOUBLE, wind_direction DOUBLE, wind_gust_speed DOUBLE, wind_gust_direction DOUBLE, voltage DOUBLE, transmit_duration SMALLINT, transmit_status TINYINT, message_counter INT UNSIGNED);
```

```
CREATE TABLE 2022_aws_pond_inlet (id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, imei BIGINT UNSIGNED, momsn INT UNSIGNED, transmit_time TEXT(24), iridium_latitude DOUBLE, iridium_longitude DOUBLE, iridium_cep DOUBLE, data TEXT(340), unixtime INT UNSIGNED, temperature_int DOUBLE, temperature_ext DOUBLE, humidity_ext DOUBLE, pitch DOUBLE, roll DOUBLE, wind_speed DOUBLE, wind_direction DOUBLE, wind_gust_speed DOUBLE, wind_gust_direction DOUBLE, voltage DOUBLE, transmit_duration SMALLINT, transmit_status TINYINT, message_counter INT UNSIGNED);
```

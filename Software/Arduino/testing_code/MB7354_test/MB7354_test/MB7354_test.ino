/*

  MaxBotix MB7374 HRXL-MaxSonar-WRST7 Ultrasonic Precision Range Finder
  https://www.maxbotix.com/Ultrasonic_Sensors/MB7374.htm

*/

// MaxBotix MB7354 HRXL-MaxSonar-WRS5
// https://www.maxbotix.com/ultrasonic_sensors/mb7354.htm
// --------------------------------------------------
// Colour    Pin    Description             Pin
// --------------------------------------------------
// White    1       Temperature Sensor      Not connected
// Orange   2       Pulse Width Output      A3
// Brown    3       Analog Voltage Output   Not connected
// Green    4       Ranging Start/Stop      Not connected
// Blue     5       Serial Output           Not connected
// Red      6       Vcc                     5V
// Black    7       GND                     GND

#include <Statistic.h>          // https://github.com/RobTillaart/Arduino/tree/master/libraries/Statistic


#define PIN_SNOW            A3  // Maxbotix pulse width output pin
#define PIN_12V_EN          5   // 12 V step-up/down regulator
#define PIN_5V_EN           6   // 5V step-down regulator

Statistic snowDepthStatsAvg;    // Maxbotix average distances
Statistic snowDepthStatsStd;    // Maxbotix std distances
Statistic snowDepthStatsMax;    // Maxbotix max distances
Statistic snowDepthStatsMin;    // Maxbotix min distances
Statistic snowDepthStatsNan;    // Maxbotix nan samples

unsigned int  snowDepthAvg      = 0;      // Average distance from Maxbotix sensor to surface (mm)
unsigned int  snowDepthStd      = 0;      // Standard deviation distance from Maxbotix sensor to surface (mm)
unsigned int  snowDepthMax      = 0;      // Max distance from Maxbotix sensor to surface (mm)
unsigned int  snowDepthMin      = 0;      // Min distance from Maxbotix sensor to surface (mm)
unsigned int  snowDepthNan      = 0;      // Number of NaN readings in Maxbotix


void setup() {
  
  // MaxBotix
  pinMode(PIN_SNOW, INPUT);
  pinMode(PIN_5V_EN, OUTPUT);
  pinMode(PIN_12V_EN, OUTPUT);
  digitalWrite(PIN_5V_EN, LOW);   // Disable 5V power
  digitalWrite(PIN_12V_EN, LOW);  // Disable 12V power
  
  Serial.begin(115200);
  while (!Serial);

  //Print header
  Serial.println("Mean\tStdev\tMax\tMin\tNan");
}

void loop() {
  readMb7354(); // Read Maxbotix Distance sensor
  delay(5000);
}

// Read Maxbotix distance to surface
void readMb7354() 
{
  // Wake sensor
  digitalWrite(PIN_5V_EN, HIGH);
  delay(100);

  // Create a temporary Statistic array to hold the maxbotix measurements
  Statistic Maxbotix;

  // Create temporary variables
  unsigned int z, zAvg, zStd, zMax, zMin, zNan;

  // Get 30 z readings in mm, filtering out reading 50 mm
  // above/below sensor minumum/maximum readings
  for (byte i = 0; i < 30; i++) {
    z = pulseIn(PIN_SNOW, HIGH); // Read distance to snow surface

    if (z > 550 && z < 4950) { // Filter readings
      Maxbotix.add(z); // Add good readings to stats array
    }
    else {
      zNan += 1; // Count bad readings
    }
    delay(100); // Delay 0.1 secs between readings
  }

  // Get stats from the Maxbotix array
  zAvg = Maxbotix.average(), 0;
  zStd = Maxbotix.pop_stdev(), 0;
  zMax = Maxbotix.maximum(), 0;
  zMin = Maxbotix.minimum(), 0;

  // Deal with issue of a maximum long number in the instance of no
  // readings within filtered range
  if (zAvg > 5000) {
    zAvg = 0;
  }
  if (zStd > 5000) {
    zStd = 0;
  }
  if (zMax > 5000) {
    zMax = 0;
  }
  if (zMin > 5000) {
    zMin = 0;
  }

  // Add sample stats to global arrays
  snowDepthStatsAvg.add(zAvg);
  snowDepthStatsStd.add(zStd);
  snowDepthStatsMax.add(zMax);
  snowDepthStatsMin.add(zMin);
  snowDepthStatsNan.add(zNan);

  // Add to sample variables
  snowDepthAvg = zAvg;
  snowDepthStd = zStd;
  snowDepthMax = zMax;
  snowDepthMin = zMin;
  snowDepthNan = zNan;

  Serial.print(snowDepthAvg); Serial.print("\t");
  Serial.print(snowDepthStd); Serial.print("\t");
  Serial.print(snowDepthMax); Serial.print("\t");
  Serial.print(snowDepthMin); Serial.print("\t");
  Serial.println(snowDepthNan);

  // Clear local array
  Maxbotix.clear();

  // Sleep sensor
  digitalWrite(PIN_5V_EN, LOW);
  delay(5000);
}

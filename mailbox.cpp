#include <Wire.h>
#include <Adafruit_VL53L0X.h>

// --------------------
// SENSOR A PINS
// --------------------
#define SCL_A 27
#define SDA_A 26
#define XSHUT_A 32

// --------------------
// SENSOR B PINS
// --------------------
#define SCL_B 18
#define SDA_B 19
#define XSHUT_B 5

// Create two separate I2C buses
TwoWire I2C_A = TwoWire(0);
TwoWire I2C_B = TwoWire(1);

// Create the two sensors
Adafruit_VL53L0X sensorA;
Adafruit_VL53L0X sensorB;

void setup()
{
  Serial.begin(115200);
  delay(1000);

  Serial.println("Starting two sensor test...");

  // --------------------
  // SET UP XSHUT
  // --------------------

  pinMode(XSHUT_A, OUTPUT);
  pinMode(XSHUT_B, OUTPUT);

  // Turn both sensors OFF
  digitalWrite(XSHUT_A, LOW);
  digitalWrite(XSHUT_B, LOW);

  delay(100);

  // --------------------
  // START I2C BUS A
  // --------------------

  I2C_A.begin(SDA_A, SCL_A);

  // Turn Sensor A ON
  digitalWrite(XSHUT_A, HIGH);
  delay(100);

  Serial.println("Starting Sensor A...");

  if (!sensorA.begin(0x29, false, &I2C_A))
  {
    Serial.println("ERROR: Sensor A failed to start!");

    while (1)
    {
      delay(10);
    }
  }

  Serial.println("SUCCESS: Sensor A ready!");

  // --------------------
  // START I2C BUS B
  // --------------------

  I2C_B.begin(SDA_B, SCL_B);

  // Turn Sensor B ON
  digitalWrite(XSHUT_B, HIGH);
  delay(100);

  Serial.println("Starting Sensor B...");

  if (!sensorB.begin(0x29, false, &I2C_B))
  {
    Serial.println("ERROR: Sensor B failed to start!");

    while (1)
    {
      delay(10);
    }
  }

  Serial.println("SUCCESS: Sensor B ready!");

  Serial.println();
  Serial.println("BOTH SENSORS READY!");
  Serial.println();
}

void loop()
{
  VL53L0X_RangingMeasurementData_t measurementA;
  VL53L0X_RangingMeasurementData_t measurementB;

  // Read Sensor A
  sensorA.rangingTest(&measurementA, false);

  // Read Sensor B
  sensorB.rangingTest(&measurementB, false);

  // --------------------
  // PRINT SENSOR A
  // --------------------

  Serial.print("A: ");

  if (measurementA.RangeStatus != 4)
  {
    Serial.print(measurementA.RangeMilliMeter);
    Serial.print(" mm");
  }
  else
  {
    Serial.print("OUT OF RANGE");
  }

  // --------------------
  // PRINT SENSOR B
  // --------------------

  Serial.print("     B: ");

  if (measurementB.RangeStatus != 4)
  {
    Serial.print(measurementB.RangeMilliMeter);
    Serial.println(" mm");
  }
  else
  {
    Serial.println("OUT OF RANGE");
  }

  delay(100);
}
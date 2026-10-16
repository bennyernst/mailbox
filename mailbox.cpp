
#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_VL53L0X.h>
#include "mailbox.h"
#include "arduino_secrets.h"
#include "discord.h"

//TODO: replace EMPTY_DISTANCE with variable distance that gets empty mailbox from average after first few run seconds.
#define EMPTY_DISTANCE 160
#define DISTANCE_THRESHOLD 50

// Connections
#define SDA_A 26
#define SCL_A 27
#define XSHUT_A 32

#define SDA_B 19
#define SCL_B 18
#define XSHUT_B 17

// Check if mailbox is empty/filled
bool isEmpty = true;

// Distance Saver
int distSaveA = -1;
int distSaveB = -1;
// ========================================
// SENSOR PINS
// ========================================

// Two independent I2C buses
TwoWire I2C_A = TwoWire(0);
TwoWire I2C_B = TwoWire(1);

Adafruit_VL53L0X sensorA;
Adafruit_VL53L0X sensorB;

int distanceA = -1;
int distanceB = -1;

// ========================================
// SETUP
// ========================================

void appSetup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Starting BergLink...");

  // Turn both sensors off
  pinMode(XSHUT_A, OUTPUT);
  pinMode(XSHUT_B, OUTPUT);

  digitalWrite(XSHUT_A, LOW);
  digitalWrite(XSHUT_B, LOW);
  delay(100);

  // Start Sensor A
  I2C_A.begin(SDA_A, SCL_A);
  digitalWrite(XSHUT_A, HIGH);
  delay(100);

  Serial.println("Starting Sensor A...");

  if (!sensorA.begin(0x29, false, &I2C_A)) {
    Serial.println("ERROR: Sensor A failed!");
    while (1) { delay(10); }
  }

  Serial.println("Sensor A ready!");

  // Start Sensor B
  I2C_B.begin(SDA_B, SCL_B);
  digitalWrite(XSHUT_B, HIGH);
  delay(100);

  Serial.println("Starting Sensor B...");

  if (!sensorB.begin(0x29, false, &I2C_B)) {
    Serial.println("ERROR: Sensor B failed!");
    while (1) { delay(10); }
  }

  Serial.println("Sensor B ready!");


  // Discord hook
  connectWIFI();
  sendDiscord("WEBHOOK CONNECTED!");
}

// ========================================
// LOOP
// ========================================

void appLoop() {
  VL53L0X_RangingMeasurementData_t measurementA;
  VL53L0X_RangingMeasurementData_t measurementB;

  // Read Sensor A
  sensorA.rangingTest(&measurementA, false);

  if (measurementA.RangeStatus != 4) {
    distanceA = measurementA.RangeMilliMeter;
  } else {
    distanceA = -1;
  }

  // Read Sensor B
  sensorB.rangingTest(&measurementB, false);

  if (measurementB.RangeStatus != 4) {
    distanceB = measurementB.RangeMilliMeter;
  } else {
    distanceB = -1;
  }

// FIXME
  if (distanceA > 0 && distanceA < (EMPTY_DISTANCE - DISTANCE_THRESHOLD) &&
      distanceB > 0 && distanceB < (EMPTY_DISTANCE - DISTANCE_THRESHOLD)) {
        distSaveA = distanceA;
        distSaveB = distanceB;
        if (isEmpty) {
            sendDiscord("MAIL DETECTED!");
        }
        isEmpty = false;
      }

// MAIL PUT IN
  // if A detected
    //if B detected
      // set isEmpty = false
      // 

// MAIL PUT OUT
  // if B detected
    //if A detected
      // set isEmpty = true

//EDGE CASE: hand goes in to grab mail

//FIXME
  if (distSaveA > 0 && distanceA >= (EMPTY_DISTANCE - DISTANCE_THRESHOLD) &&
      distSaveB > 0 && distanceB >= (EMPTY_DISTANCE - DISTANCE_THRESHOLD)) {
        if (!isEmpty) {
          sendDiscord("MAIL EMPTIED.");
          isEmpty = true;
        }
      }

  Serial.printf("A: %d mm     B: %d mm\n", distanceA, distanceB);


  delay(250);
}

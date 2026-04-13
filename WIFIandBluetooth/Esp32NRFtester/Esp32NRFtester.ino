#include <SPI.h>
#include <RF24.h>

// CE, CSN pins
#define CE_PIN 4
#define CSN_PIN 5

RF24 radio(CE_PIN, CSN_PIN); // Create radio object

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting nRF24L01 test 𒉭 𒉭 𒉭");

  if (!radio.begin()) {
    Serial.println("nRF24L01 not detected. Check wiring.");
  } else {
    Serial.println("nRF24L01 successfully initialized! yayy  𒉭 :) 𒉭");
    radio.printDetails(); // Print module details
  }
}

void loop() {
  // nothing to do
}

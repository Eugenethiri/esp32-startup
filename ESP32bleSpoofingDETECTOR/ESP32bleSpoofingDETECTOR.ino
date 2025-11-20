#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h> 

class PayloadSniffer : public BLEAdvertisedDeviceCallbacks { // how comes we define a class here? 
  void onResult(BLEAdvertisedDevice device) { 
    Serial.println("------;BLE PKT------");

    Serial.println("MAC"); Serial.print(device.getAddress().toString().c_str()); // that was a mouthfull
    Serial.println("RSSI"); Serial.println(device.getRSSI());

    const uint8_t* payload = device.getPayload();
    int len = device.getPayloadLength();

    Serial.print("payload [HEX]:");
    for(int i = 0; i < len ; i++){
      printf("%02X ", (uint8_t)payload[i]);
    }
    Serial.println("\n");

  }
};


void setup() {
  Serial.begin(115200);
  BLEDevice::init("");
  BLEScan *scan = BLEDevice::getScan();
  scan->setAdvertisedDeviceCallbacks(new PayloadSniffer(), true);
  scan->setActiveScan(true);
  scan->start(0, nullptr, false);
}

void loop() {
  // put your main code here, to run repeatedly:

}

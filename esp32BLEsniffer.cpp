//Untested till i get home

#include <BLEDevice.h> //Handle ble clients aswell as set up the server, basically setting up the ble stack, main controller for ble operations
#include <BLEUtils.h> // Handle ble operations, need to get more practical with this to understand it better
#include <BLEScan.h> // Set of tools for managing scnanning functions 
#include <BLEAdvertisedDevice.h> // Able to get back the results form the scan from advertising devices or client

int scanTime = 5;  // Scan duration in seconds

//An extended class from BLEAdvertisedDeviceCallbacks  to handle the ble scan and provide feedback based of the scan
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        Serial.print("Found BLE Device: ");
        Serial.println(advertisedDevice.toString().c_str());

        // Get and print MAC address
        BLEAddress deviceAddress = advertisedDevice.getAddress();
        Serial.print("MAC Address: ");
        Serial.println(deviceAddress.toString().c_str());

        // Get RSSI (signal strength)
        Serial.print("RSSI: ");
        Serial.println(advertisedDevice.getRSSI());

        // Get device name (if available)
        if (advertisedDevice.haveName()) {
            Serial.print("Device Name: ");
            Serial.println(advertisedDevice.getName().c_str());
        }

        // Get advertised service UUIDs
        if (advertisedDevice.haveServiceUUID()) {
            Serial.print("Service UUIDs: ");
            Serial.println(advertisedDevice.getServiceUUID().toString().c_str());
        }

        Serial.println("  ███▒▒▒▒▒▒▒  ");
    }
};

void setup() {
    Serial.begin(115200);
    Serial.println("Starting BLE Scanner...");
    Serial.print("▄︻デ══━一 ")
    Serial.println("⌖ ⌖ ⌖ ⌖ ⌖ ⌖ ⌖ ⌖")

    // Initialize BLE
    BLEDevice::init("ESP32_BLE_Sniffer"); //not sure, clever guess this is the UUID

    // Get the BLE scanner instance
    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());

    // Set scan parameters (active mode = true for better detection) >> EXPLAIN MORE ABOUT THIS PART
    pBLEScan->setActiveScan(true);
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);
}

// loop to call all the classes and functions defined
void loop() {
    Serial.println("Scanning...");
    BLEScan* pBLEScan = BLEDevice::getScan();
    BLEScanResults results = pBLEScan->start(scanTime, false);
    
    Serial.print("Devices Found: ");
    Serial.println(results.getCount());

    Serial.println("Scan Complete.");
    Serial.println("⛦=============================⛦");

    delay(5000);  // Wait 5000 ms before scanning again
}

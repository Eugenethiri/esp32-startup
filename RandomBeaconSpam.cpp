// bla bla bla unfinished v0.0.1


#include "WiFi.h"

extern "C"{
#include "esp_wifi.h"
 esp_err_t esp_wifi_set_channel(uint8_t primary, wifi_second_chan_t second);
  esp_err_t esp_wifi_80211_tx(wifi_interface_t ifx, const void *buffer, int len, bool en_sys_seq);
}

const uint8_t channels[] = {1, 6, 11};
const bool wpa2= true;
const bool appendSpaces = true; //odd but makes the ssid all 32 characters
char emptySSID[32];
uint8_t channelIndex = 0;
uint8_t macAddr[6];
uint8_t wifi_channel = 1;
uint32_t currentTime = 0;
uint32_t packetSize = 0;
uint32_t packetCounter = 0;
uint32_t attackTime = 0;
uint32_t packetRateTime = 0;

// beacon frame definition
uint8_t beaconPacket[109] = {
  /*  0 - 3  */ 0x80, 0x00, 0x00, 0x00, // Type/Subtype: managment beacon frame
  /*  4 - 9  */ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Destination: broadcast
  /* 10 - 15 */ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // Source
  /* 16 - 21 */ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // Source

  // Fixed parameters
  /* 22 - 23 */ 0x00, 0x00, // Fragment & sequence number (will be done by the SDK)
  /* 24 - 31 */ 0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00, // Timestamp
  /* 32 - 33 */ 0xe8, 0x03, // Interval: 0x64, 0x00 => every 100ms - 0xe8, 0x03 => every 1s
  /* 34 - 35 */ 0x31, 0x00, // capabilities Tnformation

  // Tagged parameters

  // SSID parameters
  /* 36 - 37 */ 0x00, 0x20, // Tag: Set SSID length, Tag length: 32
  /* 38 - 69 */ 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, // SSID

  // Supported Rates
  /* 70 - 71 */ 0x01, 0x08, // Tag: Supported Rates, Tag length: 8
  /* 72 */ 0x82, // 1(B)
  /* 73 */ 0x84, // 2(B)
  /* 74 */ 0x8b, // 5.5(B)
  /* 75 */ 0x96, // 11(B)
  /* 76 */ 0x24, // 18
  /* 77 */ 0x30, // 24
  /* 78 */ 0x48, // 36
  /* 79 */ 0x6c, // 54

  // Current Channel
  /* 80 - 81 */ 0x03, 0x01, // Channel set, length
  /* 82 */      0x01,       // Current Channel

  // RSN information
  /*  83 -  84 */ 0x30, 0x18,
  /*  85 -  86 */ 0x01, 0x00,
  /*  87 -  90 */ 0x00, 0x0f, 0xac, 0x02,
  /*  91 -  92 */ 0x02, 0x00,
  /*  93 - 100 */ 0x00, 0x0f, 0xac, 0x04, 0x00, 0x0f, 0xac, 0x04, /*Fix: changed 0x02(TKIP) to 0x04(CCMP) is default. WPA2 with TKIP not supported by many devices*/
  /* 101 - 102 */ 0x01, 0x00,
  /* 103 - 106 */ 0x00, 0x0f, 0xac, 0x02,
  /* 107 - 108 */ 0x00, 0x00
};

//l00p through the 14 chanels might make it 13 coz we arent in Japan with channel 14
void nextChannel() {
  if (sizeof(channels) > 1) {
    uint8_t ch = channels[channelIndex];
    channelIndex++;
    if (channelIndex > sizeof(channels)) channelIndex = 0;

    if (ch != wifi_channel && ch >= 1 && ch <= 14) {
      wifi_channel = ch;
      //wifi_set_channel(wifi_channel);
      esp_wifi_set_channel(wifi_channel, WIFI_SECOND_CHAN_NONE);
    }
  }
}

// generates random MAC
void randomMac() {
  for (int i = 0; i < 6; i++)
    macAddr[i] = random(256);
}


// Define the alphanumeric character set to be used @Random()
static const char alphanum[] =
    "0123456789"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz";

// Function to generate a random SSID of a given length
String generateRandomSSID(int length) {
  //sets a cap onto the length if user exceeds it
    if (length > 32) {
        length = 32;
    }
    String ssid = "";
    for (int i = 0; i < length; i++) {
        int randomIndex = random(0, sizeof(alphanum) - 2); // Pick a random index (exclude null terminator)
        ssid += alphanum[randomIndex]; // Append the character to the SSID
    }
    return ssid;
}

// static of SSIDs to generate willl make it dynamic later
const int numSSIDs = 10;

// Array to store the generated SSIDs
String ssidArray[numSSIDs];

// Function to generate and store SSIDs
void generateSSIDs() {
    for (int i = 0; i < numSSIDs; i++) {
        ssidArray[i] = generateRandomSSID(10); // Generate a 10-character SSID later on a more dynamic one who knows
    }
}

// Function to print the generated SSIDs
void printSSIDs() {
    for (int i = 0; i < numSSIDs; i++) {
        Serial.println(ssidArray[i]); // Print each SSID to the Serial Monitor
    }
}

void setup() {
    Serial.begin(115200); // Initialize serial communication
    randomSeed(analogRead(0)); // Uses the fluctuating analog reading as a seed to ensure unique random sequences.

    generateSSIDs(); // Generate the SSIDs
    printSSIDs(); // Print the SSIDs to the Serial Monitor
}

void loop() {   
 // Ill do it later so lazy and tired , will finish with a sober mind main functions are done tho
  currentTime = millis();

  // send out SSIDs
  if (currentTime - attackTime > 100) {
    attackTime = currentTime;

    // temp variables
    int i = 0;
    int j = 0;
    int ssidNum = 1;
    char tmp;
    int ssidsLen = strlen_P(ssids);
    bool sent = false;

    // go to next channel
    nextChannel();

    while (i < ssidsLen) {
      // read out next SSID
      j = 0;
      do {
        tmp = pgm_read_byte(ssids + i + j);
        j++;
      } while (tmp != '\n' && j <= 32 && i + j < ssidsLen);

      uint8_t ssidLen = j - 1;

      // set MAC address
      macAddr[5] = ssidNum;
      ssidNum++;

      // write MAC address into beacon frame
      memcpy(&beaconPacket[10], macAddr, 6);
      memcpy(&beaconPacket[16], macAddr, 6);

      // reset SSID
      memcpy(&beaconPacket[38], emptySSID, 32);

      // write new SSID into beacon frame
      memcpy_P(&beaconPacket[38], &ssids[i], ssidLen);

      // set channel for beacon frame
      beaconPacket[82] = wifi_channel;

      // send packet
      if (appendSpaces) {
        for (int k = 0; k < 3; k++) {
          //packetCounter += wifi_send_pkt_freedom(beaconPacket, packetSize, 0) == 0;
          //Serial.printf("size: %d \n", packetSize);
          packetCounter += esp_wifi_80211_tx(WIFI_IF_STA, beaconPacket, packetSize, 0) == 0;
          delay(1);
        }
      }

      // remove spaces
      else {
        uint16_t tmpPacketSize = (109 - 32) + ssidLen; // calc size
        uint8_t* tmpPacket = new uint8_t[tmpPacketSize]; // create packet buffer
        memcpy(&tmpPacket[0], &beaconPacket[0], 37 + ssidLen); // copy first half of packet into buffer
        tmpPacket[37] = ssidLen; // update SSID length byte
        memcpy(&tmpPacket[38 + ssidLen], &beaconPacket[70], 39); // copy second half of packet into buffer

        // send packet
        for (int k = 0; k < 3; k++) {
          //packetCounter += wifi_send_pkt_freedom(tmpPacket, tmpPacketSize, 0) == 0;
          packetCounter += esp_wifi_80211_tx(WIFI_IF_STA, tmpPacket, tmpPacketSize, 0) == 0;
          delay(1);
        }

        delete tmpPacket; // free memory of allocated buffer
      }

      i += j;
   
}


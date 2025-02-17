#include "freertos/FreeRTOS.h" // OS used by ESP32
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_system.h"
#include "esp_event.h"
#include "nvs_flash.h" // Non-volatile storage for WiFi credentials
#include "driver/gpio.h"
#include "esp_netif.h" // Replaces tcpip_adapter_init()

// Pin and LED init
//
#define LED_GPIO_PIN                     5
#define WIFI_CHANNEL_SWITCH_INTERVAL  (500)   // Interval for switching WiFi channels
#define WIFI_CHANNEL_MAX               (13)  // Maximum channel (1-13)

uint8_t level = 0, channel = 1; // 0 stands for OFF, 1 for ON

// Went with China this time (most relatable regulations)
static wifi_country_t wifi_country = {.cc="CN", .schan = 1, .nchan = 13}; 

typedef struct {
  unsigned frame_ctrl:16; // Frame type control 
  unsigned duration_id:16; // Duration ID and Request-to-Send (RTS) or Clear-to-Send (CTS) frames (CSMA/CA)
  uint8_t addr1[6]; /* Receiver address */
  uint8_t addr2[6]; /* Sender address */
  uint8_t addr3[6]; /* Filtering address */
  unsigned sequence_ctrl:16;
  uint8_t addr4[6]; /* Optional */
} wifi_ieee80211_mac_hdr_t;

typedef struct {
  wifi_ieee80211_mac_hdr_t hdr; // Reference the top frame configurations
  uint8_t payload[0]; /* Network data ended with 4 bytes checksum (CRC32) */
} wifi_ieee80211_packet_t;

// Init configs
static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
static void wifi_sniffer_init(void);
static void wifi_sniffer_set_channel(uint8_t channel);
static const char *wifi_sniffer_packet_type2str(wifi_promiscuous_pkt_type_t type);
static void wifi_sniffer_packet_handler(void *buff, wifi_promiscuous_pkt_type_t type);

// ESP32 WiFi event handler (updated to latest ESP32 version)
static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
  // Currently not handling events, but this is where event-based logic could be added
}

void wifi_sniffer_init(void) {
  nvs_flash_init();
  esp_netif_init(); // Replaces tcpip_adapter_init()
  ESP_ERROR_CHECK(esp_event_loop_create_default()); // Replaces esp_event_loop_init()

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  ESP_ERROR_CHECK(esp_wifi_set_country(&wifi_country)); /* Set country for channel range [1, 13] */
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_NULL));
  ESP_ERROR_CHECK(esp_wifi_start());

  // Register event handler
  ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));

  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(&wifi_sniffer_packet_handler);
}

/* Need better channel hopping 
void channel_hop_task(void *pvParameter) {
    uint8_t channel = 1;
    while (true) {
        esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
        vTaskDelay(WIFI_CHANNEL_SWITCH_INTERVAL / portTICK_PERIOD_MS);
        channel = (channel % WIFI_CHANNEL_MAX) + 1;  // Loop from 1 to 13
    }
} 
*/

void wifi_sniffer_set_channel(uint8_t channel) {
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
}

const char * wifi_sniffer_packet_type2str(wifi_promiscuous_pkt_type_t type) {
  switch(type) {
  case WIFI_PKT_MGMT: return "MGMT";   // Case for Management frames
  case WIFI_PKT_DATA: return "DATA";   // Case for Data frames
  default:  
  case WIFI_PKT_MISC: return "MISC";   // Case for Miscellaneous frames (default case)
  }
}

// Strictly avoids non-management frames
void wifi_sniffer_packet_handler(void* buff, wifi_promiscuous_pkt_type_t type) {
  if (type != WIFI_PKT_MGMT)
    return;

  const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)buff;
  const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload;
  const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr;

  printf("PACKET TYPE=%s, CHAN=%02d, RSSI=%02d,"
    " ADDR1=%02x:%02x:%02x:%02x:%02x:%02x,"
    " ADDR2=%02x:%02x:%02x:%02x:%02x:%02x,"
    " ADDR3=%02x:%02x:%02x:%02x:%02x:%02x\n",
    wifi_sniffer_packet_type2str(type),
    ppkt->rx_ctrl.channel,
    ppkt->rx_ctrl.rssi,
    /* ADDR1 */
    hdr->addr1[0], hdr->addr1[1], hdr->addr1[2],
    hdr->addr1[3], hdr->addr1[4], hdr->addr1[5],
    /* ADDR2 */
    hdr->addr2[0], hdr->addr2[1], hdr->addr2[2],
    hdr->addr2[3], hdr->addr2[4], hdr->addr2[5],
    /* ADDR3 */
    hdr->addr3[0], hdr->addr3[1], hdr->addr3[2],
    hdr->addr3[3], hdr->addr3[4], hdr->addr3[5]
  );
}

// The setup function runs once when you press reset or power the board
void setup() {
  // Initialize digital pin 5 as an output.
  Serial.begin(115200);
  delay(10);
  wifi_sniffer_init();
  pinMode(LED_GPIO_PIN, OUTPUT);
}

// The loop function runs over and over again forever
void loop() {
  // Serial.print("inside loop");
  delay(1000); // Wait for a second

  if (digitalRead(LED_GPIO_PIN) == LOW)
    digitalWrite(LED_GPIO_PIN, HIGH);
  else
    digitalWrite(LED_GPIO_PIN, LOW);

  vTaskDelay(WIFI_CHANNEL_SWITCH_INTERVAL / portTICK_PERIOD_MS);
  wifi_sniffer_set_channel(channel);
  channel = (channel % WIFI_CHANNEL_MAX) + 1;
}

/*
TODO:
- Implement accurate timing, especially for PMKID capturing
- Consider adding management frame processing in another program or allowing sniffer type selection (normal frames/mgmt)
*/

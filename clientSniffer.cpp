void wifi_sniffer_callback(void* buf, wifi_promiscuous_pkt_type_t type) {
    wifi_promiscuous_pkt_t *pkt = (wifi_promiscuous_pkt_t*)buf;
    const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t*)pkt->payload;
    const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr;

    // Extract source, destination, and BSSID MAC addresses
    Serial.printf("Packet Type: %d\n", type);
    Serial.printf("Src MAC: %02X:%02X:%02X:%02X:%02X:%02X\n", 
        hdr->addr2[0], hdr->addr2[1], hdr->addr2[2], 
        hdr->addr2[3], hdr->addr2[4], hdr->addr2[5]);

    Serial.printf("Dest MAC: %02X:%02X:%02X:%02X:%02X:%02X\n", 
        hdr->addr1[0], hdr->addr1[1], hdr->addr1[2], 
        hdr->addr1[3], hdr->addr1[4], hdr->addr1[5]);

    Serial.printf("BSSID: %02X:%02X:%02X:%02X:%02X:%02X\n", 
        hdr->addr3[0], hdr->addr3[1], hdr->addr3[2], 
        hdr->addr3[3], hdr->addr3[4], hdr->addr3[5]);

    Serial.println("-------------------------");
}

void printError(esp_err_t err) {
    switch (err) {
        case ESP_OK:
            Serial.println("Success.");
            break;
        case ESP_ERR_WIFI_NOT_INIT:
            Serial.println("Error: WiFi not initialized.");
            break;
        case ESP_ERR_WIFI_NOT_STARTED:
            Serial.println("Error: WiFi not started.");
            break;
        default:
            Serial.printf("Unknown error code: %d\n", err);
    }
}

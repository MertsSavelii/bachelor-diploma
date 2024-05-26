#pragma once

#include <WiFi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "esp_log.h"
#include "mqtt_client.h"
#include "ThingsBoard.h"

const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

const char* mqtt_server = "your_thingsboard_server";
const int mqtt_port = 1883;
const char* access_token = "your_access_token";

void WiFiEvent(WiFiEvent_t event) {
    switch(event) {
        case SYSTEM_EVENT_STA_GOT_IP:
            ESP_LOGI("WiFiEvent", "Got IP: %s", WiFi.localIP().toString().c_str());
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            ESP_LOGI("WiFiEvent", "Disconnected, retrying...");
            WiFi.begin(ssid, password);
            break;
        default:
            break;
    }
}

void initWiFi() {
    WiFi.onEvent(WiFiEvent);
    WiFi.begin(ssid, password);
    ESP_LOGI("WiFi", "Connecting to WiFi...");
}

esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event) {
    esp_mqtt_client_handle_t client = event->client;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI("MQTT", "Connected to MQTT Broker");
            esp_mqtt_client_subscribe(client, "v1/devices/me/attributes", 0);
            esp_mqtt_client_subscribe(client, "v1/devices/me/rpc/request/+", 0);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI("MQTT", "Disconnected from MQTT Broker");
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI("MQTT", "Received data: %.*s", event->data_len, event->data);
            break;
        default:
            break;
    }
    return ESP_OK;
}

esp_mqtt_client_handle_t initMQTT() {
    esp_mqtt_client_config_t mqtt_cfg = {};
    mqtt_cfg.uri = mqtt_server;
    mqtt_cfg.port = mqtt_port;
    mqtt_cfg.username = access_token;
    mqtt_cfg.event_handle = mqtt_event_handler;

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(client);

    return client;
}

void telemetryTask(void* pvParameters) {
    esp_mqtt_client_handle_t client = (esp_mqtt_client_handle_t)pvParameters;

    while (true) {
        const char* telemetry = "{\"temperature\": 25.0, \"humidity\": 70.0}";
        esp_mqtt_client_publish(client, "v1/devices/me/telemetry", telemetry, 0, 1, 0);
        vTaskDelay(10000 / portTICK_PERIOD_MS);  // Send every 10 seconds
    }
}

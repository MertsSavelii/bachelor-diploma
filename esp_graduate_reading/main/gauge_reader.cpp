#include "image_capture.hpp"
#include "image_preprocessing.hpp"
#include "needle_detection.hpp"
#include "value_extraction.hpp"
#include "connection_manager.hpp"

void captureAndProcessTask(void* pvParameters) {
    esp_mqtt_client_handle_t client = (esp_mqtt_client_handle_t)pvParameters;

    while (true) {
        camera_fb_t* fb = capture_image();
        if (!fb) {
            ESP_LOGE("Capture", "Failed to capture image");
            vTaskDelay(10000 / portTICK_PERIOD_MS);
            continue;
        }

        auto processedImage = preprocess_image(fb, BlurRadius_, Threshold_);

        auto segmentedArrow = segment_arrow(processedImage, BackModel_, DialateRad_, ErosionRad_);

        float value = extract_value(segmentedArrow, CentrX_, CentrY_, StartAngle_, AngularIncreas_);

        esp_camera_fb_return(fb);

        char telemetry[128];
        snprintf(telemetry, sizeof(telemetry), "{\"gauge_value\": %.2f}", value);
        esp_mqtt_client_publish(client, "v1/devices/me/telemetry", telemetry, 0, 1, 0);

        vTaskDelay(10000 / portTICK_PERIOD_MS);  // Отправка каждые 10 секунд
    }
}

void main() {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    initWiFi();

    while (WiFi.status() != WL_CONNECTED) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    esp_mqtt_client_handle_t mqttClient = initMQTT();

    xTaskCreate(&captureAndProcessTask, "Capture and Process Task", 8192, (void*)mqttClient, 5, NULL);
}
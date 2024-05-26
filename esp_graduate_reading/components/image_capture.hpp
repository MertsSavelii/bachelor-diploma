#pragma once

#include "esp_camera.h"
#include "esp_timer.h"
#include "esp32-hal-ledc.h"

#define CAMERA_MODEL_AI_THINKER

#define CONFIG_LED_ILLUMINATOR_ENABLED 1

#if CONFIG_LED_ILLUMINATOR_ENABLED
#define LED_LEDC_CHANNEL 2
#define CONFIG_LED_MAX_INTENSITY 255
int led_duty = 0;
bool isStreaming = false;
#endif

#include "camera_pins.h"

#define IMAGE_RESOLUTION  FRAMESIZE_SVGA // 800x600
#define IMAGE_TYPE PIXFORMAT_GRAYSCALE

void setupLedFlash(int pin) 
{
    #if CONFIG_LED_ILLUMINATOR_ENABLED
    ledcSetup(LED_LEDC_CHANNEL, 5000, 8);
    ledcAttachPin(pin, LED_LEDC_CHANNEL);
    #else
    Serial.printf("LED flash is disabled -> CONFIG_LED_ILLUMINATOR_ENABLED = 0");
    #endif
}

bool init_camera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = IMAGE_TYPE; 
  config.frame_size = IMAGE_RESOLUTION;
  config.jpeg_quality = 10;
  config.fb_count = 1;


  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return false;
  }
#if defined(LED_GPIO_NUM)
  setupLedFlash(LED_GPIO_NUM);
#endif

  return true;
}

#if CONFIG_LED_ILLUMINATOR_ENABLED
void enable_led(bool en)
{
    int duty = en ? led_duty : 0;
    if (en && isStreaming && (led_duty > CONFIG_LED_MAX_INTENSITY))
    {
        duty = CONFIG_LED_MAX_INTENSITY;
    }
    ledcWrite(LED_LEDC_CHANNEL, duty);
    log_i("Set LED intensity to %d", duty);
}
#endif

camera_fb_t* capture_image() {
#if CONFIG_LED_ILLUMINATOR_ENABLED
    enable_led(true);
    vTaskDelay(150 / portTICK_PERIOD_MS);
    fb = esp_camera_fb_get();
    enable_led(false);
#else
    fb = esp_camera_fb_get();
#endif
  if (!fb) {
    Serial.println("Camera capture failed");
    return NULL;
  }
  return fb;
}

void free_cam_fb(camera_fb_t* fb) {
  esp_camera_fb_return(fb);
}

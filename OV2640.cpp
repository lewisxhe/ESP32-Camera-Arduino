#include "OV2640.h"

#define CAMERA_PIXEL_FORMAT CAMERA_PF_GRAYSCALE
#define CAMERA_FRAME_SIZE CAMERA_FS_SVGA

void OV2640::run(void)
{
    camera_run();
}

size_t OV2640::getSize(void)
{
    return camera_get_data_size();
}

uint8_t *OV2640::getfb(void)
{
    return camera_get_fb();
}

esp_err_t OV2640::init(camera_config_t config)
{
    // Wire.begin(config->pin_sscb_sda,config->pin_sscb_scl);

    camera_model_t camera_model;
    esp_err_t err = camera_probe(&config, &camera_model);
    if (err != ESP_OK)
    {
        Serial.printf("Camera probe failed with error 0x%x", err);
        return err;
    }

    if (camera_model == CAMERA_OV7725)
    {
        _pixel_format = CAMERA_PIXEL_FORMAT;
        config.frame_size = CAMERA_FRAME_SIZE;
        Serial.printf("Detected OV7725 camera, using %s bitmap format",
                      CAMERA_PIXEL_FORMAT == CAMERA_PF_GRAYSCALE ? "grayscale" : "RGB565");
    }
    else if (camera_model == CAMERA_OV2640)
    {
        Serial.printf("Detected OV2640 camera, using JPEG format");
        _pixel_format = CAMERA_PF_JPEG;
        config.frame_size = CAMERA_FRAME_SIZE;
        config.jpeg_quality = 15;
    }
    else
    {
        Serial.printf("Camera not supported");
        return ESP_ERR_CAMERA_NOT_SUPPORTED;
    }

#define CAMERA_LED_GPIO GPIO_NUM_16

    ESP_ERROR_CHECK(gpio_install_isr_service(0));

    // blink led
    // gpio_set_direction(CAMERA_LED_GPIO, GPIO_MODE_OUTPUT);
    // gpio_set_level(CAMERA_LED_GPIO, 1);

    config.pixel_format = _pixel_format;
    err = camera_init(&config);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Camera init failed with error 0x%x", err);
        return err;
    }

    return ESP_OK;
}

int OV2640::getWidth(void)
{
    return camera_get_fb_width();
}

int OV2640::getHeight(void)
{
    return camera_get_fb_height();
}
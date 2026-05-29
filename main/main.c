#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <bmp280.h>

#define I2C_MASTER_SDA 21
#define I2C_MASTER_SCL 22
#define BUTTON_GPIO GPIO_NUM_4


bmp280_t bmp280;

inline bool is_bme280p() {
    return bmp280.id == BME280_CHIP_ID;
}

void init_bmp280(void) {
    printf("Procurando por bme/bmp280...\n");

    bmp280_params_t params;
    bmp280_init_default_params(&params);

    esp_err_t err = bmp280_init_desc(&bmp280, BMP280_I2C_ADDRESS_0, 0, I2C_MASTER_SDA, I2C_MASTER_SCL);
    ESP_ERROR_CHECK(err);

    err = bmp280_init(&bmp280, &params);
    ESP_ERROR_CHECK(err);

    printf("BMP280: encontrado %s\n", is_bme280p() ? "BME280" : "BMP280");
}

void setup(void) {
    ESP_ERROR_CHECK(i2cdev_init());
    init_bmp280();
    

    // 1. Configure the GPIO pin
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BUTTON_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE, // Use internal pull-up if button connects to GND
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    
}

void app_main(void) {
    setup();
    int buckets = 0;
    float pressure, temperature, humidity;
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(500));

        if (bmp280_read_float(&bmp280, &temperature, &pressure, &humidity) != ESP_OK) {
            printf("Leitura do BMP280 falhou\n");
            continue;
        }

       // printf("Pressao: %.2f Pa, Temperatura: %.2f C", pressure, temperature);
        if (is_bme280p()) {
        //    printf(", Umidade: %.2f", humidity);
        }
        int button_state = gpio_get_level(BUTTON_GPIO);
        if (button_state == 0) {
            printf("Button is pressed!\n");
        } else {
            printf("Button is released.\n");
        }
        //printf("\n");
    }
}

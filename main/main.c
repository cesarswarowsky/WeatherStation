#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LED_GPIO 5

void app_main(void)
{
    // Configurar GPIO 5 como saída
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LED_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);

    printf("LED teste na GPIO 5 iniciado!\n");

    // Loop infinito para piscar o LED
    while(1) {
        // LED ligado
        gpio_set_level(LED_GPIO, 1);
        printf("LED ligado\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);  // 1 segundo

        // LED desligado
        gpio_set_level(LED_GPIO, 0);
        printf("LED desligado\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);  // 1 segundo
    }
}
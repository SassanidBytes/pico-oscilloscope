#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

int main() {
    stdio_init_all();
    sleep_ms(2000);

    printf("Pico Oscilloscope - ADC ready\n");
    
    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);

    while (true) {
        uint16_t raw = adc_read();
        float voltage = raw * 3.3f / 4095.0f;
        printf("ADC: %d = %.3fV\n", raw, voltage);
        sleep_ms(500);
    }

    return 0;
}
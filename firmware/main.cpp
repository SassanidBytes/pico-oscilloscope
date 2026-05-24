#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/dma.h"

#define SAMPLE_COUNT 1024
#define SAMPLE_RATE  100000
#define START_MARKER 0xABCD
#define END_MARKER   0xDCBA

uint16_t samples[SAMPLE_COUNT];

void capture_samples() {
    adc_fifo_setup(true, true, 1, false, false);
    adc_set_clkdiv(48000000.0f / SAMPLE_RATE);

    int dma_chan = dma_claim_unused_channel(true);
    dma_channel_config cfg = dma_channel_get_default_config(dma_chan);
    channel_config_set_transfer_data_size(&cfg, DMA_SIZE_16);
    channel_config_set_read_increment(&cfg, false);
    channel_config_set_write_increment(&cfg, true);
    channel_config_set_dreq(&cfg, DREQ_ADC);

    dma_channel_configure(dma_chan, &cfg, samples, &adc_hw->fifo, SAMPLE_COUNT, true);

    adc_run(true);
    dma_channel_wait_for_finish_blocking(dma_chan);
    adc_run(false);
    adc_fifo_drain();
    dma_channel_unclaim(dma_chan);
}

void send_samples() {
    // Send start marker
    uint16_t start = START_MARKER;
    fwrite(&start, sizeof(uint16_t), 1, stdout);

    // Send all samples as binary
    fwrite(samples, sizeof(uint16_t), SAMPLE_COUNT, stdout);

    // Send end marker
    uint16_t end = END_MARKER;
    fwrite(&end, sizeof(uint16_t), 1, stdout);

    fflush(stdout);
}

int main() {
    stdio_init_all();
    sleep_ms(2000);

    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);

    while (true) {
        capture_samples();
        send_samples();
        sleep_ms(100);  // 10 frames per second
    }
} 
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "wifi_config.h"

int main() {
    stdio_init_all();
    sleep_ms(3000);

    printf("Starting WiFi test...\n");

    if (cyw43_arch_init()) {
        printf("WiFi init FAILED\n");
        return 1;
    }

    cyw43_arch_enable_sta_mode();
    printf("Connecting to: %s\n", WIFI_SSID);

    int result = cyw43_arch_wifi_connect_timeout_ms(
        WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 15000
    );

    if (result) {
        printf("WiFi FAILED: %d\n", result);
    } else {
        printf("WiFi OK! IP: %s\n", ip4addr_ntoa(netif_ip4_addr(netif_default)));
    }

    while (true) {
        cyw43_arch_poll();
        sleep_ms(100);
    }
}
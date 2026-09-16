#include <stdio.h>
#include "pico/stdlib.h"


int main() {
    gpio_init(4);
    gpio_set_dir(4, GPIO_OUT);

    gpio_init(5);
    gpio_set_dir(5, GPIO_OUT);

    gpio_put(5, 1);

    for (int i = 0; i < 200; i++) {
        gpio_put(4, 1);
        sleep_us(1000);
        gpio_put(4, 0);
        sleep_us(1000);
    }

    while (1);
}
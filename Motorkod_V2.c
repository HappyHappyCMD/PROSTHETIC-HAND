#include "pico/stdlib.h"
#include "hardware/adc.h"

#define STEP_PIN 6
#define DIR_PIN 15

#define M1_PIN 13
#define M2_PIN 14

#define POT_PIN 26

#define OFFSET 250

// Speed limits
#define MIN_DELAY_US 20000
#define MAX_DELAY_US 3000

int main()
{
    // STEP pin
    gpio_init(STEP_PIN);
    gpio_set_dir(STEP_PIN, GPIO_OUT);
    gpio_put(STEP_PIN, 0);

    // DIR pin
    gpio_init(DIR_PIN);
    gpio_set_dir(DIR_PIN, GPIO_OUT);
    gpio_put(DIR_PIN, 0);

    // M1 / MS1 pin
    gpio_init(M1_PIN);
    gpio_set_dir(M1_PIN, GPIO_OUT);
    gpio_put(M1_PIN, 0);

    // M2 / MS2 pin
    gpio_init(M2_PIN);
    gpio_set_dir(M2_PIN, GPIO_OUT);
    gpio_put(M2_PIN, 0);

    // ADC / potentiometer
    adc_init();
    adc_gpio_init(POT_PIN);
    adc_select_input(0);       // GP26 = ADC0

    while (true)
    {
        int adc = adc_read();

        // POT LEFT
        if (adc < 2048 - OFFSET)
        {
            gpio_put(DIR_PIN, 0);

            // How far from center?
            int distance = 2048 - adc;

            // Distance to speed
            int delay_us =
                MIN_DELAY_US -
                (distance * (MIN_DELAY_US - MAX_DELAY_US) / 2048);

            // STEP pulse
            gpio_put(STEP_PIN, 1);
            sleep_us(10);

            gpio_put(STEP_PIN, 0);
            sleep_us(delay_us);
        }

        // POT CENTER
        else if (adc <= 2048 + OFFSET)
        {
            gpio_put(STEP_PIN, 0);
            sleep_ms(5);
        }

        // POT RIGHT
        else
        {
            gpio_put(DIR_PIN, 1);

            // How far from center?
            int distance = adc - 2048;

            // Distance to speed
            int delay_us =
                MIN_DELAY_US -
                (distance * (MIN_DELAY_US - MAX_DELAY_US) / 2048);

            // STEP pulse
            gpio_put(STEP_PIN, 1);
            sleep_us(10);

            gpio_put(STEP_PIN, 0);
            sleep_us(delay_us);
        }
    }
}

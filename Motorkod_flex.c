#include "pico/stdlib.h"
#include "hardware/adc.h"
#include <stdio.h>

#define STEP_PIN 6
#define DIR_PIN 15

#define M1_PIN 13
#define M2_PIN 14

#define FLEX_PIN 27

#define MOVE_STEPS 500

#define BEND_THRESHOLD 40
#define STRAIGHT_THRESHOLD 20

// Speed limits
#define MIN_DELAY_US 8000
#define MAX_DELAY_US 1500

// Motor directions
#define CLOSE_DIR 1
#define OPEN_DIR 0

int read_flex()
{
    int total = 0;

    // Read sensor several times to reduce noise
    for (int i = 0; i < 16; i++)
    {
        total += adc_read();
        sleep_us(300);
    }

    return total / 16;
}

void move_motor(int direction, int steps, int delay_us)
{
    // Motor direction
    gpio_put(DIR_PIN, direction);
    sleep_us(20);

    // Move motor
    for (int i = 0; i < steps; i++)
    {
        // STEP pulse
        gpio_put(STEP_PIN, 1);
        sleep_us(10);

        gpio_put(STEP_PIN, 0);
        sleep_us(delay_us);
    }
}

int main()
{
    stdio_init_all();

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

    // ADC / flex sensor
    adc_init();
    adc_gpio_init(FLEX_PIN);
    adc_select_input(1);       // GP27 = ADC1

    // Calibrate sensor with hand straight
    sleep_ms(2000);
    int baseline = read_flex();

    printf("Flex sensor motor control\n");
    printf("Baseline: %d\n", baseline);

    bool hand_closed = false;

    while (true)
    {
        int adc = read_flex();

        // How much has the sensor bent?
        int bend = baseline - adc;

        // SENSOR BENT
        if (!hand_closed && bend >= BEND_THRESHOLD)
        {
            // Bend amount controls motor speed
            int distance = bend;

            if (distance > 200)
                distance = 200;

            int delay_us =
                MIN_DELAY_US -
                (distance * (MIN_DELAY_US - MAX_DELAY_US) / 200);

            printf("Closing hand\n");

            // Close hand
            move_motor(CLOSE_DIR, MOVE_STEPS, delay_us);

            hand_closed = true;
        }

        // SENSOR STRAIGHT
        else if (hand_closed && bend <= STRAIGHT_THRESHOLD)
        {
            printf("Opening hand\n");

            // Open hand
            move_motor(OPEN_DIR, MOVE_STEPS, MIN_DELAY_US);

            hand_closed = false;
        }

        sleep_ms(10);
    }
}

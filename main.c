#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#define STRING 64
#define MOTOR1 2
#define MOTOR2 3
#define MOTOR3 6
#define MOTOR4 13
#define OPTO 28
#define PIEZO 27
#define LED0 21
#define TIME 30000000
#define WAIT_TIME 200000
bool sensor_triggered = false;

void piezo_interrupt(uint gpio, uint32_t events);

bool detect();

void remove_newline(char *str, size_t len);

bool get_input(char *str, int max);

void motor_step(int count);

void step(bool *seq);

void run(int n, int p, bool c);

void status(bool c);

bool calibrate(int *p);

void led_blink(void);

typedef enum {
    button0,
    button1,
    calib,
    dispense,

}states;


int main() {
    const uint button_pin0 = 9; //sw0
    const uint button_pin1 = 8;  //sw1
    int per_rev = 0;
    int count=0;
    uint64_t last = time_us_64();
    bool calibrated = false;
    states state = button0;
    //led
    gpio_init(LED0);
    gpio_set_dir(LED0, GPIO_OUT);

    //button
    gpio_init(button_pin0);
    gpio_set_dir(button_pin0, GPIO_IN);
    gpio_pull_up(button_pin0);
    gpio_init(button_pin1);
    gpio_set_dir(button_pin1, GPIO_IN);
    gpio_pull_up(button_pin1);

    //opto
    gpio_init(OPTO);
    gpio_set_dir(OPTO, GPIO_IN);
    gpio_pull_up(OPTO);

    //piezo
    gpio_init(PIEZO);
    gpio_set_dir(PIEZO, GPIO_IN);
    gpio_pull_up(PIEZO);

    //motor
    gpio_init(MOTOR1);
    gpio_init(MOTOR2);
    gpio_init(MOTOR3);
    gpio_init(MOTOR4);
    gpio_set_dir(MOTOR1, GPIO_OUT);
    gpio_set_dir(MOTOR2, GPIO_OUT);
    gpio_set_dir(MOTOR3, GPIO_OUT);
    gpio_set_dir(MOTOR4, GPIO_OUT);

    stdio_init_all();
    gpio_set_irq_enabled_with_callback(PIEZO, GPIO_IRQ_EDGE_FALL, true, piezo_interrupt);

    while (true) {

        switch (state) {

            case button0:
                printf("Waiting...\n");
                printf("Press SW0\n");
                while (gpio_get(button_pin0)) {
                    led_blink();
                    sleep_ms(10);
                }
                gpio_put(LED0, false);
                sleep_ms(10);
                state = calib;
                break;

            case calib:
                printf("Calibrating\n");
                calibrated = calibrate(&per_rev);
                status(calibrated);
                state = button1;
                break;



            case button1:
                printf("Ready, press SW1\n");
                while (gpio_get(button_pin1)) {
                    gpio_put(LED0, true);
                    sleep_ms(10);
                }
                state = dispense;
                gpio_put(LED0, false);
                break;

            case dispense:

                if (count == 7) {
                    count =0;
                    state = button0;
                }
                uint64_t now = time_us_64();
                if (now - last >= TIME) {
                    run(1, per_rev, calibrated);
                    sleep_ms(100);
                    if (!sensor_triggered) {
                        for (int i = 0; i <= 5; ++i) {
                            gpio_put(LED0, true);
                            sleep_ms(200);
                            gpio_put(LED0, false);
                            sleep_ms(200);
                        }
                    }
                    last = now;
                    ++count;
                }
                break;
        }



    }
}




void led_blink(void) {
    static absolute_time_t timeout = 0;
    if (time_reached(timeout)) {
        gpio_put(LED0, !gpio_get(LED0));
        timeout = make_timeout_time_ms(1000);
    }
}

bool calibrate(int *p) {

    int sum = 0;
    int error_sum=0;
    while (gpio_get(OPTO) == 0) {
        motor_step(1);
    }

    for (int r = 0; r < 3; ++r) {
        int count = 0;
        int error_count = 0;
        while (gpio_get(OPTO) == 1) {
            motor_step(1);
        }
        while (gpio_get(OPTO) == 0) {
            motor_step(1);
            ++count;
            ++error_count;
        }
        while (gpio_get(OPTO) == 1) {
            motor_step(1);
            ++count;
        }
        error_sum += error_count;
        sum += count;
    }
    *p = sum / 3;
    motor_step((error_sum/3) >> 1);
    return true;
}


void status(bool c) {
    if (c) {
        printf("Calibrated\n");
    } else {
        printf("Not calibrated\n");
    }
}


void run(int n, int p, bool c) {
    if (!c) {
        printf("Error: not calibrated\n");
    }
    int steps = p / 8 * n;

    motor_step(steps);

}

void motor_step(int count) {
    static bool sequence[8][4] = {
        {1, 0, 0, 0},
        {1, 1, 0, 0},
        {0, 1, 0, 0},
        {0, 1, 1, 0},
        {0, 0, 1, 0},
        {0, 0, 1, 1},
        {0, 0, 0, 1},
        {1, 0, 0, 1},
    };

    static int current = 0;

    for (int i = 0; i < count; ++i) {
        step(sequence[current]);
        current = (current + 1) % 8;
    }
}


void step(bool *seq) {
    gpio_put(MOTOR1, seq[0]);
    gpio_put(MOTOR2, seq[1]);
    gpio_put(MOTOR3, seq[2]);
    gpio_put(MOTOR4, seq[3]);
    sleep_ms(1);

}




void piezo_interrupt(uint gpio, uint32_t events) {
    sensor_triggered = true;


}
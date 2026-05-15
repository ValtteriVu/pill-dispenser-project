#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#define EEPROM_ADDRESS 0x50
#define MOTOR1 2
#define MOTOR2 3
#define MOTOR3 6
#define MOTOR4 13
#define OPTO 28
#define PIEZO 27
#define LED0 21
#define TIME_MS 30000
#define EEPROM_COUNT_ADDRESS 0x0010
#define EEPROM_STATE_ADDRESS 0x0012
#define EEPROM_REVO_ADDRESS 0x0014
#define EEPROM_MOTOR_STATE_VALUE 0x0016
volatile bool sensor_triggered = false;

typedef struct {
    uint16_t count;
    bool calibrated;
    uint16_t per_revo;
    bool motor_state;
}eeprom_data;

int eeprom_read(uint16_t memory_addr);

void eeprom_write(uint16_t memory_addr, uint16_t value);

void piezo_interrupt(uint gpio, uint32_t events);

void motor_step(int count);

void step(bool *seq);

void run(int step, uint16_t per_rev, bool calibrated);

void status(bool calibrated);

bool calibrate(uint16_t *p);

void led_blink(void);

typedef enum {
    start,
    button0,
    button1,
    calib,
    dispense,

}states;


int main() {
    const uint button_pin0 = 9; //sw0
    const uint button_pin1 = 8;  //sw1
    absolute_time_t timeout = make_timeout_time_ms(0);
    states state = start;


    //i2c
    i2c_init(i2c0,400000);
    gpio_set_function(16, GPIO_FUNC_I2C);
    gpio_set_function(17, GPIO_FUNC_I2C);


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

    eeprom_data data={0};
    data.count = eeprom_read(EEPROM_COUNT_ADDRESS);
    data.calibrated = eeprom_read(EEPROM_STATE_ADDRESS);
    data.per_revo = eeprom_read(EEPROM_REVO_ADDRESS);
    data.motor_state = eeprom_read(EEPROM_MOTOR_STATE_VALUE);



    while (true) {

        switch (state) {

            case start:
                if (data.calibrated) {
                    if (data.motor_state) {
                        printf("Power off detected\n");
                        ++data.count;
                        eeprom_write(EEPROM_COUNT_ADDRESS,data.count);
                    }

                    state = dispense;
                }
                else {
                    state = button0;
                }
                break;

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
                data.calibrated = calibrate(&data.per_revo);
                eeprom_write(EEPROM_STATE_ADDRESS,data.calibrated);
                eeprom_write(EEPROM_REVO_ADDRESS,data.per_revo);
                status(data.calibrated);
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

                if (data.count >= 7) {
                    data.count =0;
                    run(1,data.per_revo,data.calibrated);
                    eeprom_write(EEPROM_COUNT_ADDRESS,0);
                    state = button0;
                }

                if (time_reached(timeout)) {
                    sensor_triggered = false;
                    data.motor_state = true;
                    eeprom_write(EEPROM_MOTOR_STATE_VALUE,data.motor_state);
                    run(1, data.per_revo, data.calibrated);
                    sleep_ms(100);
                    if (!sensor_triggered) {
                        for (int i = 0; i <= 5; ++i) {
                            gpio_put(LED0, true);
                            sleep_ms(200);
                            gpio_put(LED0, false);
                            sleep_ms(200);
                        }
                    }
                    timeout= make_timeout_time_ms(TIME_MS);
                    data.count++;
                    eeprom_write(EEPROM_COUNT_ADDRESS,data.count);
                    sleep_ms(10);
                    data.motor_state = false;
                    eeprom_write(EEPROM_MOTOR_STATE_VALUE,data.motor_state);
                    printf("%d\n", data.per_revo);
                    printf("%d\n", data.count);
                    printf("%d\n", data.calibrated);
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

bool calibrate(uint16_t *p) {

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


void status(bool calibrated) {
    if (calibrated) {
        printf("Calibrated\n");
    } else {
        printf("Not calibrated\n");
    }
}


void run(int step, uint16_t per_rev, bool calibrated) {
    if (!calibrated) {
        printf("Error: not calibrated\n");
    }
    int steps = per_rev / 8 * step;

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

int eeprom_read(uint16_t memory_addr){
    uint8_t addr_buff[2];
    uint8_t data_buff[2];
    addr_buff[0] = memory_addr >> 8 & 0xFF;
    addr_buff[1] = memory_addr & 0xFF;

    i2c_write_blocking(i2c0, EEPROM_ADDRESS, addr_buff, 2, true);

    i2c_read_blocking(i2c0, EEPROM_ADDRESS, data_buff, 2, false);
    int data = data_buff[0] << 8 | data_buff[1];
    return data;
}

void eeprom_write(uint16_t memory_addr, uint16_t value){
    uint8_t data[4];
    data[0] = memory_addr >> 8 & 0xFF;
    data[1] = memory_addr & 0xFF;
    data[2] = value >> 8 & 0xFF;
    data[3] = value & 0xFF;

    i2c_write_blocking(i2c0, EEPROM_ADDRESS, data, 4, false);
    sleep_ms(5);
}
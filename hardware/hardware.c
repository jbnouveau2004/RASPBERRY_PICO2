#include "pico/stdlib.h"

#include "hardware/adc.h"
#include "hardware/pwm.h"

#include "hardware.h"

#define BUTTON_PIN 3
#define LED_PIN 2

#define VANNE2_PIN 4

#define ADC_PIN 26
#define ADC_INPUT 0

#define PWM_PIN 5

void hardware_init(void)
{
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);

    gpio_pull_up(BUTTON_PIN);

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_init(VANNE2_PIN);
    gpio_set_dir(VANNE2_PIN, GPIO_OUT);

    adc_init();
    adc_gpio_init(ADC_PIN);

    gpio_set_function(PWM_PIN, GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(PWM_PIN);
    uint channel = pwm_gpio_to_channel(PWM_PIN);

    pwm_set_wrap(slice_num, 4095);

    pwm_set_chan_level(slice_num,
                       channel,
                       0);

    pwm_set_enabled(slice_num, true);
}
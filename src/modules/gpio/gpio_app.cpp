#include "modules/gpio/gpio_app.h"

#include "modules/gpio/actuator/gpio_actuator.h"

void gpio_app_init()
{
    gpio_actuator_init();
}

void gpio_app_status_led_off()
{
    gpio_actuator_status_led_off();
}

void gpio_app_status_led_toggle()
{
    gpio_actuator_status_led_toggle();
}

#include "modules/gpio/actuator/gpio_actuator.h"

#include <Arduino.h>

#include "project_config.h"

void gpio_actuator_init()
{
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);
}

void gpio_actuator_status_led_off()
{
    digitalWrite(LED_PIN, LED_OFF);
}

void gpio_actuator_status_led_toggle()
{
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
}

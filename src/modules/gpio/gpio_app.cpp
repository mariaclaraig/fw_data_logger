#include "modules/gpio/gpio_app.h"

#include <Arduino.h>

#include "project_config.h"

void gpio_app_init()
{
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);
}

void gpio_app_status_led_off()
{
    digitalWrite(LED_PIN, LED_OFF);
}

void gpio_app_status_led_toggle()
{
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
}

void gpio_app_status_led_blink(unsigned long intervalMs)
{
    static unsigned long lastLedBlink = 0;
    unsigned long now = millis();

    if (now - lastLedBlink >= intervalMs)
    {
        lastLedBlink = now;
        gpio_app_status_led_toggle();
    }
}

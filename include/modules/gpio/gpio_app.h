#ifndef GPIO_APP_H
#define GPIO_APP_H

void gpio_app_init();
void gpio_app_status_led_off();
void gpio_app_status_led_toggle();
void gpio_app_status_led_blink(unsigned long intervalMs);

#endif // GPIO_APP_H

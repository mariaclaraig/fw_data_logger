#include "project_config.h"

#include "modules/communications/gprs/gprs_app.h"

#include "modules/communications/gprs/components/gprs_network.h"
#include "modules/communications/gprs/components/gprs_power.h"
#include "modules/communications/gprs/components/gprs_modem_state.h"
#include "modules/gpio/gpio_app.h"

TinyGsm &get_gprs_modem()
{
    return gprs_modem();
}

void gprs_app_power_on()
{
    gprs_power_modem_on();
}

void gprs_app_init()
{
    if (!gprs_power_setup_modem())
    {
        return;
    }

    gprs_network_print_diagnostics();
    gprs_network_connect(gpio_app_status_led_toggle);
    gprs_network_connect_data();
    gpio_app_status_led_off();

    Serial.println();
    Serial.println("Device is connected.");
    Serial.println();

    gprs_network_print_system_info();
}

void gprs_app_monitor()
{
    gprs_network_monitor(
        gpio_app_status_led_off,
        gpio_app_status_led_toggle);
}

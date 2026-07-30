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

void restart_modem()
{
    gprs_power_modem_restart();
}

void gprs_app_init()
{
    if (!gprs_power_setup_modem())
    {
        Serial.println("[GPRS] Failed to setup modem.");
        return;
    }

    gprs_network_print_diagnostics();

    if(!gprs_network_connect())
    {
        Serial.println("[GPRS] Failed to connect to network.");
        return;
    }

    if(!gprs_network_connect_data())
    {
        Serial.println("[GPRS] Failed to connect to data.");
        return;
    }

    gpio_app_status_led_off();

    gprs_network_print_system_info();
}

void gprs_app_monitor()
{
    if(!gprs_network_is_connected())
    {
        Serial.println("[GPRS] Modem is not responding. Restarting modem...");
        restart_modem();
    }

    gprs_network_monitor(
        gpio_app_status_led_off,
        gpio_app_status_led_toggle);
}

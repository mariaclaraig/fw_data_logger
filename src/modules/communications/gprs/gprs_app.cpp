#include "project_config.h"

#include "modules/communications/gprs/gprs_app.h"

#include "modules/communications/gprs/components/gprs_network.h"
#include "modules/communications/gprs/components/gprs_power.h"
#include "modules/communications/gprs/components/gprs_modem_state.h"
#include "modules/communications/gprs/components/gprs_identity.h"
#include "modules/gpio/gpio_app.h"

static void gprs_app_power_on(void)
{
    gprs_power_modem_on();
}

static void gprs_app_update_status_led(gprs_connection_state_t connectionState)
{
    if (connectionState == GPRS_CONNECTION_DATA_CONNECTED)
    {
        gpio_app_status_led_off();
        return;
    }

    gpio_app_status_led_blink(LED_BLINK_INTERVAL_MS);
}

static bool gprs_app_connect_modem(void)
{
    if (!gprs_power_prepare_sim())
    {
        Serial.println("[GPRS] SIM nao esta pronto. Reinicio do modem necessario.");
        return false;
    }

    if (!gprs_network_connect())
    {
        Serial.println("[GPRS] Failed to connect to network.");
        return false;
    }

    if (!gprs_network_connect_data())
    {
        Serial.println("[GPRS] Failed to connect to data.");
        return false;
    }

    return true;
}

static void gprs_app_restart_modem(void)
{
    if (!gprs_power_modem_restart() || !gprs_app_connect_modem())
    {
        Serial.println("[GPRS] Restart do modem nao restabeleceu a conexao.");
    }
}

void gprs_app_init()
{
    int tryCount = GPRS_MODEM_SETUP_ATTEMPTS;

    while (tryCount-- && !gprs_power_setup_modem())
    {
        Serial.print("[GPRS] Failed to setup modem. Tentativas restantes: ");
        Serial.println(tryCount);
        vTaskDelay(pdMS_TO_TICKS(3000));
    }

    if (!gprs_modem().testAT())
    {
        Serial.println("[GPRS] Failed to setup modem.");
        gprs_app_restart_modem();
        return;
    }

    gprs_modem_state_print_diagnostics();

    if (!gprs_app_connect_modem())
    {
        gprs_app_restart_modem();
        return;
    }

    gpio_app_status_led_off();

    gprs_identity_print_system_info();
}

void gprs_app_monitor()
{
    static bool hasRestartedModem = false;
    static unsigned long lastModemRestart = 0;

    if (!gprs_network_reconnect())
    {
        unsigned long now = millis();

        if (!hasRestartedModem || now - lastModemRestart >= GPRS_MODEM_RESTART_COOLDOWN_MS)
        {
            Serial.println("[GPRS] Nao foi possivel restabelecer a conexao. Reiniciando modem...");
            lastModemRestart = now;
            hasRestartedModem = true;
            gprs_app_restart_modem();
        }
        else
        {
            Serial.println("[GPRS] Falha de conexao mantida. Aguardando cooldown do restart.");
        }
    }

    gprs_app_update_status_led(gprs_network_monitor());
}

#include "modules/communications/gprs/components/gprs_power.h"

#include <Arduino.h>

#include "modules/communications/gprs/components/gprs_modem_state.h"
#include "modules/communications/gprs/components/gprs_network.h"
#include "project_config.h"

void gprs_power_modem_on()
{
    pinMode(PWR_PIN, OUTPUT);
    digitalWrite(PWR_PIN, LOW);
    vTaskDelay(pdMS_TO_TICKS(1000));
    digitalWrite(PWR_PIN, HIGH);
}

static void gprs_power_modem_off(void)
{
    pinMode(PWR_PIN, OUTPUT);
    digitalWrite(PWR_PIN, LOW);
    vTaskDelay(pdMS_TO_TICKS(1000));
    digitalWrite(PWR_PIN, HIGH);
}

bool gprs_power_modem_restart()
{
    TinyGsm &modem = gprs_modem();

    modem.poweroff();
    vTaskDelay(pdMS_TO_TICKS(5000));
    if(!gprs_power_setup_modem())
    {
        Serial.println("[GPRS] Failed to setup modem after restart.");
        return false;
    }
    if (!gprs_power_prepare_sim())
    {
        Serial.println("[GPRS] SIM is not ready after restart.");
        return false;
    }
    if(!gprs_network_connect())
    {
        Serial.println("[GPRS] Failed to connect to network after restart.");
        return false;
    }
    if (!gprs_network_connect_data())
    {
        Serial.println("[GPRS] Failed to connect data after restart.");
        return false;
    }

    return true;
}

bool gprs_power_prepare_sim()
{
    TinyGsm &modem = gprs_modem();
    SimStatus simStatus = modem.getSimStatus();

    if (simStatus == SIM_LOCKED && GSM_PIN[0] != '\0')
    {
        modem.simUnlock(GSM_PIN);
        simStatus = modem.getSimStatus();
    }

    return simStatus == SIM_READY;
}

bool gprs_power_setup_modem()
{
    gprs_power_modem_on();
    SerialAT.begin(UART_BAUD, SERIAL_8N1, PIN_RX, PIN_TX);

    vTaskDelay(pdMS_TO_TICKS(5000));

    TinyGsm &modem = gprs_modem();

    Serial.println("========INIT========");
    if (!modem.init())
    {
        Serial.println("[GPRS] modem.init() failed.");
        return false;
    }

    if (!modem.testAT())
    {
        Serial.println("[GPRS] modem is not responding to AT commands.");
        return false;
    }

    modem.sendAT("+CMEE=2");
    return modem.waitResponse(1000L) == 1;
}

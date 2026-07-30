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

void gprs_power_modem_off()
{
    pinMode(PWR_PIN, OUTPUT);
    digitalWrite(PWR_PIN, LOW);
    vTaskDelay(pdMS_TO_TICKS(1000));
    digitalWrite(PWR_PIN, HIGH);
}

void gprs_power_modem_restart()
{
    TinyGsm &modem = gprs_modem();

    modem.poweroff();
    vTaskDelay(pdMS_TO_TICKS(5000));
    if(!gprs_power_setup_modem())
    {
        Serial.println("[GPRS] Failed to setup modem after restart.");
    }
    if(!gprs_network_connect())
    {
        Serial.println("[GPRS] Failed to connect to network after restart.");
    }
    if (!gprs_network_connect_data())
    {
        Serial.println("[GPRS] Failed to connect data after restart.");
    }
}

bool gprs_power_setup_modem()
{
    gprs_power_modem_on();
    SerialAT.begin(UART_BAUD, SERIAL_8N1, PIN_RX, PIN_TX);

    vTaskDelay(pdMS_TO_TICKS(5000));

    TinyGsm &modem = gprs_modem();

    Serial.println("========INIT========");
    modem.init();

    modem.sendAT("+CMEE=2");
    modem.waitResponse(1000L);
    return true;
}

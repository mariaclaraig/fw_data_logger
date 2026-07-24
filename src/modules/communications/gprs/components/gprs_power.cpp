#include "modules/communications/gprs/components/gprs_power.h"

#include <Arduino.h>

#include "modules/communications/gprs/components/gprs_modem_state.h"
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
    gprs_power_modem_off();
    gprs_power_modem_on();
    gprs_power_setup_modem();
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

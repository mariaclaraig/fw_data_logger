#include "modules/communications/gprs/components/gprs_network.h"

#include <Arduino.h>

#include "modules/communications/gprs/components/gprs_modem_state.h"
#include "project_config.h"

static bool lastConnectionState = true;
static bool currentConnectionState = true;
static unsigned long lastNetworkCheck = 0;
static unsigned long lastStatusSummary = 0;

void gprs_network_print_diagnostics()
{
    TinyGsm &modem = gprs_modem();
    String res;

    // Imprime informacoes referentes ao modem SIMCom, firmware/modelo, etc.
    Serial.println("========SIMCOMATI======");
    modem.sendAT("+SIMCOMATI");
    modem.waitResponse(1000L, res);
    res.replace("\r\nOK\r\n", "");
    Serial.println(res);
    res = "";
    Serial.println("=======================");

    // Consulta do modo do modem: automatico, GSM, LTE...
    Serial.println("=====Preferred mode selection=====");
    modem.sendAT("+CNMP?");
    if (modem.waitResponse(1000L, res) == 1)
    {
        res.replace("\r\nOK\r\n", "");
        Serial.println(res);
    }
    res = "";
    Serial.println("=======================");

    // Consulta qual a preferencia do modem: CAT-M, NB-IoT, ambos...
    Serial.println("=====Preferred selection between CAT-M and NB-IoT=====");
    modem.sendAT("+CMNB?");
    if (modem.waitResponse(1000L, res) == 1)
    {
        res.replace("\r\nOK\r\n", "");
        Serial.println(res);
    }
    res = "";
    Serial.println("=======================");

    // Consulta do nome e informacoes do modem atraves do TinyGSM.
    String name = modem.getModemName();
    Serial.println("Modem Name: " + name);

    String modemInfo = modem.getModemInfo();
    Serial.println("Modem Info: " + modemInfo);

    // Desbloqueio do SIM caso PIN esteja configurado e nao seja desbloqueado.
    if (GSM_PIN && modem.getSimStatus() != 3)
    {
        modem.simUnlock(GSM_PIN);
    }
}

bool gprs_network_connect(GprsNetworkLedHandler ledToggleHandler)
{
    TinyGsm &modem = gprs_modem();
    
    uint8_t network[] = {
        2,  /*Automatic*/
        13, /*GSM only*/
        38, /*LTE only*/
        51  /*GSM and LTE only*/
    };

    for (size_t i = 0; i < sizeof(network) / sizeof(network[0]); i++)
    {
        Serial.printf("Try %d method\n", network[i]);
        modem.setNetworkMode(network[i]);
        delay(3000);

        bool isConnected = false;
        int tryCount = 60;

        while (tryCount--)
        {
            int16_t signal = modem.getSignalQuality();
            Serial.print("Signal: ");
            Serial.print(signal);
            Serial.print(" ");
            Serial.print("isNetworkConnected: ");
            isConnected = modem.isNetworkConnected();
            Serial.println(isConnected ? "CONNECTED" : "NOT CONNECTED");
            if (isConnected)
            {
                return true;
            }

            delay(3000);
            ledToggleHandler();
        }
    }

    return false;
}

bool gprs_network_is_connected()
{
    return gprs_modem().isNetworkConnected();
}

bool gprs_network_is_data_connected()
{
    return gprs_modem().isGprsConnected();
}

void gprs_network_print_system_info()
{
    TinyGsm &modem = gprs_modem();
    String res;

    Serial.println("=====Inquiring UE system information=====");
    modem.sendAT("+CPSI?");
    if (modem.waitResponse(1000L, res) == 1)
    {
        res.replace("\r\nOK\r\n", "");
        Serial.println(res);
    }
}

void gprs_network_monitor(
    GprsNetworkLedHandler ledOffHandler,
    GprsNetworkLedHandler ledToggleHandler)
{
    TinyGsm &modem = gprs_modem();
    static unsigned long lastLedBlink = 0;
    unsigned long now = millis();

    if (now - lastNetworkCheck >= NETWORK_CHECK_INTERVAL_MS)
    {
        lastNetworkCheck = now;
        currentConnectionState = modem.isNetworkConnected();
        bool dataConnectionState = modem.isGprsConnected();

        if (currentConnectionState != lastConnectionState)
        {
            Serial.print("[GPRS] rede mudou: ");
            Serial.println(currentConnectionState ? "CONNECTED" : "NOT CONNECTED");
            lastConnectionState = currentConnectionState;
        }

        if (now - lastStatusSummary >= GPRS_STATUS_SUMMARY_INTERVAL_MS)
        {
            lastStatusSummary = now;

            Serial.print("[GPRS] rede=");
            Serial.print(currentConnectionState ? "CONNECTED" : "NOT_CONNECTED");
            Serial.print(" dados=");
            Serial.print(dataConnectionState ? "CONNECTED" : "NOT_CONNECTED");
            Serial.print(" sinal=");
            Serial.println(modem.getSignalQuality());
        }

        if (currentConnectionState)
        {
            ledOffHandler();
        }
    }

    if (!currentConnectionState && now - lastLedBlink >= LED_BLINK_INTERVAL_MS)
    {
        lastLedBlink = now;
        ledToggleHandler();
    }
}

bool gprs_network_connect_data()
{
    TinyGsm &modem = gprs_modem();

    return modem.gprsConnect(GPRS_APN, GPRS_USER, GPRS_PASS);
}

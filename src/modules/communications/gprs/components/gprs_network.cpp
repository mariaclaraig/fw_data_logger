#include "modules/communications/gprs/components/gprs_network.h"

#include <Arduino.h>

#include "modules/communications/gprs/components/gprs_modem_state.h"
#include "modules/communications/gprs/components/gprs_at.h"

static NetworkMode *gprs_network_configure_mode(uint8_t network_value, uint8_t *network_size)
{
    static NetworkMode modes[1];
    uint8_t size = 0;

    // wip: deixar network_size mesmo que tamanho seja padrão por enquanto, posteriormente pode ser alterado.

    switch (network_value)
    {
    case 1:
        modes[0] = MODEM_NETWORK_AUTO;
        size = 1;
        break;
    case 2:
        modes[0] = MODEM_NETWORK_GSM;
        size = 1;
        break;
    case 3:
        modes[0] = MODEM_NETWORK_LTE;
        size = 1;
        break;
    case 4:
        modes[0] = MODEM_NETWORK_GSM_LTE;
        size = 1;
        break;
    default:
        modes[0] = MODEM_NETWORK_AUTO;
        size = 0;
        break;
    }

    *network_size = size;
    return modes;
}

static AccessTechnology gprs_network_configure_access_technology(uint8_t technology_value)
{
    AccessTechnology preferredMode = MODEM_ACCESS_TECH_AUTO;

    switch (technology_value)
    {
    case 1:
        preferredMode = MODEM_ACCESS_TECH_CAT_M1;
        break;
    case 2:
        preferredMode = MODEM_ACCESS_TECH_NB_IOT;
        break;
    case 3:
    default:
        preferredMode = MODEM_ACCESS_TECH_AUTO;
        break;
    }

    return preferredMode;
}

bool gprs_network_connect()
{
    TinyGsm &modem = gprs_modem();
    uint8_t networkSize = 0;
    NetworkMode *network = gprs_network_configure_mode(
        GPRS_SELECTED_NETWORK_MODE,
        &networkSize);

    if (!modem.testAT())
    {
        Serial.println("[GPRS] modem não responde durante conexão de rede.");
        return false;
    }

    if (!gprs_modem_state_prepare_sim())
    {
        Serial.println("[GPRS] SIM nao esta pronto. Reinicio do modem necessario.");
        return false;
    }

    AccessTechnology preferredMode = gprs_network_configure_access_technology(
        GPRS_SELECTED_ACCESS_TECHNOLOGY);

    if (!modem.setPreferredMode(preferredMode))
    {
        Serial.println("[GPRS] falha ao configurar tecnologia de acesso.");
        return false;
    }

    for (size_t i = 0; i < networkSize; i++)
    {
        Serial.printf("Try %d method\n", network[i]);
        if (!modem.setNetworkMode(network[i]))
        {
            Serial.println("[GPRS] falha ao configurar modo de rede.");
            continue;
        }

        vTaskDelay(pdMS_TO_TICKS(3000));

        bool is_connected = false;
        int tryCount = GPRS_NETWORK_RECONNECT_ATTEMPTS;

        while (tryCount--)
        {
            int16_t signal = modem.getSignalQuality();
            Serial.print("Signal: ");
            Serial.print(signal);
            Serial.print(" ");
            Serial.print("isNetworkConnected: ");
            is_connected = modem.isNetworkConnected();
            Serial.println(is_connected ? "CONNECTED" : "NOT CONNECTED");
            if (is_connected)
            {
                return true;
            }

            vTaskDelay(pdMS_TO_TICKS(3000));
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

bool gprs_network_has_internet()
{
    if (!gprs_network_is_connected() || !gprs_network_is_data_connected())
    {
        return false;
    }

    TinyGsm &modem = gprs_modem();
    TinyGsmClient client(modem);
    bool isConnected = client.connect(
        GPRS_INTERNET_CHECK_HOST,
        GPRS_INTERNET_CHECK_PORT,
        10);

    client.stop();
    return isConnected;
}

bool gprs_network_reconnect()
{
    if (!gprs_network_is_connected())
    {
        Serial.println("[GPRS] rede perdida, tentando reconectar rede...");
        if (!gprs_network_connect())
        {
            Serial.println("[GPRS] falha ao reconectar rede.");
            return false;
        }
    }

    if (!gprs_network_is_data_connected())
    {
        Serial.println("[GPRS] sessão de dados perdida, tentando reconectar GPRS...");
        if (!gprs_network_connect_data())
        {
            Serial.println("[GPRS] falha ao reconectar dados.");
            return false;
        }
    }

    return true;
}

bool gprs_network_connect_data()
{
    TinyGsm &modem = gprs_modem();
    int tryCount = GPRS_DATA_RECONNECT_ATTEMPTS;

    if (!modem.testAT())
    {
        Serial.println("[GPRS] modem não responde durante conexão de dados.");
        return false;
    }

    while (tryCount--)
    {
        if (modem.isGprsConnected())
        {
            return true;
        }

        modem.gprsDisconnect();
        vTaskDelay(pdMS_TO_TICKS(1000));

        Serial.print("[GPRS] tentando conectar dados. Tentativas restantes: ");
        Serial.println(tryCount);

        if (modem.gprsConnect(GPRS_APN, GPRS_USER, GPRS_PASS))
        {
            return true;
        }

        if (modem.isGprsConnected())
        {
            return true;
        }

        vTaskDelay(pdMS_TO_TICKS(3000));
    }

    return modem.isGprsConnected();
}

String gprs_network_get_connected_technology()
{
    String technology = gprs_at_get_value_from_response("AT+CPSI?", 0);
    technology.toUpperCase();

    if (technology.length() == 0)
    {
        return "UNKNOWN";
    }

    if (technology.indexOf("NO SERVICE") >= 0)
    {
        return "NO_SERVICE";
    }

    if (technology.indexOf("CAT-M1") >= 0 || technology.indexOf("CAT-M") >= 0)
    {
        return "CAT-M";
    }

    if (technology.indexOf("NB-IOT") >= 0 || technology.indexOf("NBIOT") >= 0)
    {
        return "NB-IoT";
    }

    if (technology.indexOf("LTE") >= 0)
    {
        return "LTE";
    }

    if (technology.indexOf("GSM") >= 0)
    {
        return "GSM";
    }

    return technology;
}

void gprs_network_print_system_info()
{
    Serial.println("=====Inquiring UE system information=====");
    Serial.println(gprs_at_send_command("AT+CPSI?"));
}

gprs_connection_state_t gprs_network_monitor()
{
    TinyGsm &modem = gprs_modem();
    unsigned long now = millis();

    static bool lastConnectionState = true;
    static bool currentConnectionState = true;
    static bool dataConnectionState = true;
    static bool internetConnectionState = false;
    static unsigned long lastNetworkCheck = 0;
    static unsigned long lastStatusSummary = 0;
    static unsigned long lastInternetCheck = 0;

    if (now - lastNetworkCheck >= NETWORK_CHECK_INTERVAL_MS)
    {
        lastNetworkCheck = now;
        currentConnectionState = gprs_network_is_connected();
        dataConnectionState = gprs_network_is_data_connected();

        if (now - lastInternetCheck >= GPRS_INTERNET_CHECK_INTERVAL_MS)
        {
            lastInternetCheck = now;
            internetConnectionState = gprs_network_has_internet();
        }

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
            Serial.print(" internet=");
            Serial.print(internetConnectionState ? "CONNECTED" : "NOT_CONNECTED");
            Serial.print(" tecnologia=");
            Serial.print(currentConnectionState ? gprs_network_get_connected_technology() : "NO_SERVICE");
            Serial.print(" sinal=");
            Serial.println(modem.getSignalQuality());
        }

    }

    if (!currentConnectionState)
    {
        return GPRS_CONNECTION_DISCONNECTED;
    }

    if (!dataConnectionState)
    {
        return GPRS_CONNECTION_NETWORK_CONNECTED;
    }

    return GPRS_CONNECTION_DATA_CONNECTED;
}

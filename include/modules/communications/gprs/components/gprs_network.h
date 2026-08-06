#ifndef GPRS_NETWORK_H
#define GPRS_NETWORK_H

#include "project_config.h"

typedef enum
{
    MODEM_NETWORK_AUTO = 2,  /*Automatic*/
    MODEM_NETWORK_GSM = 13, /*GSM only*/
    MODEM_NETWORK_LTE = 38, /*LTE only*/
    MODEM_NETWORK_GSM_LTE = 51,  /*GSM and LTE only*/
} NetworkMode;

typedef enum
{
    MODEM_ACCESS_TECH_AUTO = 3, /*CAT-M and NB-IoT*/
    MODEM_ACCESS_TECH_CAT_M1 = 1, /*CAT-M1 only*/
    MODEM_ACCESS_TECH_NB_IOT = 2, /*NB-IoT only*/
} AccessTechnology;

typedef enum
{
    GPRS_CONNECTION_DISCONNECTED,
    GPRS_CONNECTION_NETWORK_CONNECTED,
    GPRS_CONNECTION_DATA_CONNECTED,
} gprs_connection_state_t;

bool gprs_network_connect(uint8_t networkMode, uint8_t accessTechnology);
bool gprs_network_is_connected();
bool gprs_network_is_data_connected();
bool gprs_network_has_internet();
bool gprs_network_reconnect(
    uint8_t networkMode,
    uint8_t accessTechnology,
    const char *apn,
    const char *user,
    const char *pass);
bool gprs_network_connect_data(
    const char *apn,
    const char *user,
    const char *pass);
gprs_connection_state_t gprs_network_monitor();

#endif // GPRS_NETWORK_H

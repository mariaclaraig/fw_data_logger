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

NetworkMode *gprs_network_configure_mode(uint8_t network_value, uint8_t *network_size);
AccessTechnology gprs_network_configure_access_technology(uint8_t technology_value);

typedef void (*GprsNetworkLedHandler)();

void gprs_network_print_diagnostics();
bool gprs_network_connect();
bool gprs_network_is_connected();
bool gprs_network_is_data_connected();
bool gprs_network_reconnect();
bool gprs_network_connect_data();
void gprs_network_print_system_info();
void gprs_network_monitor(
    GprsNetworkLedHandler ledOffHandler,
    GprsNetworkLedHandler ledToggleHandler);

#endif // GPRS_NETWORK_H

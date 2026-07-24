#ifndef GPRS_NETWORK_H
#define GPRS_NETWORK_H

typedef void (*GprsNetworkLedHandler)();

void gprs_network_print_diagnostics();
bool gprs_network_connect(GprsNetworkLedHandler ledToggleHandler);
void gprs_network_print_system_info();
void gprs_network_monitor(
    GprsNetworkLedHandler ledOffHandler,
    GprsNetworkLedHandler ledToggleHandler);
bool gprs_network_is_connected();
bool gprs_network_connect_data();

#endif // GPRS_NETWORK_H

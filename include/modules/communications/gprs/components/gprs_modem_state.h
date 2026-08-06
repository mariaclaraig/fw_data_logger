#ifndef GPRS_MODEM_STATE_H
#define GPRS_MODEM_STATE_H

#include "project_config.h"
#include <TinyGsmClient.h>

TinyGsm &gprs_modem();
void gprs_modem_state_print_diagnostics();

#endif // GPRS_MODEM_STATE_H

#ifndef GPRS_IDENTITY_H
#define GPRS_IDENTITY_H

#include <Arduino.h>

String gprs_identity_get_system_info();
String gprs_identity_get_connected_technology();
void gprs_identity_print_system_info();

#endif // GPRS_IDENTITY_H

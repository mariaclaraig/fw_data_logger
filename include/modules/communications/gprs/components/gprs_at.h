#ifndef GPRS_AT_H
#define GPRS_AT_H

#include <Arduino.h>

String gprs_at_send_command(const char *command);
String gprs_at_get_value_from_response(String response, int position);

#endif // GPRS_AT_H

#include "modules/communications/gprs/components/gprs_identity.h"

#include "modules/communications/gprs/components/gprs_at.h"

String gprs_identity_get_system_info()
{
    return gprs_at_send_command("AT+CPSI?");
}

String gprs_identity_get_connected_technology()
{
    String technology = gprs_at_get_value_from_response(gprs_identity_get_system_info(), 0);
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

void gprs_identity_print_system_info()
{
    Serial.println("=====Inquiring UE system information=====");
    Serial.println(gprs_identity_get_system_info());
}

#include "modules/communications/gprs/components/gprs_at.h"

#include "modules/communications/gprs/components/gprs_modem_state.h"

String gprs_at_send_command(const char *command)
{
    if (command == nullptr)
    {
        return "";
    }

    TinyGsm &modem = gprs_modem();
    modem.sendAT(command);

    String response = "";
    modem.waitResponse(1000L, response);
    return response;
}

String gprs_at_get_value_from_response(const char *command, int position)
{
    String response = gprs_at_send_command(command);
    int startIndex = response.indexOf(":") + 1;

    if (startIndex == 0)
    {
        return "";
    }

    response = response.substring(startIndex);
    response.trim();

    int currentPosition = 0;
    int lastIndex = 0;
    int nextIndex = response.indexOf(',');

    while (currentPosition < position)
    {
        lastIndex = nextIndex + 1;
        nextIndex = response.indexOf(',', lastIndex);

        if (nextIndex == -1)
        {
            nextIndex = response.indexOf('\n', lastIndex);
            if (nextIndex == -1)
            {
                return "";
            }
        }

        currentPosition++;
    }

    String value = response.substring(lastIndex, nextIndex);
    value.trim();
    return value;
}

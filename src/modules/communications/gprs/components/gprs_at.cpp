#include "modules/communications/gprs/components/gprs_at.h"

#include "project_config.h"

String gprs_at_send_command(const char *command)
{
    if (command == nullptr)
    {
        return "";
    }

    SerialAT.println(command);
    vTaskDelay(pdMS_TO_TICKS(100));

    String response = "";
    while (SerialAT.available())
    {
        response += char(SerialAT.read());
    }

    vTaskDelay(pdMS_TO_TICKS(100));
    return response;
}

String gprs_at_get_value_from_response(String response, int position)
{
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

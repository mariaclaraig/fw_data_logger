#include "system_manager.h"

#include <Arduino.h>

#include "modules/communications/gprs/gprs_app.h"
#include "modules/gpio/gpio_app.h"
#include "project_config.h"

void start_system()
{
    SerialMon.begin(115200);
    delay(10);

    gpio_app_init();
    gprs_app_init();
}

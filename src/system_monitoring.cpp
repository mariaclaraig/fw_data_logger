#include "system_monitoring.h"

#include "modules/communications/gprs/gprs_app.h"

void system_monitoring()
{
    gprs_app_monitor();
}

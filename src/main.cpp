#include "system_manager.h"
#include "system_monitoring.h"

void setup()
{
    start_system();
}

void loop()
{
    system_monitoring();
}

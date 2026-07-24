#include "modules/communications/gprs/components/gprs_modem_state.h"
#include "project_config.h"

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>

static StreamDebugger debugger(SerialAT, SerialMon);
static TinyGsm modem(debugger);
#else
static TinyGsm modem(SerialAT);
#endif

TinyGsm &gprs_modem()
{
    return modem;
}
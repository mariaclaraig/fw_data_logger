#include "modules/communications/gprs/components/gprs_modem_state.h"
#include "project_config.h"
#include "modules/communications/gprs/components/gprs_at.h"

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

bool gprs_modem_state_prepare_sim()
{
    SimStatus simStatus = modem.getSimStatus();

    if (simStatus == SIM_LOCKED && GSM_PIN[0] != '\0')
    {
        modem.simUnlock(GSM_PIN);
        simStatus = modem.getSimStatus();
    }

    return simStatus == SIM_READY;
}

void gprs_modem_state_print_diagnostics()
{
    TinyGsm &modem = gprs_modem();

    // Imprime informacoes referentes ao modem SIMCom, firmware/modelo, etc.
    Serial.println("========SIMCOMATI======");
    Serial.println(gprs_at_send_command("AT+SIMCOMATI"));

    // Consulta do modo do modem: automatico, GSM, LTE...
    Serial.println("=====Preferred mode selection=====");
    Serial.println(gprs_at_send_command("AT+CNMP?"));

    // Consulta qual a preferencia do modem: CAT-M, NB-IoT, ambos...
    Serial.println("=====Preferred selection between CAT-M and NB-IoT=====");
    Serial.println(gprs_at_send_command("AT+CMNB?"));

    // Consulta do nome e informacoes do modem atraves do TinyGSM.
    String name = modem.getModemName();
    Serial.println("Modem Name: " + name);

    String modemInfo = modem.getModemInfo();
    Serial.println("Modem Info: " + modemInfo);
}

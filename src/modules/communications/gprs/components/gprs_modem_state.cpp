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

void gprs_modem_state_print_diagnostics()
{
    TinyGsm &modem = gprs_modem();
    String res;

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

    // Desbloqueio do SIM caso PIN esteja configurado e nao seja desbloqueado.
    if (GSM_PIN && modem.getSimStatus() != 3)
    {
        modem.simUnlock(GSM_PIN);
    }
}
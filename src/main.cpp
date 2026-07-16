#define TINY_GSM_MODEM_SIM7000
#define TINY_GSM_RX_BUFFER 1024 // Set RX buffer to 1Kb

#define SerialAT Serial1
// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// See all AT commands, if wanted
#define DUMP_AT_COMMANDS

// set GSM PIN, if any
#define GSM_PIN ""

// Your GPRS credentials, if any
const char apn[] = "claro.com.br"; // SET TO YOUR APN
const char gprsUser[] = "claro";
const char gprsPass[] = "claro";

#include <TinyGsmClient.h>
#include <SPI.h>
#include <SD.h>
#include <Ticker.h>

#ifdef DUMP_AT_COMMANDS
    #include <StreamDebugger.h>
    StreamDebugger debugger(SerialAT, SerialMon);
    TinyGsm modem(debugger);
#else
    TinyGsm modem(SerialAT);
#endif

// LilyGO T-SIM7000G Pinout
#define UART_BAUD 115200
#define PIN_DTR 25 // Usado para despertar o módulo do modo de baixo consumo.
#define PIN_TX 27 //  TX do módulo SIM7000G
#define PIN_RX 26 //  RX do módulo SIM7000G
#define PWR_PIN 4 //  Liga/desliga do módulo SIM7000G. Também reinicia o módulo quando pressionado por 1,5s.

#define SD_MISO 2 //  SD manda dados para o ESP32.
#define SD_MOSI 15 // ESP32 manda dados para o SD.
#define SD_SCLK 14 // Clock da comunicação.
#define SD_CS 13 //   Seleciona cartão SD. Quando em nível lógico baixo, o cartão SD é selecionado e a comunicação é habilitada.
#define LED_PIN 12 // LED do módulo SIM7000G.

const unsigned long NETWORK_CHECK_INTERVAL_MS = 1000;
const unsigned long LED_BLINK_INTERVAL_MS = 500;
const unsigned long MANUAL_AT_IDLE_MS = 3000;
const uint8_t LED_OFF = HIGH;

unsigned long lastManualAtCommandAt = 0;

// Comandos de funcionamento do Modem SIM7000G

void modemPowerOn()
{
    pinMode(PWR_PIN, OUTPUT);
    digitalWrite(PWR_PIN, LOW);
    delay(1000);
    digitalWrite(PWR_PIN, HIGH);
}

void modemPowerOff()
{
    pinMode(PWR_PIN, OUTPUT);
    digitalWrite(PWR_PIN, LOW);
    delay(1500);
    digitalWrite(PWR_PIN, HIGH);
}

void modemRestart()
{
    modemPowerOff();
    delay(1000);
    modemPowerOn();
}

void handleAtTerminal()
{
    static String command;

    while (SerialAT.available())
    {
        SerialMon.write(SerialAT.read());
    }

    while (SerialMon.available())
    {
        char c = SerialMon.read();

        if (c == '\r' || c == '\n')
        {
            if (command.length() == 0)
            {
                continue;
            }

            SerialMon.println();

            if (command == "+++")
            {
                SerialAT.print("+++");
            }
            else
            {
                SerialAT.print(command);
                SerialAT.print("\r\n");
            }

            lastManualAtCommandAt = millis();
            command = "";
            continue;
        }

        if (c == '\b' || c == 127)
        {
            if (command.length() > 0)
            {
                command.remove(command.length() - 1);
                SerialMon.print("\b \b");
            }
            continue;
        }

        command += c;
        SerialMon.write(c);
    }
}

void waitWithAtTerminal(unsigned long durationMs)
{
    unsigned long start = millis();

    while (millis() - start < durationMs)
    {
        handleAtTerminal();
        delay(10);
    }
}

void setup()
{
    // Set console baud rate
    SerialMon.begin(115200);

    delay(10);

    // Set LED OFF
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);

    modemPowerOn();

    Serial.println("========SDCard Detect.======");
    SPI.begin(SD_SCLK, SD_MISO, SD_MOSI);
    if (!SD.begin(SD_CS))
    {
        Serial.println("SDCard MOUNT FAIL");
    }
    else
    {
        uint32_t cardSize = SD.cardSize() / (1024 * 1024);
        String str = "SDCard Size: " + String(cardSize) + "MB";
        Serial.println(str);
    }
    Serial.println("===========================");

    SerialAT.begin(UART_BAUD, SERIAL_8N1, PIN_RX, PIN_TX);

    Serial.println("/**********************************************************/");
    Serial.println("To initialize the network test, please make sure your LTE ");
    Serial.println("antenna has been connected to the SIM interface on the board.");
    Serial.println("/**********************************************************/\n\n");

    waitWithAtTerminal(10000);
}

void loop()
{
    String res;

    Serial.println("========INIT========");

    if (!modem.init())
    {
        modemRestart();
        delay(2000);
        Serial.println("Failed to restart modem, attempting to continue without restarting");
        return;
    }

    Serial.println("========SIMCOMATI======");
    modem.sendAT("+SIMCOMATI");
    modem.waitResponse(1000L, res);
    res.replace("\r\nOK\r\n", "");
    Serial.println(res);
    res = "";
    Serial.println("=======================");

    Serial.println("=====Preferred mode selection=====");
    modem.sendAT("+CNMP?");
    if (modem.waitResponse(1000L, res) == 1)
    {
        res.replace("\r\nOK\r\n", "");
        Serial.println(res);
    }
    res = "";
    Serial.println("=======================");

    Serial.println("=====Preferred selection between CAT-M and NB-IoT=====");
    modem.sendAT("+CMNB?");
    if (modem.waitResponse(1000L, res) == 1)
    {
        res.replace("\r\nOK\r\n", "");
        Serial.println(res);
    }
    res = "";
    Serial.println("=======================");

    String name = modem.getModemName();
    Serial.println("Modem Name: " + name);

    String modemInfo = modem.getModemInfo();
    Serial.println("Modem Info: " + modemInfo);

    // Unlock your SIM card with a PIN if needed
    if (GSM_PIN && modem.getSimStatus() != 3)
    {
        modem.simUnlock(GSM_PIN);
    }

// separar

    uint8_t network[] = {
        2,  /*Automatic*/
        13, /*GSM only*/
        38, /*LTE only*/
        51  /*GSM and LTE only*/
    };

    for (size_t i = 0; i < sizeof(network) / sizeof(network[0]); i++)
    {
        Serial.printf("Try %d method\n", network[i]);
        modem.setNetworkMode(network[i]);
        waitWithAtTerminal(3000);
        bool isConnected = false;
        int tryCount = 60;
        while (tryCount--)
        {
            if (millis() - lastManualAtCommandAt < MANUAL_AT_IDLE_MS)
            {
                waitWithAtTerminal(100);
                continue;
            }

            int16_t signal = modem.getSignalQuality();
            Serial.print("Signal: ");
            Serial.print(signal);
            Serial.print(" ");
            Serial.print("isNetworkConnected: ");
            isConnected = modem.isNetworkConnected();
            Serial.println(isConnected ? "CONNECT" : "NO CONNECT");
            if (isConnected)
            {
                break;
            }
            waitWithAtTerminal(1000);
            digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        }
        if (isConnected)
        {
            break;
        }
    }
    digitalWrite(LED_PIN, LED_OFF);

    Serial.println();
    Serial.println("Device is connected .");
    Serial.println();

    Serial.println("=====Inquiring UE system information=====");
    modem.sendAT("+CPSI?");
    if (modem.waitResponse(1000L, res) == 1)
    {
        res.replace("\r\nOK\r\n", "");
        Serial.println(res);
    }

    Serial.println("/**********************************************************/");
    Serial.println("After the network test is complete, please enter the  ");
    Serial.println("AT command in the serial terminal.");
    Serial.println("/**********************************************************/\n\n");

    bool lastConnectionState = true;
    bool currentConnectionState = true;
    unsigned long lastNetworkCheck = 0;
    unsigned long lastLedBlink = 0;

    while (1)
    {
        handleAtTerminal();

        unsigned long now = millis();
        if (now - lastManualAtCommandAt >= MANUAL_AT_IDLE_MS &&
            now - lastNetworkCheck >= NETWORK_CHECK_INTERVAL_MS)
        {
            lastNetworkCheck = now;
            currentConnectionState = modem.isNetworkConnected();

            if (currentConnectionState != lastConnectionState)
            {
                Serial.print("isNetworkConnected: ");
                modem.isGprsConnected();
                Serial.println(currentConnectionState ? "CONNECT" : "NO CONNECT");
                lastConnectionState = currentConnectionState;
            }

            if (currentConnectionState)
            {
                digitalWrite(LED_PIN, LED_OFF);
            }
        }

        if (!currentConnectionState && now - lastLedBlink >= LED_BLINK_INTERVAL_MS)
        {
            lastLedBlink = now;
            digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        }
    }
}

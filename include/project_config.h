#ifndef PROJECT_CONFIG_H
#define PROJECT_CONFIG_H

#include <Arduino.h>

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

#define TINY_GSM_MODEM_SIM7000
#define TINY_GSM_RX_BUFFER 1024 // Set RX buffer to 1Kb

#define SerialAT Serial1
// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// See all AT commands, if wanted
// #define DUMP_AT_COMMANDS

// set GSM PIN, if any
#define GSM_PIN ""

#define GPRS_APN "virtueyes.com.br"
#define GPRS_USER "virtu"
#define GPRS_PASS "virtu#"

// depois alterar essa config para receber do nvs
#define GPRS_SELECTED_NETWORK_MODE 1 // LTE and GSM, auto
#define GPRS_SELECTED_ACCESS_TECHNOLOGY 3 // CAT-M and NB-IoT, auto (se for LTE))

/* #define GPRS_APN "claro.com.br"
#define GPRS_USER "claro"
#define GPRS_PASS "claro" */

#define GPRS_DATA_RECONNECT_ATTEMPTS 10
#define GPRS_NETWORK_RECONNECT_ATTEMPTS 30
#define GPRS_MODEM_SETUP_ATTEMPTS 3
#define GPRS_MODEM_RESTART_COOLDOWN_MS 60000UL

#define NETWORK_CHECK_INTERVAL_MS 3000UL
#define GPRS_STATUS_SUMMARY_INTERVAL_MS 5000UL
#define LED_BLINK_INTERVAL_MS 500UL
#define LED_OFF HIGH

#endif // PROJECT_CONFIG_H

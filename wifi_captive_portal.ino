#include <WiFi.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

#include "config.h"
#include "webpages.h"
#include "utils.h"
#include "server_handlers.h"

DNSServer dnsServer;
AsyncWebServer server(80);

void setup()
{
  Serial.begin(115200);
  delay(1000);

  if (!SPIFFS.begin(true))
  {
    Serial.println("Falha ao montar SPIFFS");
    return;
  }

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  WiFi.softAP(ssid, password);
  delay(100);
  
  Serial.println("Ponto de acesso iniciado!");
  Serial.println(WiFi.softAPIP());

  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());

  setupServerRoutes(server);
  server.begin();
}

void loop()
{
  dnsServer.processNextRequest();
}
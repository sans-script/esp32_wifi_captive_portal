#include <WiFi.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

#include "config.h"
#include "webpages.h"
#include "utils.h"
#include "server_handlers.h"
#include "display.h"

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

  // Inicializar LEDs e buzzer
  initializeLeds();

  // Inicializar SSID a partir de configuração salva
  initializeSSID();

  // Inicializar display TFT
  initDisplay();

  WiFi.softAP(ssid, password);
  delay(100);

  Serial.println("Ponto de acesso iniciado!");
  Serial.println(WiFi.softAPIP());

  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());

  setupServerRoutes(server);
  server.begin();

  Serial.println("=== SERVIDOR INICIADO ===");
  Serial.print("IP do Servidor: ");
  Serial.println(WiFi.softAPIP());
  Serial.println("Porta: 80");
  Serial.println("Sistema pronto! LED azul ficará apagado até alguém navegar.");
}

void loop()
{
  dnsServer.processNextRequest();
  updateBlueLedBlinking(); // Atualiza a piscada do LED azul
  updateDisplay();         // Atualiza o display TFT
}
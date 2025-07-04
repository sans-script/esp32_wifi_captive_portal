#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <SPIFFS.h>
#include "utils.h"
#include "config.h"

// Display pin definitions
#define TFT_SCLK 14 // SLC (clock SPI)
#define TFT_MOSI 27 // SDA (data SPI)
#define TFT_RST 26  // RST (reset)
#define TFT_DC 25   // DC (data/command)
#define TFT_CS 33   // CS (chip select)
#define TFT_BLK 32  // BLK (backlight)

// Display object
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

// Colors
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF
#define ORANGE 0xFD20
#define PURPLE 0x8010
#define PINK 0xF81F
#define GRAY 0x8410

// Display variables
unsigned long displayStartTime = 0;
bool screenNeedsUpdate = true; // Flag para controlar quando redesenhar
unsigned long lastFrameTime = 0;
const unsigned long frameInterval = 16; // 16ms = ~60 FPS (1000ms/60fps = 16.67ms)
String lastKnownSSID = "";              // Para detectar mudanças no SSID

// Stats structure
struct PortalStats
{
    String ssidName;
    String portalIP;
    int pageAccesses = 0;
    int uniqueIPs = 0;
    int wifiConnections = 0;
    int loginAttempts = 0;
    int onlineDevices = 0;
    bool hasNewLogin = false;
};

PortalStats stats;

// Function declarations
void initDisplay();
void updateDisplay();
void updatePortalStats();
void updatePortalInfo();
void showBootScreen();
void showMainInfoScreen();
void updateDynamicValues();       // Nova função para atualizar apenas valores dinâmicos
String truncateSSID(String ssid); // Função para truncar SSID se muito longo

void initDisplay()
{
    Serial.println("Initializing TFT Display...");

    // Configure backlight
    pinMode(TFT_BLK, OUTPUT);
    digitalWrite(TFT_BLK, HIGH); // Turn on backlight
    delay(100);

    // Initialize display - vertical 180 graus
    tft.initR(INITR_BLACKTAB);
    tft.setRotation(2); // Vertical 180 graus

    displayStartTime = millis();
    showBootScreen();
    delay(2000);

    Serial.println("TFT Display initialized!");
}

void updateDisplay()
{
    unsigned long currentTime = millis();

    // Controle de FPS - só atualiza a cada 16ms (60 FPS)
    if (currentTime - lastFrameTime < frameInterval)
    {
        return;
    }

    // Update portal info apenas a cada 1 segundo para dados dinâmicos
    static unsigned long lastInfoUpdate = 0;
    static bool fullRedrawNeeded = true; // Flag para redesenho completo inicial
    static bool firstRun = true;         // Para garantir que a primeira execução desenhe tudo

    if (currentTime - lastInfoUpdate > 1000 || firstRun)
    {
        updatePortalInfo();
        updatePortalStats();
        lastInfoUpdate = currentTime;
        screenNeedsUpdate = true;
        firstRun = false;
    }

    // Redesenha se necessário e respeitando o FPS
    if (screenNeedsUpdate)
    {
        if (fullRedrawNeeded)
        {
            showMainInfoScreen(); // Redesenho completo apenas quando necessário
            fullRedrawNeeded = false;
        }
        else
        {
            updateDynamicValues(); // Atualiza apenas valores que mudam
        }

        screenNeedsUpdate = false;
    }

    lastFrameTime = currentTime; // Atualiza o tempo do último frame sempre

    // Força redesenho completo apenas se necessário (removido o timer automático)
    // O redesenho completo só acontece na inicialização ou quando explicitamente solicitado
}

void updatePortalStats()
{
    // Obter SSID real da variável
    stats.ssidName = String(ssid);

    // Obter IP real do portal
    stats.portalIP = WiFi.softAPIP().toString();

    // Contar dispositivos online (igual ao LED azul)
    stats.onlineDevices = WiFi.softAPgetStationNum();

    // Reset stats
    static int lastKnownLogins = -1; // -1 indica primeira execução
    static bool firstRun = true;

    stats.pageAccesses = 0;
    stats.wifiConnections = 0;
    stats.loginAttempts = 0;
    stats.uniqueIPs = 0;

    String ipList = "";

    // Read access logs
    File accessFile = SPIFFS.open("/access_logs.txt");
    if (accessFile)
    {
        while (accessFile.available())
        {
            String line = accessFile.readStringUntil('\n');
            line.trim();

            if (line.length() > 0)
            {
                if (line.indexOf("PAGE_ACCESS") >= 0)
                {
                    stats.pageAccesses++;

                    // Extract IP for unique count - usar mesma lógica do dashboard
                    int ipStart = line.indexOf("IP: ");
                    if (ipStart >= 0)
                    {
                        int ipEnd = line.indexOf(' ', ipStart + 4);
                        if (ipEnd == -1)
                            ipEnd = line.indexOf('|', ipStart + 4);
                        if (ipEnd == -1)
                            ipEnd = line.length();
                        String ip = line.substring(ipStart + 4, ipEnd);

                        // Contar IPs únicos (mesma lógica do dashboard)
                        if (ipList.indexOf(ip) == -1)
                        {
                            if (ipList.length() > 0)
                                ipList += ",";
                            ipList += ip;
                            stats.uniqueIPs++;
                        }
                    }
                }
                else if (line.indexOf("WIFI_CONNECT") >= 0)
                {
                    stats.wifiConnections++;
                }
                else if (line.indexOf("LOGIN_SUCCESS") >= 0)
                {
                    stats.loginAttempts++;
                }
            }
        }
        accessFile.close();
    }

    // Detectar novo login apenas após a primeira execução
    if (firstRun)
    {
        lastKnownLogins = stats.loginAttempts; // Inicializa com o valor atual
        firstRun = false;
        stats.hasNewLogin = false; // Garante que não mostra alerta na inicialização
    }
    else if (stats.loginAttempts > lastKnownLogins)
    {
        stats.hasNewLogin = true;
        lastKnownLogins = stats.loginAttempts; // Atualiza o contador
    }

    // Reset flag após 2 segundos
    static unsigned long loginAlertTime = 0;
    if (stats.hasNewLogin && loginAlertTime == 0)
    {
        loginAlertTime = millis();
    }
    else if (stats.hasNewLogin && millis() - loginAlertTime > 2000)
    {
        stats.hasNewLogin = false;
        loginAlertTime = 0;
    }
}

void updatePortalInfo()
{
    // Update portal IP address and SSID
    stats.portalIP = WiFi.softAPIP().toString();
    stats.ssidName = String(ssid);
    stats.onlineDevices = WiFi.softAPgetStationNum();
}

void showBootScreen()
{
    tft.fillScreen(BLACK);

    // Title - centralizado
    tft.setTextColor(GREEN);
    tft.setTextSize(1);
    const char *title = "CAPTIVE PORTAL";
    int16_t x1, y1;
    uint16_t w, h;
    tft.getTextBounds(title, 0, 0, &x1, &y1, &w, &h);
    int x = (128 - w) / 2;
    tft.setCursor(x, 50);
    tft.println(title);

    tft.setTextColor(CYAN);
    tft.setCursor(45, 70);
    tft.println("v1.0");

    // Subtitle - centralizado
    tft.setTextColor(WHITE);
    tft.setCursor(35, 90);
    tft.println("Loading...");

    // Progress bar - centralizado
    for (int i = 0; i <= 100; i += 20)
    {
        int barWidth = map(i, 0, 100, 0, 80);
        tft.drawRect(25, 110, 82, 8, WHITE);
        tft.fillRect(26, 111, barWidth, 6, GREEN);
        delay(200);
    }

    const char *msg = "Ready!";
    tft.setTextColor(GREEN);
    tft.setTextSize(1);
    tft.getTextBounds(msg, 0, 0, &x1, &y1, &w, &h);
    x = (128 - w) / 2;
    tft.setCursor(x, 130);
    tft.println(msg);
}

void showMainInfoScreen()
{
    tft.fillScreen(BLACK);

    // v1.0 + uptime
    tft.setTextColor(GREEN);
    tft.setTextSize(1);
    tft.setCursor(5, 5);
    tft.print("v1.0 ");
    tft.setTextColor(YELLOW);
    tft.print("UPTIME: ");
    tft.print(getUptime());

    // Linha separadora reta
    tft.drawLine(5, 18, 120, 18, WHITE);

    // SSID
    tft.setTextColor(CYAN);
    tft.setCursor(5, 25);
    tft.print("SSID: ");
    tft.setTextColor(WHITE);
    tft.print(truncateSSID(stats.ssidName));

    // IP
    tft.setTextColor(CYAN);
    tft.setCursor(5, 38);
    tft.print("IP: ");
    tft.setTextColor(WHITE);
    tft.print(stats.portalIP);

    // PAGE ACCESSES
    tft.setTextColor(YELLOW);
    tft.setCursor(5, 58);
    tft.print("PAGE ACCESSES: ");
    tft.setTextColor(WHITE);
    tft.print(stats.pageAccesses);

    // UNIQUE IPs
    tft.setTextColor(YELLOW);
    tft.setCursor(5, 71);
    tft.print("UNIQUE IPs: ");
    tft.setTextColor(WHITE);
    tft.print(stats.uniqueIPs);

    // WIFI CONNECTIONS
    tft.setTextColor(YELLOW);
    tft.setCursor(5, 84);
    tft.print("WIFI CONNECTIONS: ");
    tft.setTextColor(WHITE);
    tft.print(stats.wifiConnections);

    // LOGIN ATTEMPTS
    tft.setTextColor(YELLOW);
    tft.setCursor(5, 97);
    tft.print("LOGIN ATTEMPTS: ");
    tft.setTextColor(WHITE);
    tft.print(stats.loginAttempts);

    // ONLINE - ajustado para não sair da tela
    tft.setTextColor(GREEN);
    tft.setCursor(5, 115);
    tft.print("ONLINE: ");
    tft.setTextColor(stats.onlineDevices > 0 ? GREEN : RED);
    tft.print(stats.onlineDevices);

    // Aviso de novo login (se houver)
    if (stats.hasNewLogin)
    {
        tft.fillRect(5, 135, 118, 15, GREEN);
        tft.setTextColor(WHITE);
        // Calculate horizontal center: rectangle width (118) - text width (~66 pixels for "NEW LOGIN!") / 2 + start position (5)
        tft.setCursor(31, 140);
        tft.print("NEW LOGIN!");
    }
    else
    {
        // Only Educational Purposes - duas linhas centralizadas e vermelhas
        tft.setTextColor(RED);
        // Centralizar "Only Educational" (largura ~98px, então (128-98)/2 = 15)
        tft.setCursor(15, 135);
        tft.print("Only Educational");
        // Centralizar "Purposes" (largura ~56px, então (128-56)/2 = 36)
        tft.setCursor(36, 148);
        tft.print("Purposes");
    }
}

void updateDynamicValues()
{
    // Atualiza apenas os valores que mudam frequentemente, sem redesenhar toda a tela

    // Verificar se o SSID mudou (precisa de redesenho completo se mudou)
    if (stats.ssidName != lastKnownSSID)
    {
        // SSID mudou - limpa área específica e atualiza
        tft.fillRect(35, 25, 93, 8, BLACK); // Limpa área do SSID
        tft.setTextColor(CYAN);
        tft.setCursor(5, 25);
        tft.print("SSID: ");
        tft.setTextColor(WHITE);
        tft.print(truncateSSID(stats.ssidName));
        lastKnownSSID = stats.ssidName; // Atualiza último SSID conhecido
    }

    // UPTIME - limpa área específica e atualiza (ao lado da versão)
    tft.fillRect(35, 5, 93, 8, BLACK);
    tft.setTextColor(GREEN);
    tft.setCursor(5, 5);
    tft.print("v1.0 ");
    tft.setTextColor(YELLOW);
    tft.print("UPTIME: ");
    tft.print(getUptime());

    // PAGE ACCESSES - limpa área específica e atualiza (coordenadas exatas da showMainInfoScreen)
    tft.fillRect(95, 58, 33, 8, BLACK);
    tft.setTextColor(WHITE);
    tft.setCursor(95, 58);
    tft.print(stats.pageAccesses);

    // UNIQUE IPs - limpa área específica e atualiza (coordenadas exatas da showMainInfoScreen)
    tft.fillRect(75, 71, 53, 8, BLACK);
    tft.setTextColor(WHITE);
    tft.setCursor(75, 71);
    tft.print(stats.uniqueIPs);

    // WIFI CONNECTIONS - limpa área específica e atualiza (coordenadas exatas da showMainInfoScreen)
    tft.fillRect(110, 84, 18, 8, BLACK);
    tft.setTextColor(WHITE);
    tft.setCursor(110, 84);
    tft.print(stats.wifiConnections);

    // LOGIN ATTEMPTS - limpa área específica e atualiza (coordenadas exatas da showMainInfoScreen)
    tft.fillRect(95, 97, 33, 8, BLACK);
    tft.setTextColor(WHITE);
    tft.setCursor(95, 97);
    tft.print(stats.loginAttempts);

    // ONLINE devices - limpa área e redesenha (coordenadas exatas da showMainInfoScreen)
    tft.fillRect(50, 115, 78, 8, BLACK);
    tft.setTextColor(stats.onlineDevices > 0 ? GREEN : RED);
    tft.setCursor(50, 115);
    tft.print(stats.onlineDevices);

    // Controla a área da mensagem apenas quando há mudança no estado do login
    static bool lastLoginState = false;

    if (stats.hasNewLogin != lastLoginState)
    {
        // Só redesenha a área da mensagem quando o estado muda
        tft.fillRect(5, 135, 118, 28, BLACK); // Limpa área das mensagens

        if (stats.hasNewLogin)
        {
            tft.fillRect(5, 135, 118, 15, GREEN);
            tft.setTextColor(WHITE);
            tft.setCursor(31, 140);
            tft.print("NEW LOGIN!");
        }
        else
        {
            // Mensagem educacional (coordenadas exatas da showMainInfoScreen)
            tft.setTextColor(RED);
            tft.setCursor(15, 135);
            tft.print("Only Educational");
            tft.setCursor(36, 148);
            tft.print("Purposes");
        }

        lastLoginState = stats.hasNewLogin;
    }
}

// Função para truncar SSID se for muito longo para o display
String truncateSSID(String ssid)
{
    // Display tem 128px de largura
    // "SSID: " ocupa cerca de 30px, restam ~93px para o SSID
    // Cada caractere ocupa cerca de 6px na fonte padrão
    // Máximo de cerca de 15 caracteres cabe confortavelmente
    const int MAX_CHARS = 15;

    if (ssid.length() <= MAX_CHARS)
    {
        return ssid;
    }

    // Trunca e adiciona "..." no final
    return ssid.substring(0, MAX_CHARS - 3) + "...";
}

#endif

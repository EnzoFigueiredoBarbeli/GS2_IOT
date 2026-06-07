/*
 * =====================================================
 *  GUARDIÃO DE HABITATS EXTREMOS - Lunar/Mars Colony
 * =====================================================
 * Simula monitoramento de um habitat espacial com:
 *  - Potenciômetro → pressão interna (hPa simulado)
 *  - LDR           → radiação/luminosidade (lux simulado)
 *
 * Classificação TinyML (regras extraídas do Edge Impulse):
 *  SEGURO   → pressão 400–700 e radiação < 600
 *  ALERTA   → pressão 200–399 ou 701–850, ou radiação 600–799
 *  CRÍTICO  → pressão < 200 ou > 850, ou radiação >= 800
 *
 * Webserver: status disponível em http://<IP>/
 * =====================================================
 * Integrantes:
 *  - [Nome] - RM: XXXXXX
 *  - [Nome] - RM: XXXXXX
 * =====================================================
 */

#include <WiFi.h>
#include <WebServer.h>

// ── Wi-Fi (Wokwi usa rede simulada) ──────────────────
const char* ssid     = "Wokwi-GUEST";
const char* password = "";

// ── Pinos ─────────────────────────────────────────────
#define PIN_PRESSURE   34   // Potenciômetro → pressão
#define PIN_RADIATION  35   // LDR           → radiação

// ── Webserver na porta 80 ─────────────────────────────
WebServer server(80);

// ── Variáveis globais ─────────────────────────────────
int pressureRaw   = 0;
int radiationRaw  = 0;
String statusAtual = "INICIALIZANDO";
String corStatus   = "#888888";
String emoji       = "🔄";

// ── LED de status (opcional, GPIO 2) ─────────────────
#define LED_PIN 2

// =====================================================
//  CLASSIFICAÇÃO (regras extraídas do TinyML)
// =====================================================
String classificar(int pressure, int radiation) {
  // CRÍTICO: pressão fora do limite seguro OU radiação extrema
  if (pressure < 200 || pressure > 850 || radiation >= 800) {
    return "CRITICO";
  }
  // ALERTA: pressão na zona limítrofe OU radiação moderada-alta
  if (pressure < 400 || pressure > 700 || radiation >= 600) {
    return "ALERTA";
  }
  // SEGURO: todos os parâmetros dentro do nominal
  return "SEGURO";
}

// =====================================================
//  PÁGINA HTML DO WEBSERVER
// =====================================================
void handleRoot() {
  String corFundo, corTexto, icone, descricao;

  if (statusAtual == "SEGURO") {
    corFundo  = "#0a3d0a";
    corTexto  = "#00ff88";
    icone     = "✅";
    descricao = "Todos os sistemas operando nominalmente. Habitat seguro para habitação.";
  } else if (statusAtual == "ALERTA") {
    corFundo  = "#3d2d00";
    corTexto  = "#ffcc00";
    icone     = "⚠️";
    descricao = "Parâmetros fora do intervalo ideal. Verificação necessária. Astronautas em standby.";
  } else {
    corFundo  = "#3d0000";
    corTexto  = "#ff4444";
    icone     = "🚨";
    descricao = "EMERGÊNCIA DETECTADA! Evacuar setor ou acionar sistemas de suporte de vida.";
  }

  String html = R"rawhtml(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <meta http-equiv="refresh" content="3">
  <title>Guardião de Habitat - Colônia Lunar</title>
  <style>
    * { margin: 0; padding: 0; box-sizing: border-box; }
    body {
      font-family: 'Courier New', monospace;
      background: #0d0d1a;
      color: #c0c0c0;
      min-height: 100vh;
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      padding: 20px;
    }
    .header {
      text-align: center;
      margin-bottom: 30px;
    }
    .header h1 {
      font-size: 1.4em;
      color: #7eb8f7;
      letter-spacing: 3px;
      text-transform: uppercase;
    }
    .header p {
      font-size: 0.75em;
      color: #555;
      margin-top: 5px;
      letter-spacing: 2px;
    }
    .status-card {
      background: )rawhtml" + corFundo + R"rawhtml(;
      border: 2px solid )rawhtml" + corTexto + R"rawhtml(;
      border-radius: 12px;
      padding: 30px 40px;
      text-align: center;
      width: 100%;
      max-width: 420px;
      box-shadow: 0 0 30px )rawhtml" + corTexto + R"rawhtml(44;
      margin-bottom: 25px;
    }
    .status-icon { font-size: 3em; margin-bottom: 10px; }
    .status-label {
      font-size: 2em;
      font-weight: bold;
      color: )rawhtml" + corTexto + R"rawhtml(;
      letter-spacing: 4px;
    }
    .status-desc {
      font-size: 0.78em;
      color: #aaa;
      margin-top: 12px;
      line-height: 1.5;
    }
    .sensors {
      display: flex;
      gap: 15px;
      width: 100%;
      max-width: 420px;
      margin-bottom: 20px;
    }
    .sensor-box {
      flex: 1;
      background: #111122;
      border: 1px solid #2a2a4a;
      border-radius: 8px;
      padding: 15px;
      text-align: center;
    }
    .sensor-label {
      font-size: 0.65em;
      color: #555;
      letter-spacing: 2px;
      text-transform: uppercase;
      margin-bottom: 6px;
    }
    .sensor-value {
      font-size: 1.6em;
      color: #7eb8f7;
      font-weight: bold;
    }
    .sensor-unit {
      font-size: 0.65em;
      color: #444;
      margin-top: 3px;
    }
    .footer {
      font-size: 0.6em;
      color: #333;
      text-align: center;
      letter-spacing: 1px;
    }
    .blink { animation: blink 1s infinite; }
    @keyframes blink { 0%,100%{opacity:1} 50%{opacity:0.3} }
  </style>
</head>
<body>
  <div class="header">
    <h1>🛸 Guardião de Habitat</h1>
    <p>COLÔNIA LUNAR — SETOR ALFA-7</p>
  </div>

  <div class="status-card">
    <div class="status-icon )rawhtml";

  // pisca no crítico
  if (statusAtual == "CRITICO") html += "blink";
  html += R"rawhtml(">)rawhtml" + icone + R"rawhtml(</div>
    <div class="status-label">)rawhtml" + statusAtual + R"rawhtml(</div>
    <div class="status-desc">)rawhtml" + descricao + R"rawhtml(</div>
  </div>

  <div class="sensors">
    <div class="sensor-box">
      <div class="sensor-label">⚡ Pressão</div>
      <div class="sensor-value">)rawhtml" + String(pressureRaw) + R"rawhtml(</div>
      <div class="sensor-unit">RAW (0–1023)</div>
    </div>
    <div class="sensor-box">
      <div class="sensor-label">☢️ Radiação</div>
      <div class="sensor-value">)rawhtml" + String(radiationRaw) + R"rawhtml(</div>
      <div class="sensor-unit">RAW (0–1023)</div>
    </div>
  </div>

  <div class="footer">
    ATUALIZAÇÃO AUTOMÁTICA A CADA 3s &nbsp;|&nbsp; ESP32 WEBSERVER<br>
    FIAP — GLOBAL SOLUTION 2026 — SPACE CONNECT
  </div>
</body>
</html>
)rawhtml";

  server.send(200, "text/html", html);
}

// =====================================================
//  SETUP
// =====================================================
void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  Serial.println("\n╔══════════════════════════════════════╗");
  Serial.println("║   GUARDIÃO DE HABITATS EXTREMOS      ║");
  Serial.println("║   Lunar/Mars Colony - FIAP GS 2026   ║");
  Serial.println("╚══════════════════════════════════════╝");

  // Conecta Wi-Fi
  Serial.print("\n[WiFi] Conectando a: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  int tentativas = 0;
  while (WiFi.status() != WL_CONNECTED && tentativas < 20) {
    delay(500);
    Serial.print(".");
    tentativas++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[WiFi] Conectado!");
    Serial.print("[WiFi] IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n[WiFi] Falha na conexão. Modo offline.");
  }

  // Inicia webserver
  server.on("/", handleRoot);
  server.begin();
  Serial.println("[HTTP] Servidor iniciado na porta 80");
  Serial.println("[INFO] Monitoramento iniciado...\n");
}

// =====================================================
//  LOOP
// =====================================================
void loop() {
  // Lê sensores
  pressureRaw  = analogRead(PIN_PRESSURE);
  radiationRaw = analogRead(PIN_RADIATION);

  // Classifica com regras TinyML
  statusAtual = classificar(pressureRaw, radiationRaw);

  // LED: acende no crítico
  digitalWrite(LED_PIN, statusAtual == "CRITICO" ? HIGH : LOW);

  // Log serial
  Serial.printf("[SENSOR] Pressão: %4d | Radiação: %4d | Status: %s\n",
                pressureRaw, radiationRaw, statusAtual.c_str());

  // Atende requisições HTTP
  server.handleClient();

  delay(500);
}

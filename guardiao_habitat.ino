#include <WiFi.h>
#include <WebServer.h>

const char* ssid     = "Wokwi-GUEST";
const char* password = "";

#define PIN_PRESSURE   34
#define PIN_RADIATION  35
#define LED_GREEN      25
#define LED_YELLOW     26
#define LED_RED        27

WebServer server(80);

int pressureRaw  = 0;
int radiationRaw = 0;
String statusAtual = "INICIANDO";

// ── Classificação TinyML (faixas 0–4095) ─────────────
String classificar(int pressure, int radiation) {
  if (pressure < 800 || pressure > 3200 || radiation >= 2500)
    return "CRITICO";
  if (pressure < 1500 || pressure > 2500 || radiation >= 1800)
    return "ALERTA";
  return "SEGURO";
}

void atualizarLEDs() {
  digitalWrite(LED_GREEN,  statusAtual == "SEGURO"  ? HIGH : LOW);
  digitalWrite(LED_YELLOW, statusAtual == "ALERTA"  ? HIGH : LOW);
  digitalWrite(LED_RED,    statusAtual == "CRITICO" ? HIGH : LOW);
}

void handleRoot() {
  String cor, bg, titulo, desc;
  if (statusAtual == "SEGURO") {
    cor = "#00ff88"; bg = "#0a3d0a";
    titulo = "✅ SEGURO";
    desc = "Todos os sistemas nominais. Habitat seguro.";
  } else if (statusAtual == "ALERTA") {
    cor = "#ffcc00"; bg = "#3d2d00";
    titulo = "⚠️ ALERTA";
    desc = "Parametros fora do ideal. Verificacao necessaria.";
  } else {
    cor = "#ff4444"; bg = "#3d0000";
    titulo = "🚨 CRITICO";
    desc = "EMERGENCIA! Evacuar setor imediatamente.";
  }

  String html = "<!DOCTYPE html><html><head>";
  html += "<meta charset='UTF-8'><meta http-equiv='refresh' content='2'>";
  html += "<title>Guardiao Habitat</title>";
  html += "<style>body{margin:0;font-family:monospace;background:#0d0d1a;color:#ccc;";
  html += "display:flex;flex-direction:column;align-items:center;justify-content:center;min-height:100vh;}";
  html += "h1{color:#7eb8f7;letter-spacing:3px;}";
  html += ".card{background:" + bg + ";border:2px solid " + cor + ";border-radius:12px;";
  html += "padding:28px 50px;text-align:center;margin:20px;}";
  html += ".status{font-size:2.5em;font-weight:bold;color:" + cor + ";}";
  html += ".desc{color:#aaa;margin-top:10px;}";
  html += ".sensors{display:flex;gap:20px;}";
  html += ".sbox{background:#111122;border:1px solid #2a2a4a;border-radius:8px;padding:14px 20px;text-align:center;}";
  html += ".sval{font-size:2em;color:#7eb8f7;font-weight:bold;}";
  html += ".slbl{font-size:0.6em;color:#555;text-transform:uppercase;}</style></head><body>";
  html += "<h1>GUARDIAO DE HABITAT</h1>";
  html += "<p style='color:#444;font-size:0.7em'>COLONIA LUNAR - SETOR ALFA-7</p>";
  html += "<div class='card'><div class='status'>" + titulo + "</div>";
  html += "<div class='desc'>" + desc + "</div></div>";
  html += "<div class='sensors'>";
  html += "<div class='sbox'><div class='slbl'>Pressao</div><div class='sval'>" + String(pressureRaw) + "</div></div>";
  html += "<div class='sbox'><div class='slbl'>Radiacao</div><div class='sval'>" + String(radiationRaw) + "</div></div>";
  html += "</div>";
  html += "<p style='color:#333;font-size:0.55em;margin-top:20px'>FIAP GS 2026 | Auto-refresh 2s</p>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(LED_GREEN,  OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED,    OUTPUT);

  Serial.println("=== GUARDIAO DE HABITAT INICIANDO ===");
  Serial.println("Testando LEDs...");

  digitalWrite(LED_GREEN,  HIGH); delay(500);
  digitalWrite(LED_GREEN,  LOW);
  digitalWrite(LED_YELLOW, HIGH); delay(500);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED,    HIGH); delay(500);
  digitalWrite(LED_RED,    LOW);

  Serial.println("LEDs OK!");

  Serial.print("Conectando WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  int t = 0;
  while (WiFi.status() != WL_CONNECTED && t < 20) {
    delay(500);
    Serial.print(".");
    t++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi conectado!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi falhou. Modo offline.");
  }

  server.on("/", handleRoot);
  server.begin();
  Serial.println("Webserver iniciado na porta 80!");
  Serial.println("=====================================");
  Serial.println("Faixas de classificacao (0-4095):");
  Serial.println("SEGURO:  pressao 1500-2500, radiacao <1800");
  Serial.println("ALERTA:  pressao 800-1499 ou 2501-3200");
  Serial.println("CRITICO: pressao <800 ou >3200 ou radiacao >=2500");
  Serial.println("=====================================");
}

void loop() {
  pressureRaw  = analogRead(PIN_PRESSURE);
  radiationRaw = analogRead(PIN_RADIATION);
  statusAtual  = classificar(pressureRaw, radiationRaw);

  atualizarLEDs();

  Serial.print("P:"); Serial.print(pressureRaw);
  Serial.print(" R:"); Serial.print(radiationRaw);
  Serial.print(" -> "); Serial.println(statusAtual);

  server.handleClient();
  delay(500);
}

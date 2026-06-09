# Guardião de Habitats Extremos
### FIAP — Global Solution 2026 | Space Connect
**ODS 11 — Cidades e Comunidades Sustentáveis**

---

## O Problema Espacial

Em colônias lunares ou marcianas, a manutenção de condições atmosféricas internas é uma questão de sobrevivência. Variações de pressão (perda de vedação) ou picos de radiação (falha no escudo) podem ser fatais em minutos. Um sistema de monitoramento inteligente em tempo real, operando na borda (Edge AI), elimina a dependência de comunicação com a Terra (latência de até 20 minutos) e garante resposta imediata.

---

## A Solução

Sistema embarcado no **ESP32** que monitora continuamente dois parâmetros críticos do habitat:

| Sensor | Simulação | Representa |
|--------|-----------|------------|
| Potenciômetro | GPIO 34 | Pressão interna do habitat (hPa)

Um modelo de **TinyML** (treinado no Edge Impulse) classifica o ambiente em 3 estados:

| Status | Condição | Ação |
|--------|----------|------|
| **SEGURO** | Pressão 400–700 e radiação < 600 | Operação normal |
| **ALERTA** | Pressão 200–399 / 701–850 ou radiação 600–799 | Verificação necessária |
| **CRÍTICO** | Pressão < 200 / > 850 ou radiação ≥ 800 | Evacuação / emergência |

O status é exibido em tempo real via **Webserver HTTP** hospedado no próprio ESP32, acessível por qualquer dispositivo na rede local.

---

## Arquitetura

```
[Potenciômetro] ──→ GPIO34 ──┐
                              ├──→ ESP32 ──→ Classificação TinyML ──→ Webserver HTTP
[LDR]           ──→ GPIO35 ──┘                                         (porta 80)
                                                                           │
                                                              Astronauta via Browser 🧑‍🚀
```

---


## Como executar no Wokwi

1. Acesse: https://wokwi.com/projects/466303338472624129)
2. Clique em **▶ Start Simulation**
3. Gire o potenciômetro para simular variações de pressão
4. Abra o Serial Monitor para ver os logs
5. Acesse o IP exibido no serial para ver o Webserver

---

##  Integrantes

Enzo Barbeli RM554272
Felipe Santana RM554259

---

## Vídeo Demonstrativo

https://youtu.be/wAhX26mKGek

---

## Alinhamento ODS

**ODS 11 — Cidades e Comunidades Sustentáveis:** A solução garante a segurança e sustentabilidade de habitats humanos em ambientes extremos, diretamente aplicável tanto à colonização espacial quanto a instalações isoladas na Terra (bases antárticas, bunkers, plataformas offshore).

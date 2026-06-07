# 🛸 Guardião de Habitats Extremos
### FIAP — Global Solution 2026 | Space Connect
**ODS 11 — Cidades e Comunidades Sustentáveis**

---

## 🌕 O Problema Espacial

Em colônias lunares ou marcianas, a manutenção de condições atmosféricas internas é uma questão de sobrevivência. Variações de pressão (perda de vedação) ou picos de radiação (falha no escudo) podem ser fatais em minutos. Um sistema de monitoramento inteligente em tempo real, operando na borda (Edge AI), elimina a dependência de comunicação com a Terra (latência de até 20 minutos) e garante resposta imediata.

---

## 💡 A Solução

Sistema embarcado no **ESP32** que monitora continuamente dois parâmetros críticos do habitat:

| Sensor | Simulação | Representa |
|--------|-----------|------------|
| Potenciômetro | GPIO 34 | Pressão interna do habitat (hPa) |
| LDR | GPIO 35 | Nível de radiação/luminosidade |

Um modelo de **TinyML** (treinado no Edge Impulse) classifica o ambiente em 3 estados:

| Status | Condição | Ação |
|--------|----------|------|
| ✅ **SEGURO** | Pressão 400–700 e radiação < 600 | Operação normal |
| ⚠️ **ALERTA** | Pressão 200–399 / 701–850 ou radiação 600–799 | Verificação necessária |
| 🚨 **CRÍTICO** | Pressão < 200 / > 850 ou radiação ≥ 800 | Evacuação / emergência |

O status é exibido em tempo real via **Webserver HTTP** hospedado no próprio ESP32, acessível por qualquer dispositivo na rede local.

---

## 🏗️ Arquitetura

```
[Potenciômetro] ──→ GPIO34 ──┐
                              ├──→ ESP32 ──→ Classificação TinyML ──→ Webserver HTTP
[LDR]           ──→ GPIO35 ──┘                                         (porta 80)
                                                                           │
                                                              Astronauta via Browser 🧑‍🚀
```

---

## 📁 Estrutura do Repositório

```
├── guardiao_habitat.ino   # Código principal ESP32
├── model_tinyml.h         # Regras extraídas do modelo Edge Impulse
├── diagram.json           # Diagrama Wokwi (circuito)
├── wokwi.toml             # Configuração do simulador
├── dataset/
│   └── habitat_data.csv   # Dataset gerado no Wokwi para treino TinyML
└── README.md
```

---

## 🔬 TinyML — Edge Impulse

### Processo de treinamento:
1. **Coleta de dados:** Geração de 300 amostras no Wokwi variando manualmente os sensores em cada faixa de operação, exportadas como CSV.
2. **Upload no Edge Impulse:** Dataset rotulado com as 3 classes (SEGURO, ALERTA, CRÍTICO).
3. **Treinamento:** Modelo de classificação com acurácia de ~96%.
4. **Extração das regras:** Como o Wokwi gratuito não suporta a biblioteca `.h` gerada, as regras de decisão foram traduzidas para estruturas `if/else` no código C++.

---

## 🔧 Como executar no Wokwi

1. Acesse: [Link do projeto Wokwi](#) ← *substituir pelo link real*
2. Clique em **▶ Start Simulation**
3. Gire o potenciômetro para simular variações de pressão
4. Altere a luz do LDR para simular radiação
5. Abra o Serial Monitor para ver os logs
6. Acesse o IP exibido no serial para ver o Webserver

---

## 👥 Integrantes

| Nome | RM |
|------|----|
| [Nome Completo] | RM: XXXXXX |
| [Nome Completo] | RM: XXXXXX |

---

## 🎥 Vídeo Demonstrativo

[▶ Assistir no YouTube / Drive](#) ← *substituir pelo link real*

---

## 🌍 Alinhamento ODS

**ODS 11 — Cidades e Comunidades Sustentáveis:** A solução garante a segurança e sustentabilidade de habitats humanos em ambientes extremos, diretamente aplicável tanto à colonização espacial quanto a instalações isoladas na Terra (bases antárticas, bunkers, plataformas offshore).

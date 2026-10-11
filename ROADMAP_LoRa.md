# ROADMAP LoRa — LU6JMF

## Objetivo: "falar com todos os projetos"
A placa LoRa deve receber AO MESMO TEMPO:
- AX.25/HDLC do nakhonthai (payload começa com 0x7E)
- LoRa APRS padrão CA2RXU/richonguzman (payload começa com `<` 0xFF 0x01, texto TNC2)

## Achado (07/out, leitura do código)
- RX já decodifica os dois formatos pelo cabeçalho do pacote, sem depender de config
  (`lib/LibAPRS_LoRa/LibAPRS.cpp`, ramo `RF_MODE_LoRa` do poll: 0x7E -> hdlcDecodeAX25; `<\xFF\x01` -> APRS_getTNC2).
- `rf_ax25` (AX.25 Protocol) só muda o formato de TX:
  - on  -> transmite AX.25/HDLC (só nakhonthai entende)
  - off -> transmite `<\xFF\x01`+TNC2 (os dois entendem)  <- manter OFF
- Para escutar os dois, a camada física tem que ser igual à deles:
  - CA2RXU chama `radio.begin(freq)` sem setSyncWord -> padrão RadioLib = 0x12 (PRIVATE, no SX126x 0x1424)
  - nakhonthai padrão `rf_sync = 0x12` (18 decimal)
  - => Sync Word correto = **18** (0x12). **52 = 0x34 = LoRaWAN público: NÃO escuta nenhum dos dois.**
  - SF12 / BW125 / CR5 / preâmbulo 8 / 433,775 MHz.

## Próximos passos
1. Na placa: Sync Word 18, AX.25 off. Testar RX de uma estação CA2RXU e de uma nakhonthai.
2. Código: logar pacote recebido que não seja nenhum dos dois formatos (hoje é descartado em silêncio)
   e contador por formato (AX.25 / TNC2 / desconhecido) na página de status.
3. Opcional: TX duplo (mandar nos dois formatos) — só se aparecer estação nakhonthai que não decodifica TNC2.

## Pendências depois da 1.0 (10/out/2026)
- [ ] **Fixar a plataforma** no `platformio.ini`. Hoje é `releases/download/stable`, que se atualiza sozinha (mudou entre 07 e 09/out sem aviso). Fixar em `55.3.312`, a versão validada na 1.0.
- [ ] **Testes de campo em Buenos Aires:**
  - recepção dos formatos TNC2 e AX.25;
  - RSSI/SNR;
  - RF2INET;
  - digipeat;
  - LBT com tráfego real.
  - Depois disso, tirar a linha de "field tests" das release notes da 1.0.
- [ ] **Teste local de recepção** com a Heltec de 915 como tracker de teste (LU6JMF-7, 433.775, potência baixa, com antena, a poucos metros). Não é garantido, porque o filtro dela é para 915.
- [ ] **Tracker para o carro:** segunda Heltec V3 433 + GPS (ATGM336H/NEO-M8N em RX19/TX20), ou Wireless Tracker / T-Beam Supreme. Essas duas últimas têm perfil no código, mas não foram testadas.
- [ ] **Bateria da Heltec V3.2:** o ADC_CTRL (GPIO37) é ativo em **HIGH** na V3.2 e ativo em LOW na V3/V3.1. Detectar a versão no boot como o RNode (ler o pino) antes de implementar leitura de bateria e modo de economia. Hoje o setup deixa o GPIO37 em HIGH, e na V3.2 o divisor fica sempre ligado.
- [ ] **Apply sem TX imediato (decidir):** cada Apply nas abas IGATE/DIGI faz `initInterval` e, 10 s depois, transmite 3 frames (status iGate, posição, status digi; ~3 s cada em SF12). Opções:
  - Apply só salva;
  - atraso mínimo de 2 min.
- [ ] **Login de fábrica:** confirmar com o Marcelo se fica `admin`/`admin` ou `admin`/`Admin` (pergunta em aberto em 10/out).
- [ ] **Ícone do símbolo na página IGATE/DIGI:** a imagem ainda vem de `aprs.nakhonthai.net`. No áudio já foi resolvido com "servidor primeiro, fallback local".
- [ ] **Nome do job no `/txdebug`:** o status do digi aparece como "DIGI POS" porque usa `LU6JMF_TXJOB_DIGI_POSITION`. Separar para não confundir.
- [ ] **BLE derruba a Wireless Tracker V1.1 (10/out/2026, bloqueia a 1.1):** o testador de Buenos Aires ligou o Bluetooth (bin `1.1-dev-lu6jmf-test`, commit db1ebb7) e a placa entrou em loop de reinícios. Saiu com reinstalação.
  - No código, `bluetooth_init()` (NimBLE + NuSerial) roda no `taskNetwork` logo depois do `webService()`, então a placa reinicia antes de dar para desligar o BLE pela página.
  - Causa ainda não confirmada, falta o log serial (Guru Meditation/Backtrace). Hipóteses a verificar, não confirmadas:
    - falta de memória (sem PSRAM, mais o buffer da TFT de cerca de 25 KB);
    - conflito BLE/WiFi.
  - Teste a fazer: ligar o BLE na V3 (LU6JMF-3, `heltec_V3_GPS` também tem `-DBLUETOOTH`) com USB no Mac e o monitor serial com decoder. Isso mostra se o problema é geral, e afetaria a 1.0 publicada, ou só da Tracker.
  - Saída do loop: segurar o PRG durante a tela de boot ("Factory Reset!") sobe com os padrões só em RAM; o default.cfg antigo continua lá até um Apply. Melhoria possível: proteção contra loop, que desliga o BLE depois de N reinícios seguidos.
  - O perfil BLE (serviço 00000001-ba2a-…, notify 0003, write 0002) é o mesmo do app aprs.fi/Mobilinkd. Usar o MODE KISS.
  - **Medido na V3 (11/out, build de diagnóstico):** BLE desligado ~117 KB livres estável; BLE ligado ~47 KB (mínimo 24 KB). O BLE consome cerca de 70 KB.
    - Abaixo de 50 KB por mais de 5 s, a proteção "Automatic Restart when heap memory not enough" (`loop()`) reinicia a placa → loop. Confirmado pelo backtrace com o ELF certo.
    - O reinício ainda dá `assert failed`, porque `vTaskSuspendAll()` vem antes de `WiFi.disconnect()`. Corrigir a ordem.
    - Tentativa 1 (não testada, no `platformio.ini` da V3 e da Tracker): `CONFIG_BT_NIMBLE_ROLE_CENTRAL_DISABLED`, `ROLE_OBSERVER_DISABLED`, `MAX_CONNECTIONS=1`. Se não bastar: desativar o BLE sem PSRAM ou desligar o AP com o BLE ativo.
  - **Resolvido (11/out), no mesmo critério do CA2RXU: BLE e WiFi nunca juntos.**
    - BLE desabilitado: WiFi normal.
    - BLE habilitado (gravado): todo boot tem 2 min de WiFi (janela para desabilitar o BLE pela página); depois WiFi OFF e BLE ON.
    - Apply no bloco Bluetooth grava e reinicia. A página agora mostra a resposta real da placa (antes era sempre "Submited Successfully").
    - PRG 2 s na tela WiFi do display com BLE ativo: reinicia (volta a janela de WiFi).
    - Reinício por memória baixa corrigido: WiFi off antes do `vTaskSuspendAll()`. Mesmo ajuste no auto-reset.
    - NimBLE só periférico, 1 conexão (`platformio.ini` da V3 e da Tracker).
    - Medido na V3: BLE ON + WiFi OFF = ~93 KB livres estável (mínimo 88 KB).
    - App aprs.fi (iPhone) conectou na V3 e o envio de baliza pelo app pareceu funcionar. Falta testar RX com tráfego (Buenos Aires).
- [ ] **Menu do botão PRG (implementado 11/out, falta compilar e testar na V3 e na Tracker):**
  - 1 toque = próxima página (espera 0,4 s para ver se é toque duplo). 2 toques = MENU: Screen OFF, BLE ON/WiFi OFF ou BLE OFF/WiFi ON (grava e reinicia), Power OFF, Reboot, Factory Reset, Exit.
  - No menu: 1 toque = próximo item, segurar 2 s = escolher. Power OFF, Reboot e Factory Reset pedem confirmação. Fecha sozinho com 15 s sem uso.
  - Power OFF saiu da página System (segurar 2 s) e foi para o menu; religa com o PRG (deep sleep com ext0 no GPIO0). Consumo em deep sleep (display/rádio) não medido.
  - Factory Reset ao ligar: só com o PRG segurado 15 s (contagem na tela, soltar cancela). Grava os padrões no default.cfg (apaga a configuração) e reinicia.
  - msgBox agora desenha também na TFT da Tracker (antes só no OLED).

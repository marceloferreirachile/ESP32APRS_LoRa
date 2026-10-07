# ESP32APRS_LoRa-LU6JMF — estudo e plano (branch lu6jmf/stability)

Alvo: Heltec V3 (ESP32-S3, SX1262, sem PSRAM), env `heltec_V3_GPS`, 915 MHz. Base: nakhonthai V0.9b (27ce8ed).
Tudo abaixo é leitura de código; nada foi medido no aparelho.

## Decisões
- Paleta LoRa: roxo violeta / lavanda escuro. Áudio continua preto. Na versão 2 em 1 cada parte mantém a sua.
- Atualização OTA sem perder configuração: mesma tabela de partições (`default_8MB.csv`), mesmo formato e nomes de campo de config; campos novos só no fim, com valor padrão.
- Instalação do zero: um único comando esptool com um .bin completo (bootloader + partições + app + littlefs).

## Como o TX funciona hoje
- `pkgTxPush`/`pkgTxSend` (main.cpp) -> `APRS_sendTNC2Pkt` (LibAPRS.cpp) enche `txFifo` -> `APRS_poll()` em `taskAPRSPoll` chama `radioHal->transmit()` (bloqueante, timeout 5x airtime), depois `startRx()`.
- Retornos de `transmit`/`startReceive` são ignorados. Sem retry, sem contador, sem LBT/CAD.
- Fila: 5 slots (`PKGTXSIZE`), slot só libera após 60 s; fila cheia descarta em silêncio e devolve true.
- `ax25_sendBuf` faz `fifo_flush`: dois pacotes vencidos no mesmo ciclo -> o segundo apaga o primeiro.

## Riscos de reinício (ordem)
1. `allocateStringMemory` sem NULL check (webservice.cpp) — CORRIGIDO neste branch.
2. Stacks de 4096 B em `taskAPRS` e `taskAPRSPoll` com locais grandes (pkg[500], str[300]...). Medir antes de aumentar.
3. Estouros de heap remotos: `igate.cpp` (buffer 300 B) — CORRIGIDO; `aprs_msg_callback` (`msg->info[255]` + [RSSI]/[SNR]/[FREQERR]) — PENDENTE.
4. Sem locks em txQueue, pkgList, msgQueue, PacketBuffer, FIFOs AX.25 e rádio (sem PSRAM o `psramBusy` nem compila). `APRS_init` pode rodar durante TX (página web de rádio).
5. `vTaskSuspendAll()` antes de `WiFi.*`/`esp_restart` — REMOVIDO neste branch.
6. `APRS_init` vaza `RadioHal`/`Module` a cada re-init (a cada 10–15 min sem RX) — PENDENTE.
7. Fila: slot preso 60 s, descarte silencioso, flush do FIFO — resolve o scheduler portado.
8. `transmit()` longo em task de prioridade 3 pode matar IDLE0.
9. Modo sleep B apaga tasks e depois `vTaskResume` em handle morto — PENDENTE.
10. Sem diagnóstico de reset — PARCIAL: motivo do reset (inclui "low heap"/"auto reset") no log e na página About; heap mínima e maior bloco na About.

## Já aplicado neste branch (não compilado ainda)
- allocateStringMemory: NULL check.
- igate.cpp: limite de 299 B em cabeçalho + info.
- setup(): reinicia cedo se alguma tabela principal não alocar.
- Sequência de reset (auto reset e heap): sem `vTaskSuspendAll()`; grava motivo em RTC.
- Motivo do reset legível (POWERON/SW/PANIC/INT_WDT/TASK_WDT/WDT/BROWNOUT) + causa SW (low heap / auto reset), About mostra "Last reset" e "Heap min / largest block".

## O que portar do motor do áudio (inventário)
PORTAR COMO ESTÁ: enums de job/prioridade, `pkgTxPushJob`, fila com preempção, ring de eventos TX, caixa-preta RTC, campanha BLN/NEWS, Objects, PRIME, motor de templates, queries ?APRSV/?IGATE/?DX, correções de upload.
PORTAR ADAPTANDO: `pkgTxSend` (TX concluído = retorno de `transmit()`, sem PTT; remover bloco SA818), watchdog (recuperação = reset do SX1262), `/txdebug` (rótulos; incluir RSSI/SNR/airtime), casca SPA (abas e paleta violeta), retry-on-full (estender a BLN/NEWS/TLM/posição).
NÃO PORTAR: calibração de áudio, AFSK/AX.25 timers, `pkgTxPushBeacon`/`nextTxSlot` (revertido em campo), netdebug e TCP KISS (inacabados).
Ordem: base do TX + job/prioridade -> ring+caixa-preta -> scheduler -> reclassificar call-sites -> watchdog/reset SX1262 -> txdebug/PRIME/templates -> mensagens/ACK -> BLN/Objects -> queries -> shell/paleta.

## Recuperação do SX1262 sem reboot (desenho)
mutex do rádio -> `clearDio1Action` -> zera flags (`received`, `ax25_stateTx`) -> pulso em RST -> deletar objetos antigos (corrigir o vazamento antes) -> repetir sequência de `APRS_init` checando o retorno de `begin()` -> `startRx()` -> liberar o scheduler. Conferir TCXO (código usa 1,6 V; Heltec V3 documenta 1,8 V).

## Pendências / decisões abertas
- Definir "TX concluído" no LoRa (proposta: retorno de `transmit()` com código checado).
- `/txdebug` depende de internet (PRIME); site offline não abre. Decidir se vale um modo local.
- Medir stack high-water e heap com a build de diagnóstico antes de mudar tamanhos.

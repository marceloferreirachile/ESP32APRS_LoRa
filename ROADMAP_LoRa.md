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

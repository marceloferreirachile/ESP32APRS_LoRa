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

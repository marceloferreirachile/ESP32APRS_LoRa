# AR-LORA — releases e procedimento de publicação

## v1.1-lu6jmf — 13/out/2026
- **Código:** o do `v1.1-beta-lu6jmf` com `-DVERSION="1.1"`, mais o "Send Beacon" no menu do PRG. O Send Beacon foi testado pelo Marcelo e entrou em 13/out com troca silenciosa dos .bin, sem mudar a versão. A tag `v1.1-lu6jmf` foi movida para esse commit.
- **Motivo:** o Marcelo confirmou que tudo funcionou na Tracker em Buenos Aires (13/out).
- **md5 do firmware (final, com Send Beacon):**
  - V3 `660a5aab…`;
  - Tracker `70b2dc63…`.
  - Primeira publicação, sem Send Beacon: V3 `25efdce3…`, Tracker `364b4f9b…`.
- **littlefs:** igual ao do 1.0/1.1-beta (`8414d3cb…`).
- **version.json:** 1.1. Avisa quem está no 1.0 e no 1.1-beta.

## v1.1-beta-lu6jmf — 11/out/2026 (pre-release)
- **Código:** tag privada `v1.1-beta-lu6jmf` (commit `2a29f68`), branch `lu6jmf/from-2.1.5`.
- **Público:** commit `ff2d601` no `master`. Release `v1.1-beta-lu6jmf` marcada como **pre-release**, com `HELTEC_V3_v1.1-beta-lu6jmf.bin`, `HELTEC_TRACKER_v1.1-beta-lu6jmf.bin`, `main_shell.html`, `style.css` e `icons.dat`. Os arquivos web são idênticos aos do 1.0.
- **Instalador:** um botão por placa.
  - `manifest.json` → `docs/bin/heltec_v3/`;
  - `manifest-tracker.json` → `docs/bin/tracker/`.
  - Mesmas posições de gravação do 1.0.
- **version.json:** 1.1-beta. Decisão do Marcelo: avisar quem está no 1.0.
- **md5 conferido na release:**
  - V3 `2d0fdc66…`;
  - Tracker `7f68f574…`.
  - Idênticos aos compilados.
- **Novidades:**
  - Wireless Tracker V1.1;
  - BLE KISS TNC alternando com o WiFi (janela de 2 min);
  - menu do PRG;
  - Factory Reset com 15 s;
  - KISS reconhecido automaticamente;
  - reinícios limpos.
- **Validado na LU6JMF-3 (V3.2):**
  - BLE com 92 KB livres;
  - APRSSwift e aprs.fi conectam;
  - baliza do app vai para a fila de TX (`[KISS] TX queued`);
  - menu: BLE ON e BLE mode.
- **Confirmado em Buenos Aires (11/out, LU2APR, Wireless Tracker 1.1-beta):** app no celular por BLE em modo KISS.
  - Mensagem LU2APR-8 → LU2APR-9 saiu em RF.
  - O eco veio pelo digi LU2APR-10*.
  - Os ACKs da LU2APR-9 chegaram ao celular.
  - Ou seja, TX e RX pelo Bluetooth funcionando nos dois sentidos.
  - Em modo TNC2 o app não vê nada, porque a placa manda texto e o app espera KISS.
  - Teve um loop de reinício ao ativar o BLE, que o testador atribuiu a algo local. Causa não confirmada.
- **Falta confirmar (Buenos Aires):**
  - Tracker com o menu na TFT;
  - Power OFF / acordar com o PRG;
  - contagem de 15 s;
  - RX de outras estações e envio dos pacotes recebidos ao celular;
  - baliza do app chegando ao aprs.fi por iGate LoRa.

## v1.0-lu6jmf — 10/out/2026
- **Código:** tag privada `v1.0-lu6jmf` (commit `1eba251`), branch `lu6jmf/from-2.1.5`, repositório privado `ESP32APRS_LoRa-LU6JMF`.
- **Público:** https://github.com/marceloferreirachile/ESP32APRS_LoRa (branch `master`, só binários, instalador, README e LICENSE).
- **Instalador:** https://marceloferreirachile.github.io/ESP32APRS_LoRa/ (Pages a partir de `master` /docs).
- **Release:** `v1.0-lu6jmf`, com `HELTEC_V3_v1.0-lu6jmf.bin`, `main_shell.html`, `style.css` e `icons.dat`. As notas estão em inglês, com a oferta GPL v3 §6(b).
- **Compilação:** env `heltec_V3_GPS`, ESP-IDF 5.5.5 (plataforma pioarduino "stable" = 55.3.312 na data).
- **md5 do firmware.bin publicado:** `614af31a…`, idêntico ao validado.
- **Validado na LU6JMF-3** (Heltec V3.2, 433 MHz):
  - 22 h sem reinício;
  - iGate APRS-IS;
  - beacons em RF e Internet;
  - fila TX, LBT e `/txdebug`;
  - OTA pela página About;
  - LAST HEARD;
  - calculadora de PHG;
  - página 5 do OLED;
  - About.
- **Ainda não testado:** recepção e digipeat de outras estações LoRa. Não há estações LoRa em Concepción del Uruguay; isso fica para os testes em Buenos Aires.
- **Nota temporária "field tests":** retirada das release notes em 13/out (`gh release edit`). Notas públicas ficam curtas: novidades e instalação.

## Padrões de fábrica (placa nova ou Factory Reset)
O firmware cria o `default.cfg` sozinho no primeiro boot. O instalador **não** leva nenhum `default.cfg`.
- **Rede de configuração:** `AR-LORA_AP`, **sem senha**.
- **Login web:** `admin` / `admin`.
- **Indicativo:** NOCALL, com todos os modos desligados (não transmite até ser configurada).
- **Rádio:** 433.775 MHz, SF12, BW125, CR 4/5, sync 0x12, 20 dBm, TX em TNC2.
- **Símbolos:** iGate **L&**, digi **L#**. A = AR-APRS (áudio), L = AR-LORA.

## Procedimento de publicação (próximas versões)
1. **Validar antes na LU6JMF-3, por OTA:** About → Manual Firmware Update com o `firmware.bin` compilado. Nunca publicar sem validar.
2. **Versão e tag privada:**
   - trocar `-DVERSION` no `platformio.ini` (`X.Y`, sem `-dev`);
   - commit, `git tag vX.Y-lu6jmf`, push do branch e da tag no privado.
3. **Compilar:**
   - firmware: `pio run -e heltec_V3_GPS`;
   - littlefs: `pio run -e heltec_V3_GPS -t buildfs`.
   - Conferir que o littlefs **não** tem `default.cfg` nem dados pessoais.
4. **Repositório público** `~/dev/ESP32APRS_LoRa-public`:
   - `docs/bin/` ← `bootloader.bin`, `partitions.bin`, `firmware.bin` e `littlefs.bin` de `.pio/build/heltec_V3_GPS/`;
   - `docs/manifest.json` e `docs/index.html`: versão;
   - `version.json`: versão, data, tag e `filename` = `HELTEC_V3_vX.Y-lu6jmf.bin`, que é o nome que o About procura.
5. **Posições de gravação ESP32-S3**, lidas do `partitions.bin` (`default_8MB`):

   | Arquivo | Posição |
   |---|---|
   | bootloader | 0x0 |
   | partitions | 0x8000 (32768) |
   | firmware | 0x10000 (65536) |
   | littlefs | 0x670000 (6750208) |

   Tamanhos: app0/app1 têm 3264 KB cada; spiffs/littlefs tem 1536 KB.
6. **Branch público:** sempre `master`. O firmware busca `.../master/version.json`.
7. **Release:**
   - `gh release create vX.Y-lu6jmf` com o .bin renomeado, mais `main_shell.html`, `style.css` e `icons.dat`;
   - notas em inglês com a oferta GPL e o aviso de que os "Source code" do GitHub não contêm o firmware.
8. **Conferir o que foi publicado:**
   - md5 do .bin da release = md5 do validado;
   - `version.json` raw;
   - deploy do github-pages ✓;
   - página do instalador abre.

## Regras
- Placa configurada: **nunca `uploadfs`**, porque apaga a configuração. Os arquivos da interface vão pela aba File.
- Código-fonte só no repositório privado. O público recebe só binários e instalador.

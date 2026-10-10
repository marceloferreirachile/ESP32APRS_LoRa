# AR-LORA — releases e procedimento de publicação

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
- **Nota temporária:** as release notes têm a linha "Reception and digipeating of other LoRa stations will be confirmed in field tests". **Tirar depois que Buenos Aires confirmar** (`gh release edit`).

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

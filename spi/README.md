# SPI

[SPI (*eng. Serial Peripheral Interface*)](https://www.analog.com/en/resources/analog-dialogue/articles/introduction-to-spi-interface.html) je serijski i sinkroni dvosmjerni full-duplex protokol kojeg je razvila Motorola.

## Način rada SPI

Uređaji na koji koriste SPI mogu imati jednu od sljedećih uloga:

- *master* (u novije vrijeme zvan *main*)
	- inicira komunikaciju pisanja ili čitanja
- *slave* (u novije vrijeme zvan *subnode*)
	- sluša mastera i odgovara mu na zahtjeve pisanja i čitanja

SPI koristi četiri linije za komunikaciju između dva uređaja:

- CS (*eng. Chip Select*)
	- odabir ili aktivacija čipa
- SCLK (*eng. Serial Clock*)
	- sabirnica takt
- MOSI (*eng. Master Out Slave In*)
	- sabirnica gdje *master* piše na odabrani *slave*
- MISO (*eng. Master In Slave Out*)
	- sabirnica gdje *slave* piše na odabrani *master*

Jedan *master* može komunicirati s više *slaveova* na različite načine kao što će se vidjeti u primjerima.

### Brzine rada SPI

Brzina rada SPI sklopa nije definirana u specifikaciji protokola već ovisi o mogućnostima uređaja spojenih na sabirnicama i samim sabirnicama.

### Spajanje uređaja na SPI

```
        +-------------+                         +-------------+
        | SPI Master  |                         | SPI Slave   |
        |             |                         |             |
        |        MOSI |------------------------>| MOSI        |
        |             |                         |             |
        |        MISO |<------------------------| MISO        |
        |             |                         |             |
        |        SCLK |------------------------>| SCLK        |
        |          __ |                         | __          |
        |          CS |------------------------>| CS          |
        |             |                         |             |
        +-------------+                         +-------------+  
```

U slučaju kada *master* upravlja s više *slaveova* s više CS linija:

```
        +-------------+                         +-------------+
        | SPI Master  |                         | SPI Slave   |
        |             |                         |             |
        |        MOSI |-------------------+---->| MOSI        |
        |             |                   |     |             |
        |        MISO |<---------------+--|-----| MISO        |
        |             |                |  |     |             |
        |        SCLK |-------------+--|--|---->| SCLK        |
        |         ___ |             |  |  |     | __          |
        |         CS1 |-------------|--|--|---->| CS          |
        |         ___ |             |  |  |     |             |
        |         CS2 |----------+  |  |  |     +-------------+
        |          .  |  .       |  |  |  |
        |          .  |  .       |  |  |  |     +-------------+
        |          .  |  .       |  |  |  |     | SPI Slave   |
        |         ___ |          |  |  |  |     |             |
        |         CSN |----+     |  |  |  +---->| MOSI        |
        |             |    |     |  |  |  |     |             |
        +-------------+    |     |  |  +--|-----| MISO        |
                           | ... |  |  |  |     |             |
                           |     |  +--|--|---->| SCLK        |
                           |     |  |  |  |     | __          |
                           |     +--|--|--|---->| CS          |
                           |        |  |  |     |             |
                           |        |  |  |     +-------------+
                           |        |  |  |            .
                           |        .  .  .            .
                           |        .  .  .            .
                           |        .  .  .
                           |        |  |  |     +-------------+
                           |        |  |  |     | SPI Slave   |
                           |        |  |  |     |             |
                           |        |  |  +---->| MOSI        |
                           |        |  |        |             |
                           |        |  +--------| MISO        |
                           |        |           |             |
                           |        +---------->| SCLK        |
                           |                    | __          |
                           +------------------->| CS          |
                                                |             |
                                                +-------------+
```

U prethodnom slučaju *master* odabire *slave* s kojim će razmijeniti podatke odgovarajućom CS linijom.

U slučaju kada *master* upravlja s više *slaveova* s metodom ulančavanja:

```
        +-------------+                         +-------------+
        | SPI Master  |                         | SPI Slave   |
        |             |                         |             |
        |        MOSI |------------------------>| MOSI        |
        |             |                         |             |
        |        MISO |<---------------+  +-----| MISO        |
        |             |                |  |     |             |
        |        SCLK |-------------+--|--|---->| SCLK        |
        |          __ |             |  |  |     | __          |
        |          CS |----------+--|--|--|---->| CS          |
        |             |          |  |  |  |     |             |
        +-------------+          |  |  |  |     +-------------+
                                 |  |  |  |
                                 |  |  |  |     +-------------+
                                 |  |  |  |     | SPI Slave   |
                                 |  |  |  |     |             |
                                 |  |  |  +---->| MOSI        |
                                 |  |  |        |             |
                                 |  |  |  +-----| MISO        |
                                 |  |  |  |     |             |
                                 |  +--|--|---->| SCLK        |
                                 |  |  |  |     | __          |
                                 +--|--|--|---->| CS          |
                                 |  |  |  |     |             |
                                 |  |  |  |     +-------------+
                                 |  |  |  |            .
                                 .  .  .  .            .
                                 .  .  .  .            .
                                 .  .  .  .
                                 |  |  |  |     +-------------+
                                 |  |  |  |     | SPI Slave   |
                                 |  |  |  |     |             |
                                 |  |  |  +---->| MOSI        |
                                 |  |  |        |             |
                                 |  |  +--------| MISO        |
                                 |  |           |             |
                                 |  +---------->| SCLK        |
                                 |              | __          |
                                 +------------->| CS          |
                                                |             |
                                                +-------------+
```

U prethodnom slučaju *master* odabire sve *slaveove* te se podatci propagiraju od jednog do drugog *slavea*. Uređaji moraju imati potporu za ovakav način rada. Prednost ovakvog načina rada je što se ne zahtijeva pojedinačna CS linija po čipu.

### Načini komunikacije s obzirom na polaritet i fazu takta

Postoje 4 načina komunikacije s obzirom na polaritet i fazu takta:

- polaritet takta je 0 u trenutku mirovanja, a podatci se uzorkuju na rastući brid i izmjenjuju na padajući odnosno CPOL = 0, CPHA = 0
- polaritet takta je 0 u trenutku mirovanja, a podatci se uzorkuju na padajući brid i izmjenjuju na rastući odnosno CPOL = 0, CPHA = 1
- polaritet takta je 1 u trenutku mirovanja, a podatci se uzorkuju na rastući brid i izmjenjuju na padajući odnosno CPOL = 1, CPHA = 0
- polaritet takta je 1 u trenutku mirovanja, a podatci se uzorkuju na padajući brid i izmjenjuju na rastući odnosno CPOL = 1, CPHA = 1

Šalje li se prvo najmanje značajan bit (LSB) ili najviše značajan bit (MSB) nije definirano u specifikaciji dakle ovisi o konfiguraciji samih uređaja.

Kada je CPOL = 0, CPHA = 0:

```
             +------+                                                                                                                                                                            +------+
        CS           \                                                                                                                                                                          /
                      \                                                                                                                                                                        /
                       +----------------------------------------------------------------------------------------------------------------------------------------------------------------------+


                                 +------+            +------+            +------+            +------+            +------+            +------+            +------+            +------+
        SCLK                    /        \          /        \          /        \          /        \          /        \          /        \          /        \          /        \
                               /          \        /          \        /          \        /          \        /          \        /          \        /          \        /          \
             +----------------+            +------+            +------+            +------+            +------+            +------+            +------+            +------+            +----------------+

                                A                   A                   A                   A                   A                   A                   A                   A
                                | SAMPLE            | SAMPLE            | SAMPLE            | SAMPLE            | SAMPLE            | SAMPLE            | SAMPLE            | SAMPLE
                                |                   |                   |                   |                   |                   |                   |                   |
                                V                   V                   V                   V                   V                   V                   V                   V

             +-----------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +-----------+
        MOSI       X      \/       W-D0       \/       W-D1       \/       W-D2       \/       W-D3       \/       W-D4       \/       W-D5       \/       W-D6       \/       W-D7       \/      X
                          /\                  /\                  /\                  /\                  /\                  /\                  /\                  /\                  /\
             +-----------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +-----------+

                                A                   A                   A                   A                   A                   A                   A                   A
                                | SAMPLE            | SAMPLE            | SAMPLE            | SAMPLE            | SAMPLE            | SAMPLE            | SAMPLE            | SAMPLE
                                |                   |                   |                   |                   |                   |                   |                   |
                                V                   V                   V                   V                   V                   V                   V                   V

             +-----------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +-----------+
        MISO       X      \/       R-D0       \/       R-D1       \/       R-D2       \/       R-D3       \/       R-D4       \/       R-D5       \/       R-D6       \/       R-D7       \/      X
                          /\                  /\                  /\                  /\                  /\                  /\                  /\                  /\                  /\
             +-----------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +-----------+


             W-DN - Bits written from master to slave
             R-DN - Bits read from slave to master
             X - Undefined state
```

Kada je CPOL = 0, CPHA = 1:

```
             +------+                                                                                                                                                                            +------+
        CS           \                                                                                                                                                                          /
                      \                                                                                                                                                                        /
                       +----------------------------------------------------------------------------------------------------------------------------------------------------------------------+


                                 +------+            +------+            +------+            +------+            +------+            +------+            +------+            +------+
        SCLK                    /        \          /        \          /        \          /        \          /        \          /        \          /        \          /        \
                               /          \        /          \        /          \        /          \        /          \        /          \        /          \        /          \
             +----------------+            +------+            +------+            +------+            +------+            +------+            +------+            +------+            +----------------+

                                          A                   A                   A                   A                   A                   A                   A                   A
                                          | SAMPLE            | SAMPLE            | SAMPLE            | SAMPLE            | SAMPLE            | SAMPLE            | SAMPLE            | SAMPLE
                                          |                   |                   |                   |                   |                   |                   |                   |
                                          V                   V                   V                   V                   V                   V                   V                   V

             +-----------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +-----------+
        MOSI       X      \/       W-D0       \/       W-D1       \/       W-D2       \/       W-D3       \/       W-D4       \/       W-D5       \/       W-D6       \/       W-D7       \/      X
                          /\                  /\                  /\                  /\                  /\                  /\                  /\                  /\                  /\
             +-----------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +-----------+

                                          A                   A                   A                   A                   A                   A                   A                   A
                                          | SAMPLE            | SAMPLE            | SAMPLE            | SAMPLE            | SAMPLE            | SAMPLE            | SAMPLE            | SAMPLE
                                          |                   |                   |                   |                   |                   |                   |                   |
                                          V                   V                   V                   V                   V                   V                   V                   V

             +-----------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +-----------+
        MISO       X      \/       R-D0       \/       R-D1       \/       R-D2       \/       R-D3       \/       R-D4       \/       R-D5       \/       R-D6       \/       R-D7       \/      X
                          /\                  /\                  /\                  /\                  /\                  /\                  /\                  /\                  /\
             +-----------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +-----------+


             W-DN - Bits written from master to slave
             R-DN - Bits read from slave to master
             X - Undefined state
```

Kada je CPOL = 1, CPHA = 0:

```
             +------+                                                                                                                                                                            +------+
        CS           \                                                                                                                                                                          /
                      \                                                                                                                                                                        /
                       +----------------------------------------------------------------------------------------------------------------------------------------------------------------------+


             +----------------+            +------+            +------+            +------+            +------+            +------+            +------+            +------+            +----------------+
        SCLK                   \          /        \          /        \          /        \          /        \          /        \          /        \          /        \          /
                                \        /          \        /          \        /          \        /          \        /          \        /          \        /          \        /
                                 +------+            +------+            +------+            +------+            +------+            +------+            +------+            +------+

                                A                   A                   A                   A                   A                   A                   A                   A
                                | SAMPLE            | SAMPLE            | SAMPLE            | SAMPLE            | SAMPLE            | SAMPLE            | SAMPLE            | SAMPLE
                                |                   |                   |                   |                   |                   |                   |                   |
                                V                   V                   V                   V                   V                   V                   V                   V

             +-----------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +-----------+
        MOSI       X      \/       W-D0       \/       W-D1       \/       W-D2       \/       W-D3       \/       W-D4       \/       W-D5       \/       W-D6       \/       W-D7       \/      X
                          /\                  /\                  /\                  /\                  /\                  /\                  /\                  /\                  /\
             +-----------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +-----------+

                                A                   A                   A                   A                   A                   A                   A                   A
                                | SAMPLE            | SAMPLE            | SAMPLE            | SAMPLE            | SAMPLE            | SAMPLE            | SAMPLE            | SAMPLE
                                |                   |                   |                   |                   |                   |                   |                   |
                                V                   V                   V                   V                   V                   V                   V                   V

             +-----------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +-----------+
        MISO       X      \/       R-D0       \/       R-D1       \/       R-D2       \/       R-D3       \/       R-D4       \/       R-D5       \/       R-D6       \/       R-D7       \/      X
                          /\                  /\                  /\                  /\                  /\                  /\                  /\                  /\                  /\
             +-----------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +-----------+


             W-DN - Bits written from master to slave
             R-DN - Bits read from slave to master
             X - Undefined state
```

Kada je CPOL = 1, CPHA = 1:

```
             +------+                                                                                                                                                                            +------+
        CS           \                                                                                                                                                                          /
                      \                                                                                                                                                                        /
                       +----------------------------------------------------------------------------------------------------------------------------------------------------------------------+


             +----------------+            +------+            +------+            +------+            +------+            +------+            +------+            +------+            +----------------+
        SCLK                   \          /        \          /        \          /        \          /        \          /        \          /        \          /        \          /
                                \        /          \        /          \        /          \        /          \        /          \        /          \        /          \        /
                                 +------+            +------+            +------+            +------+            +------+            +------+            +------+            +------+

                                          A                   A                   A                   A                   A                   A                   A                   A
                                          | SAMPLE            | SAMPLE            | SAMPLE            | SAMPLE            | SAMPLE            | SAMPLE            | SAMPLE            | SAMPLE
                                          |                   |                   |                   |                   |                   |                   |                   |
                                          V                   V                   V                   V                   V                   V                   V                   V

             +-----------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +-----------+
        MOSI       X      \/       W-D0       \/       W-D1       \/       W-D2       \/       W-D3       \/       W-D4       \/       W-D5       \/       W-D6       \/       W-D7       \/      X
                          /\                  /\                  /\                  /\                  /\                  /\                  /\                  /\                  /\
             +-----------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +-----------+

                                          A                   A                   A                   A                   A                   A                   A                   A
                                          | SAMPLE            | SAMPLE            | SAMPLE            | SAMPLE            | SAMPLE            | SAMPLE            | SAMPLE            | SAMPLE
                                          |                   |                   |                   |                   |                   |                   |                   |
                                          V                   V                   V                   V                   V                   V                   V                   V

             +-----------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +-----------+
        MISO       X      \/       R-D0       \/       R-D1       \/       R-D2       \/       R-D3       \/       R-D4       \/       R-D5       \/       R-D6       \/       R-D7       \/      X
                          /\                  /\                  /\                  /\                  /\                  /\                  /\                  /\                  /\
             +-----------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +----------------+  +-----------+


             W-DN - Bits written from master to slave
             R-DN - Bits read from slave to master
             X - Undefined state
```

### ESP32 i SPI

ESP32 sadrži četiri [SPI sklopa](https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf#spi) čije se linije mogu usmjeriti na bilo koji GPIO matricom ili se mogu koristiti zadani GPIO pinovi (IO_MUX). Prva dva SPI sklopa se općenito koriste za interni pristup flash memoriji, iako ih je moguće koristiti za druge svrhe. Druga dva SPI sklopa, zvana HSPI i VSPI, se mogu slobodno koristiti za druge svrhe. Za primjere se koristila pločica [*ESP32-DevKitM-1*](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp32-devkitm-1/user_guide.html#getting-started) koja na sebi ima modul [ESP32-MINI-1](https://www.espressif.com/sites/default/files/documentation/esp32-mini-1_datasheet_en.pdf).

IO_MUX pinovi za HSPI (SPI2) su sljedeći:

- CS0 - GPIO 15
- SCLK - GPIO 14
- MISO - GPIO 12
- MOSI - GPIO 13
- QUADWP - GPIO 2
- QUADHD - GPIO 4

IO_MUX pinovi za HSPI (SPI2) su sljedeći:

- CS0 - GPIO 5
- SCLK - GPIO 18
- MISO - GPIO 19
- MOSI - GPIO 23
- QUADWP - GPIO 22
- QUADHD - GPIO 21

Za ESP32 se preporučuje koristiti maksimalnu frekvenciju takta 10 MHz ako se koriste kao *slaveovi*. SPI upravljački program **nije** dretveno siguran (*eng. Thread-Safe*) pa je potrebno osigurati pristup SPI uređaju kao pristup kritičnoj sekciji (primjerice FreeRTOS monitorom).

Primjer gdje ESP32 radi kao *master* koristeći jedan SPI sklop nalazi se [ovdje](spi_master).

Primjer gdje ESP32 radi kao *slave* koristeći jedan SPI sklop nalazi se [ovdje](spi_slave).

Primjer gdje ESP32 radi istovremena i kao *master* i kao *slave* koristeći dva SPI sklopa nalazi se [ovdje](spi_master_slave).

#### ESP32 kao SPI master

Zaglavlje koje je potrebno uključiti za [postavljanje ESP32 kao *master*](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/spi_master.html#id14) je ```driver/spi_master.h```. Ovim se uključuje i upravljački program koji predstavlja apstrakciju *mastera*.

##### Inicijaliziraj sklop
```
esp_err_t spi_bus_initialize(spi_host_device_t host_id, const spi_bus_config_t *bus_config, spi_dma_chan_t dma_chan)
```

Parametri ove funkcije su:

- *host_id*
	- sklop koji je potrebno inicijalizirati, enumerator može biti:
		- *SPI1_HOST* - drugi interni sklop kojeg je moguće usmjeriti prema van kao master, nije podržan
		- *SPI2_HOST* - treći sklop, HSPI
		- *SPI3_HOST* - četvrti sklop, VSPI
		- *SPI_HOST_MAX* - nevaljani sklop
- *bus_config*
	- konfiguracija sabirnice, struktura ima članove:
		- *mosi_io_num*
			- varijabla tipa *int* koja koja označava GPIO MOSI linije, -1 ako se ne koristi
		- *miso_io_num*
			- varijabla tipa *int* koja koja označava GPIO MISO linije, -1 ako se ne koristi
		- *sclk_io_num*
			- varijabla tipa *int* koja koja označava GPIO SCLK linije, -1 ako se ne koristi
		- *data0_io_num*
			- varijabla tipa *int* koja koja označava GPIO DATA0 linije koja se koristi u posebnom QUAD/OCTAL načinu rada, -1 ako se ne koristi
		- *data1_io_num*
			- varijabla tipa *int* koja koja označava GPIO DATA1 linije koja se koristi u posebnom QUAD/OCTAL načinu rada, -1 ako se ne koristi
		- *data2_io_num*
			- varijabla tipa *int* koja koja označava GPIO DATA2 linije koja se koristi u posebnom QUAD/OCTAL načinu rada, -1 ako se ne koristi
		- *data3_io_num*
			- varijabla tipa *int* koja koja označava GPIO DATA3 linije koja se koristi u posebnom QUAD/OCTAL načinu rada, -1 ako se ne koristi
		- *data4_io_num*
			- varijabla tipa *int* koja koja označava GPIO DATA4 linije koja se koristi u posebnom QUAD/OCTAL načinu rada, -1 ako se ne koristi
		- *data5_io_num*
			- varijabla tipa *int* koja koja označava GPIO DATA5 linije koja se koristi u posebnom QUAD/OCTAL načinu rada, -1 ako se ne koristi
		- *data6_io_num*
			- varijabla tipa *int* koja koja označava GPIO DATA6 linije koja se koristi u posebnom QUAD/OCTAL načinu rada, -1 ako se ne koristi
		- *data7_io_num*
			- varijabla tipa *int* koja koja označava GPIO DATA7 linije koja se koristi u posebnom QUAD/OCTAL načinu rada, -1 ako se ne koristi
		- *quadwp_io_num*
			- varijabla tipa *int* koja koja označava GPIO WRITE_PROTECT linije koja se koristi najčešće u flash memorijama kada se želi spriječiti destruktivna operacija pisanja ili brisanja, -1 ako se ne koristi
		- *quadhd_io_num*
			- varijabla tipa *int* koja koja označava GPIO HOLD linije koja se koristi najčešće u flash memorijama kada je potrebno pauzirati transfer bez zaustavljanja takta, -1 ako se ne koristi
		- *max_transfer_sz*
			- varijabla tipa *int* koja koja označava maksimalnu veličinu podatka koji se prenosi, ako je 0 kad je DMA omogućen koristi se 4096 bajtova, dok kada je DMA onemogućen koristi se *SOC_SPI_MAXIMUM_BUFFER_SIZE*
		- *flags*
			- varijabla tipa *int* koja koja označava zastavice koje označavaju koja svojstva upravljački program može provjeravati
			- enumeratori imaju oblik *SPICOMMON_BUSFLAG_\** i definirani su u zaglavlju ```driver/spi_common.h```, primjerice:
				- *SPICOMMON_BUSFLAG_MASTER* - inicijalizacija SPI sklopa kao *mastera*
				- *SPICOMMON_BUSFLAG_SLAVE* - inicijalizacija SPI sklopa kao *slavea*
				- *SPICOMMON_BUSFLAG_IOMUX_PINS* - provjeri jesu li linije prolaze kroz IO_MUX (direktan prolaz)
				- *SPICOMMON_BUSFLAG_GPIO_PINS* - forsiraj linije da prolaze kroz GPIO matricu umjesto IO_MUX ako se koriste direktni GPIO pinovi (IO_MUX)
				- *SPICOMMON_BUSFLAG_SCLK* - provjeri koristi li se SCLK
				- *SPICOMMON_BUSFLAG_MOSI* - provjeri koristi li se MOSI
				- *SPICOMMON_BUSFLAG_MISO* - provjeri koristi li se MISO
			- nad enumeratorima se vrši operacija OR ako se želi postaviti više zastavica
		- *isr_cpu_id*
			- enumerator koji označava koji jezgra procesora će se koristiti za registraciju SPI ISR-a, vrijednosti mogu biti:
				- *ESP_INTR_CPU_AFFINITY_AUTO* - automatski
				- *ESP_INTR_CPU_AFFINITY_0* - jezgra 0
				- *ESP_INTR_CPU_AFFINITY_1* - jezgra 1
		- *intr_flags*
			- svojstva SPI prekida (ISR-a) oblika *ESP_INTR_FLAG_\**, više informacija se može naći [ovdje](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/system/intr_alloc.html#id2)
- *dma_chan*
	- DMA kanal koji će se koristiti za SPI transfere, vrijednosti mogu biti:
		- *SPI_DMA_DISABLED* - ne koristi se DMA
		- *SPI_DMA_CH1* - kanal 1
		- *SPI_DMA_CH2* - kanal 2
		- *SPI_DMA_CH_AUTO* - upravljački program automatski odabire kanal
	- ako se koristi DMA, memoriju za niz je potrebno alocirati na lokaciju gdje je DMA u mogućnosti obavljati svoje operacije, to se radi funkcijom ```heap_caps_malloc(size, MALLOC_CAP_DMA)```
	- DMA se preporučuje koristiti ako je veličina podataka veća od 32 bajta

Funkcija vraća *ESP_OK* ako je uspješno inicijaliziran sklop.

##### Deinicijaliziraj sklop
```
esp_err_t spi_bus_free(spi_host_device_t host_id)
```

Parametar ove funkcije je:

- *host_id*
	- sklop koji je potrebno deinicijalizirati, enumerator može biti:
		- *SPI1_HOST* - drugi interni sklop kojeg je moguće usmjeriti prema van kao master, nije podržan
		- *SPI2_HOST* - treći sklop, HSPI
		- *SPI3_HOST* - četvrti sklop, VSPI
		- *SPI_HOST_MAX* - nevaljani sklop

Funkcija vraća *ESP_OK* ako je uspješno deinicijaliziran sklop.

##### Dodaj uređaj na sklopu
```
esp_err_t spi_bus_add_device(spi_host_device_t host_id, const spi_device_interface_config_t *dev_config, spi_device_handle_t *handle)
```

Parametri ove funkcije su:

- *host_id*
	- sklop koji je potrebno inicijalizirati, enumerator može biti:
		- *SPI1_HOST* - drugi interni sklop kojeg je moguće usmjeriti prema van kao master, nije podržan
		- *SPI2_HOST* - treći sklop, HSPI
		- *SPI3_HOST* - četvrti sklop, VSPI
		- *SPI_HOST_MAX* - nevaljani sklop
- *dev_config*
	- konfiguracija uređaja, struktura ima članove:
		- *command_bits*
			- varijabla tipa *uint8_t* koja koja označava broj bitova naredbe koja se opcionalno šalje prije pravih podataka, često korišteno kod SPI *slave* flash memorije
		- *address_bits*
			- varijabla tipa *uint8_t* koja koja označava broj bitova adrese koja se opcionalno šalje prije pravih podataka, često korišteno kod SPI *slave* flash memorije, primjerice za adresiranje ispravnog bloka
		- *dummy_bits*
			- varijabla tipa *uint8_t* koja koja označava broj bitova koji se umeće između opcionalne adrese i pravih podataka, često korišteno kod SPI *slave* flash memorije
		- *mode*
			- varijabla tipa *uint8_t* koja koja označava način rada SPI sklopa:
				- 0 - CPOL = 0, CPHA = 0
				- 1 - CPOL = 0, CPHA = 1
				- 2 - CPOL = 1, CPHA = 0
				- 3 - CPOL = 1, CPHA = 1
		- *clock_source*
			- enumerator tipa *spi_clock_source_t* koji definira izvorni takt za SPI sklop, vrijednosti mogu biti:
				- *SPI_CLK_SRC_DEFAULT* - APB_CLK takt
				- *SPI_CLK_SRC_APB* - zadani takt (APB_CLK takt)
		- *duty_cycle_pos*
			- varijabla tipa *uint16_t* koja označava postotak perioda (*eng. duty cycle*) pozitivnog dijela takta u inkrementima od 1 do 256, 128 označava 50 %, ako se postavi 0 uzima se 128
		- *cs_ena_pretrans*
			- varijabla tipa *uint16_t* koja označava broj SPI bit ciklusa između kada CS postane postane aktivan i stvarne transakcije podataka, radi samo za half-duplex način rada
			- vrijednosti mogu biti od 0 do 16
		- *cs_ena_posttrans*
			- varijabla tipa *uint16_t* koja označava broj SPI bit ciklusa između stvarne transakcije podataka i kada CS postane postane neaktivan
			- vrijednosti mogu biti od 0 do 16
		- *clock_speed_hz*
			- varijabla tipa *int* koja označava SCLK takt u Hz
			- maksimalno 80 MHz za bez kašnjenja ako se koristi IO_MUX, a 24 MHz ako se koristi GPIO matrica
		- *input_delay_ns*
			- varijabla tipa *int* koja označava dodatno dozvoljeno vrijeme uzorkovanja nakon što se pojavi rastući ili padajući brid (ovisno o konfiguraciji)
		- *spics_io_num*
			- varijabla tipa *int* koja označava GPIO CS linije
		- *flags*
			- varijabla tipa *uint32_t* koja označava zastavice koje označavaju dodatne funkcionalnosti
			- makroi imaju oblik *SPI_DEVICE_\**, primjerice:
				- *SPI_DEVICE_TXBIT_LSBFIRST* - prenesi naredbu, adresu i onda LSB prvo umjesto zadanog MSB
				- *SPI_DEVICE_RXBIT_LSBFIRST* - prihvati naredbu, adresu i onda LSB prvo umjesto zadanog MSB
			- nad makroima se vrši operacija OR ako se želi postaviti više zastavica
		- *queue_size*
			- varijabla tipa *int* koja označava veličinu reda čekanja transakcija
		- *pre_cb*
			- pokazivač na *callback* funkciju koja će se pozvati prije izvršenja transakcije
			- oblik *callback* funkcije je ```void callback_function(spi_transaction_t *trans)```
		- *post_cb*
			- pokazivač na *callback* funkciju koja će se pozvati poslije izvršenja transakcije
			- oblik *callback* funkcije je ```void callback_function(spi_transaction_t *trans)```
- *handle*
	- handle na uređaj

Funkcija vraća *ESP_OK* ako je uspješno dodan uređaj.

##### Ukloni uređaj na sklopu
```
esp_err_t spi_bus_remove_device(spi_device_handle_t handle)
```

Parametar ove funkcije je:

- *handle*
	- handle na uređaj

Funkcija vraća *ESP_OK* ako je uspješno uklonjen uređaj.

##### Pokreni transakciju (blokirajući način koristeći SPI ISR)
```
esp_err_t spi_device_transmit(spi_device_handle_t handle, spi_transaction_t *trans_desc)
```

Parametri ove funkcije su:

- *handle*
	- handle na uređaj
- *trans_desc*
	- opisnik transakcije, struktura ima sljedeće članove:
		- *flags*
			- varijabla tipa *uint32_t* koja označava zastavice koje označavaju svojstva transakcije
			- makroi imaju oblik *SPI_TRANS_\**, primjerice:
				- *SPI_TRANS_USE_RXDATA* - prilikom primanja podataka, kopiraj ih u *rx_data* član ove strukture umjesto na pokazivač na memoriju *rx_buffer* ove strukture
				- *SPI_TRANS_USE_TXDATA* - prilikom slanja podataka, kopiraj ih iz *tx_data* član ove strukture umjesto sa pokazivača na memoriju *tx_buffer* ove strukture
			- nad makroima se vrši operacija OR ako se želi postaviti više zastavica
		- *cmd*
			- varijabla tipa *uint16_t* koja označava naredbu koja će se poslati (ako se koristi) prije stvarnih podataka
		- *addr*
			- varijabla tipa *uint64_t* koja označava adresu koja će se poslati (ako se koristi) prije stvarnih podataka
		- *length*
			- varijabla tipa *size_t* koja označava ukupnu veličinu podataka u bitovima kojih se šalje i prima
		- *rxlength*
			- varijabla tipa *size_t* koja označava ukupnu veličinu podataka u bitovima kojih se prima, u full-duplex načinu ova varijabla mora biti manja ili jednaka vrijednosti varijable *length*, ako se postavi u 0 bit će automatski jednaka vrijednosti varijable *length*
		- *user*
			- pokazivač tipa *void* koji pokazuje na korisničku definiranu strukturu transakcije, primjerice, strukturu koja sadrži identifikator transakcije
		- *tx_buffer*
			- pokazivač tipa *void* koji pokazuje na međuspremnik u kojem se nalaze podatci za slanje
			- ako se postavi na NULL neće se koristiti MOSI faza (faza slanja) odnosno slat će se lažni bitovi (najčešće 0)
			- ako se koristi DMA, memoriju za niz je potrebno alocirati na lokaciju gdje je DMA u mogućnosti obavljati svoje operacije, to se radi funkcijom ```heap_caps_malloc(size, MALLOC_CAP_DMA)```
			- DMA se preporučuje koristiti ako je veličina podataka veća od 32 bajta
		- *tx_data*
			- niz od 4 varijabli tipa *uint8_t* koji će se slati umjesto podataka na lokaciji *tx_buffer*, ako se postavi zastavica *SPI_TRANS_USE_TXDATA*
		- *rx_buffer*
			- pokazivač tipa *void* koji pokazuje na međuspremnik u kojem će se nalaziti podatci koji će biti primljeni
			- ako se postavi na NULL neće se koristiti MISO faza (faza primanja) odnosno primat će se lažni bitovi (najčešće 0)
			- ako se koristi DMA, memoriju za niz je potrebno alocirati na lokaciju gdje je DMA u mogućnosti obavljati svoje operacije, to se radi funkcijom ```heap_caps_malloc(size, MALLOC_CAP_DMA)```
			- DMA se preporučuje koristiti ako je veličina podataka veća od 32 bajta
		- *rx_data*
			- niz od 4 varijabli tipa *uint8_t* u kojem će se primati podatci umjesto na lokaciji *rx_buffer*, ako se postavi zastavica *SPI_TRANS_USE_RXDATA*

Funkcija vraća *ESP_OK* ako je uspješno izvršena transakcija. Ova funkcija je blokirajuća, transakcija će se postaviti u red čekanja te će funkcija biti odblokirana nakon što se dogodi SPI ISR prekid da je navedena transakcija izvršena. Ako *slave* uređaj nije pripremio nikakve transakcije u svome redu čekanja, vratić će ili sve 0 bitove ili nedefinirane bitove *masteru* tijekom izvršenja ove transakcije. Uređaj *master* je onaj koji inicira komunikaciju i *slave* mu mora slati što god ima kad *master* to kaže.

#### ESP32 kao SPI slave

Zaglavlje koje je potrebno uključiti za [postavljanje ESP32 kao *slave*](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/spi_slave.html#header-file) je ```driver/spi_slave.h```. Ovim se uključuje i upravljački program koji predstavlja apstrakciju *slavea*.

##### Inicijaliziraj sklop
```
esp_err_t spi_slave_initialize(spi_host_device_t host, const spi_bus_config_t *bus_config, const spi_slave_interface_config_t *slave_config, spi_dma_chan_t dma_chan)
```

Parametri ove funkcije su:

- *host_id*
	- sklop koji je potrebno inicijalizirati, enumerator može biti:
		- *SPI1_HOST* - drugi interni sklop kojeg je moguće usmjeriti prema van kao master, nije podržan
		- *SPI2_HOST* - treći sklop, HSPI
		- *SPI3_HOST* - četvrti sklop, VSPI
		- *SPI_HOST_MAX* - nevaljani sklop
- *bus_config*
	- konfiguracija sabirnice, struktura ima članove:
		- *mosi_io_num*
			- varijabla tipa *int* koja koja označava GPIO MOSI linije, -1 ako se ne koristi
		- *miso_io_num*
			- varijabla tipa *int* koja koja označava GPIO MISO linije, -1 ako se ne koristi
		- *sclk_io_num*
			- varijabla tipa *int* koja koja označava GPIO SCLK linije, -1 ako se ne koristi
		- *data0_io_num*
			- varijabla tipa *int* koja koja označava GPIO DATA0 linije koja se koristi u posebnom QUAD/OCTAL načinu rada, -1 ako se ne koristi
		- *data1_io_num*
			- varijabla tipa *int* koja koja označava GPIO DATA1 linije koja se koristi u posebnom QUAD/OCTAL načinu rada, -1 ako se ne koristi
		- *data2_io_num*
			- varijabla tipa *int* koja koja označava GPIO DATA2 linije koja se koristi u posebnom QUAD/OCTAL načinu rada, -1 ako se ne koristi
		- *data3_io_num*
			- varijabla tipa *int* koja koja označava GPIO DATA3 linije koja se koristi u posebnom QUAD/OCTAL načinu rada, -1 ako se ne koristi
		- *data4_io_num*
			- varijabla tipa *int* koja koja označava GPIO DATA4 linije koja se koristi u posebnom QUAD/OCTAL načinu rada, -1 ako se ne koristi
		- *data5_io_num*
			- varijabla tipa *int* koja koja označava GPIO DATA5 linije koja se koristi u posebnom QUAD/OCTAL načinu rada, -1 ako se ne koristi
		- *data6_io_num*
			- varijabla tipa *int* koja koja označava GPIO DATA6 linije koja se koristi u posebnom QUAD/OCTAL načinu rada, -1 ako se ne koristi
		- *data7_io_num*
			- varijabla tipa *int* koja koja označava GPIO DATA7 linije koja se koristi u posebnom QUAD/OCTAL načinu rada, -1 ako se ne koristi
		- *quadwp_io_num*
			- varijabla tipa *int* koja koja označava GPIO WRITE_PROTECT linije koja se koristi najčešće u flash memorijama kada se želi spriječiti destruktivna operacija pisanja ili brisanja, -1 ako se ne koristi
		- *quadhd_io_num*
			- varijabla tipa *int* koja koja označava GPIO HOLD linije koja se koristi najčešće u flash memorijama kada je potrebno pauzirati transfer bez zaustavljanja takta, -1 ako se ne koristi
		- *max_transfer_sz*
			- varijabla tipa *int* koja koja označava maksimalnu veličinu podatka koji se prenosi, ako je 0 kad je DMA omogućen koristi se 4096 bajtova, dok kada je DMA onemogućen koristi se *SOC_SPI_MAXIMUM_BUFFER_SIZE*
		- *flags*
			- varijabla tipa *int* koja koja označava zastavice koje označavaju koja svojstva upravljački program može provjeravati
			- enumeratori imaju oblik *SPICOMMON_BUSFLAG_\** i definirani su u zaglavlju ```driver/spi_common.h```, primjerice:
				- *SPICOMMON_BUSFLAG_MASTER* - inicijalizacija SPI sklopa kao *mastera*
				- *SPICOMMON_BUSFLAG_SLAVE* - inicijalizacija SPI sklopa kao *slavea*
				- *SPICOMMON_BUSFLAG_IOMUX_PINS* - provjeri jesu li linije prolaze kroz IO_MUX (direktan prolaz)
				- *SPICOMMON_BUSFLAG_GPIO_PINS* - forsiraj linije da prolaze kroz GPIO matricu umjesto IO_MUX ako se koriste direktni GPIO pinovi (IO_MUX)
				- *SPICOMMON_BUSFLAG_SCLK* - provjeri koristi li se SCLK
				- *SPICOMMON_BUSFLAG_MOSI* - provjeri koristi li se MOSI
				- *SPICOMMON_BUSFLAG_MISO* - provjeri koristi li se MISO
			- nad enumeratorima se vrši operacija OR ako se želi postaviti više zastavica
		- *isr_cpu_id*
			- enumerator koji označava koji jezgra procesora će se koristiti za registraciju SPI ISR-a, vrijednosti mogu biti:
				- *ESP_INTR_CPU_AFFINITY_AUTO* - automatski
				- *ESP_INTR_CPU_AFFINITY_0* - jezgra 0
				- *ESP_INTR_CPU_AFFINITY_1* - jezgra 1
		- *intr_flags*
			- svojstva SPI prekida (ISR-a) oblika *ESP_INTR_FLAG_\**, više informacija se može naći [ovdje](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/system/intr_alloc.html#id2)
- *slave_config*
	- konfiguracija uređaja, struktura ima članove:
		- *spics_io_num*
			- varijabla tipa *int* koja označava GPIO CS linije
		- *flags*
			- varijabla tipa *uint32_t* koja označava zastavice koje označavaju dodatne funkcionalnosti
			- makroi imaju oblik *SPI_SLAVE_\**, primjerice:
				- *SPI_SLAVE_TXBIT_LSBFIRST* - prenesi naredbu, adresu i onda LSB prvo umjesto zadanog MSB
				- *SPI_SLAVE_RXBIT_LSBFIRST* - prihvati naredbu, adresu i onda LSB prvo umjesto zadanog MSB
		- *queue_size*
			- varijabla tipa *int* koja označava veličinu reda čekanja transakcija
		- *mode*
			- varijabla tipa *uint8_t* koja koja označava način rada SPI sklopa:
				- 0 - CPOL = 0, CPHA = 0
				- 1 - CPOL = 0, CPHA = 1
				- 2 - CPOL = 1, CPHA = 0
				- 3 - CPOL = 1, CPHA = 1
		- *post_setup_cb*
			- pokazivač na *callback* funkciju koja će se pozvati nakon postavljanja konfiguracije
			- oblik *callback* funkcije je ```void callback_function(spi_slave_transaction_t *trans)```
		- *post_trans_cb*
			- pokazivač na *callback* funkciju koja će se pozvati nakon izvršenja transakcije
			- oblik *callback* funkcije je ```void callback_function(spi_slave_transaction_t *trans)```
- *dma_chan*
	- DMA kanal koji će se koristiti za SPI transfere, vrijednosti mogu biti:
		- *SPI_DMA_DISABLED* - ne koristi se DMA
		- *SPI_DMA_CH1* - kanal 1
		- *SPI_DMA_CH2* - kanal 2
		- *SPI_DMA_CH_AUTO* - upravljački program automatski odabire kanal
	- ako se koristi DMA, memoriju za niz je potrebno alocirati na lokaciju gdje je DMA u mogućnosti obavljati svoje operacije, to se radi funkcijom ```heap_caps_malloc(size, MALLOC_CAP_DMA)```
	- DMA se preporučuje koristiti ako je veličina podataka veća od 32 bajta

Funkcija vraća *ESP_OK* ako je uspješno inicijaliziran sklop.

##### Deinicijaliziraj sklop
```
esp_err_t spi_slave_free(spi_host_device_t host)
```

Parametar ove funkcije je:

- *host_id*
	- sklop koji je potrebno deinicijalizirati, enumerator može biti:
		- *SPI1_HOST* - drugi interni sklop kojeg je moguće usmjeriti prema van kao master, nije podržan
		- *SPI2_HOST* - treći sklop, HSPI
		- *SPI3_HOST* - četvrti sklop, VSPI
		- *SPI_HOST_MAX* - nevaljani sklop

Funkcija vraća *ESP_OK* ako je uspješno deinicijaliziran sklop.

##### Čekaj na transakciju (koristeći SPI ISR)
```
esp_err_t spi_slave_transmit(spi_host_device_t host, spi_slave_transaction_t *trans_desc, TickType_t ticks_to_wait)
```

Parametri ove funkcije su:

- *host*
	- sklop koji je potrebno deinicijalizirati, enumerator može biti:
		- *SPI1_HOST* - drugi interni sklop kojeg je moguće usmjeriti prema van kao master, nije podržan
		- *SPI2_HOST* - treći sklop, HSPI
		- *SPI3_HOST* - četvrti sklop, VSPI
		- *SPI_HOST_MAX* - nevaljani sklop
- *trans_desc*
	- opisnik transakcije, struktura ima sljedeće članove:
		- *flags*
			- varijabla tipa *uint32_t* koja označava zastavice koje označavaju svojstva transakcije
			- makroi imaju oblik *SPI_TRANS_\**, primjerice:
				- *SPI_SLAVE_TRANS_DMA_BUFFER_ALIGN_AUTO* - automatski poravnaj DMA međuspremnike kako bi prijenosi bili optimalniji i brži
			- nad makroima se vrši operacija OR ako se želi postaviti više zastavica
		- *length*
			- varijabla tipa *size_t* koja označava ukupnu veličinu podataka u bitovima kojih se šalje ili prima
		- *trans_length*
			- varijabla tipa *size_t* koja označava ukupnu veličinu podataka u bitovima kojih se šalje
		- *user*
			- pokazivač tipa *void* koji pokazuje na korisničku definiranu strukturu transakcije, primjerice, strukturu koja sadrži identifikator transakcije
		- *tx_buffer*
			- pokazivač tipa *void* koji pokazuje na međuspremnik u kojem se nalaze podatci za slanje
			- ako se postavi na NULL neće se koristiti MOSI faza (faza slanja) odnosno slat će se lažni bitovi (najčešće 0)
			- ako se koristi DMA, memoriju za niz je potrebno alocirati na lokaciju gdje je DMA u mogućnosti obavljati svoje operacije, to se radi funkcijom ```heap_caps_malloc(size, MALLOC_CAP_DMA)```
			- DMA se preporučuje koristiti ako je veličina podataka veća od 32 bajta
		- *rx_buffer*
			- pokazivač tipa *void* koji pokazuje na međuspremnik u kojem će se nalaziti podatci koji će biti primljeni
			- ako se postavi na NULL neće se koristiti MISO faza (faza primanja) odnosno primat će se lažni bitovi (najčešće 0)
			- ako se koristi DMA, memoriju za niz je potrebno alocirati na lokaciju gdje je DMA u mogućnosti obavljati svoje operacije, to se radi funkcijom ```heap_caps_malloc(size, MALLOC_CAP_DMA)```
			- DMA se preporučuje koristiti ako je veličina podataka veća od 32 bajta
- *ticks_to_wait*
	- čekanje na događaj za takt sustava
	- ako je portMAX_DELAY onda je funkcija blokirajuća

Funkcija vraća *ESP_OK* ako je uspješno izvršena transakcija. Transakcija će se postaviti u red čekanja te će funkcija tek biti odblokirana nakon što se dogodi SPI ISR prekid da je navedena transakcija u redu izvršena ili je isteklo vrijeme *ticks_to_wait*.

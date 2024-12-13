# I2C

[I2C (*eng. Inter-Intergrated Circuit*)](https://www.nxp.com/docs/en/user-guide/UM10204.pdf) je serijski i sinkroni dvosmjerni half-duplex protokol kojeg je razvio Philips industries (danas NXP). Omogućuje komunikaciju uređaja koji su spojeni na jednoj podatkovnoj sabirnici.

## Način rada I2C sabirnice

Uređaji na koji koriste I2C mogu imati jednu od sljedećih uloga:

- *master* (u novije vrijeme zvan *controller*)
	- inicira komunikaciju pisanja ili čitanja
- *slave* (u novije vrijeme zvan *peripheral*)
	- sluša mastera i odgovara mu na zahtjeve pisanja i čitanja
	- adresabilan, ima jedinstvenu 7 bitnu ili 10 bitnu adresu na sabirnici

Svi *slave* uređaji koji su spojeni na istu sabirnicu moraju imati 7 bitnu ili 10 bitnu **jedinstvenu adresu**. Također, moguće je spojiti više *mastera* na sabirnicu.

I2C koristi dvije sabirnice kako bi ostvarila komunikaciju između uređaja:

- SDA (*eng. Serial Data*)
	- podatkovna sabirnica
- SDC (*eng. Serial Clock*)
	- sabirnica takta

Uređaji koji se spajaju na sabirnice *open drain* konekcijom uz pomoć *pull-up* otpornika. Tipične naponske razine rada su 5 volti (logički *1* je 5 V, logički *0* je 0 V) ili 3.3 (logički *1* je 3.3 V, logički *0* je 0 V) volti, ali je za implementaciju dopušteno koristiti vlastite naponske razine.

### Brzine rada I2C sabirnice

Brzine rada I2C sabirnice su sljedeće:

- 10 kb/s - *slow mode*
- 100 kb/s - *standard mode*
- 400 kb/s - *fast mode*
- 1 kb/s - *fast mode plus*
- 3.4 kb/s - *high speed mode*
- 5 kb/s - *ultra fast mode*

### Spajanje uređaja na I2C sabirnicu

```
                                                                                   +-----+--------- Vcc
                                                                                   |     |
                                                                                  +++   +++
                                                                                  | |   | |
                                                                                  | |   | |
                                                                                  +++   +++
        +--------+                                                                 |     |
        | I2C    +----------------+-------------+-------------+-------------+------|-----+--------- SDA
        | Master |                |             |             |             |      |
        |        +-----------+----|--------+----|--------+----|--------+----|------+--------------- SCL
        +--------+           |    |        |    |        |    |        |    |
         _|_                 |    |        |    |        |    |        |    |
         GND               +-+----+-+    +-+----+-+    +-+----+-+    +-+----+-+
                           | I2C    |    | I2C    |    | I2C    |    | I2C    |
                           | Slave  |    | Slave  |    | Slave  |    | Slave  |
                           |        |    |        |    |        |    |        |
                           +--------+    +--------+    +--------+    +--------+
                            _|_           _|_           _|_           _|_
                            GND           GND           GND           GND
```

Uređaji (*masteri* i *slaveovi*) imaju zajedničku SDA sabirnicu i zajedničku SCL sabirnicu. Svaka sabirnica ima *pull-up* otpornik priključen na izvor napona. Svi uređaji na sabirnici imaju izlazni spoj zvan *open collector* ili *open drain* što znači da ni jedan uređaj ne može direktno povući napon prema gore, već ga mogu samo spustiti prema dolje. Shema spoja *open collector* na sabirnici jednog uređaja:

```
                             Vcc
                              |
                             +++
                             | |  Pull down resistor
                             | |
                             +++
                              |
                    +---------+--------- BUS
                    |
        READ  ------+
        STATE       |
                    |
        PULL      |/
        DOWN  ----|  Transistor
                  |\
                    |
                   _|_
                   GND
```

U priloženoj shemi se može vidjeti spoj na izlaz sabirnice nekog uređaja. U inicijalnom stanju *READ STATE* očitava višu naponsku razinu (logički *1*). Uređaj može samo djelovati s *PULL DOWN*. Aktiviranjem *PULL DOWN* tranzistorske sklopke izmjerena naponska razina na *READ STATE* će postati niža naponska razina (logički *0*) jer je tranzistor sada u direktom propusnom stanju povezan s *GND*. Ako bi na sabirnici postojali drugi uređaji, i oni bi isto na svojim *READ STATE* očitavanjima vidjeli da se dogodila promjena naponske razine u nižu:

```
                                                                                         Vcc
                                                                                          |
                                                                                         +++
                                                                                         | |  Pull down resistor
                                                                                         | |
                                                                                         +++
                                                                                          |
                    +-----------------------------+-----------------------------+---------+--------- BUS
                    |                             |                             |
        READ  ------+                 READ  ------+                 READ  ------+
        STATE       |                 STATE       |                 STATE       |
                    |                             |                             |
        PULL      |/                  PULL      |/                  PULL      |/
        DOWN  ----|  Transistor       DOWN  ----|  Transistor       DOWN  ----|  Transistor
                  |\                            |\                            |\
                    |                             |                             |
                   _|_                           _|_                           _|_
                   GND                           GND                           GND
```

Ova shema prikazuje tri uređaja spojena na zajedničku sabirnicu. I da sva tri u isto vrijeme postave *PULL DOWN* i dalje bi se svima očitavala samo niska razina na *READ STATE*, odnosno stanje sabirnice ne bi bilo nepoznato već samo logička *0*.

### Način komunikacije u slučaju adresiranja 7 bitnom adresom

Prijenos započinje spuštanjem linije SDA pa linije SCL (*eng. start condition*). Onda kreće tijek prijenosa paketa gdje se svakim otkucajem takta piše ili čita jedan bit. Nakon toga se linija podižu SCL pa SDA linije (*eng. stop condition*).

Tijek prijenosa paketa je sljedeći:

- *master* adresira *slave** i usput mu govori koju operaciju treba izvest: čitanje ili pisanje
- *master* adresira početni registar *slavea* nad kojim će se izvršavati operacija čitanja ili pisanja
- kreće komunikacija u jednom od smjerova, ovisno je li se izvršava operacija čitanja iz registra ili pisanje u registre

Čitanje ili pisanje više od jednog podatka u *slave* moguće je ako *slave* to podržava.

Kao potvrdu prijenosa paketa podataka koriste se signali *ACK (eng. Acknowledge)* i *NACK (eng. Not Acknowledge)*. Kad *slave* odgovara *masteru* šalje *ACK* kao znak da je primio paket, a *NACK* da ga nije primio. Kad *master* odgovara *slaveu* šalje *ACK* kao znak da je primio sve pakete osim zadnjeg, na zadnji odgovara s *NACK*. Kad *master* šalje paket *NACK* to znači da terminira vezu, u bilo kojem kontekstu.

Primjer kad *master* čita jedan podatak iz *slavea*:

```
              START                                                                                      STOP
            CONDITION                                                                                 CONDITION
                |                                                                                         |
                |                           SLAVE                      SLAVE                      MASTER  |
                |                            ACK                        ACK                        NACK   |
                |                             |                          |                          |     |
                |    +----- MASTER DATA ----+ | +----- MASTER DATA ----+ | +----- SLAVE DATA -----+ |     |
                |    |                      | | |                      | | |                      | |     |
                V    V                      V V V                      V V V                      V V     V
        SDA ____      _  _  _  _  _  _  _  _     _  _  _  _  _  _  _  _     _  _  _  _  _  _  _  _  _      __
                \____/X\/X\/X\/X\/X\/X\/X\/1\_0_/X\/X\/X\/X\/X\/X\/X\/X\_0_/X\/X\/X\/X\/X\/X\/X\/X\/1\____/
                      A6 A5 A4 A3 A2 A1 A0 R     R7 R6 R5 R4 R3 R2 R1 R0    D7 D6 D5 D4 D3 D2 D1 D0

        SCL ______    _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _    ____
                  \__/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \__/

        A - slave address bits
        R - register address bits
        D - data bits
```

Primjer kad *master* piše jedan podatak u *slave*:

```
              START                                                                                      STOP
            CONDITION                                                                                 CONDITION
                |                                                                                         |
                |                           SLAVE                      SLAVE                      SLAVE   |
                |                            ACK                        ACK                        ACK    |
                |                             |                          |                          |     |
                |    +----- MASTER DATA ----+ | +----- MASTER DATA ----+ | +----- MASTER DATA ----+ |     |
                |    |                      | | |                      | | |                      | |     |
                V    V                      V V V                      V V V                      V V     V
        SDA ____      _  _  _  _  _  _  _        _  _  _  _  _  _  _  _     _  _  _  _  _  _  _  _         __
                \____/X\/X\/X\/X\/X\/X\/X\_0__0_/X\/X\/X\/X\/X\/X\/X\/X\_0_/X\/X\/X\/X\/X\/X\/X\/X\_0_____/
                      A6 A5 A4 A3 A2 A1 A0 W     R7 R6 R5 R4 R3 R2 R1 R0    D7 D6 D5 D4 D3 D2 D1 D0

        SCL ______    _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _    ____
                  \__/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \__/

        A - slave address bits
        R - register address bits
        D - data bits
```

Neki *slave* uređaji imaju mogućnost rastezanje takta (*eng. clock stretching*) što znači da mogu držati takt spuštenim ako još nisu spremni prihvatit ili poslat paket. Primjer rastezanja takta kod čitanja sa *slavea*:

```
              START                                                                                          STOP
            CONDITION                                                                                     CONDITION
                |                                                                                             |
                |                           SLAVE                      SLAVE                          MASTER  |
                |                            ACK                        ACK                            NACK   |
                |                             |                          |                              |     |
                |    +----- MASTER DATA ----+ | +----- MASTER DATA ----+ |     +----- SLAVE DATA -----+ |     |
                |    |                      | | |                      | |     |                      | |     |
                V    V                      V V V                      V V     V                      V V     V
        SDA ____      _  _  _  _  _  _  _  _     _  _  _  _  _  _  _  _         _  _  _  _  _  _  _  _  _      __
                \____/X\/X\/X\/X\/X\/X\/X\/1\_0_/X\/X\/X\/X\/X\/X\/X\/X\_0_____/X\/X\/X\/X\/X\/X\/X\/X\/1\____/
                      A6 A5 A4 A3 A2 A1 A0 R     R7 R6 R5 R4 R3 R2 R1 R0        D7 D6 D5 D4 D3 D2 D1 D0

        SCL ______    _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _      _  _  _  _  _  _  _  _  _    ____
                  \__/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \____/ \/ \/ \/ \/ \/ \/ \/ \/ \__/

        A - slave address bits                                               A
        R - register address bits                                            |
        D - data bits                                                        |
                                                                             |
                                                                         SLAVE CLOCK
                                                                         STRETCHING
```

Primjer kad *master* piše tri podatka u *slave* ako to *slave* omogućuje:

```
              START                                                                                                                                            STOP
            CONDITION                                                                                                                                       CONDITION
                |                                                                                                                                               |
                |                           SLAVE                      SLAVE                      SLAVE                      SLAVE                      SLAVE   |
                |                            ACK                        ACK                        ACK                        ACK                        ACK    |
                |                             |                          |                          |                          |                          |     |
                |    +----- MASTER DATA ----+ | +----- MASTER DATA ----+ | +----- MASTER DATA ----+ | +----- MASTER DATA ----+ | +----- MASTER DATA ----+ |     |
                |    |                      | | |                      | | |                      | | |                      | | |                      | |     |
                V    V                      V V V                      V V V                      V V V                      V V V                      V V     V
        SDA ____      _  _  _  _  _  _  _        _  _  _  _  _  _  _  _     _  _  _  _  _  _  _  _     _  _  _  _  _  _  _  _     _  _  _  _  _  _  _  _         __
                \____/X\/X\/X\/X\/X\/X\/X\_0__0_/X\/X\/X\/X\/X\/X\/X\/X\_0_/X\/X\/X\/X\/X\/X\/X\/X\_0_/X\/X\/X\/X\/X\/X\/X\/X\_0_/X\/X\/X\/X\/X\/X\/X\/X\_0_____/
                      A6 A5 A4 A3 A2 A1 A0 W     R7 R6 R5 R4 R3 R2 R1 R0    D7 D6 D5 D4 D3 D2 D1 D0    D7 D6 D5 D4 D3 D2 D1 D0    D7 D6 D5 D4 D3 D2 D1 D0

        SCL ______    _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _    ____
                  \__/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \__/

        A - slave address bits
        R - register address bits
        D - data bits
```

Primjer kad *master* čita tri podatka iz *slavea* ako to *slave* omogućuje:

```
              START                                                                                                                                            STOP
            CONDITION                                                                                                                                       CONDITION
                |                                                                                                                                               |
                |                           SLAVE                      SLAVE                      MASTER                     MASTER                     MASTER  |
                |                            ACK                        ACK                        ACK                        ACK                        NACK   |
                |                             |                          |                          |                          |                          |     |
                |    +----- MASTER DATA ----+ | +----- MASTER DATA ----+ | +----- SLAVE DATA -----+ | +----- SLAVE DATA -----+ | +----- SLAVE DATA -----+ |     |
                |    |                      | | |                      | | |                      | | |                      | | |                      | |     |
                V    V                      V V V                      V V V                      V V V                      V V V                      V V     V
        SDA ____      _  _  _  _  _  _  _  _     _  _  _  _  _  _  _  _     _  _  _  _  _  _  _  _     _  _  _  _  _  _  _  _     _  _  _  _  _  _  _  _  _      __
                \____/X\/X\/X\/X\/X\/X\/X\/1\_0_/X\/X\/X\/X\/X\/X\/X\/X\_0_/X\/X\/X\/X\/X\/X\/X\/X\_0_/X\/X\/X\/X\/X\/X\/X\/X\_0_/X\/X\/X\/X\/X\/X\/X\/X\/1\____/
                      A6 A5 A4 A3 A2 A1 A0 R     R7 R6 R5 R4 R3 R2 R1 R0    D7 D6 D5 D4 D3 D2 D1 D0    D7 D6 D5 D4 D3 D2 D1 D0    D7 D6 D5 D4 D3 D2 D1 D0

        SCL ______    _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _  _    ____
                  \__/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \__/

        A - slave address bits
        R - register address bits
        D - data bits
```

Ukratko, kada *master* čita podatke sa *slavea* komunikacija ide sljedećim tokom:

- *master* adresira *slave*, *slave* odgovara s *ACK*, a ako adresirani *slave* ne postoji i ne dobije se *ACK* smatra se da je poslan *NACK*
- *master* adresira početni registar *slavea* za čitanje, *slave* odgovara s *ACK*, a ako adresirani registar *slavea* ne postoji *slave* šalje *NACK*
- *slave* šalje podatke i *master* mu odgovara s *ACK* sve do zadnjeg gdje odgovara s *NACK*

Kada *master* piše podatke na *slave* komunikacija ide sljedećim tokom:

- *master* adresira *slave*, *slave* odgovara s *ACK*, a ako adresirani *slave* ne postoji i ne dobije se *ACK* smatra se da je poslan *NACK*
- *master* adresira početni registar *slavea* za čitanje, *slave* odgovara s *ACK*, a ako adresirani registar *slavea* ne postoji *slave* šalje *NACK*
- *master* šalje podatke i *slave* mu odgovara s *ACK* uključujući i na zadnji

#### Arbitraža između dva mastera

U slučaju rada dva ili više *mastera*, svaki *master* provjerava je li SDA linija započela odnosno završila s radom kako bi mogao inicirati svoju transakciju. Ako se slučajno dogodi da oba *mastera* iniciraju transakciju u istom trenu jer su oba *mastera* vidjeli da se SDA linija trenutačno ne koristi, provjerava se dominirajuća 0 (*open drain* sustav): *master* koji u jednom trenutku u prijenosu paketa ima bit *1*, a drugi *master* u tom trenutku ima bit *0*, odustaje od slanja i pokušava kasnije kad se linija oslobodi.

### ESP32 i I2C

ESP32 [I2C sklopovi](https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf#i2c) mogu raditi u načinu *standard mode (100 kb/s)* i *fast mode (400 kb/s)*. ESP32 I2C sklopovi mogu također imati ulogu i *mastera* i *slavea*. Za primjere se koristila pločica [*ESP32-DevKitM-1*](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp32-devkitm-1/user_guide.html#getting-started) koja na sebi ima modul [ESP32-MINI-1](https://www.espressif.com/sites/default/files/documentation/esp32-mini-1_datasheet_en.pdf). ESP32 sadrži dva zasebna I2C sklopa odnosno dvije zasebne I2C sabirnice.

Primjer gdje ESP32 radi kao *master* koristeći jedan I2C sklop nalazi se [ovdje](i2c_master).

Primjer gdje ESP32 radi kao *slave* koristeći jedan I2C sklop nalazi se [ovdje](i2c_slave).

Primjer gdje ESP32 radi istovremena i kao *master* i kao *slave* koristeći oba I2C sklopa nalazi se [ovdje](i2c_master_slave).

#### ESP32 kao I2C master

Zaglavlje koje je potrebno uključiti za [postavljanje ESP32 kao *master*](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/i2c.html#header-file) je ```driver/i2c_master.h```. Ovim se uključuje i upravljački program koji predstavlja apstrakciju *mastera*.

##### Inicijaliziraj sabirnicu
```
esp_err_t i2c_new_master_bus(const i2c_master_bus_config_t *bus_config, i2c_master_bus_handle_t *ret_bus_handle)
```

Parametri ove funkcije su:

- *bus_config*
	- konfiguracija sabirnice, struktura ima članove:
		- *i2c_port*
			- varijabla tipa *i2c_port_num_t* koja označava koju sabirnicu odabrati
			- vrijednost može biti *I2C_NUM_0* (prvi sklop) ili *I2C_NUM_1* (drugi sklop)
		- *sda_io_num*
			- varijabla tipa *gpio_num_t*, GPIO broj I2C SDA sabirnice, oblika ```GPIO_NUM_[broj]```
		- *scl_io_num*
			- varijabla tipa *gpio_num_t*, GPIO broj I2C SCL sabirnice, oblika ```GPIO_NUM_[broj]```
		- *clk_source*
			- varijabla tipa *i2c_clock_source_t*, izvor takta
			- vrijednost može biti *I2C_CLK_SRC_APB* (APB sabirnica) ili *I2C_CLK_SRC_DEFAULT* (opet odabire APB sabirnicu)
		- *glitch_ignore_cnt*
			- varijabla tipa *uint8_t* koja označava broj APB ciklusa gdje će šum biti isfiltriran, ovaj broj se tipično postavlja na 7
			- ako je ovaj broj veći onda se više filtrira, ali je osjetljivost na visokofrekventne signale lošija
			- ako je ovaj broj manji onda se manje filtrira, ali je osjetljivost na visokofrekventne signale bolja
		- *intr_priority*
			- varijabla tipa *int*, prioritet ISR I2C prekida
			- ako se postavi na 0 upravljački program sam odabire prioritet (1, 2, 3)
		- *trans_queue_depth*
			- varijabla tipa *size_t*, koliko paketa se smije staviti u red za čekanje prije nego li upravljački program počne odbijati (blokirati) daljnje zahtjeve za prijenos
			- zadana vrijednost je 1
		- *enable_internal_pullup*
			- varijabla tipa *uint32_t*, omogućuje odnosno onemogućuje interne *pull-up* otpornike
			- ako se koristi *400 kb/s (fastmode)* način rada onda se preporučuje koristiti vanjske *pull-up* otpornike
- *ret_bus_handle*
	- handle na sabirnicu

Funkcija vraća *ESP_OK* ako je uspješno inicijalizirana sabirnica.

##### Deinicijaliziraj sabirnicu
```
esp_err_t i2c_del_master_bus(i2c_master_bus_handle_t bus_handle)
```

Parametar ove funkcije je:

- *bus_handle*
	- handle na sabirnicu

Funkcija vraća *ESP_OK* ako je uspješno deinicijalizirana sabirnica.

##### Ispitaj postoji li slave uređaj s danom adresom
```
esp_err_t i2c_master_probe(i2c_master_bus_handle_t bus_handle, uint16_t address, int xfer_timeout_ms)
```

Parametri ove funkcije su:

- *bus_handle*
	- handle na sabirnicu
- *address*
	- adresa *slavea*
- *xfer_timeout_ms*
	- vremensko čekanje na završetak funkcije, -1 ako se želi čekati beskonačno dugo vremena

Funkcija vraća *ESP_OK* ako je uspješno pronađen *slave* uređaj. Nije potrebno uopće imati stvoren *master* uređaj za izvršenje ove funkcije.

##### Dodaj master uređaj
```
esp_err_t i2c_master_bus_add_device(i2c_master_bus_handle_t bus_handle, const i2c_device_config_t *dev_config, i2c_master_dev_handle_t *ret_handle)
```

Parametri ove funkcije su:

- *bus_handle*
	- handle na sabirnicu
- *dev_config*
	- konfiguracija za ciljani *slave* uređaj, struktura ima članove:
		- *dev_addr_length*
			- enumerator tipa *i2c_addr_bit_len_t* koji označava veličinu *slave* adrese, može biti samo 7 bitna enumeratorom *I2C_ADDR_BIT_LEN_7*, ovo vrijedi za ESP32 model, ostali modeli imaju potporu za 10-bitni način rada
		- *device_address*
			- varijabla tipa *uint16_t* koja definira adresu *slavea*
		- *scl_speed_hz*
			- varijabla tipa *uint16_t* koja definira frekvenciju SCL sabirnice u Hz
		- *scl_wait_us*
			- varijabla tipa *uint16_t* koja definira toleranciju kašnjenja SCL taktova u mikrosekundama
			- ako se postavi 0 onda se koristi zadana vrijednost
- *ret_handle*
	- handle na *master* uređaj

Funkcija vraća *ESP_OK* ako je uspješno dodan *master* uređaj.

##### Ukloni master uređaj
```
esp_err_t i2c_master_bus_rm_device(i2c_master_dev_handle_t handle)
```

Parametar ove funkcije je:

- *handle*
	- handle na *master* uređaj

Funkcija vraća *ESP_OK* ako je uspješno uklonjen *master* uređaj.

##### Šalji podatke na slave uređaj
```
esp_err_t i2c_master_transmit(i2c_master_dev_handle_t i2c_dev, const uint8_t *write_buffer, size_t write_size, int xfer_timeout_ms)
```

Parametri ove funkcije su:

- *i2c_dev*
	- handle na *master* uređaj
- *write_buffer*
	- polje u kojem su bajtovi koji će se poslati određenom *slaveu*
- *write_size*
	- broj bajtova koji će se poslati *slaveu*
- *xfer_timeout_ms*
	- vremensko čekanje na završetak funkcije, -1 ako se želi čekati beskonačno dugo vremena

Funkcija vraća *ESP_OK* ako je uspješno odrađen prijenos podataka preko sabirnice. **Ako je registriran callback** onda će ova funkcija biti neblokirajuća (asinkrona).

##### Primi podatke sa slave uređaja
```
esp_err_t i2c_master_receive(i2c_master_dev_handle_t i2c_dev, uint8_t *read_buffer, size_t read_size, int xfer_timeout_ms)
```

Parametri ove funkcije su:

- *i2c_dev*
	- handle na *master* uređaj
- *read_buffer*
	- polje u kojem će se spremiti bajtovi sa *slavea*
- *read_size*
	- broj bajtova koji će biti spremljeni sa *slavea*
- *xfer_timeout_ms*
	- vremensko čekanje na završetak funkcije, -1 ako se želi čekati beskonačno dugo vremena

Funkcija vraća *ESP_OK* ako je uspješno odrađen prijenos podataka preko sabirnice. **Ako je registriran callback** onda će ova funkcija biti neblokirajuća (asinkrona).

##### Postavi callback funkciju na I2C događaj
```
esp_err_t i2c_master_register_event_callbacks(i2c_master_dev_handle_t i2c_dev, const i2c_master_event_callbacks_t *cbs, void *user_data)
```

Parametri ove funkcije su:

- *i2c_dev*
	- handle na *master* uređaj
- *cbs*
	- struktura s članom:
		- *on_trans_done*
			- tip podatka *i2c_master_callback_t* koji sadrži pokazivač na *callback* funkciju oblika: ```bool i2c_master_callback(i2c_master_dev_handle_t i2c_dev, const i2c_master_event_data_t *evt_data, void *arg)```
				- *callback* funkcija nakon I2C ISR prekida
				- struktura *i2c_master_event_data_t* sadrži član *i2c_master_event_t* koji je enumerator koji može imati vrijednosti:
					- *I2C_EVENT_ALIVE* - I2C sabirnica je funkcionalna
					- *I2C_EVENT_DONE* - transakcija je završena
					- *I2C_EVENT_NACK* - NACK događaj
					- *I2C_EVENT_TIMEOUT* - vremensko čekanje neke funkcije je prošlo
- *user_data*
	- korisnički podatci koji će se proslijediti *callback* funkciji

Funkcija vraća *ESP_OK* ako je uspješno postavljen *callback* *mastera*. Uklanjanje *callbacka* se može napraviti tako što se u strukturi *cbs* član *on_trans_done* postavi u *NULL* te se ista struktura ponovno proslijedi u *i2c_master_register_event_callbacks(...)* funkciju.

##### Resetiraj I2C sabirnicu
```
esp_err_t i2c_master_bus_reset(i2c_master_bus_handle_t bus_handle)
```

Parametar ove funkcije je:

- *bus_handle*
	- handle na sabirnicu

Funkcija vraća *ESP_OK* ako je uspješno resetirana sabirnica.

##### Čekaj na dovršetak svih transakcija u redu
```
esp_err_t i2c_master_bus_wait_all_done(i2c_master_bus_handle_t bus_handle, int timeout_ms)
```

Parametri ove funkcije su:

- *bus_handle*
	- handle na sabirnicu
- *timeout_ms*
	- vremensko čekanje na završetak funkcije, -1 ako se želi čekati beskonačno dugo vremena

Funkcija vraća *ESP_OK* ako su se sve transakcije u redu izvršile.

#### ESP32 kao I2C slave

Zaglavlje koje je potrebno uključiti za [postavljanje ESP32 kao *slave*](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/i2c.html#id1) je ```driver/i2c_slave.h```. Ovim se uključuje i upravljački program koji predstavlja apstrakciju *slavea*.

##### Inicijaliziraj slave uređaj
```
esp_err_t i2c_new_slave_device(const i2c_slave_config_t *slave_config, i2c_slave_dev_handle_t *ret_handle)
```

Parametri ove funkcije su:

- *slave_config*
	- konfiguracija uređaja, struktura ima članove:
		- *i2c_port*
			- varijabla tipa *i2c_port_num_t* koja označava na koju sabirnicu se spojiti kao *slave*
			- vrijednost može biti *I2C_NUM_0* (prvi sklop) ili *I2C_NUM_1* (drugi sklop)
		- *sda_io_num*
			- varijabla tipa *gpio_num_t*, GPIO broj I2C SDA sabirnice, oblika ```GPIO_NUM_[broj]```
		- *scl_io_num*
			- varijabla tipa *gpio_num_t*, GPIO broj I2C SCL sabirnice, oblika ```GPIO_NUM_[broj]```
		- *clk_source*
			- varijabla tipa *i2c_clock_source_t*, izvor takta
			- vrijednost može biti *I2C_CLK_SRC_APB* (APB sabirnica) ili *I2C_CLK_SRC_DEFAULT* (opet odabire APB sabirnicu)
		- *send_buf_depth*
			- varijabla tipa *uint32_t* koja postavlja veličinu internog kružnog međuspremnika ovog *slavea*, najefikasnije je postaviti veličinu potencije broja 2
		- *slave_addr*
			- varijabla tipa *uint16_t* koja definira adresu ovog *slavea*
		- *addr_bit_len*
			- enumerator tipa *i2c_addr_bit_len_t* koji označava veličinu adrese, može biti samo 7 bitna enumeratorom *I2C_ADDR_BIT_LEN_7*, ovo vrijedi za ESP32 model, ostali modeli imaju potporu za 10-bitni način rada
		- *intr_priority*
			- varijabla tipa *int*, prioritet I2C ISR prekida
			- ako se postavi na 0 upravljački program sam odabire prioritet (1, 2, 3)
			- koristi 
- *ret_handle*
	- handle na *slave* uređaj

Funkcija vraća *ESP_OK* ako je uspješno inicijaliziran uređaj.

##### Deinicijaliziraj slave uređaj
```
esp_err_t i2c_del_slave_device(i2c_slave_dev_handle_t i2c_slave)
```

Parametar ove funkcije je:

- *i2c_slave*
	- handle na *slave* uređaj

Funkcija vraća *ESP_OK* ako je uspješno deinicijaliziran uređaj.

##### Iščitaj podatke iz internog kružnog međuspremnika
```
esp_err_t i2c_slave_receive(i2c_slave_dev_handle_t i2c_slave, uint8_t *data, size_t buffer_size)
```

Parametri ove funkcije su:

- *i2c_slave*
	- handle na *slave* uređaj
- *data*
	- mjesto gdje spremiti iščitane podatke
- *buffer_size*
	- broj bajtova podataka kojih treba iščitati

Funkcija vraća *ESP_OK* ako je uspješno zadan zadatak za čitanje prstenastog međuspremnika. **Ova funkcija je neblokirajuća**, kada se pozove postavit će se zadatak koji će zadati pokretanje registriranog *callbacka* nakon *buffer_size* pridošlih bajtova u kružni međuspremnik.

##### Piši podatke u interni kružnog međuspremnik
```
esp_err_t i2c_slave_transmit(i2c_slave_dev_handle_t i2c_slave, const uint8_t *data, int size, int xfer_timeout_ms)
```

Parametri ove funkcije su:

- *i2c_slave*
	- handle na *slave* uređaj
- *data*
	- mjesto odakle pisati podatke
- *size*
	- broj bajtova podataka kojih treba pisati
- *xfer_timeout_ms*
	- vremensko čekanje na završetak funkcije, -1 ako se želi čekati beskonačno dugo vremena

Funkcija vraća *ESP_OK* ako je uspješno prepisan kružni međuspremnik.

##### Postavi callback funkciju na I2C događaj
```
esp_err_t i2c_slave_register_event_callbacks(i2c_slave_dev_handle_t i2c_slave, const i2c_slave_event_callbacks_t *cbs, void *user_data)
```

Parametri ove funkcije su:

- *i2c_slave*
	- handle na *slave* uređaj
- *cbs*
	- struktura s članom:
		- *on_recv_done*
			- tip podatka *i2c_master_callback_t* koji sadrži pokazivač na *callback* funkciju oblika: ```bool i2c_slave_received_callback_t(i2c_slave_dev_handle_t i2c_slave, const i2c_slave_rx_done_event_data_t *evt_data, void *arg)```
				- *callback* funkcija koja će se pozvati kada se u kružni međuspremnik upiše onoliko bajtova koliko je postavljeno s funkcijom *i2c_slave_receive(...)*
				- struktura *i2c_slave_rx_done_event_data_t* sadrži član *buffer* koji je niz elemenata tipa *uint8_t*, *buffer* pokazuje na poziciju kružnog međuspremnika gdje se upisao prvi podatak
- *user_data*
	- korisnički podatci koji će se proslijediti *callback* funkciji

Funkcija vraća *ESP_OK* ako je uspješno postavljen *callback* *slavea*. Uklanjanje *callbacka* se može napraviti tako što se u strukturi *cbs* član *on_recv_done* postavi u *NULL* te se ista struktura ponovno proslijedi u *i2c_slave_register_event_callbacks(...)* funkciju.

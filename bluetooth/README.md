# BLUETOOTH

[Bluetooth](https://www.bluetooth.com/) je bežična tehnologija koja omogućava prijenos podataka na kratkim udaljenostima između različitih uređaja kao što su pametni telefona, tableti, laptopi, računala, slušalice, zvučnici i mnogi drugi. 

Prva verzija Bluetootha je razvijena 1989 u tvrtki Ericsson od inženjera Jaap Haartsen i Sven Mattisson. Cilj je bio ostvariti bežični standard uz koji bi omogućio komunikaciju između uređaja na kratkim udaljenostima.

Nakon razvitka prvih verzija Bluetootha osnovana je organizacija za standardiziranje Bluetooth verzija [Bluetooth Special Interest Group (SIG)](https://en.wikipedia.org/wiki/Bluetooth_Special_Interest_Group). Inicijalni članovi organizacije bili su Ericsson, Intel, Nokia, IBM i Toshiba. Danas organizacija uključuje preko 10000 tvrtki. [IEEE organizacija] je također standardizirala Bluetooth 2002. i 2005. godine (https://standards.ieee.org/ieee/802.15.1/3513/), ali više ne održava taj standard.

Proizvođač Bluetooth uređaja mora se držati standarda koje postavlja SIG kako bi mogao uređaje plasirati na tržište kao Bluetooth uređaje.

Postoji više verzija Bluetooth i svi su oni međusobno unatrag kompatibilni. Svaka novija verzija dovodi neka poboljšanja i popravke.

## Bluetooth Classic i BLE

Od Bluetooth verzije 4.0, uveden je *Bluetooth Low Energy (BLE)* način rada koji nudi nisku potrošnju (uz manju brzinu prijenosa podataka). Od tada obični Bluetooth dobiva naziv *Bluetooth Classic* ili *Bluetooth Basic Rate/Enhanced Raze (BR/EDR)*, a Bluetooth za uređaje niskih potrošnji *Bluetooth Low Energy (BLE)*.

[Usporedba načina rada](https://www.bluetooth.com/learn-about-bluetooth/tech-overview/) može se vidjeti u tablici:

|                           | BLE                                                            | Bluetooth Classic                        |
| ------------------------- |:--------------------------------------------------------------:|:----------------------------------------:|
| Frekvencijski pojas       | 2.402 GHz - 2.480 GHz (2.4 GHz)                                | 2.402 GHz - 2.480 GHz (2.4 GHz)          |
| Kanali                    | 40 kanala međusobno odvojeni 2 MHz-a (3 kanala za oglašavanje) | 79 kanala međusobno odvojeni 1 MHz-a     |
| Način korištenja kanala   | FHSS (Frequency-Hopping Spread Spectrum)                       | FHSS (Frequency-Hopping Spread Spectrum) |
| Potrošnja energije        | Jako mala                                                      | Mala                                     |
| Topologije komunikacije   | Point to point, broadcast, mesh                                | Point to point                           |
| Broj aktivnih slaveova    | Neograničen                                                    | 7                                        |
| Uparivanje                | Nije potrebno                                                  | Potrebno                                 |
| Primjeri korištenja       | Uređaji interneta stvari, uređaji male potrošnje               | Slušalice, mikrofoni, prijenos podataka  |
| Brzina prijenosa podataka | 2 Mb/s, 1 Mb/s, 500 Kb/s, 125 Kb/s                             | 3 Mb/s, 2 Mb/s, 1 Mb/s                   |
| Raspon                    | manje od 100 m                                                 | 100 m                                    |

Podatci se između dva Bluetooth uređaja šalju u paketima. Uređaji koriste FHSS za skakanje po kanalima. Jedan paket se proslijedi nakon skoka u novi kanal. Također, koristi se adaptivno skakanje u kanalima gdje se izbjegavaju kanali koji imaju veliku interferenciju. FHSS također onemogućuje kontinuirano prisluškivanje kanala jer samo uređaji koji međusobno komuniciraju znaju kojim redoslijedom skakati s kanala na kanal.

### Bluetooth protokolni stog

Općenito, [Bluetooth protokolni stog](https://www.mathworks.com/help/bluetooth/ug/bluetooth-protocol-stack.html) se sastoji od dva dijela:

- *controller* stog
	- niži slojevi komunikacije
	- upravlja radio prijenosom
	- nudi sučelje prema stogu operacijskog sustava (*HCI Host Controller Interface*)

- *host* stog
	- viši slojevi komunikacije
	- manipulacija podatcima
	- nudi sučelje aplikacijama

Bluetooth Classic i BLE imaju razlike u protokolima na stoga, ali podjela na *host* stog i *controller* stog i dalje vrijedi.

### Bluetooth adrese uređaja

Svaki Bluetooth uređaj ima vlastitu adresu poznatija kao Bluetooth MAC adresa kojom se uređaj identificira. Ove adrese se općenito ne prikazuju kad se Bluetooth uređaj oglašava već se koriste tekstualna imena. Ove adrese mogu biti javne (tvorničke) ili slučajno generirane. Slučajno generirane mogu biti pak biti statične ili dinamičke (promjenjive kroz vrijeme). Na kraju, dinamičke adrese mogu biti razrješive adrese (*eng. RPA - Resolvable Private Address*) i nerazrješive adrese. Razrješive privatne adrese se koriste kad uređaj i dalje želi ostati poznat s uređajima kojima vjeruje.

Shemom se vrste Bluetooth adresa mogu prikazati ovako:

```
           [Address type]
               /    \
              /      \
             /        \
        [Public]   [Random]
                    /    \
                   /      \
                  /        \
             [Static]   [Private]
                          /    \
                         /      \
                        /        \
                [Resolvable]  [Non resolvable]
```

## ESP32 i Bluetooth

Većina ESP32 serija mikroupravljača podržava *Bluetooth*. U sljedećim primjerima se koristila razvojna pločica [*ESP32-DevKitM-1*](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp32-devkitm-1/user_guide.html#getting-started) koji koristi modul [ESP32-MINI-1](https://www.espressif.com/sites/default/files/documentation/esp32-mini-1_datasheet_en.pdf). Verzija Bluetootha na modulu je verzija Bluetooth 4.2 i podržava *Bluetooth Classic* način rada i *BLE* način rada.

ESP-IDF ima mogućnost korištenja dvije različite implementacije *host* stoga:

- [*Bluedroid*](https://android.googlesource.com/platform/external/bluetooth/bluedroid/)
	- zadani *host* stog
	- za Bluetooth Classic i BLE
- [*NimBLE*](https://github.com/apache/mynewt-nimble)
	- preporučeno ako se koristi samo BLE

Kako bi se omogućilo korištenje Bluetooth komponente potrebno je omogućiti Bluetooth, Bluedroid i Bluetooth sklop uz pomoć *menuconfig* naredbe. Varijable koje trebaju biti u *y* stanju u *sdkconfig* datoteci moraju biti varijable *CONFIG_BT_ENABLED*, *CONFIG_BT_BLUEDROID_ENABLED* i *CONFIG_BT_CONTROLLER_ENABLED*.

### BLE GAP

[GAP - Generic Access Profile](https://software-dl.ti.com/lprf/sdg-latest/html/ble-stack-3.x/gap.html) je dio *host* stoga koji definira kako BLE uređaji pristupaju, otkrivaju i uspostavljaju veze između sebe.

GAP upravlja stanjem uređaja koja mogu biti:

- *Standby*
	- uređaj je u mirovanju
- *Advertising*
	- uređaj se oglašava drugim uređajima
- *Scanning*
	- uređaj traži uređaje koje se oglašavaju
- *Initiating*
	- uređaj koji odgovara drugim uređajima koji se oglašavaju
	- uređaj inicira želju za povezivanjem s uređajima koji se oglašavaju
- *Connected*
	- uređaj koji je spojen s uređajima koji su se oglašavali
	- uređaji koji su se oglašavali pa su prihvatili želju za povezivanjem od inicijatora
	- *master-slave* konfiguracija
		- *slave:* uređaj koji se oglašavao
		- *master:* uređaj koji je inicirao želju za povezivanjem

Shemom se stanja mogu prikazati ovako:

```
                 [Standby]             IDLE
                   /    \
        ----------/------\-----------------------
                 /        \            DEVICE
        [Advertiser]     [Scanner]     DISCOVERY
             |               |
             |          [Initiator]
             |               |
        -----|---------------|-------------------
             |               |         CONNECTION
          [Slave]        [Master]
```

Sam GAP sloj odnosno BLE uređaj može imati jednu od četiri uloge:

- *Broadcaster* (oglašivač)
	- uređaj je oglašivač na kojeg se nije moguće spojiti

- *Observer* (promatrač)
	- uređaj skenira uređaje oglašivače ali ne može inicirati konekciju

- *Peripheral* (periferija)
	- uređaj je oglašivač na kojeg se moguće spojiti i kad se to dogodi ima ulogu *slavea*

- *Central* (središnji uređaj)
	- uređaj skenira uređaje oglašivače te može inicirati konekciju i kad se to dogodi ima ulogu *mastera*

Parametri za spajanje:

- *Connection Interval*
	- signalizira spremnost komunikacije oba uređaja (i *mastera* i *slavea*)
	- BLE uređaji rade skokove po kanalima (FHSS), kada uređaji razmjenjuju podatke u trenutačnom kanalu, to se naziva događaj konekcije (eng. *connection event*)
	- uređaji ne moraju razmjenjivati podatke svaki put kad skoknu u novi kanal već mogu periodično napravit konekciju, ovaj period može biti od 7.5 ms do 4 s
	- smanjenje ovog parametra povećava potrošnju, ali povećava i prijenos podataka
	- dogovor između *mastera* i *slavea*

- *Slave Latency*
	- mogućnost BLE *slave* uređaja da preskoči odgovoriti na nekoliko uzastopnih događaja konekcija (maksimalno može preskočiti za vrijednost *Slave Latency* parametra)
	- korisno ako *slave* nema ništa za poslati
	- smanjenje ovog parametra povećava potrošnju za *slavea*, ali su podatci učestaliji

- *Supervision Time-out*
	- maksimalni vremenski interval između dva uspješna događaja konekcija (uspješan događaj konekcije znači da je *slave* odgovorio na događaj konekcije)
	- vrijednost ovog parametra mora biti veća nego *Effective Connection Interval*
	- od 100 ms do 32 s
	- ako *master* i *slave* ne uspostave ni jedan događaja konekcija u ovom intervalu, prekidaju međusobnu povezanost (uređaji više nisu povezani)

Potrebno je uzeti u obzir:

- *Effective Connection Interval*
	- interval između uspješnih događaja konekcija ako *slave* preskoči *Slave Latency* broj događaja konekcija
	- formula: *Effective Connection Interval* = (*Connection Interval*) * (1 + *Slave Latency*)

Tijekom oglašavanja, periferija može u podatcima oglašavanja uključivati UUID servisa oglašavanja (koji je različit od UUID servisa, UUID karakteristike ili UUID opisnika kod GATT-a), naziv uređaja, snagu odašiljanja, podatke o proizvođaču i slično.

UUID servis oglašavanja se može definirati kao 16 bitni UUID koji je [standardiziran od SIG-a](https://www.bluetooth.com/wp-content/uploads/Files/Specification/HTML/Assigned_Numbers/out/en/Assigned_Numbers.pdf?v=1731291575293#5.2) i označava neku ulogu uređaja koji se oglašava. Ako se ne želi pridržavati standarda, umjesto da se koriste već rezervirani 16 bitni UUID-ovi, preporučuje se koristiti 128 bitne UUID-ove za vlastite svrhe.

### BLE GATT

[GATT - Generic Attribute Profile](https://software-dl.ti.com/lprf/sdg-latest/html/ble-stack-3.x/gatt.html) je dio *host* stoga koji se koristi za razmjenu podataka između BLE uređaja. BLE uređaji nakon povezivanja mogu imati uloge:

- *GATT poslužitelj*
	- sadrži bazu podataka karakteristika koju GATT klijent čita ili piše
	- ovo primjerice može biti mikroupravljač kojem je karakteristika stanje pina na kojem je spojena svjetleća dioda

- *GATT klijent*
	- čita ili piše podatke u bazu podataka karakteristika na GATT poslužitelj
	- ovo primjerice može biti pametni mobitel

Ima li uređaj ulogu GATT klijenta ili GATT poslužitelja uopće nema veze s tim je li uređaj ima ulogu BLE središnjeg uređaja ili BLE periferije.

Osnovni pojmovi:

- *Profile*
	- profil
	- specificira niz funkcionalnosti koji BLE nudi
	- sadrži nula ili više servisa
- *Service*
	- servis
	- ima svoj UUID
	- logički skup povezanih karakteristika
	- sadrži nula ili više karakteristika
- *Characteristic*
	- karakteristika
	- ima svoj UUID
	- osnovna jedinica podataka
	- najčešće ima neku vrijednost
	- sadrži nula ili više opisnika
- *Descriptor*
	- opisnik
	- ima svoj UUID
	- metapodatci o karakteristici

Slikovito:
```
        +------------------------------------------------------+        +------------------------------------------------------+
        | PROFILE 1                                            |        | PROFILE 2                                            |
        |                                                      |        |                                                      |
        |  +------------------------------------------------+  |        |  +------------------------------------------------+  |
        |  | SERVICE 1                                      |  |        |  | SERVICE 1                                      |  |
        |  |                                                |  |        |  |                                                |  |
        |  |  +-------------------+  +-------------------+  |  |        |  |  +-------------------+  +-------------------+  |  |
        |  |  | CHARACTERISTIC 1  |  | CHARACTERISTIC 2  |  |  |        |  |  | CHARACTERISTIC 1  |  | CHARACTERISTIC 2  |  |  |
        |  |  |                   |  |                   |  |  |        |  |  |                   |  |                   |  |  |
        |  |  | +---------------+ |  | +---------------+ |  |  |        |  |  | +---------------+ |  | +---------------+ |  |  |
        |  |  | | DESCRIPTOR 1  | |  | | DESCRIPTOR 1  | |  |  |        |  |  | | DESCRIPTOR 1  | |  | | DESCRIPTOR 1  | |  |  |
        |  |  | +---------------+ |  | +---------------+ |  |  |        |  |  | +---------------+ |  | +---------------+ |  |  |
        |  |  | +---------------+ |  | +---------------+ |  |  |        |  |  | +---------------+ |  | +---------------+ |  |  |
        |  |  | | DESCRIPTOR 2  | |  | | DESCRIPTOR 2  | |  |  |        |  |  | | DESCRIPTOR 2  | |  | | DESCRIPTOR 2  | |  |  |
        |  |  | +---------------+ |  | +---------------+ |  |  |        |  |  | +---------------+ |  | +---------------+ |  |  |
        |  |  +-------------------+  +-------------------+  |  |        |  |  +-------------------+  +-------------------+  |  |
        |  |                                                |  |        |  |                                                |  |
        |  +------------------------------------------------+  |        |  +------------------------------------------------+  |
        |                                                      |        |                                                      |
        |  +------------------------------------------------+  |        |  +------------------------------------------------+  |
        |  | SERVICE 2                                      |  |        |  | SERVICE 2                                      |  |
        |  |                                                |  |        |  |                                                |  |
        |  |  +-------------------+  +-------------------+  |  |        |  |  +-------------------+  +-------------------+  |  |
        |  |  | CHARACTERISTIC 1  |  | CHARACTERISTIC 2  |  |  |        |  |  | CHARACTERISTIC 1  |  | CHARACTERISTIC 2  |  |  |
        |  |  |                   |  |                   |  |  |        |  |  |                   |  |                   |  |  |
        |  |  | +---------------+ |  | +---------------+ |  |  |        |  |  | +---------------+ |  | +---------------+ |  |  |
        |  |  | | DESCRIPTOR 1  | |  | | DESCRIPTOR 1  | |  |  |        |  |  | | DESCRIPTOR 1  | |  | | DESCRIPTOR 1  | |  |  |
        |  |  | +---------------+ |  | +---------------+ |  |  |        |  |  | +---------------+ |  | +---------------+ |  |  |
        |  |  | +---------------+ |  | +---------------+ |  |  |        |  |  | +---------------+ |  | +---------------+ |  |  |
        |  |  | | DESCRIPTOR 2  | |  | | DESCRIPTOR 2  | |  |  |        |  |  | | DESCRIPTOR 2  | |  | | DESCRIPTOR 2  | |  |  |
        |  |  | +---------------+ |  | +---------------+ |  |  |        |  |  | +---------------+ |  | +---------------+ |  |  |
        |  |  +-------------------+  +-------------------+  |  |        |  |  +-------------------+  +-------------------+  |  |
        |  |                                                |  |        |  |                                                |  |
        |  +------------------------------------------------+  |        |  +------------------------------------------------+  |
        +------------------------------------------------------+        +------------------------------------------------------+
```

UUID servisi, karakteristike i opisnici se također mogu definirati kao 16 bitni UUID-ovi koji su [standardizirani od SIG-a](https://www.bluetooth.com/wp-content/uploads/Files/Specification/HTML/Assigned_Numbers/out/en/Assigned_Numbers.pdf?v=1731291575293#6.2) i označavaju neku ulogu. Ako se ne želi pridržavati standarda, umjesto da se koriste već rezervirani 16 bitni UUID-ovi, preporučuje se koristiti 128 bitne UUID-ove za vlastite svrhe.

### ESP32 kao GATT poslužitelj

U nastavku slijede bitne funkcije koje se koriste za GATT poslužitelj (službeni primjer ESP-IDF-a može se naći [ovdje](https://github.com/espressif/esp-idf/blob/v5.2.3/examples/bluetooth/bluedroid/ble/gatt_server/tutorial/Gatt_Server_Example_Walkthrough.md)). ESP32 će se konfigurirati kao **BLE periferija**.

Primjer za GATT poslužitelj nalazi se [ovdje](ble_gatt_server).

#### Flash memorija za trajnu pohranu podataka

Bluetooth sprema neke svoje podatke u [NVS (*eng. Non-Volatile Storage*) particiju](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/storage/nvs_flash.html) koja se nalazi u Flash memoriji. Podatci se u NVS particiju spremaju u obliku ključa i vrijednosti.

Zaglavlje koje je potrebno uključiti za rad s [NVS-om](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/storage/nvs_flash.html#header-file) je ```nvs_flash.h```.

##### Inicijaliziraj uobičajenu NVS particiju
```
esp_err_t nvs_flash_init(void)
```

Funkcija vraća *ESP_OK* ako je inicijalizacija NVS-a uspjela. Funkcija inicijalizira opisnike u memoriji potrebne za pristup NVM particiji.

##### Briši sve ključeve i vrijednosti uobičajene NVS particije
```
esp_err_t nvs_flash_erase(void)
```

Funkcija vraća *ESP_OK* ako je brisanje ključeva i vrijednosti uspjelo.

##### Deinicijaliziraj uobičajenu NVS particije
```
esp_err_t nvs_flash_deinit(void)
```

Funkcija vraća *ESP_OK* ako je deinicijalizacija NVS-a uspjela. Funkcija deinicijalizira opisnike u memoriji potrebne za pristup NVM particiji.

#### Upravljanje Bluetooth upravljačkim sklopom i virtualnim HCI-om

Kako bi mogli upravljati [Bluetooth sklopom i virtualnim HCI-om](https://docs.espressif.com/projects/esp-idf/en/v5.3.1/esp32/api-reference/bluetooth/controller_vhci.html#header-file), potrebno je uključiti zaglavlje ```esp_bt.h```.

##### Oslobodi memoriju Bluetooth sklopa
```
esp_err_t esp_bt_controller_mem_release(esp_bt_mode_t mode)
```

Parametar ove funkcije je:

- *mode*
	- enumerator koji dealocira alociranu memoriju za način rada Bluetooth upravljača, enumeratori mogu biti:
		- *ESP_BT_MODE_BLE* - oslobađa alociranu memoriju za BLE način rada
		- *ESP_BT_MODE_CLASSIC_BT* - oslobađa alociranu memoriju za Bluetooth Classic način rada
		- *ESP_BT_MODE_BTDM* - oslobađa alociranu memoriju i za Bluetooth Classic i za BLE način rada

Funkcija vraća *ESP_OK* ako je memorija uspješno oslobođena. **Bluetooth sklop mora biti neinicijaliziran prije poziva ove funkcije.**

##### Konfiguriraj Bluetooth sklop
```
BT_CONTROLLER_INIT_CONFIG_DEFAULT()
```

Makro vraća strukturu tipa *esp_bt_controller_config_t* koja predstavlja uobičajenu konfiguraciju Bluetooth sklopa.

##### Inicijaliziraj Bluetooth sklop
```
esp_err_t esp_bt_controller_init(esp_bt_controller_config_t *cfg)
```

Parametar ove funkcije je:

- *cfg*
	- pokazivač na konfiguraciju kojom će se postaviti Bluetooth sklop

Funkcija vraća *ESP_OK* kada ako je Bluetooth sklop uspješno inicijaliziran danom konfiguracijom. Ovo je prva funkcija koja se poziva prije bilo koje druge Bluetooth funkcije.

##### Deinicijaliziraj Bluetooth sklop
```
esp_err_t esp_bt_controller_deinit(void)
```

Funkcija vraća *ESP_OK* kada ako je Bluetooth sklop uspješno inicijaliziran danom konfiguracijom. Ako se pozove ova funkcija ne smije se više zvati ni jedna Bluetooth funkcija. Uređaj ne smije biti u stanju oglašavanja ili skeniranja kada se poziva ova funkcija.

##### Omogući Bluetooth sklop
```
esp_err_t esp_bt_controller_enable(esp_bt_mode_t mode)
```

Parametar ove funkcije je:

- *mode*
	- enumerator koji predstavlja način rada Bluetooth upravljača, enumeratori mogu biti:
		- *ESP_BT_MODE_IDLE* - upravljač nije pokrenut
		- *ESP_BT_MODE_BLE* - pokreće BLE način rada
		- *ESP_BT_MODE_CLASSIC_BT* - pokreće Bluetooth Classic način rada
		- *ESP_BT_MODE_BTDM* - pokreće dvostruki Bluetooth Classic + BLE način rada

Funkcija vraća *ESP_OK* kada ako je Bluetooth sklop uspješno omogućen u danom načinu rada. Preporučljivo je način rada ne mijenjat dinamički, već prvo onemogućiti sklop funkcijom ```esp_bt_controller_disable()``` i onda ponovno omogućiti sklop s ```esp_bt_controller_enable()``` u novom načinu rada

##### Onemogući Bluetooth sklop
```
esp_err_t esp_bt_controller_disable(void)
```

Funkcija vraća *ESP_OK* kada ako je Bluetooth sklop uspješno onemogućen.

##### Dohvati stanje Bluetooth sklopa
```
esp_bt_controller_status_t esp_bt_controller_get_status(void)
```

Funkcija vraća enumerator koji može imati vrijednosti:
	- *ESP_BT_CONTROLLER_STATUS_IDLE* - sklop neinicijaliziran
	- *ESP_BT_CONTROLLER_STATUS_INITED* - sklop inicijaliziran, ali nije omogućen
	- *ESP_BT_CONTROLLER_STATUS_ENABLED* - sklop inicijaliziran i omogućen

#### Upravljanje Bluedroid host stogom Bluetootha

Za upravljanje [Bluedroid host stogom](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/bluetooth/esp_bt_main.html#header-file), potrebno je uključiti zaglavlje ```esp_bt_main.h```.

##### Inicijaliziraj Bluedroid host stog
```
esp_err_t esp_bluedroid_init(void)
```

Funkcija vraća *ESP_OK* kada ako je stog uspješno inicijaliziran.

##### Deinicijaliziraj Bluedroid host stog
```
esp_err_t esp_bluedroid_deinit(void)
```

Funkcija vraća *ESP_OK* kada ako je stog uspješno deinicijaliziran.

##### Omogući Bluedroid host stog
```
esp_err_t esp_bluedroid_enable(void)
```

Funkcija vraća *ESP_OK* kada ako je stog uspješno omogućen.

##### Onemogući Bluedroid host stog
```
esp_err_t esp_bluedroid_disable(void)
```

Funkcija vraća *ESP_OK* kada ako je stog uspješno onemogućen.

##### Dohvati stanje Bluedroid stoga
```
esp_bluedroid_status_t esp_bluedroid_get_status(void)
```

Funkcija vraća enumerator koji može imati vrijednosti:
	- *ESP_BLUEDROID_STATUS_UNINITIALIZED* - stog neinicijaliziran
	- *ESP_BLUEDROID_STATUS_INITIALIZED* - stog inicijaliziran, ali nije omogućen
	- *ESP_BLUEDROID_STATUS_ENABLED* - stog inicijaliziran i omogućen

#### GAP funkcionalnost

Kako bi upravljali [GAP funkcionalnostima](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/bluetooth/esp_gap_ble.html#header-file), potrebno je uključiti zaglavlje ```esp_gap_ble_api.h```. GAP funkcionalnostima se može samo upravljati kada je Bluetooth postavljen u *ESP_BT_MODE_BLE* ili *ESP_BT_MODE_BTDM* načinu rada.

##### Registriraj callback (handler) funkcije za GAP događaj
```
esp_err_t esp_ble_gap_register_callback(esp_gap_ble_cb_t callback)
```

Parametar ove funkcije je:

- *callback*
	- funkcija oblika *void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)* čiji su parametri:
		- *event*
			- GAP događaj na kojeg će se pozvati *callback*, enumerator koji može primjerice poprimiti vrijednosti:
				- *ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT* - kada je oglašavanje zadane strukture podataka je dovršeno
				- *ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT* - kada je slanje odgovora na skeniranje mreže nekog drugog BLE uređaja (ako je oglašena zadana struktura podataka) dovršeno
				- *ESP_GAP_BLE_ADV_START_COMPLETE_EVT* - kada je općenito pokrenuto oglašavanja
				- *ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT* - kada je općenito zaustavljeno oglašavanje
				- *ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT* - kada su se BLE parametri za konekciju ažurirali
				- *ESP_GAP_BLE_SET_PKT_LENGTH_COMPLETE_EVT* - kada se postavila veličina paketa
				- *ESP_GAP_BLE_GET_DEV_NAME_COMPLETE_EVT* - kada se ime BLE uređaja dohvati

		- *param*
			- pokazivač na uniju koja se proslijedi funkciji kada se dogodi neki GAP događaj

Funkcija vraća *ESP_OK* kada ako je *callback* uspješno registriran.

##### Postavi lokalno ime uređaja
```
esp_err_t esp_ble_gap_set_device_name(const char *name)
```

Parametar ove funkcije je:

- *name*
	- pokazivač na niz znakova koje predstavlja ime ovog uređaja koje će se vidjeti pri oglašavanju

Funkcija vraća *ESP_OK* kada ako je ime uspješno postavljeno.

##### Dohvati lokalno ime uređaja
```
esp_err_t esp_ble_gap_get_device_name(void)
```

Funkcija vraća *ESP_OK* kada ako je ime dohvaćeno postavljeno. Nakon što se ime dohvati dogodit će se događaj *ESP_GAP_BLE_GET_DEV_NAME_COMPLETE_EVT*. Ime će biti dostupno u *callback* funkciji u strukturi podataka parametra *param*.

##### Postavi konfiguracije podataka oglašavanja i konfiguracije odgovora na skeniranje
```
esp_err_t esp_ble_gap_config_adv_data(esp_ble_adv_data_t *adv_data)
```

Parametar ove funkcije je:

- *adv_data*
	- pokazivač na korisničku definiranu konfiguraciju podataka oglašavanja ili odgovora na skeniranje

Funkcija vraća *ESP_OK* kada ako je konfiguracija podataka oglašavanja uspješno postavljena. Član *set_scan_rsp* definira je li se struktura koristi kao struktura podataka za oglašavanje ili struktura podataka za odgovor na skeniranje. Drugi uređaji će detektirati strukture oglašavanja ovog uređaja skeniranjem. Ako drugi uređaji žele više informacija o ovom uređaju, poslat će zahtjev točno ovom uređaju, a ovaj uređaj će odgovoriti strukturom podataka za odgovor na skeniranje. Ako su se postavili podatci za oglašavanje dogodit će se događaj *ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT*, a ako su se postavili podatci za odgovor na skeniranje dogodit će se *ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT*.

##### Struktura podataka za konfiguriranje podataka oglašavanja i odgovora na skeniranje

Struktura *esp_ble_adv_data_t* sastoji se od sljedećih članova:

- *set_scan_rsp*
	- podatak tipa *bool* koji govori je li ova struktura namijenjena kao struktura podataka za oglašavanje ili za odgovor na skeniranje
- *include_name*
	- podatak tipa *bool* koji odlučuje hoće li se uključiti i ime ovog uređaja u oglašavanju
- *include_txpower*
	- podatak tipa *bool* koji odlučuje je li treba uključiti snagu odašiljača u podatke oglašavanja
- *min_interval*
	- podatak tipa *int* koji postavlja BLE-ov minimalni preferirani interval konekcije (*Connection Interval*)
- *max_interval*
	- podatak tipa *int* koji postavlja BLE-ov maksimalni preferirani interval konekcije (*Connection Interval*)
- *appearance*
	- podatak tipa *int* koji definira [širu kategorizaciju BLE uređaja](https://www.bluetooth.com/wp-content/uploads/Files/Specification/Assigned_Numbers.html#bookmark49), primjerice: 0 - nepoznato, 1 - nije specificirano, 2 - telefon, 3 - računalo ...
- *manufacturer_len*
	- podatak tipa *uint16_t* koji definira veličinu podatka *p_manufacturer_data*
- *p_manufacturer_data*
	- pokazivač na polje tipa *uint8_t*  koji predstavlja ime proizvođača uređaja
- *service_data_len*
	- podatak tipa *uint16_t* koji definira veličinu podatka *p_service_data*
- *p_service_data*
	- pokazivač na polje tipa *uint8_t* koji predstavlja ime servisa koju nudi uređaj
- *service_uuid_len*
	- podatak tipa *uint16_t* koji definira veličinu podatka *p_service_uuid*
- *p_service_uuid*
	- pokazivač na polje tipa *uint8_t* koji predstavlja [UUID servisa](https://www.bluetooth.com/wp-content/uploads/Files/Specification/HTML/Assigned_Numbers/out/en/Assigned_Numbers.pdf?v=1731099497829) koju nudi uređaj
- *flag*
	- podatak tipa *uint8_t* koji definira zastavice koje označavaju svojstva oglašavanja, neke zastavice su:
		- *ESP_BLE_ADV_FLAG_GEN_DISC* - ovaj uređaj je vidljiv svim drugim uređajima
		- *ESP_BLE_ADV_FLAG_LTD_DISC* - ovaj uređaj je vidljiv svim drugim na kratko vrijeme
		- *ESP_BLE_ADV_FLAG_BREDR_NOT_SPT* - ovaj uređaj ne podupire dvostruki Bluetooth Classic + BLE način rada

##### Postavi parametre konekcije
```
esp_err_t esp_ble_gap_update_conn_params(esp_ble_conn_update_params_t *params)
```

Parametar ove funkcije je:

- *params*
	- pokazivač na definiranu konfiguraciju parametara konekcije

Funkcija vraća *ESP_OK* kada ako je konfiguracija parametara konekcije uspješno postavljena. Nakon postavljanja parametara konekcije dogodit će se događaj *ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT*.

##### Struktura podataka za konfiguriranje parametara konekcije

Struktura *esp_ble_conn_update_params_t* sastoji se od sljedećih članova:

- *bda*
	- podatak tipa *esp_bd_addr_t* koji definira adresu ovog uređaja tijekom konekcije
- *min_int*
	- podatak tipa *uint16_t* koji definira minimalni interval konekcije (*Connection Interval*)
- *max_int*
	- podatak tipa *uint16_t* koji definira maksimalni interval konekcije (*Connection Interval*)
- *latency*
	- podatak tipa *uint16_t* koji koliko maksimalno uzastopnih događaja konekcija (*eng. connection event*) preskočiti (*Slave Latency*)
	- raspon od 0x0000 do 0x01F3
- *timeout*
	- podatak tipa *uint16_t* koji definira maksimalni vremenski interval između dva uspješna događaja konekcije (*Supervision Time-out*)
	- raspon od 0x000A do 0x0C80, vremenski interval = *timeout* * 10 ms

##### Započni oglašavanje
```
esp_err_t esp_ble_gap_start_advertising(esp_ble_adv_params_t *adv_params)
```

Parametar ove funkcije je:

- *adv_params*
	- pokazivač na definiranu konfiguraciju parametara oglašavanja, definira način rada BLE-a

Funkcija vraća *ESP_OK* kada ako je konfiguracija parametara oglašavanja uspješno postavljena i započeto je oglašavanje. Nakon započinjanja oglašavanja dogodit će se događaj *ESP_GAP_BLE_ADV_START_COMPLETE_EVT*.

##### Struktura podataka za konfiguriranje parametara oglašavanja

Struktura *esp_ble_adv_params_t* sastoji se od sljedećih članova:

- *adv_int_min*
	- podatak tipa *uint16_t* koji definira minimalni interval oglašavanja
	- od 0x0020 do 0x4000, zadano 0x0800 (1.28 sekundi), vremenski interval = *adv_int_min* * 0.625 ms
- *adv_int_max*
	- podatak tipa *uint16_t* koji definira maksimalni interval oglašavanja
	- od 0x0020 do 0x4000, zadano 0x0800 (1.28 sekundi), vremenski interval = *adv_int_max* * 0.625 ms
- *adv_type*
	- podatak tipa *esp_ble_adv_type_t* koji definira tip oglašavanja, enumeratori:
		- *ADV_TYPE_IND* - najčešći način rada, jednostavno oglašavanje svim uređajima
		- *ADV_TYPE_DIRECT_IND_HIGH* - oglašavanje ciljnom drugom uređaju
		- *ADV_TYPE_SCAN_IND* - oglašavanje s namjerom skeniranja bez mogućnosti konekcija
		- *ADV_TYPE_NONCONN_IND* - oglašavanje s namjerom skeniranja gdje se ne želi uspostavljati konekcija
		- *ADV_TYPE_DIRECT_IND_LOW* - oglašavanje ciljnom drugom uređaju, ali uz manju učestalost
- *own_addr_type*
	- podatak tipa *esp_ble_addr_type_t* koji definira tip vlastite Bluetooth adrese, enumeratori:
		- *BLE_ADDR_TYPE_PUBLIC* - statička, javna adresa
		- *BLE_ADDR_TYPE_RANDOM* - dinamička, slučajno generirana adresa
		- *BLE_ADDR_TYPE_RPA_PUBLIC* - RPA (*eng. Resolvable Private Address*) koja sadrži elemente javne (tvorničke) adrese
		- *BLE_ADDR_TYPE_RPA_RANDOM* - RPA (*eng. Resolvable Private Address*) koja je potpuno slučajno generirana adresa
- *peer_addr*
	- podatak tipa *esp_bd_addr_t* koji definira Bluetooth adresu uređaja na kojeg se ovaj uređaj želi spojiti
		- *BLE_ADDR_TYPE_PUBLIC* - statička, javna adresa
		- *BLE_ADDR_TYPE_RANDOM* - dinamička, slučajno generirana adresa
		- *BLE_ADDR_TYPE_RPA_PUBLIC* - RPA (*eng. Resolvable Private Address*) koja sadrži elemente javne (tvorničke) adrese
		- *BLE_ADDR_TYPE_RPA_RANDOM* - RPA (*eng. Resolvable Private Address*) koja je potpuno slučajno generirana adresa
- *peer_addr_type*
	- podatak tipa *esp_ble_addr_type_t* koji definira tip Bluetooth adrese drugog uređaja na kojeg se ovaj uređaj želi spojiti, enumeratori:

- *channel_map*
	- podatak tipa *esp_ble_adv_channel_t* koji definira kanale kojima će se ovaj uređaj oglašavati, enumeratori:
		- *ADV_CHNL_37* - kanal 37
		- *ADV_CHNL_38* - kanal 38
		- *ADV_CHNL_39* - kanal 39
		- *ADV_CHNL_ALL* - sva tri kanala
- *adv_filter_policy*
	- podatak tipa *esp_ble_adv_filter_t* koji definira koji drugi uređaj smije skenirati i povezati se na ovaj uređaj, enumeratori:
		- *ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY* - bilo koji uređaj
		- *ADV_FILTER_ALLOW_SCAN_WLST_CON_ANY* - samo uređaji s liste odobrenih uređaja (*eng. whitelist*) mogu skenirati i otkriti trenutačni uređaj, ali bilo koji uređaji se mogu povezati na trenutačni uređaj
		- *ADV_FILTER_ALLOW_SCAN_ANY_CON_WLST* - bilo koji uređaji mogu skenirati i otkriti trenutačni uređaj, ali samo uređaji s liste odobrenih uređaja se mogu povezati na trenutačni uređaj
		- *ADV_FILTER_ALLOW_SCAN_WLST_CON_WLST* - samo uređaji s liste odobrenih uređaja mogu skenirati, otkriti i povezati se na trenutačni uređaj

##### Zaustavi oglašavanje
```
esp_err_t esp_ble_gap_stop_advertising(void)
```

Funkcija vraća *ESP_OK* kada ako je zaustavljeno oglašavanje. Nakon zaustavljanja oglašavanja dogodit će se događaj *ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT*.

##### Postavi veličinu MTU paketa
```
esp_err_t esp_ble_gatt_set_local_mtu(uint16_t mtu)
```

Parametar ove funkcije je:

- *mtu*
	- veličina MTU paketa koje će uređaj koristiti

Funkcija vraća *ESP_OK* kada ako je uspješno postavljena veličina MTU paketa. Nakon postavljanja veličine MTU paketa dogodit će se događaj *ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT*. Za korištenje ove funkcije potrebno je uključiti zaglavlje ```esp_gatt_common_api.h```.

#### GATT poslužitelj funkcionalnost

Kako bi upravljali [GATT poslužitelj funkcionalnostima](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/bluetooth/esp_gatts.html#header-file), potrebno je uključiti zaglavlje ```esp_gatts_api.h```. GAP funkcionalnostima se može samo upravljati kada je Bluetooth postavljen u *ESP_BT_MODE_BLE* ili *ESP_BT_MODE_BTDM* načinu rada.

##### Registriraj callback (handler) funkciju za GATT događaj
```
esp_err_t esp_ble_gatts_register_callback(esp_gatts_cb_t callback)
```

Parametar ove funkcije je:

- *callback*
	- funkcija oblika *gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)* čiji parametri mogu biti:
		- *event*
			- GATT događaj na kojeg će se pozvati *callback*, enumerator koji može primjerice poprimiti vrijednosti:
				- *ESP_GATTS_REG_EVT* - nakon registracije profila aplikacije
				- *ESP_GATTS_UNREG_EVT* - nakon odjave profila aplikacije
				- *ESP_GATTS_READ_EVT* - nakon što GATT klijent napravi zahtjev za čitanjem
				- *ESP_GATTS_WRITE_CHAR_EVT* - nakon što GATT klijent napravi zahtjev za pisanjem
				- *ESP_GATTS_CONNECT_EVT* - nakon što se GATT klijent spoji
				- *ESP_GATTS_DISCONNECT_EVT* - nakon što se GATT klijent odspoji
				...
		- *param*
			- pokazivač na uniju koja se proslijedi funkciji kada se dogodi neki GATT događaj
			- za gornje primjere događaja, *param* postaje tip:
				- *ESP_GATTS_REG_EVT* -> *gatts_reg_evt_param*
				- *ESP_GATTS_UNREG_EVT* -> nije navedeno
				- *ESP_GATTS_READ_EVT* -> *gatts_read_evt_param*
				- *ESP_GATTS_WRITE_EVT* -> *gatts_write_evt_param*
				- *ESP_GATTS_CONNECT_EVT* -> *gatts_connect_evt_param*
				- *ESP_GATTS_DISCONNECT_EVT* -> *gatts_disconnect_evt_param*
				...

Funkcija vraća *ESP_OK* kada ako je *callback* uspješno registriran.

##### Registriraj profil aplikacije
```
esp_err_t esp_ble_gatts_app_register(uint16_t app_id)
```

Parametar ove funkcije je:

- *app_id*
	- podatak koji označava identifikator profila

Funkcija vraća *ESP_OK* kada je profil aplikacije uspješno registriran. Nakon prijave profila aplikacije dogodit će se događaj *ESP_GATTS_REG_EVT*.

##### Odjavi profil aplikacije
```
esp_err_t esp_ble_gatts_app_unregister(esp_gatt_if_t gatts_if)
```

Parametar ove funkcije je:

- *gatts_if*
	- GATT sučelje koje BLE stog dodjeljuje prilikom registracije

Funkcija vraća *ESP_OK* kada je profil aplikacije uspješno odjavljen. Nakon odjave profila aplikacije dogodit će se događaj *ESP_GATTS_UNREG_EVT*.

##### Stvori servis
```
esp_err_t esp_ble_gatts_create_service(esp_gatt_if_t gatts_if, esp_gatt_srvc_id_t *service_id, uint16_t num_handle)
```

Parametri ove funkcije su:

- *gatts_if*
	- GATT sučelje koje BLE stog dodjeljuje prilikom registracije
- *service_id*
	- identificira servis
- *num_handle*
	- broj handleova koji će biti rezervirani za taj servis

Funkcija vraća *ESP_OK* kada je servis uspješno stvoren. Nakon stvaranja servisa dogodit će se događaj *ESP_GATTS_CREATE_EVT*.

##### Struktura identifikatora servisa

Struktura *esp_gatt_srvc_id_t* sastoji se od sljedećih članova:

- *id*
	- struktura tipa *esp_gatt_id_t* koja definira UUID i ID instance servisa
	- sastoji se od sljedećih članova:
		- *uuid*
			- struktura tipa *esp_bt_uuid_t* koja predstavlja UUID informacije
			- sastoji se od sljedećih članova:
				- *len* - varijabla tipa *uint16_t* koja definira UUID duljinu, može biti 16 bitna, 32 bitna i 128 bitna
				- *uuid16* - varijabla tipa *uint16_t* koja definira 16 bitni UUID
				- *uuid32* - varijabla tipa *uint16_t* koja definira 32 bitni UUID
				- *uuid128* - polje tipa *uint8_t* koje definira 128 bitni UUID
		- *inst_id*
			- varijabla tipa *uint8_t* koja definira identifikator instance

- *is_primary*
	- podatak tipa *bool* koji definira je li servis primaran

##### Izbriši servis
```
esp_err_t esp_ble_gatts_delete_service(uint16_t service_handle)
```

Parametar ove funkcije je:

- *service_handle*
	- handle na servis

Funkcija vraća *ESP_OK* kada je servis uspješno odjavljen. Nakon brisanja servisa dogodit će se događaj *ESP_GATTS_DELETE_EVT*.

##### Započni rad servisa
```
esp_err_t esp_ble_gatts_start_service(uint16_t service_handle)
```

Parametar ove funkcije je:

- *service_handle*
	- handle na servis

Funkcija vraća *ESP_OK* kada je rad servisa uspješno započet. Nakon započinjanja rada servisa dogodit će se događaj *ESP_GATTS_START_EVT*.

##### Zaustavi rad servisa
```
esp_err_t esp_ble_gatts_stop_service(uint16_t service_handle)
```

Parametar ove funkcije je:

- *service_handle*
	- handle na servis

Funkcija vraća *ESP_OK* kada je rad servisa uspješno započet. Nakon zaustavljanja rada servisa dogodit će se događaj *ESP_GATTS_STOP_EVT*.

##### Dodaj karakteristiku servisu
```
esp_err_t esp_ble_gatts_add_char(uint16_t service_handle, esp_bt_uuid_t *char_uuid, esp_gatt_perm_t perm, esp_gatt_char_prop_t property, esp_attr_value_t *char_val, esp_attr_control_t *control)
```

Parametri ove funkcije su:

- *service_handle*
	- handle na servis
- *char_uuid*
	- UUID karakteristike
- *perm*
	- dozvole koje se mogu vršit nad karakteristikom, enumerator može imati primjerice vrijednost:
		- *ESP_GATT_PERM_READ* - dozvola za čitanje karakteristike
		- *ESP_GATT_PERM_WRITE* - dozvola za pisanje karakteristike
- *property*
	- svojstva karakteristike, enumerator može imati primjerice sljedeće vrijednosti:
		- *ESP_GATT_CHAR_PROP_BIT_READ* - omogućuje čitanje karakteristike
		- *ESP_GATT_CHAR_PROP_BIT_WRITE* - omogućuje pisanje karakteristike
		- *ESP_GATT_CHAR_PROP_BIT_NOTIFY* -  omogućuje notifikaciju promjene vrijednosti karakteristike
- *char_val*
	- struktura koja definira početnu vrijednost karakteristike, sastoji se od sljedećih članova:
		- *attr_max_len* - maksimalna duljina vrijednost karakteristike
		- *attr_len* - trenutačna duljina vrijednosti karakteristike
		- *attr_value* - pokazivač na početnu vrijednost karakteristike
- *control*
	- dodatne kontrole pristupa, najčešće NULL

Funkcija vraća *ESP_OK* kada je karakteristika uspješno dodana. Nakon dodavanja opisnika karakteristike servisu dogodit će se događaj *ESP_GATTS_ADD_CHAR_EVT*.

##### Dodaj opisnik karakteristike karakteristici
```
esp_err_t esp_ble_gatts_add_char_descr(uint16_t service_handle, esp_bt_uuid_t *descr_uuid, esp_gatt_perm_t perm, esp_attr_value_t *char_descr_val, esp_attr_control_t *control)
```

Parametri ove funkcije su:

- *service_handle*
	- handle na servis
- *descr_uuid*
	- UUID opisnika karakteristike
- *perm*
	- dozvole koje se mogu vršit nad opisnikom karakteristike, enumerator može imati primjerice vrijednost:
		- *ESP_GATT_PERM_READ* - dozvola za čitanje karakteristike
		- *ESP_GATT_PERM_WRITE* - dozvola za pisanje karakteristike
- *char_descr_val*
	- struktura koja definira početnu vrijednost opisnika karakteristike, sastoji se od sljedećih članova:
		- *attr_max_len* - maksimalna duljina vrijednost karakteristike
		- *attr_len* - trenutačna duljina vrijednosti karakteristike
		- *attr_value* - pokazivač na početnu vrijednost karakteristike
- *control*
	- dodatne kontrole pristupa, najčešće NULL

Funkcija vraća *ESP_OK* kada je opisnik karakteristike uspješno dodan. Nakon dodavanja opisnika karakteristike karakteristici dogodit će se događaj *ESP_GATTS_ADD_DESCR_EVT*.

##### Šalji odgovor na zahtjev
```
esp_err_t esp_ble_gatts_send_response(esp_gatt_if_t gatts_if, uint16_t conn_id, uint32_t trans_id, esp_gatt_status_t status, esp_gatt_rsp_t *rsp)
```

Parametri ove funkcije su:

- *gatts_if*
	- GATT sučelje koje BLE stog dodijelio profilu prilikom registracije
	- zna kojem profilu pripada odgovor na GATT klijentu
- *conn_id*
	- identifikator veze između GATT poslužitelja i GATT klijenta
- *trans_id*
	- identifikator transakcije koji omogućuje GATT klijentu prepoznavanje odgovora na zahtjev koji je poslao
- *status*
	- stanje odgovora koje govori o uspješnosti zahtjeva, u slučaju uspješnosti zahtjeva enumerator mora bit *ESP_GATT_OK*
- *rsp*
	- unija koja definira odgovor koji će se poslati GATT klijentu, sastoji se od sljedećih članova:
		- *attr_value*
			- struktura tipa *esp_attr_value_t* koja definira početnu vrijednost karakteristike, sastoji se od sljedećih članova:
				- *attr_max_len* - maksimalna duljina vrijednost karakteristike
				- *attr_len* - trenutačna duljina vrijednosti karakteristike
				- *attr_value* - vrijednost karakteristike
		- *handle*
			- handle karakteristike

Funkcija vraća *ESP_OK* kada je karakteristika uspješno dodana. Nakon slanja odgovora na zahtjev dogodit će se događaj *ESP_GATTS_RESPONSE_EVT*.

### ESP32 kao GATT klijent

U nastavku slijede bitne funkcije koje se koriste za GATT klijent (službeni primjer ESP-IDF-a može se naći [ovdje](https://github.com/espressif/esp-idf/blob/v5.2.3/examples/bluetooth/bluedroid/ble/gatt_server/tutorial/Gatt_Server_Example_Walkthrough.md)). ESP32 će se konfigurirati kao **BLE centralni uređaj**.

Funkcije za upravljanje Flash memorijom, upravljanje Bluetooth upravljačkim sklopom i virtualnim HCI-om te upravljanje Bluedroid host stogom Bluetootha su iste kao i za GATT poslužitelj. Funkcije za GAP i GATT funkcionalnosti su drugačije.

Primjer za GATT klijent nalazi se [ovdje](ble_gatt_client).

#### GAP funkcionalnost

Kako bi upravljali [GAP funkcionalnostima](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/bluetooth/esp_gap_ble.html#header-file), potrebno je uključiti zaglavlje ```esp_gap_ble_api.h```. GAP funkcionalnostima se može samo upravljati kada je Bluetooth postavljen u *ESP_BT_MODE_BLE* ili *ESP_BT_MODE_BTDM* načinu rada.

##### Registriraj callback (handler) funkciju za GAP događaj
```
esp_err_t esp_ble_gap_register_callback(esp_gap_ble_cb_t callback)
```

Parametar ove funkcije je:

- *callback*
	- funkcija oblika *void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)* čiji su parametri:
		- *event*
			- GAP događaj na kojeg će se pozvati *callback*, enumerator koji može primjerice poprimiti vrijednosti:
				- *ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT* - kada je postavljanje parametara za skeniranje dovršeno
				- *ESP_GAP_BLE_SCAN_START_COMPLETE_EVT* - kada završilo započinjanje skeniranja
				- *ESP_GAP_BLE_SCAN_RESULT_EVT* - kada je dostupan rezultat skeniranja
				- *ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT* - kada završilo zaustavljanje skeniranja
				- *ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT* - kada su se BLE parametri za konekciju ažurirali
				- *ESP_GAP_BLE_SET_PKT_LENGTH_COMPLETE_EVT* - kada se postavila veličina paketa

		- *param*
			- pokazivač na uniju koja se proslijedi funkciji kada se dogodi neki GAP događaj

Funkcija vraća *ESP_OK* kada ako je *callback* uspješno registriran. Iako je ova funkcija već bila navedena, potrebno je obratiti pažnju na drugačije događaje.

##### Započni skeniranje
```
esp_err_t esp_ble_gap_start_scanning(uint32_t duration)
```

Parametar ove funkcije je:

- *duration*
	- broj sekundi koliko treba dugo treba skenirati

Funkcija vraća *ESP_OK* ako je skeniranje uspješno započeto. Nakon započinjanja skeniranja dogodit će se događaj *ESP_GAP_BLE_SCAN_START_COMPLETE_EVT*.

##### Zaustavi skeniranje
```
esp_err_t esp_ble_gap_stop_scanning(void)
```

Funkcija vraća *ESP_OK* ako je skeniranje uspješno završilo. Nakon zaustavljanja skeniranja dogodit će se događaj *ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT*.

##### Postavi parametre za skeniranje
```
esp_err_t esp_ble_gap_set_scan_params(esp_ble_scan_params_t *scan_params)
```

Parametar ove funkcije je:

- *scan_params*
	- struktura koja definira parametre koji će se koristiti prilikom skeniranja

Funkcija vraća *ESP_OK* ako je postavljanje parametara uspješno završilo. Nakon postavljanje parametara za skeniranje dogodit će se događaj *ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT*.

##### Struktura podataka za konfiguriranje parametara oglašavanja

Struktura *esp_ble_scan_params_t* sastoji se od sljedećih članova:

- *scan_type*
	- podatak tipa *esp_ble_scan_type_t* koji označava vrstu skeniranja, enumeratori:
		- *BLE_SCAN_TYPE_PASSIVE* - samo skeniraj
		- *BLE_SCAN_TYPE_ACTIVE* - skeniraj i odgovori
- *own_addr_type*
	- podatak tipa *esp_ble_addr_type_t* koji označava vrstu vlastite Bluetooth adrese, enumeratori:
		- *BLE_ADDR_TYPE_PUBLIC* - statička, javna adresa
		- *BLE_ADDR_TYPE_RANDOM* - dinamička, slučajno generirana adresa
		- *BLE_ADDR_TYPE_RPA_PUBLIC* - RPA (*eng. Resolvable Private Address*) koja sadrži elemente javne (tvorničke) adrese
		- *BLE_ADDR_TYPE_RPA_RANDOM* - RPA (*eng. Resolvable Private Address*) koja je potpuno slučajno generirana adresa
- *scan_filter_policy*
	- podatak tipa *esp_ble_scan_filter_t* koji definira politiku skeniranja, neki enumeratori:
		- *BLE_SCAN_FILTER_ALLOW_ALL* - prihvati sve pakete osim usmjerenih koji nisu usmjereni na ovaj uređaj
		- *BLE_SCAN_FILTER_ALLOW_ONLY_WLST* - prihvati sve pakete s uređaja koji su odobreni osim usmjerenih koji nisu usmjereni na ovaj uređaj
		- *BLE_SCAN_FILTER_ALLOW_UND_RPA_DIR* - prihvati neusmjerene pakete, usmjerene pakete za ovaj uređaj i usmjerene pakete za ovaj uređaj gdje je adresa inicijatora RPA 
		- *BLE_SCAN_FILTER_ALLOW_WLIST_RPA_DIR* - prihvati neusmjerene pakete s uređaja koji su odobreni, usmjerene pakete za ovaj uređaj i usmjerene pakete za ovaj uređaj gdje je adresa inicijatora RPA
- *scan_interval*
	- podatak tipa *uint16_t* koji označava interval između dva skeniranja
	- od 0x0004 do 0x4000, zadano 0x0010 (10 milisekundi), vremenski interval = *scan_interval* * 0.625 ms
- *scan_window*
	- podatak tipa *uint16_t* koji označava trajanje skeniranja
	- od 0x0004 do 0x4000, zadano 0x0010 (10 milisekundi), vremenski interval = *scan_window* * 0.625 ms
- *scan_duplicate*
	- podatak tipa *uint16_t* koji označava je li treba skenirati duplikate, enumeratori:
		- *BLE_SCAN_DUPLICATE_DISABLE* - onemogući
		- *BLE_SCAN_DUPLICATE_ENABLE* - omogući
		- *BLE_SCAN_DUPLICATE_ENABLE_RESET* - omogući s resetiranjem u svakom intervalu skeniranja (samo od BLE 5.0)
		- *BLE_SCAN_DUPLICATE_MAX* - rezervirano

##### Izvuci podatak za specifični atribut iz dohvaćenih podataka oglašavanja
```
uint8_t *esp_ble_resolve_adv_data_by_type(uint8_t *adv_data, uint16_t adv_data_len, esp_ble_adv_data_type type, uint8_t *length)
```

Parametri ove funkcije su:

- *adv_data*
	- pokazivač na podatke oglašavanja
- *adv_data_len*
	- veličina podataka oglašavanja koja se treba obraditi
- *type*
	- tip podatka oglašavanja, enumerator može imati primjerice sljedeće vrijednosti:
		- *ESP_BLE_AD_TYPE_16SRV_CMPL* - potpuni 16 bitni UUID
		- *ESP_BLE_AD_TYPE_32SRV_CMPL* - potpuni 32 bitni UUID
		- *ESP_BLE_AD_TYPE_128SRV_CMPL* - potpuni 128 bitni UUID
		- *ESP_BLE_AD_TYPE_NAME_CMPL* - potpuno ime uređaja
		- *ESP_BLE_AD_TYPE_TX_PWR* - snaga odašiljanja
		- ...
- *length*
	- mjesto u kojem će se spremiti veličina izvučenih podataka

Funkcija vraća pokazivač na podatak oglašavanja koji je tražen.

Postoji i druga starija funkcija za izvlačenje specifičnih atributa iz dohvaćenih podataka oglašavanja:
```
uint8_t *esp_ble_resolve_adv_data(uint8_t *adv_data, uint8_t type, uint8_t *length)
```

Parametri ove funkcije su:

- *adv_data*
	- pokazivač na podatke oglašavanja
- *type*
	- tip podatka oglašavanja, može se poslati i enumerator *esp_ble_adv_data_type* naveden u prethodnom primjeru
- *length*
	- mjesto u kojem će se spremiti veličina izvučenih podataka

Funkcija vraća pokazivač na podatak oglašavanja koji je tražen.

##### Postavi veličinu MTU paketa
```
esp_err_t esp_ble_gatt_set_local_mtu(uint16_t mtu)
```

Parametar ove funkcije je:

- *mtu*
	- veličina MTU paketa koje će uređaj koristiti

Funkcija vraća *ESP_OK* kada ako je uspješno postavljena veličina MTU paketa. Nakon postavljanje veličine MTU paketa dogodit će se događaj *ESP_GAP_BLE_SET_PKT_LENGTH_COMPLETE_EVT*. Za korištenje ove funkcije potrebno je uključiti zaglavlje ```esp_gatt_common_api.h```.

#### GATT klijent funkcionalnost

Kako bi upravljali [GATT klijent funkcionalnostima](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/bluetooth/esp_gattc.html#header-file), potrebno je uključiti zaglavlje ```esp_gattc_api.h```. GAP funkcionalnostima se može samo upravljati kada je Bluetooth postavljen u *ESP_BT_MODE_BLE* ili *ESP_BT_MODE_BTDM* načinu rada.

##### Registriraj callback (handler) funkciju za GATT događaj
```
esp_err_t esp_ble_gattc_register_callback(esp_gattc_cb_t callback)
```

Parametar ove funkcije je:

- *callback*
	- funkcija oblika *gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)* čiji parametri mogu biti:
		- *event*
			- GATT događaj na kojeg će se pozvati *callback*, enumerator koji može primjerice poprimiti vrijednosti:
				- *ESP_GATTC_REG_EVT* - nakon registracije profila aplikacije
				- *ESP_GATTC_UNREG_EVT* - nakon odjave profila aplikacije
				- *ESP_GATTC_OPEN_EVT* - nakon što GATT klijent napravi zahtjev za čitanjem
				- *ESP_GATTC_CLOSE_EVT* - nakon što se GATT klijent odspoji
				- *ESP_GATTC_READ_CHAR_EVT* - nakon što GATT klijent napravi zahtjev za pisanjem
				- *ESP_GATTC_WRITE_CHAR_EVT* - nakon što se GATT klijent spoji
				- *ESP_GATTC_CONNECT_EVT* - 
				- *ESP_GATTC_DISCONNECT_EVT* -
				...
		- *param*
			- pokazivač na uniju koja se proslijedi funkciji kada se dogodi neki GATT događaj
			- za gornje primjere događaja, *param* postaje tip:
				- *ESP_GATTC_REG_EVT* -> *gatts_reg_evt_param*
				- *ESP_GATTC_UNREG_EVT* -> nije navedeno
				- *ESP_GATTC_OPEN_EVT* -> *gattc_open_evt_param*
				- *ESP_GATTC_CLOSE_EVT* -> *gattc_close_evt_param*
				- *ESP_GATTC_READ_CHAR_EVT* -> *gattc_read_char_evt_param*
				- *ESP_GATTC_WRITE_CHAR_EVT* -> *gattc_write_evt_param*
				- *ESP_GATTC_CONNECT_EVT* -> *gattc_connect_evt_param*
				- *ESP_GATTC_DISCONNECT_EVT* -> *gattc_disconnect_evt_param*
				...

Funkcija vraća *ESP_OK* kada ako je *callback* uspješno registriran.

##### Registriraj profil aplikacije
```
esp_err_t esp_ble_gattc_app_register(uint16_t app_id)
```

Parametar ove funkcije je:

- *app_id*
	- podatak koji označava identifikator profila

Funkcija vraća *ESP_OK* kada je profil aplikacije uspješno registriran. Nakon prijave profila aplikacije dogodit će se događaj *ESP_GATTC_REG_EVT*.

##### Odjavi profil aplikacije
```
esp_err_t esp_ble_gattc_app_unregister(esp_gatt_if_t gatts_if)
```

Parametar ove funkcije je:

- *gatts_if*
	- GATT sučelje koje BLE stog dodjeljuje prilikom registracije

Funkcija vraća *ESP_OK* kada je profil aplikacije uspješno odjavljen. Nakon odjave profila aplikacije dogodit će se događaj *ESP_GATTC_UNREG_EVT*.

##### Otvori direktnu konekciju prema GATT poslužitelju
```
esp_err_t esp_ble_gattc_open(esp_gatt_if_t gattc_if, esp_bd_addr_t remote_bda, esp_ble_addr_type_t remote_addr_type, bool is_direct)
```

Parametri ove funkcije:

- *gatts_if*
	- GATT sučelje koje BLE stog dodjeljuje prilikom registracije
- *remote_bda*
	- Bluetooth adresa poslužitelja
- *remote_addr_type*
	- označava vrstu Bluetooth adrese poslužitelja, enumerator može biti:
		- *BLE_ADDR_TYPE_PUBLIC* - statička, javna adresa
		- *BLE_ADDR_TYPE_RANDOM* - dinamička, slučajno generirana adresa
		- *BLE_ADDR_TYPE_RPA_PUBLIC* - RPA (*eng. Resolvable Private Address*) koja sadrži elemente javne (tvorničke) adrese
		- *BLE_ADDR_TYPE_RPA_RANDOM* - RPA (*eng. Resolvable Private Address*) koja je potpuno slučajno generirana adresa
- *is_direct*
	- direktna konekcija (*true*) ili pozadinska auto konekcija (*false*)

Funkcija vraća *ESP_OK* kada je uspješno otvorena direktna konekcija. Nakon otvaranja konekcije dogodit će se događaj *ESP_GATTC_OPEN_EVT*.

##### Zatvori konekciju prema GATT poslužitelju
```
esp_err_t esp_ble_gattc_close(esp_gatt_if_t gattc_if, uint16_t conn_id)
```

Parametri ove funkcije:

- *gatts_if*
	- GATT sučelje koje BLE stog dodjeljuje prilikom registracije
- *conn_id*
	- identifikator veze između GATT poslužitelja i GATT klijenta

Funkcija vraća *ESP_OK* kada je uspješno zatvorena direktna konekcija. Nakon zatvaranja konekcije dogodit će se događaj *ESP_GATTC_CLOSE_EVT*.

##### Dohvati servise u bazu podataka
```
esp_err_t esp_ble_gattc_search_service(esp_gatt_if_t gattc_if, uint16_t conn_id, esp_bt_uuid_t *filter_uuid)
```

Parametri ove funkcije:

- *gatts_if*
	- GATT sučelje koje BLE stog dodjeljuje prilikom registracije
- *conn_id*
	- identifikator veze između GATT poslužitelja i GATT klijenta
- filter_uuid
	- UUID po kojima će se filtrirati rezultat, struktura se sastoji od članova:
		- *len* - varijabla tipa *uint16_t* koja definira UUID duljinu, može biti 16 bitna, 32 bitna i 128 bitna
		- *uuid16* - varijabla tipa *uint16_t* koja definira 16 bitni UUID
		- *uuid32* - varijabla tipa *uint16_t* koja definira 32 bitni UUID
		- *uuid128* - polje tipa *uint8_t* koje definira 128 bitni UUID

Funkcija vraća *ESP_OK* kada je dohvat uspješno izvršen. Nakon pronalaska jednog servisa na drugom uređaju dogodit će se događaj *ESP_GATTC_SEARCH_RES_EVT*, a nakon pronalaska svih servisa dogodit će se događaj *ESP_GATTC_SEARCH_CMPL_EVT*.

##### Izvuci atribute iz dohvaćenih servisa iz baze podataka
```
esp_gatt_status_t esp_ble_gattc_get_attr_count(esp_gatt_if_t gattc_if, uint16_t conn_id, esp_gatt_db_attr_type_t type, uint16_t start_handle, uint16_t end_handle, uint16_t char_handle, uint16_t *count)
```

Parametri ove funkcije:

- *gatts_if*
	- GATT sučelje koje BLE stog dodjeljuje prilikom registracije
- *conn_id*
	- identifikator veze između GATT poslužitelja i GATT klijenta
- *type*
	- tip atributa koji će se povući iz baze podataka, enumeratori mogu biti:
		- *ESP_GATT_DB_PRIMARY_SERVICE* - primarni servis
		- *ESP_GATT_DB_SECONDARY_SERVICE* - sekundarni servis
		- *ESP_GATT_DB_CHARACTERISTIC* - karakteristika
		- *ESP_GATT_DB_DESCRIPTOR* - opisnik karakteristike
		- *ESP_GATT_DB_INCLUDED_SERVICE* - servis koji uključuje povezana servise
		- *ESP_GATT_DB_ALL* - svi atributi
- *start_handle*
	- početni handle za atribut, u slučaju da je *type* ESP_GATT_DB_DESCRIPTOR ignorira se
- *end_handle*
	- završni handle za atribut, u slučaju da je *type* ESP_GATT_DB_DESCRIPTOR ignorira se
- *char_handle*
	- handle karakteristike ako je *type* ESP_GATT_DB_DESCRIPTOR, inače se ignorira
- *count*
	- nakon izvršenja funkcije spremnik broja pronađenih atributa

Funkcija vraća *ESP_OK* kada je dohvat uspješno izvršen.

##### Izvuci samo servise iz baze podataka
```
esp_gatt_status_t esp_ble_gattc_get_service(esp_gatt_if_t gattc_if, uint16_t conn_id, esp_bt_uuid_t *svc_uuid, esp_gattc_service_elem_t *result, uint16_t *count, uint16_t offset)
```

Parametri ove funkcije:

- *gatts_if*
	- GATT sučelje koje BLE stog dodjeljuje prilikom registracije
- *conn_id*
	- identifikator veze između GATT poslužitelja i GATT klijenta
- *svc_uuid*
	- UUID po kojima će se filtrirati rezultat, struktura se sastoji od članova:
		- *len* - varijabla tipa *uint16_t* koja definira UUID duljinu, može biti 16 bitna, 32 bitna i 128 bitna
		- *uuid16* - varijabla tipa *uint16_t* koja definira 16 bitni UUID
		- *uuid32* - varijabla tipa *uint16_t* koja definira 32 bitni UUID
		- *uuid128* - polje tipa *uint8_t* koje definira 128 bitni UUID
- *result*
	- pokazivač na servis koji je pronađen
- *count*
	- broj servisa kojih je potrebno naći, nakon izvršenja funkcije spremnik broja pronađenih servisa
- *offset*
	- pomak servisa od kud početi pretraživanje

Funkcija vraća *ESP_OK* kada je izvlačenje servisa uspješno izvršeno.

##### Izvuci karakteristike po UUID-u
```
esp_gatt_status_t esp_ble_gattc_get_char_by_uuid(esp_gatt_if_t gattc_if, uint16_t conn_id, uint16_t start_handle, uint16_t end_handle, esp_bt_uuid_t char_uuid, esp_gattc_char_elem_t *result, uint16_t *count)
```

Parametri ove funkcije:

- *gatts_if*
	- GATT sučelje koje BLE stog dodjeljuje prilikom registracije
- *conn_id*
	- identifikator veze između GATT poslužitelja i GATT klijenta
- *start_handle*
	- početni handle za atribut
- *end_handle*
	- završni handle za atribut
- *char_uuid*
	- UUID po kojima će se filtrirati rezultat, struktura se sastoji od članova:
		- *len* - varijabla tipa *uint16_t* koja definira UUID duljinu, može biti 16 bitna, 32 bitna i 128 bitna
		- *uuid16* - varijabla tipa *uint16_t* koja definira 16 bitni UUID
		- *uuid32* - varijabla tipa *uint16_t* koja definira 32 bitni UUID
		- *uuid128* - polje tipa *uint8_t* koje definira 128 bitni UUID
- *result*
	- pokazivač na karakteristiku koja je pronađena
- *count*
	- broj karakteristika kojih je potrebno naći, nakon izvršenja funkcije spremnik broja pronađenih karakteristika

Funkcija vraća *ESP_OK* kada je izvlačenje karakteristika uspješno izvršeno.

##### Izvuci opisnike karakteristika po UUID-u karakteristike i UUID-u opisnika karakteristike
```
esp_gatt_status_t esp_ble_gattc_get_descr_by_uuid(esp_gatt_if_t gattc_if, uint16_t conn_id, uint16_t start_handle, uint16_t end_handle, esp_bt_uuid_t char_uuid, esp_bt_uuid_t descr_uuid, esp_gattc_descr_elem_t *result, uint16_t *count)
```

Parametri ove funkcije:

- *gatts_if*
	- GATT sučelje koje BLE stog dodjeljuje prilikom registracije
- *conn_id*
	- identifikator veze između GATT poslužitelja i GATT klijenta
- *start_handle*
	- početni handle za atribut
- *end_handle*
	- završni handle za atribut
- *char_uuid*
	- UUID po kojima će se filtrirati rezultat, struktura se sastoji od članova:
		- *len* - varijabla tipa *uint16_t* koja definira UUID duljinu, može biti 16 bitna, 32 bitna i 128 bitna
		- *uuid16* - varijabla tipa *uint16_t* koja definira 16 bitni UUID
		- *uuid32* - varijabla tipa *uint16_t* koja definira 32 bitni UUID
		- *uuid128* - polje tipa *uint8_t* koje definira 128 bitni UUID
- *descr_uuid*
	- UUID po kojima će se filtrirati rezultat, struktura se sastoji od članova:
		- *len* - varijabla tipa *uint16_t* koja definira UUID duljinu, može biti 16 bitna, 32 bitna i 128 bitna
		- *uuid16* - varijabla tipa *uint16_t* koja definira 16 bitni UUID
		- *uuid32* - varijabla tipa *uint16_t* koja definira 32 bitni UUID
		- *uuid128* - polje tipa *uint8_t* koje definira 128 bitni UUID
- *result*
	- pokazivač na opisnik karakteristike koja je pronađena
- *count*
	- broj opisnika karakteristika kojih je potrebno naći, nakon izvršenja funkcije spremnik broja pronađenih opisnika karakteristika

Funkcija vraća *ESP_OK* kada je izvlačenje opisnika karakteristika uspješno izvršeno.

##### Izvuci opisnike karakteristika po handleu karakteristike i po UUID-u opisnika karakteristike
```
esp_gatt_status_t esp_ble_gattc_get_descr_by_char_handle(esp_gatt_if_t gattc_if, uint16_t conn_id, uint16_t char_handle, esp_bt_uuid_t descr_uuid, esp_gattc_descr_elem_t *result, uint16_t *count)
```

Parametri ove funkcije:

- *gatts_if*
	- GATT sučelje koje BLE stog dodjeljuje prilikom registracije
- *conn_id*
	- identifikator veze između GATT poslužitelja i GATT klijenta
- *char_handle*
	- handle karakteristike
- *descr_uuid*
	- UUID po kojima će se filtrirati rezultat, struktura se sastoji od članova:
		- *len* - varijabla tipa *uint16_t* koja definira UUID duljinu, može biti 16 bitna, 32 bitna i 128 bitna
		- *uuid16* - varijabla tipa *uint16_t* koja definira 16 bitni UUID
		- *uuid32* - varijabla tipa *uint16_t* koja definira 32 bitni UUID
		- *uuid128* - polje tipa *uint8_t* koje definira 128 bitni UUID
- *result*
	- pokazivač na opisnik karakteristike koja je pronađena
- *count*
	- broj opisnika karakteristika kojih je potrebno naći, nakon izvršenja funkcije spremnik broja pronađenih opisnika karakteristika

Funkcija vraća *ESP_OK* kada je izvlačenje opisnika karakteristika uspješno izvršeno.

##### Čitaj podatke iz karakteristike
```
esp_err_t esp_ble_gattc_read_char(esp_gatt_if_t gattc_if, uint16_t conn_id, uint16_t handle, esp_gatt_auth_req_t auth_req)
```

Parametri ove funkcije:

- *gatts_if*
	- GATT sučelje koje BLE stog dodjeljuje prilikom registracije
- *conn_id*
	- identifikator veze između GATT poslužitelja i GATT klijenta
- *handle*
	- handle karakteristike
- *auth_req*
	- način autentifikacije koji je potreban za pristupit karakteristici, enumeratori mogu biti:
		- *ESP_GATT_AUTH_REQ_NONE* - nema autentifikacije
		- *ESP_GATT_AUTH_REQ_NO_MITM* - neautentificirana enkripcija
		- *ESP_GATT_AUTH_REQ_MITM* - autentificirana enkripcija, zaštita protiv MITM (*eng. Man in the middle*) napada
		- *ESP_GATT_AUTH_REQ_SIGNED_NO_MITM* - potpisani podatci
		- *ESP_GATT_AUTH_REQ_SIGNED_MITM* - potpisani podatci sa zaštitom protiv MITM-a

Funkcija vraća *ESP_OK* kada je čitanje vrijednosti iz karakteristike uspješno izvršeno. Nakon čitanja podataka iz karakteristike dogodit će se događaj *ESP_GATTC_READ_CHAR_EVT*. Pravo čitanje podataka se izvodi u handleru.

##### Čitaj podatke iz opisnika karakteristike
```
esp_err_t esp_ble_gattc_read_char_descr(esp_gatt_if_t gattc_if, uint16_t conn_id, uint16_t handle, esp_gatt_auth_req_t auth_req)
```

Parametri ove funkcije:

- *gatts_if*
	- GATT sučelje koje BLE stog dodjeljuje prilikom registracije
- *conn_id*
	- identifikator veze između GATT poslužitelja i GATT klijenta
- *handle*
	- handle opisnika karakteristike
- *auth_req*
	- način autentifikacije koji je potreban za pristupit opisniku karakteristike, enumeratori mogu biti:
		- *ESP_GATT_AUTH_REQ_NONE* - nema autentifikacije
		- *ESP_GATT_AUTH_REQ_NO_MITM* - neautentificirana enkripcija
		- *ESP_GATT_AUTH_REQ_MITM* - autentificirana enkripcija, zaštita protiv MITM (*eng. Man in the middle*) napada
		- *ESP_GATT_AUTH_REQ_SIGNED_NO_MITM* - potpisani podatci
		- *ESP_GATT_AUTH_REQ_SIGNED_MITM* - potpisani podatci sa zaštitom protiv MITM-a

Funkcija vraća *ESP_OK* kada je čitanje vrijednosti iz opisnika karakteristike uspješno izvršeno. Nakon čitanja podataka iz opisnika karakteristike dogodit će se događaj *ESP_GATTC_READ_DESCR_EVT*. Pravo čitanje podataka se izvodi u handleru.

##### Piši podatke u karakteristiku
```
esp_err_t esp_ble_gattc_write_char(esp_gatt_if_t gattc_if, uint16_t conn_id, uint16_t handle, uint16_t value_len, uint8_t *value, esp_gatt_write_type_t write_type, esp_gatt_auth_req_t auth_req)
```

Parametri ove funkcije:

- *gatts_if*
	- GATT sučelje koje BLE stog dodjeljuje prilikom registracije
- *conn_id*
	- identifikator veze između GATT poslužitelja i GATT klijenta
- *handle*
	- handle karakteristike
- *value_len*
	- veličina podataka
- *value*
	- podatci
- *write_type*
	- tip pisanja, enumeratori mogu biti:
		- *ESP_GATT_WRITE_TYPE_NO_RSP* - šalje se zahtjev koji ne zahtijeva odgovor (naredba)
		- *ESP_GATT_WRITE_TYPE_RSP* - šalje se zahtjev koji zahtijeva odgovor
- *auth_req*
	- način autentifikacije koji je potreban za pristupit karakteristike, enumeratori mogu biti:
		- *ESP_GATT_AUTH_REQ_NONE* - nema autentifikacije
		- *ESP_GATT_AUTH_REQ_NO_MITM* - neautentificirana enkripcija
		- *ESP_GATT_AUTH_REQ_MITM* - autentificirana enkripcija, zaštita protiv MITM (*eng. Man in the middle*) napada
		- *ESP_GATT_AUTH_REQ_SIGNED_NO_MITM* - potpisani podatci
		- *ESP_GATT_AUTH_REQ_SIGNED_MITM* - potpisani podatci sa zaštitom protiv MITM-a

Funkcija vraća *ESP_OK* kada je pisanje vrijednosti u karakteristiku uspješno izvršeno. Nakon pisanja podataka u opisnik karakteristike dogodit će se događaj *ESP_GATTC_WRITE_CHAR_EVT*.

##### Piši podatke u opisnik karakteristike
```
esp_err_t esp_ble_gattc_write_char_descr(esp_gatt_if_t gattc_if, uint16_t conn_id, uint16_t handle, uint16_t value_len, uint8_t *value, esp_gatt_write_type_t write_type, esp_gatt_auth_req_t auth_req)
```

Parametri ove funkcije:

- *gatts_if*
	- GATT sučelje koje BLE stog dodjeljuje prilikom registracije
- *conn_id*
	- identifikator veze između GATT poslužitelja i GATT klijenta
- *handle*
	- handle opisnika karakteristike
- *value_len*
	- veličina podataka
- *value*
	- podatci
- *write_type*
	- tip pisanja, enumeratori mogu biti:
		- *ESP_GATT_WRITE_TYPE_NO_RSP* - šalje se zahtjev koji ne zahtijeva odgovor (naredba)
		- *ESP_GATT_WRITE_TYPE_RSP* - šalje se zahtjev koji zahtijeva odgovor
- *auth_req*
	- način autentifikacije koji je potreban za pristupit opisniku karakteristike, enumeratori mogu biti:
		- *ESP_GATT_AUTH_REQ_NONE* - nema autentifikacije
		- *ESP_GATT_AUTH_REQ_NO_MITM* - neautentificirana enkripcija
		- *ESP_GATT_AUTH_REQ_MITM* - autentificirana enkripcija, zaštita protiv MITM (*eng. Man in the middle*) napada
		- *ESP_GATT_AUTH_REQ_SIGNED_NO_MITM* - potpisani podatci
		- *ESP_GATT_AUTH_REQ_SIGNED_MITM* - potpisani podatci sa zaštitom protiv MITM-a

Funkcija vraća *ESP_OK* kada je čitanje vrijednosti iz opisnika karakteristike uspješno izvršeno. Nakon pisanja podataka u opisnik karakteristike dogodit će se događaj *ESP_GATTC_WRITE_DESCR_EVT*.

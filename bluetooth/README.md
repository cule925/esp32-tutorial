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
	- BLE uređaji rade skokove po kanalima (FHSS), kada uređaji razmjenjuju podatke u trenutnom kanalu, to se naziva događaj konekcije (eng. *connection event*)
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

### ESP32 kao GATT poslužitelj

U nastavku slijede bitne funkcije koje se koriste za GATT poslužitelj (službeni primjer ESP-IDF-a može se naći [ovdje](https://github.com/espressif/esp-idf/blob/v5.2.3/examples/bluetooth/bluedroid/ble/gatt_server/tutorial/Gatt_Server_Example_Walkthrough.md)). ESP32 će se konfigurirati kao **BLE periferija** uređaj.

#### Flash memorija za trajnu pohranu podataka

Bluetooth sprema neke svoje podatke u [NVS (*eng. Non-Volatile Storage*) particiju](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/storage/nvs_flash.html) koja se nalazi u Flash memoriji. Podatci se u NVS particiju spremaju u obliku ključa i vrijednosti.

Zaglavlje koje je potrebno uključiti za rad s [NVS-om](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/storage/nvs_flash.html#api-reference) je ```nvs_flash.h```.

##### Inicijalizacija uobičajene NVS particije
```
esp_err_t nvs_flash_init(void)
```

Funkcija vraća *ESP_OK* ako je inicijalizacija NVS-a uspjela. Funkcija inicijalizira opisnike u memoriji potrebne za pristup NVM particiji.

##### Brisanje svih ključeva i vrijednosti uobičajene NVS particije
```
esp_err_t nvs_flash_erase(void)
```

Funkcija vraća *ESP_OK* ako je brisanje ključeva i vrijednosti uspjelo.

##### Naredba za deinicijalizaciju uobičajene NVS particije
```
esp_err_t nvs_flash_deinit(void)
```

Funkcija vraća *ESP_OK* ako je deinicijalizacija NVS-a uspjela. Funkcija deinicijalizira opisnike u memoriji potrebne za pristup NVM particiji.

#### Upravljanje Bluetooth upravljačkim sklopom i virtualnim HCI-om

Kako bi mogli upravljati [Bluetooth sklopom i virtualnim HCI-om]((https://docs.espressif.com/projects/esp-idf/en/v5.3.1/esp32/api-reference/bluetooth/controller_vhci.html#api-reference)), potrebno je uključiti zaglavlje ```esp_bt.h```.

##### Oslobađanje memorije Bluetooth sklopa
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

##### Uobičajena konfiguracija Bluetooth sklopa
```
BT_CONTROLLER_INIT_CONFIG_DEFAULT()
```

Makro vraća strukturu tipa *esp_bt_controller_config_t* koja predstavlja uobičajenu konfiguraciju Bluetooth sklopa.

##### Inicijalizacija Bluetooth sklopa
```
esp_err_t esp_bt_controller_init(esp_bt_controller_config_t *cfg)
```

Parametar ove funkcije je:

- *cfg*
	- pokazivač na konfiguraciju kojom će se postaviti Bluetooth sklop

Funkcija vraća *ESP_OK* kada ako je Bluetooth sklop uspješno inicijaliziran danom konfiguracijom. Ovo je prva funkcija koja se poziva prije bilo koje druge Bluetooth funkcije.

##### Deinicijalizacija Bluetooth sklopa
```
esp_err_t esp_bt_controller_deinit(void)
```

Funkcija vraća *ESP_OK* kada ako je Bluetooth sklop uspješno inicijaliziran danom konfiguracijom. Ako se pozove ova funkcija ne smije se više zvati ni jedna Bluetooth funkcija. Uređaj ne smije biti u stanju oglašavanja ili skeniranja kada se poziva ova funkcija.

##### Omogućivanje Bluetooth sklopa
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

##### Onemogućivanje Bluetooth sklopa
```
esp_err_t esp_bt_controller_disable(void)
```

Funkcija vraća *ESP_OK* kada ako je Bluetooth sklop uspješno onemogućen.

##### Stanje Bluetooth sklopa
```
esp_bt_controller_status_t esp_bt_controller_get_status(void)
```

Funkcija vraća enumerator koji može imati vrijednosti:
	- *ESP_BT_CONTROLLER_STATUS_IDLE* - sklop neinicijaliziran
	- *ESP_BT_CONTROLLER_STATUS_INITED* - sklop inicijaliziran, ali nije omogućen
	- *ESP_BT_CONTROLLER_STATUS_ENABLED* - sklop inicijaliziran i omogućen

#### Upravljanje Bluedroid host stogom Bluetootha

Za upravljanje [Bluedroid host stogom](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/bluetooth/esp_bt_main.html#api-reference), potrebno je uključiti zaglavlje ```esp_bt_main.h```.

##### Inicijalizacija Bluedroid host stoga
```
esp_err_t esp_bluedroid_init(void)
```

Funkcija vraća *ESP_OK* kada ako je stog uspješno inicijaliziran.

##### Deinicijalizacija Bluedroid host stoga
```
esp_err_t esp_bluedroid_deinit(void)
```

Funkcija vraća *ESP_OK* kada ako je stog uspješno deinicijaliziran.

##### Omogućivanje Bluedroid host stoga
```
esp_err_t esp_bluedroid_enable(void)
```

Funkcija vraća *ESP_OK* kada ako je stog uspješno omogućen.

##### Onemogućivanje Bluedroid host stoga
```
esp_err_t esp_bluedroid_disable(void)
```

Funkcija vraća *ESP_OK* kada ako je stog uspješno onemogućen.

##### Stanje Bluedroid stoga
```
esp_bluedroid_status_t esp_bluedroid_get_status(void)
```

Funkcija vraća enumerator koji može imati vrijednosti:
	- *ESP_BLUEDROID_STATUS_UNINITIALIZED* - stog neinicijaliziran
	- *ESP_BLUEDROID_STATUS_INITIALIZED* - stog inicijaliziran, ali nije omogućen
	- *ESP_BLUEDROID_STATUS_ENABLED* - stog inicijaliziran i omogućen

#### GAP funkcionalnost

Kako bi upravljali [GAP funkcionalnostima](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/bluetooth/esp_gap_ble.html#api-reference), potrebno je uključiti zaglavlje ```esp_gap_ble_api.h```. GAP funkcionalnostima se može samo upravljati kada je Bluetooth postavljen u *ESP_BT_MODE_BLE* ili *ESP_BT_MODE_BTDM* načinu rada.

##### Registriranje callback (handler) funkcije za GAP događaj
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
				- *ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT* - kada se BLE parametri za konekciju ažuriraju
				- *ESP_GAP_BLE_SET_PKT_LENGTH_COMPLETE_EVT* - kada se postavi veličina paketa
				- *ESP_GAP_BLE_GET_DEV_NAME_COMPLETE_EVT* - kada se ime BLE uređaja dohvati

		- *param*
			- pokazivač na uniju koja se proslijedi funkciji kada se dogodi neki GAP događaj

Funkcija vraća *ESP_OK* kada ako je *callback* uspješno registriran.

##### Postavljanje lokalnog imena BLE uređaju
```
esp_err_t esp_ble_gap_set_device_name(const char *name)
```

Parametar ove funkcije je:

- *name*
	- pokazivač na niz znakova koje predstavlja ime BLE uređaja koje će se vidjeti pri oglašavanju

Funkcija vraća *ESP_OK* kada ako je ime uspješno postavljeno.

##### Dohvaćanje lokalnog imena BLE uređaju
```
esp_err_t esp_ble_gap_get_device_name(void)
```

Funkcija vraća *ESP_OK* kada ako je ime dohvaćeno postavljeno. Ime će biti dostupno u *callback* funkciji kao parametar *param* za događaj *ESP_GAP_BLE_GET_DEV_NAME_COMPLETE_EVT*.

##### Postavljanje konfiguracije podataka oglašavanja i konfiguracije odgovora na skeniranje
```
esp_err_t esp_ble_gap_config_adv_data(esp_ble_adv_data_t *adv_data)
```

Parametar ove funkcije je:

- *adv_data*
	- pokazivač na korisničku definiranu konfiguraciju podataka oglašavanja ili odgovora na skeniranje

Funkcija vraća *ESP_OK* kada ako je konfiguracija podataka oglašavanja uspješno postavljena. Član *set_scan_rsp* definira je li se struktura koristi kao struktura podataka za oglašavanje ili struktura podataka za odgovor na skeniranje.

BLE središnji uređaj će detektirati strukture oglašavanja BLE periferija skeniranjem. Ako BLE središnji uređaj želi više informacija o konkretnoj BLE periferiji, poslat će zahtjev točno toj BLE periferiji, a ta periferija će odgovoriti strukturom podataka za odgovor na skeniranje.

##### Struktura podataka za konfiguriranje podataka oglašavanja i odgovora na skeniranje

Struktura *esp_ble_adv_data_t* sastoji se od sljedećih članova:

- *set_scan_rsp*
	- podatak tipa *bool* koji govori je li ova struktura namijenjena kao struktura podataka za oglašavanje ili za odgovor na skeniranje
- *include_name*
	- podatak tipa *bool* koji odlučuje hoće li se uključiti i ime BLE uređaja u oglašavanju
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
		- *ESP_BLE_ADV_FLAG_GEN_DISC* - BLE uređaj je vidljiv svim uređajima
		- *ESP_BLE_ADV_FLAG_LTD_DISC* - BLE uređaj je vidljiv na kratko vrijeme
		- *ESP_BLE_ADV_FLAG_BREDR_NOT_SPT* - BLE uređaj ne podupire dvostruki Bluetooth Classic + BLE način rada

##### Postavljanje parametara konekcije
```
esp_err_t esp_ble_gap_update_conn_params(esp_ble_conn_update_params_t *params)
```

Parametar ove funkcije je:

- *params*
	- pokazivač na definiranu konfiguraciju parametara konekcije

Funkcija vraća *ESP_OK* kada ako je konfiguracija parametara konekcije uspješno postavljena.

##### Struktura podataka za konfiguriranje parametara konekcije

Struktura *esp_ble_conn_update_params_t* sastoji se od sljedećih članova:

- *bda*
	- podatak tipa *esp_bd_addr_t* koji definira adresu BLE uređaja tijekom konekcije
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

##### Započinjanje oglašavanja
```
esp_err_t esp_ble_gap_start_advertising(esp_ble_adv_params_t *adv_params)
```

Parametar ove funkcije je:

- *adv_params*
	- pokazivač na definiranu konfiguraciju parametara oglašavanja, definira način rada BLE-a

Funkcija vraća *ESP_OK* kada ako je konfiguracija parametara oglašavanja uspješno postavljena i započeto je oglašavanje.

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
		- *ADV_TYPE_DIRECT_IND_HIGH* - oglašavanje ciljnom BLE središnjem uređaju
		- *ADV_TYPE_SCAN_IND* - oglašavanje s namjerom skeniranja bez mogućnosti konekcija
		- *ADV_TYPE_NONCONN_IND* - oglašavanje s namjerom skeniranja gdje se ne želi uspostavljati konekcija
		- *ADV_TYPE_DIRECT_IND_LOW* - oglašavanje ciljnom BLE središnjem uređaju, ali uz manju učestalost
- *own_addr_type*
	- podatak tipa *esp_ble_addr_type_t* koji definira tip adrese BLE uređaja, enumeratori:
		- *BLE_ADDR_TYPE_PUBLIC* - statička, javna adresa
		- *BLE_ADDR_TYPE_RANDOM* - dinamička, slučajno generirana adresa
		- *BLE_ADDR_TYPE_RPA_PUBLIC* - statička, RPA (*eng. Resolvable Private Address*) javna adresa
		- *BLE_ADDR_TYPE_RPA_RANDOM* - dinamička, RPA (*eng. Resolvable Private Address*) slučajno generirana adresa
- *peer_addr*
	- podatak tipa *esp_bd_addr_t* koji definira jedinstvenu adresu BLE periferije na kojeg se trenutni BLE centralni uređaj želi spojiti
	- svakom BLE uređaju je ugrađena BLE adresa
- *peer_addr_type*
	- podatak tipa *esp_ble_addr_type_t* koji definira tip adrese *peer* BLE uređaja, enumeratori već navedeni pod podatkom *own_addr_type*
- *channel_map*
	- podatak tipa *esp_ble_adv_channel_t* koji definira kanale kojima će se BLE periferija oglašavati, enumeratori:
		- *ADV_CHNL_37* - kanal 37
		- *ADV_CHNL_38* - kanal 38
		- *ADV_CHNL_39* - kanal 39
		- *ADV_CHNL_ALL* - sva tri kanala
- *adv_filter_policy*
	- podatak tipa *esp_ble_adv_filter_t* koji definira koji BLE centralni uređaj smije skenirati i povezati se na trenutni BLE uređaj, enumeratori:
		- *ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY* - bilo koji uređaj
		- *ADV_FILTER_ALLOW_SCAN_WLST_CON_ANY* - samo uređaji s liste odobrenih uređaja (*eng. whitelist*) mogu skenirati i otkriti trenutni uređaj, ali bilo koji uređaji se mogu povezati na trenutni uređaj
		- *ADV_FILTER_ALLOW_SCAN_ANY_CON_WLST* - bilo koji uređaji mogu skenirati i otkriti trenutni uređaj, ali samo uređaji s liste odobrenih uređaja se mogu povezati na trenutni uređaj
		- *ADV_FILTER_ALLOW_SCAN_WLST_CON_WLST* - samo uređaji s liste odobrenih uređaja mogu skenirati, otkriti i povezati se na trenutni uređaj

#### Zaustavljanje oglašavanja
```
esp_err_t esp_ble_gap_stop_advertising(void)
```

Funkcija vraća *ESP_OK* kada ako je zaustavljeno oglašavanje.

#### GATT funkcionalnost

Kako bi upravljali [GATT funkcionalnostima](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/bluetooth/esp_gatts.html#api-reference), potrebno je uključiti zaglavlje ```esp_gatts_api.h```. GAP funkcionalnostima se može samo upravljati kada je Bluetooth postavljen u *ESP_BT_MODE_BLE* ili *ESP_BT_MODE_BTDM* načinu rada.

##### Registriranje callback (handler) funkcije za GATT događaj
```
esp_err_t esp_ble_gatts_register_callback(gatts_event_handler)
```

Parametar ove funkcije je:

- *callback*
	- funkcija oblika *gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)* čiji su parametri:
		- *event*
			- GATT događaj na kojeg će se pozvati *callback*, enumerator koji može primjerice poprimiti vrijednosti:
				- *ESP_GATTS_REG_EVT* - nakon poziva funkcije ```esp_ble_gatts_app_register(...)```
				- *ESP_GATTS_UNREG_EVT* - nakon poziva funkcije ```esp_ble_gatts_app_unregister(...)```
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

##### Registracija profila aplikacije
```
esp_err_t esp_ble_gatts_app_register(uint16_t app_id)
```

Parametar ove funkcije je:

- *app_id*
	- podatak koji označava identifikator profila

Funkcija vraća *ESP_OK* kada je profil aplikacije uspješno registriran. Pozivanjem ove funkcije dogodit će se događaj *ESP_GATTS_REG_EVT*.

##### Odjava profila aplikacije
```
esp_err_t esp_ble_gatts_app_unregister(esp_gatt_if_t gatts_if)
```

Parametar ove funkcije je:

- *gatts_if*
	- GATT sučelje koje BLE stog dodjeljuje prilikom registracije

Funkcija vraća *ESP_OK* kada je profil aplikacije uspješno odjavljen. Pozivanjem ove funkcije dogodit će se događaj *ESP_GATTS_UNREG_EVT*.

##### Stvaranje servisa
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

Funkcija vraća *ESP_OK* kada je servis uspješno stvoren. Pozivanjem ove funkcije dogodit će se događaj *ESP_GATTS_CREATE_EVT*.

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

##### Brisanje servisa
```
esp_err_t esp_ble_gatts_delete_service(uint16_t service_handle)
```

Parametar ove funkcije je:

- *service_handle*
	- handle na servis

Funkcija vraća *ESP_OK* kada je servis uspješno odjavljen. Pozivanjem ove funkcije dogodit će se događaj *ESP_GATTS_DELETE_EVT*.

##### Započinjanje rada servisa
```
esp_err_t esp_ble_gatts_start_service(uint16_t service_handle)
```

Parametar ove funkcije je:

- *service_handle*
	- handle na servis

Funkcija vraća *ESP_OK* kada je rad servisa uspješno započet. Pozivanjem ove funkcije dogodit će se događaj *ESP_GATTS_START_EVT*.

##### Zaustavljanje rada servisa
```
esp_err_t esp_ble_gatts_stop_service(uint16_t service_handle)
```

Parametar ove funkcije je:

- *service_handle*
	- handle na servis

Funkcija vraća *ESP_OK* kada je rad servisa uspješno započet. Pozivanjem ove funkcije dogodit će se događaj *ESP_GATTS_STOP_EVT*.

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
		- *attr_len* - trenutna duljina vrijednosti karakteristike
		- *attr_value* - pokazivač na početnu vrijednost karakteristike
- *control*
	- dodatne kontrole pristupa, najčešće NULL

Funkcija vraća *ESP_OK* kada je karakteristika uspješno dodana. Pozivanjem ove funkcije dogodit će se događaj *ESP_GATTS_ADD_CHAR_EVT*.

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
		- *attr_len* - trenutna duljina vrijednosti karakteristike
		- *attr_value* - pokazivač na početnu vrijednost karakteristike
- *control*
	- dodatne kontrole pristupa, najčešće NULL

Funkcija vraća *ESP_OK* kada je opisnik karakteristike uspješno dodan. Pozivanjem ove funkcije dogodit će se događaj *ESP_GATTS_ADD_DESCR_EVT*.

##### Slanje odgovora na zahtjev

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
				- *attr_len* - trenutna duljina vrijednosti karakteristike
				- *attr_value* - vrijednost karakteristike
		- *handle*
			- handle karakteristike

Funkcija vraća *ESP_OK* kada je karakteristika uspješno dodana. Pozivanjem ove funkcije dogodit će se događaj *ESP_GATTS_RESPONSE_EVT*.



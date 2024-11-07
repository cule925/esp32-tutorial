# WI-FI

[Wi-Fi](https://en.wikipedia.org/wiki/Wi-Fi#Versions_and_generations) je skup protokola temeljen na [IEEE 802.11 standardima](https://www.ieee802.org/11/) razvijen od [IEEE organizacije](https://www.ieee.org/) koji omogućuje bežičnu povezanost uz pomoć radio valova ne mrežu najčešće internet. Wi-Fi je najčešći protokol korišten za spajanje na bežičnu mrežu. Kod ESP32 serije mikroupravljača, najbitnija značajka upravo i jest Wi-Fi jer omogućuje mikroupravljačima mrežnu komunikaciju.

[Wi-Fi Alliance](https://www.google.com/search?client=firefox-b-d&q=wifi+alliance) je neprofitna organizacija koja igra ključnu ulogu u promociji i standardizaciji Wi-Fi tehnologije. Njena glavna funkcija je osigurati interoperabilnost između uređaja različitih proizvođača kako bi se osigurao besprijekoran rad Wi-Fi uređaja u različitim okruženjima. Popularizirala naziv *Wi-Fi* za bežičnu mrežnu tehnologiju temeljenu na IEEE 802.11 standardima. Također bavi se promoviranjem Wi-Fi tehnologije širom svijeta i obrazovanjem korisnika o prednostima bežičnih mreža.

Wi-Fi uređaji koji su osigurali interoperabilnost po standardima Wi-Fi Alliance organizacije mogu dobiti certifikat koji potvrđuje standard.

Za identifikaciju WiFi uređaja, slično kao i kod Ethernet protokola, koristi se 48 bitna MAC adresa. Wi-Fi je zaslužan za protokole na fizičkom i podatkovnom MAC podsloju OSI modela za razliku od Ethernet protokola je protokol za fizički ali i cijeli podatkovni sloj odnosno MAC + LLC. LLC protokol koji se koristi u slučaju korištenja Wi-Fi protokola na nižim slojevima je [IEEE 802.2](https://en.wikipedia.org/wiki/IEEE_802.2). Wi-Fi okviri koji se prenose su vrlo slični Ethernet okvirima, uz još dodatnih par polja.

## Verzije Wi-Fi protokola

Wi-Fi uređaji najčešće podupiru više verzija protokola kako bi osigurala kompatibilnost sa starijim uređajima. Postoje mnoge verzije, ali najbitnije verzije su:

- **Wi-Fi 4 (802.11n)**
	- frekvencijski pojasi: 2.4 GHz i 5 GHz
	- maksimalna brzina prijenosa: 600 Mb/s
	- godina usvajanja: 2009
- **Wi-Fi 5 (802.11ac)**
	- frekvencijski pojasi: 5 GHz
	- maksimalna brzina prijenosa: 6933 Mb/s
	- godina usvajanja: 2013
- **Wi-Fi (802.11ax)**
	- frekvencijski pojasi: 2.4 GHz i 5 GHz
	- maksimalna brzina prijenosa: 9608 Mb/s
	- godina usvajanja: 2019

Umreženi Wi-Fi uređaji komuniciraju okvirima *half duplex* načinom rada što znači da se paketi ne mogu u isto vrijeme razmjenjivati. Okviri se moduliraju i demoduliraju između Wi-Fi odašiljača s jedne strane i Wi-Fi prijemnika s druge strane. Wi-Fi odašiljač i prijemnik komuniciraju na istom kanalu. Može se dogoditi da se kanali odašiljača i prijemnika iz drugih mreža preklapaju s trenutnom što znači da će nastati korupcija podataka. Kako bi se spriječile ovakve kolizije koristi se [CSMA/CA protokol](https://en.wikipedia.org/wiki/Carrier-sense_multiple_access_with_collision_avoidance).

## Načini rada Wi-Fi uređaja

Postoji nekoliko načina rada Wi-Fi uređaja, neki od njih su:

- Station mode (STATION)
	- uređaj se ponaša kao klijent koji pristupa uređaju u načinu rada *Access point*
	- primjer: osobna računala, laptopi i mobiteli koji su spojeni na Wi-Fi mrežu kod kuće
- Access point (AP)
	- uređaj se ponaša kao pristupna točka u mreži na koju se drugi uređaji (najčešće uređaj u načinu rada *Station*) mogu spojiti i priključiti se mreži
	- najčešće je sam spojen na mrežu žičano, primjerice Ethernet ili nekim drugim medijem
	- poznat pod imenom *hotspot*
	- primjer: pristupna točka u velikim zgradama, mobilna pristupna točka, usmjernici
	- ima sličnu ulogu kao mrežni preklopnik (*eng. network switch*)
- Ad-Hoc
	- ostvaruje *peer-to-peer* mrežu
	- stvara lokalnu mrežu bez *Access pointa*
- Repeater mode
	- ponavlja Wi-Fi signal kako bi povećao pokrivenost
	- primjer: Wi-Fi ponavljači
- Bridge mode
	- povezuje dvije mreže radi međusobne komunikacije
	- primjer: usmjernik (povezuje LAN i internet)
- Direct mode
	- slično *Ad-Hoc* načinu rada, samo jednostavniji
	- primjer: povezivanje dva mobitela za prijenos podataka

## ESP32 i Wi-Fi

ESP32 može raditi u načinu rada *Station*, načinu rada *Access Point* i u oba načina rada odjednom. Koristeći pločicu [*ESP32-DevKitM-1*](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp32-devkitm-1/user_guide.html#getting-started) može se saznati da on na sebi ima modul [ESP32-MINI-1](https://www.puntoflotante.net/Wifi-modes-of-operation.pdf) te da su njegove Wi-Fi karakteristike sljedeće:

- podržava Wi-Fi 4 standard (802.11n) te je unatrag kompatibilan sa standardima Wi-Fi 3 (802.11g) i Wi-Fi (802.11b) 1 standardima
- maksimalna brzina prijenosa za Wi-Fi 4 je 150 Mbps
- sadrži antenu koja se nalazi na modulu koja je povezana sa SoC-em

Zaglavlje koje je potrebno uključiti za rad s [Wi-Fi operacijama (eng. GPIO API)](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/network/esp_wifi.html#wi-fi) je ```esp_wifi.h```.

### Flash memorija za trajnu pohranu podataka

Kako bi inicijalizirali Wi-Fi stog potrebno je inicijalizirati [NVS (*eng. Non-Volatile Storage*) particiju](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/storage/nvs_flash.html) za spremanje Wi-Fi podataka u Flash memoriju, primjerice ime spojene mreže (*SSID*) i zaporka mreže u slučaju ponovnog pokretanja ESP32 mikroupravljača.

Podatci se u NVS particiju spremaju u obliku ključa i vrijednosti. Maksimalna veličina ključa je 15 ASCII znakova. Vrijednosti mogu biti cijeli brojevi, nizovi znakova i niz binarnih podataka (*blob*). Ključevi moraju biti jedinstveni iako postoje imenski prostori kako bi se donekle riješio problem s sukobljenim imenima.

Zaglavlje koje je potrebno uključiti za rad s [NVS-om](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/storage/nvs_flash.html#api-reference) je ```nvs_flash.h```.

#### Naredba za inicijalizaciju uobičajene NVS particije
```
esp_err_t nvs_flash_init(void)
```

Funkcija vraća *ESP_OK* ako je inicijalizacija NVS-a uspjela. Funkcija inicijalizira opisnike u memoriji potrebne za pristup NVM particiji.

#### Brisanje svih ključeva i vrijednosti uobičajene NVS particije
```
esp_err_t nvs_flash_erase(void)
```

Funkcija vraća *ESP_OK* ako je brisanje ključeva i vrijednosti uspjelo.

#### Naredba za deinicijalizaciju uobičajene NVS particije
```
esp_err_t nvs_flash_deinit(void)
```

Funkcija vraća *ESP_OK* ako je deinicijalizacija NVS-a uspjela. Funkcija deinicijalizira opisnike u memoriji potrebne za pristup NVM particiji.

### Petlja događaja za registriranje Wi-Fi događaja

[Petlja događaja (eng. event loop)](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/system/esp_event.html) omogućuje obradu asinkronih događaja. Primjerice, u slučaju nekog događaja pozvat će se *handler* funkcija odnosno funkcija za obradu događaja. Postoji uobičajena sustavska petlja događaja koja prati događaje u sustavu i korisnički definirana petlja događaja. Sustavska petlja događaja prati sustavske događaje, primjerice Wi-Fi događaje. Korisnici mogu registrirati više *handler* funkcija za obradu istog događaja u obje vrste petlji. Međutim, vrijedi pravilo da se *handler* funkcija ne smije odjaviti s događaja dok je petlja događaja aktivna osim ako *handler* sam sebe odjavljuje.

Zaglavlje koje je potrebno uključiti za rad s [petljom događaja](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/system/esp_event.html#api-reference) je ```nvs_flash.h```.

Za Wi-Fi je bitna sustavska petlja događaja pa će se priložene funkcije odnositi samo na nju.

#### Stvaranje sustavske petlje događaja
```
esp_err_t esp_event_loop_create_default(void)
```

Funkcija vraća *ESP_OK* ako je stvaranje petlje uspjelo.

#### Brisanje sustavske petlje događaja
```
esp_err_t esp_event_loop_delete_default(void)
```

Funkcija vraća *ESP_OK* ako je brisanje petlje uspjelo.

#### Registracija handlera u sustavskoj petlji događaja
```
esp_err_t esp_event_handler_instance_register(esp_event_base_t event_base, int32_t event_id, esp_event_handler_t event_handler, void *event_handler_arg, esp_event_handler_instance_t *instance)
```

Parametri ove funkcije su:

- *event_base*
	- identifikator općenitog događaja koji se registrira, kad se dogodi ovaj događaj pozvat će se *handler* funkcija
	- neki od takvih mogu biti:
		- Wi-Fi događaj, deklariran u zaglavlju ```esp_wifi_types.h```, makro koji označava ovaj događaj je *WIFI_EVENT*
		- IP događaj, deklariran u zaglavlju ```esp_netif_types.h```, makro koji označava ovaj događaj je *IP_EVENT*
		- makro koji označava bilo koji općeniti događaj ```ESP_EVENT_ANY_BASE```

- *event_id*
	- identifikator specifičnog događaja navedenog općenitog događaja koji se registrira
	- neki od takvih mogu biti:
		- Wi-Fi specifični događaji, definirani u zaglavlju ```esp_wifi_types.h```, neki od makroa mogu biti *WIFI_EVENT_WIFI_READY*, *WIFI_EVENT_WIFI_SCAN_DONE*, *WIFI_EVENT_STA_START*, *WIFI_EVENT_STA_STOP*, *WIFI_EVENT_STA_CONNECTED*, *WIFI_EVENT_STA_DISCONNECTED*, ...
		- IP specifični događaji, definirani u zaglavlju ```esp_netif_types.h```, neki od makroa mogu biti *IP_EVENT_STA_GOT_IP* (dobivena IP adresa od pristupne točke), *IP_EVENT_STA_LOST_IP* (izgubljena IP adresa), ...
		- makro koji označava bilo koji specifični događaj ```ESP_EVENT_ANY_ID```

- *event_handler*
	- handler funkcija koja se poziva pri općenitom ili specifičnom događaju koji se registrira
	- funkcija mora imati sljedeću deklaraciju: ```static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)```
		- parametri su sljedeći:
			- *arg* - proslijeđeni parametar koji se proslijedio pri registraciji
			- *event_base* - identifikator općenitog događaja koji se dogodio
			- *event_id* - identifikator specifičnog događaja koji se dogodio
			- *event_data* - struktura podataka koja je proslijeđena u trenutku događaja koji se dogodio
				- **strukture podataka koje se proslijede s obzirom na tip identifikatora općenitog događaja i tip specifičnog događaja mogu se naći [ovdje](https://docs.espressif.com/projects/esp-idf/en/v5.0/esp32/api-guides/event-handling.html#event-ids-and-corresponding-data-structures)**

- *event_handler_arg*
	- pokazivač na argument koji će biti proslijeđen kada se dogodi općeniti ili specifični događaj koji se registrira

- *instance*
	- pokazivač na argument koji pokazuje na specifičnu registriranu instancu *handler* instance koju treba registrirati
	- koristi se ako se želi odjaviti specifična registrirana instanca, u suprotnom je dovoljno proslijediti *NULL*

Funkcija vraća *ESP_OK* ako je registracija uspjela.

#### Odjava handlera iz sustavskoj petlji događaja
```
esp_err_t esp_event_handler_instance_unregister(esp_event_base_t event_base, int32_t event_id, esp_event_handler_instance_t instance)
```

Parametri ove funkcije su:

- *event_base*
	- identifikator općenitog događaja koji se odjavljuje
- *event_id*
	- identifikator specifičnog događaja navedenog općenitog događaja koji se odjavljuje
- *instance*
	- pokazivač na argument koji pokazuje na specifičnu registriranu instancu *handler* instance koju treba odjaviti

Funkcija vraća *ESP_OK* ako je odjava uspjela.

### FreeRTOS grupa događaja

FreeRTOS grupa događaja je (*eng. FreeRTOS Event Group*) je mehanizam za sinkronizaciju i komunikaciju između različitih zadataka u sustavu. Koristi bit masku kao način pohrane stanja događaja. 

Zaglavlje koje je potrebno uključiti za rad s [grupama događaja](https://docs.espressif.com/projects/esp-idf/en/v5.0/esp32/api-reference/system/freertos.html#event-group-api) je ```freertos/event_groups.h```.

**Funkcije za rad se grupama događaja su atomarne.**

#### Stvaranje grupe događaja
```
EventGroupHandle_t xEventGroupCreate(void)
```

Funkcija vraća *handle* na grupu događaja ako je stvaranje uspjelo, inače vraća NULL. Dostupni broj bitova za događaje u grupi može biti 8 ili 24, ovisno je li postavljeno *configUSE_16_BIT_TICKS* u 1 ili u 0 u *FreeRTOSConfig.h* zaglavlju.

#### Brisanje grupe događaja
```
void vEventGroupDelete(EventGroupHandle_t xEventGroup)
```

Parametar ove funkcije je:

- *xEventGroup*
	- *handle* na grupu događaja

Funkcija ne vraća ništa.

#### Čekaj na bitove u grupi događaja
```
EventBits_t xEventGroupWaitBits(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToWaitFor, const BaseType_t xClearOnExit, const BaseType_t xWaitForAllBits, TickType_t xTicksToWait)
```

Parametri ove funkcije su:

- *xEventGroup*
	- *handle* na grupu događaja
- *uxBitsToWaitFor*
	- maska koja govori koje bitove grupe događaja treba pričekati da se postave u 1
- *xClearOnExit*
	- ako se pošalje *pdTRUE* onda se svi bitovi grupe događaja koji su postavljeni u 1 postavljaju u 0, a ako se pošalje *pdFALSE* stanje ostaje bitova grupe događaja ostaje kako jest
- *waitForAllBits*
	- ako se pošalje *pdTRUE* čeka se dok se svi bitovi grupe događaja definirani maskom *uxBitsToWaitFor* ne postave u 1, a ako se pošalje *pdFALSE* čeka se dok se barem jedan bit grupe događaja definirani maskom *uxBitsToWaitFor* ne postavi u 1
- *xTicksToWait*
	- vrijeme čekanja dok se ne postave bitovi događaja
	- ako je 0 onda je funkcija neblokirajuća, zadatak nastavlja se radom istog trena
	- ako je *portMAX_DELAY* onda je funkcija blokirajuća, zadatak čeka sve dok se ne postave svi ili barem jedan od maskiranih bitova

Funkcija vraća vrijednost bitova grupe događaja od trenutka kad su bili postavljeni i funkcija se odblokirala ili vrijednost bitova grupe događaja u trenutku isteka *xTicksToWait*.

#### Čišćenje bitova u grupi događaja
```
EventBits_t xEventGroupClearBits(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToClear)
```

Parametri ove funkcije su:

- *xEventGroup*
	- *handle* na grupu događaja
- *uxBitsToClear*
	- maska koja govori koje bitove grupe događaja treba postaviti u 0

Funkcija vraća vrijednost bitova grupe događaja prije nego se izvršila postavljanje bitova grupe događaja definiranih maskom u 0.

#### Postavljanje bitova u grupi događaja
```
EventBits_t xEventGroupSetBits(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet)
```

Parametri ove funkcije su:

- *xEventGroup*
	- *handle* na grupu događaja
- *uxBitsToSet*
	- maska koja govori koje bitove grupe događaja treba postaviti u 1

Funkcija vraća trenutnu vrijednost bitova grupe događaja definiranih maskom. Razlog zašto nije isto kao i kod ```xEventGroupClearBits(...)``` funkcije je zato što postavljanjem bitova se mogu odblokirati zadatci koji čekaju zbog funkcije ```xEventGroupWaitBits(...)``` koja možda ima postavljenu vrijednost *xClearOnExit* na *pdTRUE* koja će očistiti sve bitove.

### TCP/IP stog

Kako bi aplikacija na ESP32 mikroupravljaču mogla komunicirati s drugim aplikacijama na drugim uređajima potrebno je koristiti [apstrakciju TCP/IP stoga](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/network/esp_netif.html). Ova apstrakcija upravlja mrežnim sučeljima na ESP32 mikroupravljaču. ESP32 zadano koristi *lwIP (eng. lightweight IP)* TCP/IP stog otvorenog koda, ali moguće je dodati i vlastite implementacije.

Neke funkcije TCP/IP stoga se zovu iz sloja aplikacije dok se neke funkcije zovu iz mrežnog sloja.

Zaglavlje koje je potrebno uključiti za rad s [TCP/IP stogom](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/network/esp_netif.html#api-reference) je ```esp_netif.h```.

Zaglavlje nudi sučelja na funkcije koje:

- sprovode inicijalizaciju TCP/IP stoga:
	- inicijaliziraju ulazno-izlazni upravljački program (primjerice Wi-FI upravljački program)
	- stvaraju instancu mrežnog sučelja
	- spajaju navedeni upravljački program na navedenu instancu mrežnog sučelja
	- konfiguriraju *handler*
- interakciju s navedenom instancom mrežnog sučelja
	- postavlja i dohvaća TCP/IP parametre (primjerice trenutnu IP adresu ili trenutnu MAC adresu)
	- pokretanje i zaustavljanje rada
	- prihvat događaja i pokretanje odgovarajuće *handler* funkcije

Funkcija za rad s TCP/IP stogom ima mnogo. U nastavku su neke od bitnijih funkcija potrebne za postavljanje mrežnog sučelja.

#### Inicijalizacija i deinicijalizacija TCP/IP stoga

Zaglavlje koje je potrebno za  inicijalizaciju i deinicijalizaciju [TCP/IP stoga](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/network/esp_netif.html#api-reference) je ```esp_netif.h```.

##### Inicijalizacija TCP/IP stoga
```
esp_err_t esp_netif_init(void)
```

Funkcija vraća *ESP_OK* ako je inicijalizacija uspjela.

##### Deinicijalizacija TCP/IP stoga
```
esp_err_t esp_netif_deinit(void)
```

Funkcija vraća *ESP_ERR_INVALID_STATE* ako TCP/IP uopće nije inicijaliziran. U suprotnom vraća *ESP_ERR_NOT_SUPPORTED* jer **deinicijalizacija još nije implementirana**.

#### Stvaranje i brisanje instance mrežnog sučelja

Zaglavlje koje je potrebno uključiti za [stvaranje i brisanje instance mrežnog sučelja (*eng. Wi-Fi Default API*)](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/network/esp_netif.html#wi-fi-default-api-reference) je ```esp_wifi_default.h```.

##### Stvaranje mrežnog sučelja za korištenje u Wi-Fi STATION načinu rada
```
esp_netif_t *esp_netif_create_default_wifi_sta(void)
```

Funkcija vraća pokazivač na objekt koji predstavlja instancu mrežnog sučelja, povezuje WiFi upravljački program sa stvorenim mrežnim sučeljem te registrira WI-FI *handler* funkcije na sustavsku petlju događaja (koja mora biti stvorena prije poziva ove funkcije). Mrežno sučelje se dodaje u internu listu mrežnih sučelja.

##### Stvaranje mrežnog sučelja za korištenje u Wi-Fi AP načinu rada
```
esp_netif_t *esp_netif_create_default_wifi_ap(void)
```

Funkcija vraća pokazivač na objekt koji predstavlja instancu mrežnog sučelja, povezuje WiFi upravljački program sa stvorenim mrežnim sučeljem te registrira WI-FI *handler* funkcije na sustavsku petlju događaja (koja mora biti stvorena prije poziva ove funkcije). Mrežno sučelje se dodaje u internu listu mrežnih sučelja.

##### Brisanje stvorenog mrežnog sučelja
```
void esp_netif_destroy_default_wifi(void *esp_netif)
```

Parametar ove funkcije je:

- *esp_netif*
	- pokazivač na stvoreni objekt koji predstavlja instancu mrežnog sučelja
c
Funkcija ne vraća ništa. Mrežno sučelje se uklanja iz interne liste mrežnih sučelja.

#### Postavljanje i dohvat postavljene MAC adrese mrežnog sučelja

Zaglavlje koje je potrebno uključiti za postavljanje i pregled trenutne MAC adrese je ```esp_netif.h```.

##### Postavljanje MAC adrese na mrežnom sučelju
```
esp_err_t esp_netif_set_mac(esp_netif_t *esp_netif, uint8_t mac[])
```

Parametri ove funkcije su:

- *esp_netif*
	- pokazivač na stvoreni objekt koji predstavlja instancu mrežnog sučelja
- *mac*
	- pokazivač na polje pozitivnih cijelih brojeva koji predstavljaju novu 48 bitnu (6 bajtova) MAC adresu

Funkcija vraća *ESP_OK* ako je uspješno postavljena MAC adresa, *ESP_ERR_ESP_NETIF_IF_NOT_READY* ako se dogodila greška u vezi mrežnog sučelja i *ESP_ERR_NOT_SUPPORTED* ako MAC nije podržan na sučelju. Ako nije ručno postavljena MAC adresa onda se koristi tvorničko postavljena MAC adresa.

##### Dohvat MAC adrese s mrežnog sučelja
```
esp_err_t esp_netif_get_mac(esp_netif_t *esp_netif, uint8_t mac[])
```

Parametri ove funkcije su:

- *esp_netif*
	- pokazivač na stvoreni objekt koji predstavlja instancu mrežnog sučelja
- *mac*
	- pokazivač na polje pozitivnih cijelih brojeva u kojeg će se spremiti 48 bitnu (6 bajtova) MAC adresa

Funkcija vraća *ESP_OK* ako je uspješno dohvaćena MAC adresa, *ESP_ERR_ESP_NETIF_IF_NOT_READY* ako se dogodila greška u vezi mrežnog sučelja i *ESP_ERR_NOT_SUPPORTED* ako MAC nije podržan na sučelju.

#### Postavljanje i dohvaćanje ime mikroupravljača na mreži

Zaglavlje koje je potrebno uključiti za postavljanje i pregled trenutnog imena ESP32 mikroupravljača je ```esp_netif.h```. Inače, zadano ime mikroupravljača na mreži je zadano varijablom *CONFIG_LWIP_LOCAL_HOSTNAME* u *sdkconfig* datoteci (mijenja se s *menuconfig*).

##### Postavljanje imena mikroupravljača na mreži
```
esp_err_t esp_netif_set_hostname(esp_netif_t *esp_netif, const char *hostname)
```

Parametri ove funkcije su:

- *esp_netif*
	- pokazivač na stvoreni objekt koji predstavlja instancu mrežnog sučelja
- *hostname*
	- pokazivač na polje znakova koje predstavlja korisničko ime, maksimalna veličina je 32 znaka (odnosno 32 bajta)

Funkcija vraća *ESP_OK* ako je uspješno postavljeno ime, *ESP_ERR_ESP_NETIF_IF_NOT_READY* ako se dogodila greška u vezi mrežnog sučelja i *ESP_ERR_ESP_NETIF_INVALID_PARAMS* ako je greška u poslanom parametru.

##### Dohvaćanje imena mikroupravljača na mreži
```
esp_err_t esp_netif_get_hostname(esp_netif_t *esp_netif, const char **hostname)
```

Parametri ove funkcije su:

- *esp_netif*
	- pokazivač na stvoreni objekt koji predstavlja instancu mrežnog sučelja
- *hostname*
	- pokazivač na pokazivač na polje znakova koje predstavlja korisničko ime, NULL ako korisničko ime nije postavljeno

Funkcija vraća *ESP_OK* ako je uspješno dohvaćeno ime, *ESP_ERR_ESP_NETIF_IF_NOT_READY* ako se dogodila greška u vezi mrežnog sučelja i *ESP_ERR_ESP_NETIF_INVALID_PARAMS* ako je greška u poslanom parametru.

#### Postavljanje i dohvat postavljenog zadanog mrežnog sučelja koje se koristi pri komunikaciji

U slučaju da postoji više mrežnih sučelja, potrebno je postaviti zadano mrežno sučelje u listi internih mrežnih sučelja koje se koristi pri komunikaciji. U slučaju jednog mrežnog sučelja ovo nije potrebno. Zaglavlje koje je potrebno uključiti za postavljanje i pregled zadanog mrežnog sučelja ESP32 mikroupravljača je ```esp_netif.h```.

##### Postavljanje zadanog mrežnog sučelja koje se koristi pri komunikaciji
```
esp_err_t esp_netif_set_default_netif(esp_netif_t *esp_netif)
```

Parametar ove funkcije je:

- *esp_netif*
	- pokazivač na stvoreni objekt koji predstavlja instancu mrežnog sučelja

Funkcija vraća *ESP_OK* ako je uspješno postavljeno zadano mrežno sučelje u listi internih mrežnih sučelja koje se koristi pri komunikaciji.

##### Dohvaćanje zadanog mrežnog sučelja koje se koristi pri komunikaciji
```
esp_err_t *esp_netif_set_default_netif(void)
```

Funkcija vraća pokazivač na objekt mrežnog sučelja koji predstavlja zadano mrežno sučelje u listi internih mrežnih sučelja koje se koristi pri komunikaciji.

#### Postavljanje i dohvaćanje statičke IP adrese mrežnog sučelja

**DHCP klijent ne smije biti aktivan tijekom kada se ručno postavlja IP adresa.** Zaglavlje koje je potrebno uključiti za postavljanje i dohvaćanje statičke IP adrese mrežnog sučelja ESP32 mikroupravljača je ```esp_netif.h```.

##### Postavljanje statičke IP adrese mrežnog sučelja
```
esp_err_t esp_netif_set_ip_info(esp_netif_t *esp_netif, const esp_netif_ip_info_t *ip_info)
```

Parametri ove funkcije su:

- *esp_netif*
	- pokazivač na stvoreni objekt koji predstavlja instancu mrežnog sučelja
- *ip_info*
	- pokazivač na strukturu koja sadrži informacije o IP postavkama

Funkcija vraća *ESP_OK* ako je uspješno postavljena IP adresa na mrežnom sučelju.

##### Dohvat statičke IP adrese mrežnog sučelja
```
esp_err_t esp_netif_get_ip_info(esp_netif_t *esp_netif, esp_netif_ip_info_t *ip_info)
```

Parametri ove funkcije su:

- *esp_netif*
	- pokazivač na stvoreni objekt koji predstavlja instancu mrežnog sučelja
- *ip_info*
	- pokazivač na strukturu u kojoj će se kopirati informacije o IP postavkama

Funkcija vraća *ESP_OK* ako je uspješno dohvaćena IP adresa na mrežnom sučelju.

##### Struktura koja predstavlja informacije o IP postavkama

Struktura *esp_netif_ip_info_t* sastoji se od sljedećih članova:

- *ip*
	- podatak tipa *esp_ip4_addr_t* koji predstavlja IP adresu mrežnog sučelja
- *netmask*
	- podatak tipa *esp_ip4_addr_t* koji predstavlja *subnet* masku mrežnog sučelja
- *gw*
	- podatak tipa *esp_ip4_addr_t* koji predstavlja *gateway* mrežnog sučelja

Struktura *esp_ip4_addr_t* sadrži član:

- *addr*
	- podatak tipa *uint32_t* koji predstavlja 32 bitnu IP adresu

###### Pisanje IP adrese u strukturu koja predstavlja IP adresu
```
void esp_netif_set_ip4_addr(esp_ip4_addr_t *addr, uint8_t a, uint8_t b, uint8_t c, uint8_t d)
```

Parametri ove funkcije su:

- *addr*
	- pokazivač na strukturu koja predstavlja IP adresu, član strukture *esp_netif_ip_info_t*
- *a*, *b*, *c*, *d*
	- okteti IP adrese (*a.b.c.d*)

Funkcija ne vraća ništa.

###### Pretvorba IP adrese iz strukture koja predstavlja IP adresu u niz znakova
```
char *esp_ip4addr_ntoa(const esp_ip4_addr_t *addr, char *buf, int buflen)
```

Parametri ove funkcije su:

- *addr*
	- pokazivač na strukturu koja predstavlja IP adresu odakle se čita IP adresa, član strukture *esp_netif_ip_info_t*
- *buf*
	- pokazivač na ciljnu lokaciju gdje će se spremiti rezultat pretvorbe koji predstavlja IP adresu kao niz znakova
- *buflen*
	- veličina ciljne lokacije

Funkcija vraća pokazivač na ciljnu lokaciju (isto što je i *buf*) ako je pretvorba bila uspješna ili NULL ako je spremnik bio premalen.

###### Pretvorba IP adrese iz niza znakova u strukture koja predstavlja IP adresu
```
esp_err_t esp_netif_str_to_ip4(const char *src, esp_ip4_addr_t *dst)
```

Parametri ove funkcije su:

- *src*
	- pokazivač na ciljnu lokaciju odakle će se čitati niz znakova koji predstavljaju IP adresu
- *dst*
	- pokazivač na strukturu koja predstavlja IP adresu gdje će se pisati rezultat pretvorbe, član strukture *esp_netif_ip_info_t*

Funkcija vraća *ESP_OK* ako je uspješno obavljena konverzija.

#### Pokretanje i zaustavljanje DHCP klijenta

Pokretanje i zaustavljanje DHCP klijenta moguća je samo kada je DHCP klijent omogućen na tom sučelju. Stvaranje mrežnog sučelja uz pomoć ```esp_netif_create_default_wifi_sta(void)``` (STATION) automatski omogućuje i pokreće DHCP klijent. Zaglavlje koje je potrebno uključiti za pokretanje i zaustavljanje DHCP klijenta mrežnog sučelja ESP32 mikroupravljača je ```esp_netif.h```.

##### Pokretanje DHCP klijenta
```
esp_err_t esp_netif_dhcpc_start(esp_netif_t *esp_netif)
```

Parametar ove funkcije je:

- *esp_netif*
	- pokazivač na stvoreni objekt koji predstavlja instancu mrežnog sučelja

Funkcija vraća *ESP_OK* ako je uspješno pokrenut DHCP klijent na mrežnom sučelju.

##### Zaustavljanje DHCP klijenta
```
esp_err_t esp_netif_dhcpc_stop(esp_netif_t *esp_netif)
```

Parametar ove funkcije je:

- *esp_netif*
	- pokazivač na stvoreni objekt koji predstavlja instancu mrežnog sučelja

Funkcija vraća *ESP_OK* ako je uspješno zaustavljen DHCP klijent na mrežnom sučelju.

#### Pokretanje i zaustavljanje DHCP poslužitelja

Pokretanje i zaustavljanje DHCP poslužitelja moguća je samo kada je DHCP poslužitelja omogućen na tom sučelju. Stvaranje mrežnog sučelja uz pomoć ```esp_netif_create_default_wifi_ap(void)``` (AP) automatski omogućuje i pokreće DHCP poslužitelj. Zaglavlje koje je potrebno uključiti za pokretanje i zaustavljanje DHCP poslužitelja mrežnog sučelja ESP32 mikroupravljača je ```esp_netif.h```.

##### Pokretanje DHCP klijenta
```
esp_err_t esp_netif_dhcps_start(esp_netif_t *esp_netif)
```

Parametar ove funkcije je:

- *esp_netif*
	- pokazivač na stvoreni objekt koji predstavlja instancu mrežnog sučelja

Funkcija vraća *ESP_OK* ako je uspješno pokrenut DHCP poslužitelj na mrežnom sučelju.

##### Zaustavljanje DHCP klijenta
```
esp_err_t esp_netif_dhcp_stop(esp_netif_t *esp_netif)
```

Parametar ove funkcije je:

- *esp_netif*
	- pokazivač na stvoreni objekt koji predstavlja instancu mrežnog sučelja

Funkcija vraća *ESP_OK* ako je uspješno zaustavljen DHCP poslužitelj na mrežnom sučelju.

#### Aktiviranje i deaktiviranje mrežnog sučelja

Uobičajeno, mrežno sučelje se aktivira nakon poziva funkcije ```esp_wifi_start()``` ili ```esp_eth_start()```, ali to se može učiniti i ručno. Zaglavlje koje je potrebno uključiti za aktivaciju i deaktivaciju mrežnog sučelja ESP32 mikroupravljača je ```esp_netif.h```.

##### Aktiviranje mrežnog sučelja
```
void esp_netif_action_start(void *esp_netif, esp_event_base_t base, int32_t event_id, void *data)
```

Parametri ove funkcije su:

- *esp_netif*
	- pokazivač na stvoreni objekt koji predstavlja instancu mrežnog sučelja
- *base*
	- identifikator općenitog događaja koji se registrira, kad se dogodi ovaj događaj aktivirat će se mrežno sučelje ako se poklapa sa specifičnim događajem
	- ako je NULL onda se mrežno sučelje odmah aktivira
- *event_id*
	- identifikator specifičnog događaja navedenog općenitog događaja
	- ako je identifikator općenitog događaja NULL, ovdje se može proslijediti bilo koji broj (najčešće 0) jer ionako neće imati efekta
- *data*
	- pokazivač na strukturu podataka koja se prosljeđuje *handleru*

Funkcija ne vraća ništa.

##### Deaktiviranje mrežnog sučelja
```
void esp_netif_action_stop(void *esp_netif, esp_event_base_t base, int32_t event_id, void *data)
```

Parametri ove funkcije su:

- *esp_netif*
	- pokazivač na stvoreni objekt koji predstavlja instancu mrežnog sučelja
- *base*
	- identifikator općenitog događaja koji se registrira, kad se dogodi ovaj događaj deaktivirat će se mrežno sučelje ako se poklapa sa specifičnim događajem
	- ako je NULL onda se mrežno sučelje odmah deaktivira
- *event_id*
	- identifikator specifičnog događaja navedenog općenitog događaja
	- ako je identifikator općenitog događaja NULL, ovdje se može proslijediti bilo koji broj (najčešće 0) jer ionako neće imati efekta
- *data*
	- pokazivač na strukturu podataka koja se prosljeđuje *handleru*

Funkcija ne vraća ništa.

##### Ispitivanje je li mrežno sučelje aktivirano
```
bool esp_netif_is_netif_up(esp_netif_t *esp_netif)
```

Parametar ove funkcije je:

- *esp_netif*
	- pokazivač na stvoreni objekt koji predstavlja instancu mrežnog sučelja

Funkcija vraća *true* ako je sučelje aktivirano i *false* ako nije aktivirano.

### Wi-Fi konfiguracija

ESP32 mikroupravljač se najčešće konfigurira sljedeća dva načina:

- STATION
	- spaja se na AP
- AP (ACCESS POINT)
	- na njega se spajaju STATION-i

Zaglavlje koje je potrebno uključiti za rad s [Wi-Fiom](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/network/esp_wifi.html#api-reference) je ```esp_wifi.h```.

Funkcija za rad s Wi-Fiom ima mnogo. U nastavku su neke od bitnijih funkcija potrebne za postavljanje mrežnog sučelja.

#### Inicijalizacija Wi-Fi stoga i alokacija resursa za Wi-Fi upravljački program
```
esp_err_t esp_wifi_init(const wifi_init_config_t *config)
```

Parametar ove funkcije je:

- *config*
	- pokazivač na strukturu koja predstavlja konfiguraciju Wi-Fi stoga
	- **najbolje inicijalizirati s WIFI_INIT_CONFIG_DEFAULT makroom**

Funkcija vraća *ESP_OK* ako je inicijalizacija i alokacija uspješno izvršena.

#### Deinicijalizacija Wi-Fi stoga i dealokacija resursa za Wi-Fi upravljački program
```
esp_err_t esp_wifi_deinit(void)
```

Funkcija vraća *ESP_OK* ako je deinicijalizacija i dealokacija uspješno izvršena.

#### Postavljanje načina rada Wi-Fia
```
esp_err_t esp_wifi_set_mode(wifi_mode_t mode)
```

Parametar ove funkcije je:

- *mode*
	- definira način rada Wi-Fia, neki načini rada definirani su makroima:
		- *WIFI_MODE_STA* - način rada STATION
		- *WIFI_MODE_AP* - način rada AP

Funkcija vraća *ESP_OK* ako je postavljanje uspješno izvršena.

#### Dohvat načina rada Wi-Fia
```
esp_err_t esp_wifi_get_mode(wifi_mode_t *mode)
```

Parametar ove funkcije je:

- *mode*
	- pokazivač koji definira način rada Wi-Fia

Funkcija vraća *ESP_OK* ako je dohvaćanje uspješno izvršena.

#### Postavljanje konfiguracije Wi-Fia
```
esp_err_t esp_wifi_set_config(wifi_interface_t interface, wifi_config_t *conf)
```

Parametri ove funkcije su:

- *interface*
	- definira na koji Wi-Fi uređaj na SoC-u će se konfiguracija odnositi, makroima:
		- *WIFI_IF_STA* - uređaj STATION
		- *WIFI_IF_AP* - uređaj AP
		- *WIFI_IF_NAN* - nedefinirano
- *conf*
	- pokazivač koji definira konfiguraciju Wi-Fi mreže

Funkcija vraća *ESP_OK* ako je postavljanje uspješno izvršena.

##### Unija koja predstavlja Wi-Fi konfiguraciju

Unija *wifi_config_t* sastoji se od sljedećih članova:

- *ap*
	- podatak tipa *wifi_ap_config_t* koji predstavlja konfiguraciju za AP uređaj
- *sta*
	- podatak tipa *wifi_sta_config_t* koji predstavlja konfiguraciju za STATION uređaj
- *nan*
	- podatak tipa *wifi_nan_config_t* koji predstavlja konfiguraciju za nedefinirani uređaj

Bitni članovi strukture *wifi_sta_config_t* su:

- *ssid*
	- polje tipa *uint8_t* koje sadrži ime mreže koju pristupna točka (AP) odašilje na koju se ESP32 (STATION) može pristupiti
- *password*
	- polje tipa *uint8_t* koje sadrži zaporku mreže koju pristupna točka (AP) odašilje na koju se ESP32 (STATION) može pristupiti
- *bssid*
	- polje tipa *uint8_t* koje sadrži MAC adresu pristupne (AP uređaj) točke mreže na koju se spaja ESP32
- *threshold.authmode*
	- enumerator tipa *wifi_auth_mode_t* koji definira najslabiji način autentifikacije na koju je ESP32 voljan pristati radi autentifikacije na mrežu
	- najčešće vrijednosti:
		- *WIFI_AUTH_OPEN* - ESP32 se pristaje priključivati otvorenim mrežama
		- *WIFI_AUTH_WEP* - ESP32 se pristaje priključivati na mreže koje imaju minimalno WEP autentifikaciju
		- *WIFI_AUTH_WPA_PSK* - ESP32 se pristaje priključivati na mreže koje imaju minimalno WPA PSK autentifikaciju
		- *WIFI_AUTH_WPA2_PSK* - ESP32 se pristaje priključivati na mreže koje imaju minimalno WPA2 PSK autentifikaciju
		- *WIFI_AUTH_WPA_WPA2_PSK* - ESP32 se pristaje priključivati na mreže koje imaju minimalno WPA WPA2 PSK autentifikaciju
		- *WIFI_AUTH_WPA3_PSK* - ESP32 se pristaje priključivati na mreže koje imaju minimalno WPA3 PSK autentifikaciju
		- *WIFI_AUTH_WPA2_WPA3_PSK* - ESP32 se pristaje priključivati na mreže koje imaju minimalno WPA2 WPA3 PSK autentifikaciju
- *sae_pwe_h2e*
	- enumerator tipa *wifi_sae_pwe_method_t* koji definira specifičan način autentifikacije SAE (*eng. Simultaneous Authentication of Equals*) koji se koristi kod sigurnosnog protokola WPA3
	- najčešće vrijednosti:
		- *WPA3_SAE_PWE_UNSPECIFIED* - nije specificirana metoda
		- *WPA3_SAE_PWE_HUNT_AND_PECK* - starija, složenija i sporija metoda
		- *WPA3_SAE_PWE_HASH_TO_ELEMENT* - novija, jednostavnija i brža metoda
		- *WPA3_SAE_PWE_BOTH* - obje metode
- *sae_pk_mode*
	- enumerator tipa *wifi_sae_pk_mode_t* koji specifičnost autentifikacije SAE protokolom
	- najčešće vrijednosti:
		- *WPA3_SAE_PK_MODE_AUTOMATIC* - automatsko postavljanje po potrebama mreže s kojom se ESP32 spaja
		- *WPA3_SAE_PK_MODE_ONLY* - sigurniji
		- *WPA3_SAE_PK_MODE_DISABLED*
- *sae_h2e_identifier*
	- polje tipa *uint8_t* koje sadrži SAE identifikator koji se koristi pri autentifikaciji SAE protokolom
	- kada se način autentifikacije *WPA3_SAE_PWE_HASH_TO_ELEMENT* (u članu *sae_pwe_h2e*), onda se koristi identifikator
	- u većini najbolje ostavit kako jest

Bitni članovi strukture *wifi_ap_config_t* su:

- *ssid*
	- polje tipa *uint8_t* koje sadrži ime mreže kojom klijenti (STATION) mogu pristupiti mreži koju će ESP32 (AP) odašiljati kao pristupnu točku
- *password*
	- polje tipa *uint8_t* koje sadrži zaporku mreže kojom klijenti (STATION) mogu pristupiti mreži koju će ESP32 (AP) odašiljati kao pristupnu točku
- *authmode*
	- enumerator tipa *wifi_auth_mode_t* koji definira obavezni način autentifikacije drugih klijenata (STATION) na ESP32 (AP)
	- najčešće vrijednosti:
		- *WIFI_AUTH_OPEN* - ESP32 se pristaje priključivati otvorenim mrežama
		- *WIFI_AUTH_WEP* - ESP32 se pristaje priključivati na mreže koje imaju minimalno WEP autentifikaciju
		- *WIFI_AUTH_WPA_PSK* - ESP32 se pristaje priključivati na mreže koje imaju minimalno WPA PSK autentifikaciju
		- *WIFI_AUTH_WPA2_PSK* - ESP32 se pristaje priključivati na mreže koje imaju minimalno WPA2 PSK autentifikaciju
		- *WIFI_AUTH_WPA_WPA2_PSK* - ESP32 se pristaje priključivati na mreže koje imaju minimalno WPA WPA2 PSK autentifikaciju
		- *WIFI_AUTH_WPA3_PSK* - ESP32 se pristaje priključivati na mreže koje imaju minimalno WPA3 PSK autentifikaciju
		- *WIFI_AUTH_WPA2_WPA3_PSK* - ESP32 se pristaje priključivati na mreže koje imaju minimalno WPA2 WPA3 PSK autentifikaciju
- *sae_pwe_h2e*
	- enumerator tipa *wifi_sae_pwe_method_t* koji definira specifičan način autentifikacije SAE (*eng. Simultaneous Authentication of Equals*) koji se koristi kod sigurnosnog protokola WPA3
	- najčešće vrijednosti:
		- *WPA3_SAE_PWE_UNSPECIFIED* - nije specificirana metoda
		- *WPA3_SAE_PWE_HUNT_AND_PECK* - starija, složenija i sporija metoda
		- *WPA3_SAE_PWE_HASH_TO_ELEMENT* - novija, jednostavnija i brža metoda
		- *WPA3_SAE_PWE_BOTH* - obje metode
- *max_connection*
	- varijabla tipa *uint8_t* koja definira maksimalni broj klijenata koji mogu biti spojeni na ESP32

#### Dohvaćanje konfiguracije Wi-Fia
```
esp_err_t esp_wifi_get_config(wifi_interface_t interface, wifi_config_t *conf)
```

Parametri ove funkcije su:

- *interface*
	- definira na koji Wi-Fi uređaj na SoC-u se konfiguracija odnosi, makroima:
		- *WIFI_IF_STA* - uređaj STATION
		- *WIFI_IF_AP* - uređaj AP
		- *WIFI_IF_NAN* - nedefinirano
- *conf*
	- pokazivač u kojem će biti trenutna konfiguracija Wi-Fi mreže

Funkcija vraća *ESP_OK* ako je dohvaćanje uspješno izvršena.

#### Započinjanje rada Wi-Fia prema trenutnim konfiguracijama
```
esp_err_t esp_wifi_start(void)
```

Funkcija vraća *ESP_OK* ako je rad Wi-Fia uspješno započet prema trenutnim konfiguracijama.

#### Zaustavljanje rada Wi-Fia
```
esp_err_t esp_wifi_stop(void)
```

Funkcija vraća *ESP_OK* ako je rad Wi-Fia uspješno zaustavljen.

#### Spajanje na Wi-Fi mrežu definiranoj u konfiguraciji
```
esp_err_t esp_wifi_connect(void)
```

Funkcija vraća *ESP_OK* ako je uređaj uspješno spojen na Wi-Fi mrežu definiranoj u konfiguraciji. Ova funkcija radi samo u *WIFI_MODE_STA* načinu rada i u *WIFI_MODE_APSTA*.

#### Odspajanje s Wi-Fi mreže
```
esp_err_t esp_wifi_disconnect(void)
```

Funkcija vraća *ESP_OK* ako je uređaj odspojen s Wi-Fi mreže. Ova funkcija radi samo u *WIFI_MODE_STA* načinu rada i u *WIFI_MODE_APSTA*.

### Općenito postavljanje STATION načina rada

Proces postavljanja ESP32 u STATION način rada je sljedeći:

- inicijalizacija NVS flash particije

- inicijalizacija mrežnog sučelja i stvaranje uobičajene konfiguracije za STATION način rada

- registracija Wi-Fi i IP događaja (ako treba)

- inicijalizacija Wi-Fi upravljačkog programa, stvaranje konfiguracije mreže (ime mreže, zaporka, ...), postavljanje načina rada Wi-Fija (STATION)

- započinjanje rada Wi-Fia

- spajanje na Wi-Fi mrežu

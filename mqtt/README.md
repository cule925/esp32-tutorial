# MQTT

[MQTT (Message Queuing Telemetry Transport)](https://mqtt.org/) je nezahtjevni mrežni protokol za razmjene poruka koji se koristi u komunikaciji uređaja interneta stvari (*eng. Internet of Things*). Razvili su ga Andy Stanford-Clark i Arlen Nipper pod pokroviteljstvom IBM-a. Protokol je aplikacijskog sloja koji koristi protokole nižih slojeva za komunikaciji, najčešće TCP/IP protokole nižih slojeva.

## Način rada MQTT-a

Protokol se zasniva na konceptu objavi-pretplati (*eng. publish-subscribe*) gdje uređaji odnosno klijenti komuniciraju uz pomoć posrednika (nekada zvani i poslužitelji). Klijenti su najčešće jednostavni uređaji niske potrošnje, a posrednik neki jači uređaj, možda Raspberry Pi. Postoje razne implementacije MQTT-a [klijenata](https://mqtt.org/software/) i [posrednika](https://mqtt.org/software/).

Podatci na posredniku su hijerarhijski organizirani u obliku tema (*eng. topic*). Klijenti se mogu pretplatiti na temu odnosno prisluškivati nadolazeće poruke koje su namijenjene za tu temu. Drugi klijenti mogu objavljivati poruke namijenjene za temu. Posrednik će u tom slučaju proslijediti sve poruke klijentima koji su se pretplatili na tu temu.

```
        +---------------+  Publish on topic "humidity": 45%
        |               |  <-----------------------------------+
        |  MQTT Client  |                                      |
        |               |  -- -- -- -- -- -- -- -- -- -- -- +  |
        +---------------+   Subscribed to topic "humidity"  |  |
                                                            |  |
                                                            V  |
                                                          +---------------+
        +---------------+  Publish on topic "temp": 30°C  |               |                                 +---------------+
        |               |  <----------------------------  |               |  Publish on topic "temp": 30°C  |               |
        |  MQTT Client  |                                 |  MQTT Client  |  <----------------------------  |  MQTT Client  |
        |               |  -- -- -- -- -- -- -- -- -- ->  |               |                                 |               |
        +---------------+    Subscribed to topic "temp"   |               |                                 +---------------+
                                                          +---------------+
                                                            A  |        A
                                                            |  |        |
        +---------------+     Subscribed to topic "temp"    |  |        |                                   +---------------+
        |               |  -- -- -- -- -- -- -- -- -- -- -- +  |        |   Publish on topic "temp": 45%    |               |
        |  MQTT Client  |                                      |        +---------------------------------  |  MQTT Client  |
        |               |  <-----------------------------------+                                            |               |
        +---------------+   Publish on topic "temp": 30°C                                                   +---------------+
```

Posrednik distribuira nadolazeću poruku klijentima koji su bili pretplaćeni na temu kojoj je poruka bila namijenjena. Ako je poruka došla na temu na kojoj nema pretplaćen ni jedan klijent, poruka se odbacuje osim ako je poruci postavljena zastavica zadržavanje poruke (*eng. retained message*). Poruka koja se zadržava šalje se automatski svim novim klijentima koji se pretplate na temu za koju je poruka bila namijenjena. Posrednik sprema samo **jednu** zadržanu poruku po temi tako da svaka nova objavljena poruka koja se zadržava zamjenjuje staru. Klijenti međusobno ne znaju za broj drugih klijenata.

Klijenti također obično svakih 60 sekundi objavljuju *keep alive* poruku kako bi posredniku pokazali da su još pretplaćeni na temu. Svaki MQTT klijent mora imati jedinstveno ime. Ako se klijent pokuša spojiti s MQTT posrednikom na kojeg je već spojen istoimeni klijent, posrednik neće dozvoliti trenutačnom klijentu uspostavljanje konekcije.

### Teme

[MQTT teme](http://www.steves-internet-guide.com/understanding-mqtt-topics/) su hijerarhijski strukturirane slično kao i datotečni sustavi (odvojene su s */*). Imena su UTF-8 enkodirana te su osjetljiva na velika i mala slova. Postoji samo jedno rezervirano ime za temu: **$SYS** koje posrednik koristi za objavljivanje informacija o sebi.

Klijent se može pretplatiti na jednu ili više tema odjednom. To je moguće s posebnim znakovima **#** i **+**. Primjerice, ako već postoje teme: *house/kitchen/lights*, *house/bedroom/lights*, *house/bedroom/temperature*, pretplaćivanjem na *house/#* pretplatit će se na sve tri teme uključujući i samu *house/* i *house* temu. Preplaćivanjem na *house/+/lights* pretplatit će se na teme *house/kitchen/lights* i *house/bedroom/lights*. Posebni znakovi se moraju nalaziti jedini između ili na kraju razdjeljivača */*.

Ime korijenskih tema ne moraju započinjati s */*. U slučaju da započinju s */* onda se samo nadodaju na korijensku strukturu (može ih se sve označiti s */#* ili */+/*)

MQTT se dinamički stvaraju i uništavaju. Teme se stvaraju kad se klijent pretplati na nepostojeću temu ili klijent pošalje poruku sa zastavicom zadržavanja poruke na temu koja još ne postoji. Teme se uništavaju kada se zadnji klijent koji je pretplaćen na temu odjavi.

Zadana opcija za spajanje klijenta s posrednikom je čista sesija. Ova opcija omogućuje da posrednik ne sprema nikakve podatke o klijentu. Primjerice, ako se klijent odspoji i ponovno spoji posrednik ga neće automatski pretplatiti na teme na koje je bio prethodno pretplaćen.

### Protokolni paketi

[MQTT protokolni paketi](http://www.steves-internet-guide.com/mqtt-protocol-messages-overview/) se sastoje od sljedećih polja:

- upravljačko zaglavlje: 1 bajt
- duljina paketa: od 1 bajta do 4 bajta
- ostatak zaglavlja: 0 ili više bajtova (ovisi o tipu poruke)
- podatci: 0 ili više bajtova (ovisi o duljini poruke, enkodira se s UTF-8)

Iz navedenih se može vidjeti da je najmanja moguća MQTT poruka veličine 2 bajta (upravljačko zaglavlje i duljina paketa). Upravljačko zaglavlje definira dvije stvari:

- tip paketa (prvih 4 bitova)
	- bit 0 je rezerviran
	- bit 1 se postavlja ako je poruka tipa *CONNECT*, šalje se od klijenta do posrednika kao zahtjev za spajanje na posrednik
	- bit 2 se postavlja ako je poruka tipa *CONNACK*, šalje se od posrednika do klijenta kao potvrda posrednika na zahtjev spajanja klijenta
	- bit 3 se postavlja ako je poruka tipa *PUBLISH*, šalje se od klijenta do posrednika kao objava podatka na temu
	- ... (16 kombinacija)
- zastavice (drugih 4 bitova)
	- bit 0 se postavlja ako se želi koristiti zadržavanje poruke
	- bit 2 i bit 1 se postavljaju kad se želi postaviti kvaliteta usluge (*eng. QoS - Quality Of Service*): 00, 01, 10 -> 0, 1, 2
	- bit 3 se postavlja ako se želi duplicirati poruka u slučaju kvalitete usluge 1 i 2
	- ... (16 kombinacija)

Duljina paketa definira veličinu podataka. 7 bitova se koristi za podatak o veličini paketa dok se bit najznačajnije vrijednosti (*eng. MSB - Most Significant Bit*) koristi kao zastavica za kontinuaciju vrijednosti veličine. Zato je veličina ovog polja od 1 do 4 bajta:

```
        
                   Continuation  Cont.    No cont.
                        bit       bit       bit        => Packet length field length: 3 bytes
                         |         |         |
                         V         V         V
        [Control header][10000100][10000100][01000000][10110010]....
                          |     |   |     |   |     |
                          \     \   |     |   /     /
                           \     \  |     |  /     /
                            \     \ |     | /     /
                             |     \|     |/     |
                         MSB 000010000001001000000 LSB => Rest of header and data field length: 66112 bytes
```

### Razine kvalitete usluge

MQTT nudi tri [razine kvalitete usluga](http://www.steves-internet-guide.com/understanding-mqtt-qos-levels-part-1/):

- QOS 0
	- klijent šalje poruku samo jednom i ona se ne potvrđuje od posrednika
	- poruka se ne sprema na posrednik
	- brz, ali nepouzdan način rad
- QOS 1
	- klijent šalje poruku te očekuje potvrdu na nju od posrednika (potvrda *PUBACK*)
	- ako posrednik ne pošalje potvrdu, poruka se ponovno šalje sa zastavicom *DUP (Duplicate Flag)*
	- klijent će nastaviti slati poruke sve dok ne dobije potvrdu od posrednika
	- klijenti pretplatnici mogu dobivati duplikate ovih poruka
- QOS 2
	- klijent šalje poruku garantirano samo jednom
	- najpouzdaniji način rada
	- četiri razine isporuke poruka:
		- klijent šalje poruku posredniku
		- posrednik odgovara s *PUBREC*
		- klijent šalje *PUBREL* potvrdu posredniku
		- posrednik šalje *PUBCOMP* potvrdu klijentu

### ESP32 i MQTT klijent

ESP-IDF nudi [implementaciju MQTT klijenta ESP-MQTT](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/protocols/mqtt.html) za ESP32 mikroupravljače. Podupire razne načine rada:

- MQTT preko TCP-a
- MQTT preko Mbed TLS-a
- MQTT preko WebSocketa
- MQTT preko Secure WebSocketa

ESP-MQTT zahtijeva da je ESP32 povezan na mrežu te pokretanje sustavske petlje događaja. Upute se mogu naći [ovdje](../wifi).

Zaglavlje koje je potrebno uključiti za [postavljanje MQTT klijenta](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/protocols/mqtt.html#header-file) je ```mqtt_client.h```. Zadano se koristi MQTT 3.1.1 verzija protokola. Ako se želi postaviti korištenje verzije 5.0 to je moguće postaviti uz pomoć *menuconfig*. Varijabla koja treba biti u *y* stanju u *sdkconfig* datoteci mora biti varijabla *CONFIG_MQTT_PROTOCOL_5*.

#### Inicijaliziraj klijenta
```
esp_mqtt_client_handle_t esp_mqtt_client_init(const esp_mqtt_client_config_t *config)
```

Parametar ove funkcije je:

- *config*
	- definira konfiguraciju klijenta
	- najlakše inicijalizirati sve na 0 pa promijeniti vrijednost članova po potrebi
	- članovi strukture su:
		- *broker*
			- struktura tipa *broker_t* koja sadrži informacije o posredniku, ima članove:
				- *address*
					- struktura tipa *address_t* koja sadrži informacije o adresi posrednika, ima članove:
						- *uri*
							- pokazivač na niz znakova koji predstavlja identifikator resursa (*eng. URI - Uniform Resource Identifier*)
							- ima prednost nad svim ostalim članovima ove strukture
						- *hostname*
							- pokazivač na niz znakova koji predstavlja ime ili IPv4 adresu računala na kojem je posrednik
						- *transport*
							- enumerator tipa *esp_mqtt_transport_t*, označava način transporta podataka (TCP, SSL, WebSocket), enumeratori mogu biti:
								- *MQTT_TRANSPORT_UNKNOWN* - nepoznato
								- *MQTT_TRANSPORT_OVER_TCP* - preko TCP-a, koristeći shemu *mqtt*
								- *MQTT_TRANSPORT_OVER_SSL* - preko SSL-a, koristeći shemu *mqtts*
								- *MQTT_TRANSPORT_OVER_WS* - preko WebSocketa, koristeći shemu *ws*
								- *MQTT_TRANSPORT_OVER_WSS* - preko Websocket Securea, koristeći shemu *wss*
						- *path*
							- pokazivač na niz znakova koji predstavlja put do resursa u URI-ju
						- *port*
							- podatak tipa *uint32_t* koji predstavlja port na kojem je posrednik dostupan
				- *verification*
					- struktura tipa *verification_t* koja sadrži informacije za verifikaciju posrednika, više informacija o članovima se može saznati [ovdje](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/protocols/mqtt.html#_CPPv4N24esp_mqtt_client_config_t8broker_t14verification_tE)
		- *credentials*
			- struktura tipa *credentials_t* koja sadrži informacije potrebne za prijavu na posrednik, ima članove:
				- *username*
					- pokazivač na niz znakova koji predstavlja korisničko ime za posrednik
				- *client_id*
					- pokazivač na niz znakova koji predstavlja identifikator ovog klijenta
				- *set_null_client_id*
					- podatak tipa *bool* koji govori koristi li se identifikator klijenta definiranog u *client_id* (*false* - da, *true* - ne)
				- *authentication*
					- struktura tipa *authentication_t* koja predstavlja potrebne podatke za autentifikaciju, sadrži članove:
						- *password*
							- pokazivač na niz znakova koji predstavlja zaporku za posrednik
						- *certificate*
							- pokazivač na niz znakova koji predstavlja SSL certifikat za međusobnu autentifikaciju
						- *certificate_len*
							- podatak tipa *size_t* koji predstavlja veličinu SSL certifikata proslijeđen u *certificate*
						- *key*
							- pokazivač na niz znakova koji predstavlja SSL privatni ključ
						- *key_len*
							- podatak tipa *size_t* koji predstavlja veličinu SSL privatnog ključa proslijeđen u *key*
						- *key_password*
							- pokazivač na niz znakova koji predstavlja zaporku SSL privatnog ključa proslijeđen u *key*
						- *key_password_len*
							- podatak tipa *size_t* koji predstavlja veličinu zaporke za SSL privatni ključ proslijeđenog u *key*
						- *use_secure_element*
							- podatak tipa *bool* koji omogućuje korištenje sigurnosne komponente u hardveru mikroupravljača za upravljanje osjetljivim podatcima
						- *ds_data*
							- pokazivač tipa *void* koji pokazuje na parametre za digitalni potpis
		- *session*
			- struktura tipa *session_t* koja predstavlja konfiguraciju MQTT sesije, sastoji se od članova:
				- *last_will*
					- struktura tipa *last_will_t* koja definira poruku koja će se objaviti u slučaju da se klijent odspoji, više informacija o članovima se može saznati [ovdje](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/protocols/mqtt.html#_CPPv4N24esp_mqtt_client_config_t9session_t11last_will_tE)
				- *disable_clean_session*
					- podatak tipa *bool* koji onemogućuje čistu sesiju
				- *keepalive*
					- podatak tipa *int* koji postavlja interval u sekundama prije nego li će posrednik odspojiti klijenta ako mu se klijent ne javi
				- *disable_keepalive*
					- podatak tipa *bool* koji ako se vrijednost postavi na *false* postavlja zadanu vrijednost *keepalive* varijable
				- *protocol_ver*
					- enumerator tipa *esp_mqtt_protocol_ver_t* koji označava koju verziju MQTT protokola koristiti, enumeratori mogu biti:
						- *MQTT_PROTOCOL_UNDEFINED* - nespecificirano
						- *MQTT_PROTOCOL_V_3_1* - verzija 3.1
						- *MQTT_PROTOCOL_V_3_1_1* - verzija 3.1.1
						- *MQTT_PROTOCOL_V_5* - verzija 5
				- *message_retransmit_timeout*
					- podatak tipa *int* koji postavlja interval u sekundama nakon kojeg će se paket ponovno poslati ako nije dobio potvrdu od posrednika
		- *network*
			- struktura tipa *network_t* koja predstavlja mrežnu konfiguraciju, sastoji se od članova:
				- *reconnect_timeout_ms*
					- podatak tipa *int* koji predstavlja interval nakon kojeg će se klijent ponovno spojiti na posrednik ako se kojim slučajem odspoji
				- *timeout_ms*
					- podatak tipa *int* koji predstavlja interval nakon kojeg će abortirati bilo koja mrežna operacija koja nije završila od početka intervala
				- *refresh_connection_after_ms*
					- podatak tipa *int* koji predstavlja interval nakon kojeg će se osvježiti konekcija prema posredniku
				- *disable_auto_reconnect*
					- podatak tipa *bool* koji onemogućuje automatsko ponovno spajanje u slučaju odspajanja klijenta
				- *transport*
					- pokazivač na *handler* funkciju tipa *esp_transport_handle_t* koja će se pozvati u slučaju komunikacije
				- *if_name*
					- ime mrežnog sučelja klijenta kroz kojeg bi išli podatci, koristi se zadano mrežno sučelje ako ovo nije postavljeno
		- *task*
			- struktura tipa *task_t* koja predstavlja konfiguraciju *FreeRTOS* zadatka klijenta koji će se stvoriti inicijalizacijom, sastoji se od članova:
				- *priority*
					- podatak tipa *int* koji predstavlja prioritet zadatka
				- *stack_size*
					- podatak tipa *int* koji predstavlja veličinu stoga zadatka
		- *buffer*
			- struktura tipa *buffer_t* koja predstavlja konfiguraciju klijentskih međuspremnika, sastoji se od članova:
				- *size*
					- podatak tipa *int* koji predstavlja veličinu međuspremnika za slanje i primanje podataka
				- *out_size*
					- podatak tipa *int* koji definira specifično veličinu međuspremnika za slanje
		- *outbox*
			- struktura tipa *outbox_config_t* koja predstavlja međuspremnik za spremanje poruka koje čekaju u redu za slanje, sastoji se od jednog člana:
				- *limit*
					- podatak tipa *uint64_t* koji predstavlja veličinu međuspremnika
		

Funkcija vraća *handle* na klijenta ako je uspješno inicijaliziran, u suprotnom vraća *NULL*.

#### Deinicijaliziraj klijent
```
esp_err_t esp_mqtt_client_destroy(esp_mqtt_client_handle_t client)
```

Parametar ove funkcije je:

- *client*
	- *handle* na klijenta

Funkcija vraća *ESP_OK* ako klijent uspješno deinicijaliziran.

#### Započni rad klijenta
```
esp_err_t esp_mqtt_client_start(esp_mqtt_client_handle_t client)
```

Parametar ove funkcije je:

- *client*
	- *handle* na klijenta

Funkcija vraća *ESP_OK* ako je rad klijenta uspješno započeo.

#### Zaustavi rad klijenta
```
esp_err_t esp_mqtt_client_stop(esp_mqtt_client_handle_t client)
```

Parametar ove funkcije je:

- *client*
	- *handle* na klijenta

Funkcija vraća *ESP_OK* ako je rad klijenta uspješno završio.

#### Ponovno se spoji na posrednik
```
esp_err_t esp_mqtt_client_reconnect(esp_mqtt_client_handle_t client)
```

Parametar ove funkcije je:

- *client*
	- *handle* na klijenta

Funkcija vraća *ESP_OK* ako je klijent ponovno spojen na posrednik. Koristi se u *handleru* događaja.

#### Odspoji se s posrednika
```
esp_err_t esp_mqtt_client_disconnect(esp_mqtt_client_handle_t client)
```

Parametar ove funkcije je:

- *client*
	- *handle* na klijenta

Funkcija vraća *ESP_OK* ako je klijent odspojen na posrednik.

#### Pretplati se na temu
```
int esp_mqtt_client_subscribe_single(esp_mqtt_client_handle_t client, const char *topic, int qos)
```

Parametri ove funkcije su:

- *client*
	- *handle* na klijenta
- *topic*
	- tema
- *qos*
	- kvalitet usluge: 0, 1 ili 2

Funkcija vraća identifikator poruke ako je pretplata uspjela, -1 ako pretplata nije uspjela i -2 ako je *outbox* pun. Za *qos* 0 identifikator poruke će uvijek biti 0.

#### Odjavi se s teme
```
int esp_mqtt_client_unsubscribe(esp_mqtt_client_handle_t client, const char *topic)
```

Parametri ove funkcije su:

- *client*
	- *handle* na klijenta
- *topic*
	- tema, niz znakova mora biti terminiran s *\0*

Funkcija vraća identifikator poruke ako je poruka objavljena i -1 ako poruka nije objavljena.

#### Objavi poruku na temu
```
int esp_mqtt_client_publish(esp_mqtt_client_handle_t client, const char *topic, const char *data, int len, int qos, int retain)
```

Parametri ove funkcije su:

- *client*
	- *handle* na klijenta
- *topic*
	- tema, niz znakova mora biti terminiran s *\0*
- *data*
	- podatci
- *len*
	- veličina podataka
- *qos*
	- kvalitet usluge: 0, 1 ili 2
- *retain*
	- zadržavanje poruke

Funkcija vraća identifikator poruke ako je objava uspjela, -1 ako objava nije uspjela i -2 ako je *outbox* pun. Za *qos* 0 identifikator poruke će uvijek biti 0.

#### Registriraj handler za MQTT događaj
```
esp_err_t esp_mqtt_client_register_event(esp_mqtt_client_handle_t client, esp_mqtt_event_id_t event, esp_event_handler_t event_handler, void *event_handler_arg)
```

- *client*
	- *handle* na klijenta
- *event*
	- specifični događaj za kojeg će se pokrenuti *handler*, u slučaju MQTT-a može postaviti *ESP_EVENT_ANY_ID*
- *event_handler*
	- pokazivač na *handler* funkciju
	- funkcija mora imati sljedeću deklaraciju: ```void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)```
		- parametri su sljedeći:
			- *arg* - proslijeđeni parametar koji se proslijedio pri registraciji
			- *event_base* - identifikator općenitog događaja koji se dogodio
			- *event_id* - identifikator specifičnog događaja koji se dogodio
			- *event_data* - struktura podataka koja je proslijeđena u trenutku događaja koji se dogodio
	
- *event_handler_arg*
	- parametar za *handler*

Funkcija vraća *ESP_OK* ako je uspješno prijavljen *handler*.

##### Događaji za obradu u handleru

Događaji za obradu u *handleru* ```void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)``` mogu primjerice biti:

- *MQTT_EVENT_BEFORE_CONNECT* - klijent se inicijalizirao i kreće s povezivanjem na posrednik
- *MQTT_EVENT_CONNECTED* - klijent se povezao na posrednik
- *MQTT_EVENT_DISCONNECTED* - klijent se odspojio s posrednika
- *MQTT_EVENT_SUBSCRIBED* - klijent se pretplatio na temu
- *MQTT_EVENT_UNSUBSCRIBED* - klijent se odjavio s teme
- *MQTT_EVENT_PUBLISHED* - klijent je objavio podatke na temu
- *MQTT_EVENT_DATA* - klijent je dobio podatke s neke pretplate
- *MQTT_EVENT_ERROR* - dogodila se neka greška na klijentu

Argument *event_data* se tada može pretvoriti u pokazivač na strukturu *esp_mqtt_event_t* odnosno podatak tipa *esp_mqtt_client_handle_t* čiji su neki od članova:

- *event_id* - tip podatka *esp_mqtt_event_id_t* MQTT događaja
- *client* - tip podatka *esp_mqtt_client_handle_t* koji predstavlja *handle* na klijent
- *data* - pokazivač na niz znakova koji predstavlja pridošle podatke
- *data_len* - podatak tipa *int* koji predstavlja veličinu pridošlih podataka
- *total_data_length* - ukupna veličina podataka (u slučaju više događaja)
- *current_data_offset* - pomak od podataka asociranih s ovim događajem
- *topic* - pokazivač na niz znakova koji predstavljaju temu asociranu s pridošlim podatcima
- *topic_len* - podatak tipa *int* koji predstavlja veličinu teme
- *msg_id* - podatak tipa *int* koji predstavlja MQTT identifikator poruke
- *session_present* - podatak tipa *int* koji govori je li prisutna sesija
- *error_handle* - pokazivač tipa *esp_mqtt_error_codes_t* koji predstavlja *handle* greške
- *retain* - podatak tipa *bool* koji označava je li se poruka zadržava
- *qos* - podatak tipa *int* koji označava kvalitetu usluge poruke
- *dup* - podatak tipa *bool* koji označava je li poruka duplikat
- *protocol_ver* - podatak tipa *esp_mqtt_protocol_ver_t* koji označava verziju protokola MQTT
- *property* - podatak tipa *esp_mqtt5_event_property_t* koji označava svojstva poruke, koristi se u MQTT verziji 5

#### Odjavi handler za MQTT događaj
```
esp_err_t esp_mqtt_client_unregister_event(esp_mqtt_client_handle_t client, esp_mqtt_event_id_t event, esp_event_handler_t event_handler)
```

- *client*
	- *handle* na klijenta
- *event*
	- specifični MQTT događaj
- *event_handler*
	- pokazivač na *handler*

Funkcija vraća *ESP_OK* ako je uspješno odjavljen *handler*.

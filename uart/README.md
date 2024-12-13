# UART

[UART (*eng. Universal Asynchronous Receiver Transmitter*)](https://www.rohde-schwarz.com/cz/products/test-and-measurement/essentials-test-equipment/digital-oscilloscopes/understanding-uart_254524.html) je serijski asinkroni protokol kojeg je razvio [Gordon Bell](https://en.wikipedia.org/wiki/Gordon_Bell). Omogućuje komunikaciju između dva uređaja. Može se koristiti za jednosmjernu simplex komunikaciju, dvosmjernu half-duplex i dvosmjernu full-duplex komunikaciju. UART se koristi u standardima poput RS-232 i RS-422.

## Način rada UART-a

Kako bi dva uređaja mogli komunicirati koristeći UART, moraju se unaprijed dogovoriti za brzinu prijenosa simbola [(*eng. Baud rate*)](https://en.wikipedia.org/wiki/Baud). Tipične brzine prijenosa simbola mogu biti 4800 Bd, 9600 Bd, 19200 Bd, 38400 Bd, 57600 Bd, 115200 Bd, 230400 Bd, 460800 Bd, 921600 Bd.

Kako bi jedan uređaj slao (TX) a drugi primao podatke (RX), potrebno ih je međusobno povezati. Za ovakav jedan komunikacijski kanal dovoljno je povezati dva uređaja jednom linijom, najčešće pin TX uređaja koji šalje podatke na pin RX uređaja koji prima podatke.

```
        +--------+    DATA COMMUNICATION    +--------+
        |  UART  |         CHANNEL          |  UART  |
        |     TX |--------------------------| RX     |
        |        |                          |        |
        +--------+                          +--------+
         _|_                                 _|_
         GND                                 GND
```

### Uobičajeni način komunikacije

U početku, uređaj koji šalje podatke drži liniju komunikacije u logičko *1*. Kada se želi prenijeti podatak, linija se spušta u logičku *0* i onda se šalju bitovi podatka od najmanje značajnog do najviše značajnog. Opcionalno, može se poslati i bit pariteta za kraj. Nakon odašiljanja podataka, uređaj koji šalje podatke diže liniju u logičko *1*.

```
                      START                          STOP
                        |                             |
                        |                             |
          DATA          V                             V
          COMM.   ____     _  _  _  _  _  _  _  _  _  ____
          CHANNEL     \_0_/X\/X\/X\/X\/X\/X\/X\/X\/X\/1
                           D0 D1 D2 D3 D4 D5 D6 D7 P

          D - data bits
          P - parity bit
```

Opcionalni paritetni bit može raditi u načinu:

- parni paritet - ako je broj *1* bitova paran u poslanom podatku onda je ovaj bit u *0*
- neparni paritet - ako je broj *1* bitova neparan u poslanom podatku onda je ovaj bit u *0*

Ovakav način provjeravanja grešaka funkcionira samo ako se dogodila promjena na jednom podatkovnom bitu.

### ESP32 i UART

ESP32 sadrži tri UART sklopa čije se linije mogu usmjeriti na bilo koji GPIO pin multipleksiranjem ili se mogu koristiti zadani GPIO pinovi (IO_MUX). Međutim, ako se želi koristiti UART s brzinom prijenosa veću od 40 MHz-a, najbolje je koristiti pinove koji su direktno spojeni na pojedine UART sklopove. Za primjere se koristila pločica [*ESP32-DevKitM-1*](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp32-devkitm-1/user_guide.html#getting-started) koja na sebi ima modul [ESP32-MINI-1](https://www.espressif.com/sites/default/files/documentation/esp32-mini-1_datasheet_en.pdf).

Primjer gdje ESP32 šalje i prima podatke uz pomoć UART protokola nalazi se [ovdje](uart_tx_rx).

Zaglavlje koje je potrebno uključiti za [korištenje UART-a](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/uart.html#header-file) na ESP32 je ```driver/uart.h```.

Ako se žele koristiti [makroi mapiranih UART GPIO pinova](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/uart.html#id6) na ESP32, potrebno je uključiti zaglavlje ```soc/uart_channel.h```.

Preporučuje se koristiti drugi ili treći UART sklop (*UART_NUM_1* ili *UART_NUM_2*). Prvi sklop (*UART_NUM_0*) se koristi u serijskoj komunikaciji s računalom (TX - GPIO 1 i RX - GPIO 3). Drugi sklop ne može koristiti zadane pinove za direktnu komunikaciju (TX - GPIO 10 i RX - GPIO 9) jer se oni koriste za SPI flash memoriju. Trećem sklopu su pinovi za direktnu komunikaciju (TX - GPIO 17 i RX - GPIO 16) nezauzeti, ali pinovi nisu dostupni na pločici *ESP32-DevKitM-1*.

Postupak slanja podataka uz pomoć UART-a je sljedeći:

```
          LOCAL BUFFER
          | | | | | | | | ... | |

                    |
                    |   APPLICATION COPIES TO TX RING BUFFER
                    V

          TX RING BUFFER
          | | | | | | | | | | | | | ... | |

                    |
                    |   UART ISR COPIES TO TX FIFO BUFFER
                    V

          TX FIFO BUFFER
          | | | | | | | | | | ... | |

           |
           |   UART HARDWARE WRITES THE DATA AND REMOVES IT FROM FIFO
           V

          TX OUT
```

Postupak primanja podataka uz pomoć UART-a je sljedeći:

```
                                 RX IN

                                   |
                                   |   UART HARDWARE READS THE DATA AND PUTS IT IN FIFO
                                   V
          RX FIFO BUFFER
          | | | | | | | | | | ... | |

                    |
                    |   UART ISR COPIES TO RX RING BUFFER
                    V

          RX RING BUFFER
          | | | | | | | | | | | | | ... | |

                    |
                    |   APPLICATION COPIES TO LOCAL BUFFER
                    V

          LOCAL BUFFER
          | | | | | | | | ... | |
```

Svaki UART sklop ima svoje TX FIFO i RX FIFO međuspremnike. Svaki od njih je veličine 128 bajta. Kada aplikacija zahtjeva slanje podataka uz pomoć UART-a, prvo se podatci u lokalnom međuspremniku kopiraju u TX kružni međuspremnik ako postoji. Zatim UART ISR kopira te podatke iz TX kružnog međuspremnika u TX FIFO međuspremnik. Nakon toga se podatci iz TX FIFO međuspremnika dalje šalju na TX izlaz i pri svakim slanjem zove se ISR koji kopira sljedeće podatke ako ih ima iz TX kružnog međuspremnika u TX FIFO međuspremnik. S druge strane, podatci koji dolaze na RX ulaz se spremaju u RX FIFO međuspremnik i UART ISR ih kopira u RX kružni međuspremnik. Aplikacija može čitati ove podatke iz RX kružnog međuspremnika.

#### Inicijaliziraj upravljački program

```
esp_err_t uart_driver_install(uart_port_t uart_num, int rx_buffer_size, int tx_buffer_size, int queue_size, QueueHandle_t *uart_queue, int intr_alloc_flags)
```

Parametri ove funkcije su:

- *uart_num*
	- ciljani UART sklop, enumerator može imati vrijednosti:
		- *UART_NUM_0*
		- *UART_NUM_1*
		- *UART_NUM_2*
- *rx_buffer_size*
	- veličina RX kružnog međuspremnika
- *tx_buffer_size*
	- veličina TX kružnog međuspremnika
- *queue_size*
	- veličina FreeRTOS reda poruka koji služi za spremanje UART događaja
- *uart_queue*
	- handle na FreeRTOS red poruka koji služi za spremanje UART događaja
- *intr_alloc_flags*
	- svojstva UART prekida (ISR-a) oblika *ESP_INTR_FLAG_\**, više informacija se može naći [ovdje](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/system/intr_alloc.html#id2)
	- enumerator ne smije imati svojstvo *ESP_INTR_FLAG_IRAM* jer se ISR handler upravljačkog programa ne nalazi u IRAM-u

Funkcija vraća *ESP_OK* ako je uspješno inicijaliziran UART upravljački program.

#### Deinicijaliziraj upravljački program

```
esp_err_t uart_driver_delete(uart_port_t uart_num)
```

Parametar ove funkcije je:

- *uart_num*
	- ciljani UART sklop, enumerator može imati vrijednosti:
		- *UART_NUM_0*
		- *UART_NUM_1*
		- *UART_NUM_2*

Funkcija vraća *ESP_OK* ako je uspješno inicijaliziran UART upravljački program.

#### Postavi konfiguraciju pariteta

```
esp_err_t uart_set_parity(uart_port_t uart_num, uart_parity_t parity_mode)
```

Parametri ove funkcije su:

- *uart_num*
	- ciljani UART sklop, enumerator može imati vrijednosti:
		- *UART_NUM_0*
		- *UART_NUM_1*
		- *UART_NUM_2*
- *parity_mode*
	- konfiguracija pariteta, enumerator može biti:
		- *UART_PARITY_DISABLE* - nema paritetnog bita
		- *UART_PARITY_EVEN* - parni paritet
		- *UART_PARITY_ODD* - neparni paritet

Funkcija vraća *ESP_OK* ako je uspješno postavljena konfiguracija pariteta.

#### Dohvati konfiguraciju pariteta

```
esp_err_t uart_get_parity(uart_port_t uart_num, uart_parity_t *parity_mode)
```

Parametri ove funkcije su:

- *uart_num*
	- ciljani UART sklop, enumerator može imati vrijednosti:
		- *UART_NUM_0*
		- *UART_NUM_1*
		- *UART_NUM_2*
- *parity_mode*
	- pokazivač na varijablu u kojoj će se spremiti dohvaćena konfiguracija pariteta

Funkcija vraća *ESP_OK* ako je uspješno dohvaćena konfiguracija pariteta.

#### Postavi brzinu prijenosa simbola

```
esp_err_t uart_set_baudrate(uart_port_t uart_num, uint32_t baudrate)
```

Parametri ove funkcije su:

- *uart_num*
	- ciljani UART sklop, enumerator može imati vrijednosti:
		- *UART_NUM_0*
		- *UART_NUM_1*
		- *UART_NUM_2*
- *baudrate*
	- brzina prijenosa simbola (*eng. Baud Rate*)

Funkcija vraća *ESP_OK* ako je uspješno postavljena brzina prijenosa simbola.

#### Dohvati brzinu prijenosa simbola

```
esp_err_t uart_get_baudrate(uart_port_t uart_num, uint32_t *baudrate)
```

Parametri ove funkcije su:

- *uart_num*
	- ciljani UART sklop, enumerator može imati vrijednosti:
		- *UART_NUM_0*
		- *UART_NUM_1*
		- *UART_NUM_2*
- *baudrate*
	- pokazivač na varijablu u kojoj će se spremiti dohvaćena brzina prijenosa simbola (*eng. Baud Rate*)

Funkcija vraća *ESP_OK* ako je uspješno postavljena brzina prijenosa simbola.

#### Postavi opću konfiguraciju

```
esp_err_t uart_param_config(uart_port_t uart_num, const uart_config_t *uart_config)
```

Parametri ove funkcije su:

- *uart_num*
	- ciljani UART sklop, enumerator može imati vrijednosti:
		- *UART_NUM_0*
		- *UART_NUM_1*
		- *UART_NUM_2*
- *uart_config*
	- konfiguracija ciljanog UART sklopa, struktura ima članove:
		- *baud_rate*
			- varijabla tipa *int* koja definira brzinu prijenosa simbola (*eng. Baud Rate*)
		- *data_bits*
			- enumerator tipa *uart_word_length_t* koji definira veličinu jednog simbola u bitovima
			- vrijednosti mogu biti:
				- *UART_DATA_5_BITS* - 5 podatkovnih bitova
				- *UART_DATA_6_BITS* - 6 podatkovnih bitova
				- *UART_DATA_7_BITS* - 7 podatkovnih bitova
				- *UART_DATA_8_BITS* - 8 podatkovnih bitova
				- *UART_DATA_BITS_MAX* - maksimalni broj podatkovnih bitova
		- *parity*
			- enumerator tipa *uart_parity_t* koji konfigurira paritet
			- vrijednosti mogu biti:
				- *UART_PARITY_DISABLE* - nema paritetnog bita
				- *UART_PARITY_EVEN* - parni paritet
				- *UART_PARITY_ODD* - neparni paritet
		- *stop_bits*
			- enumerator tipa *uart_stop_bits_t* koji definira broj stop bitova nakon podatkovnih bitova i bita pariteta ako postoji
			- vrijednosti mogu biti:
				- *UART_STOP_BITS_1* - 1 stop bit
				- *UART_STOP_BITS_1_5* - 1.5 stop bita
				- *UART_STOP_BITS_2* - 2 stop bita
				- *UART_STOP_BITS_MAX* - maksimalni broj stop bitova
		- *flow_ctrl*
			- enumerator tipa *uart_hw_flowcontrol_t* koji definira postoji li dodatni upravljački signali koji signaliziraju je li RX spreman za primanje podataka ili TX spreman za slanje podataka
			- vrijednosti mogu biti:
				- *UART_HW_FLOWCTRL_DISABLE* - nema dodatnih signala
				- *UART_HW_FLOWCTRL_RTS* - omogući signal koji RX koristi za dojavu spremnosti
				- *UART_HW_FLOWCTRL_CTS* - omogući signal koji TX koristi za dojavu spremnosti
				- *UART_HW_FLOWCTRL_CTS_RTS* - omogući signale koji TX i RX koriste za dojavu spremnosti
				- *UART_HW_FLOWCTRL_MAX* - omogući sve signale
			- koristi se primjerice u RS-232 standardu
		- *rx_flow_ctrl_thresh*
			- varijabla tipa *uint8_t* koja definira maksimalni broj podataka u RX kružnom međuspremniku prije nego li se RTS deaktivira (RX nije spreman za primanje podataka)
			- maksimum je 128 bajtova
		- *source_clk*
			- enumerator tipa *uart_sclk_t* koji definira izvorni takt za UART sklop
			- vrijednosti mogu biti:
				- *UART_SCLK_APB* - APB_CLK takt
				- *UART_SCLK_REF_TICK* - REF_TICK takt
				- *UART_SCLK_DEFAULT* - zadani takt (APB_CLK takt)
		- *backup_before_sleep*
			- varijabla tipa *uint32_t* koja definira hoće li se spremiti HP (*eng. High Performance*) UART registri u memoriju tijekom načina rada spavanja
			- sustav može isključiti HP UART tijekom načina rada spavanja
			- ako je vrijednost postavljena u *0* onda se ne spremaju registri

Funkcija vraća *ESP_OK* ako je uspješno postavljena opća konfiguracija ciljanog UART sklopa.

#### Postavi TX i RX pinove koji će sklop koristiti

```
esp_err_t uart_set_pin(uart_port_t uart_num, int tx_io_num, int rx_io_num, int rts_io_num, int cts_io_num)
```

Parametri ove funkcije su:

- *uart_num*
	- ciljani UART sklop, enumerator može imati vrijednosti:
		- *UART_NUM_0*
		- *UART_NUM_1*
		- *UART_NUM_2*
- *tx_io_num*
	- ciljani TX GPIO pin
- *rx_io_num*
	- ciljani RX GPIO pin
- *rts_io_num*
	- ciljani RTS GPIO pin, -1 ako se ne koristi
- *cts_io_num*
	- ciljani CTS GPIO pin, -1 ako se ne koristi

Funkcija vraća *ESP_OK* ako su uspješno postavljeni GPIO pinovi ciljanog UART sklopa. Ako se proslijede već hardverski zadani GPIO pinovi, koristit će se direktna konekcija (IO_MUX) umjesto GPIO matrice.

#### Postavi način rada sklopa

```
esp_err_t uart_set_mode(uart_port_t uart_num, uart_mode_t mode)
```

Parametri ove funkcije su:

- *uart_num*
	- ciljani UART sklop, enumerator može imati vrijednosti:
		- *UART_NUM_0*
		- *UART_NUM_1*
		- *UART_NUM_2*
- *mode*
	- način rada ciljanog sklopa, enumerator može imati vrijednosti:
		- *UART_MODE_UART* - obični način rada UART-a
		- *UART_MODE_RS485_HALF_DUPLEX* - half-duplex RS-485 UART standard način rada (kontrola uz pomoć RTS pina)
		- *UART_MODE_IRDA* - IRDA UART
		- *UART_MODE_RS485_COLLISION_DETECT*
		- *UART_MODE_RS485_APP_CTRL*

Funkcija vraća *ESP_OK* ako je uspješno postavljen način rada ciljanog UART sklopa.

#### Kopiraj podatke iz lokalnog međuspremnika u TX kružni međuspremnik i šalji podatke

```
int uart_write_bytes(uart_port_t uart_num, const void *src, size_t size)
```

Parametri ove funkcije su:

- *uart_num*
	- ciljani UART sklop, enumerator može imati vrijednosti:
		- *UART_NUM_0*
		- *UART_NUM_1*
		- *UART_NUM_2*
- *src*
	- adresa lokalnog međuspremnika iz kojeg će se kopirati podatci u TX kružni međuspremnik
- *size*
	- veličina međuspremnika iz kojeg će se kopirati podatci u TX kružni međuspremnik

Funkcija vraća broj bajtova koji su kopirani u TX kružni međuspremnik. Ako je TX kružni međuspremnik veličine 0, funkcija će se tek vratiti ako su poslani svi podatci. Ako je TX kružni međuspremnik veličinom veći od 0, funkcija će se vratiti iz izvođenja kad se kopiraju svi podatci u TX kružni međuspremnik. UART ISR će u pozadini kopirati podatke za slanje u TX FIFO međuspremnik. Funkcija također vraća -1 ako se dogodila neka greška.

#### Pričekaj dok se TX FIFO međuspremnik isprazni (dok su svi podatci poslani)

```
esp_err_t uart_wait_tx_done(uart_port_t uart_num, TickType_t ticks_to_wait)
```

Parametri ove funkcije su:

- *uart_num*
	- ciljani UART sklop, enumerator može imati vrijednosti:
		- *UART_NUM_0*
		- *UART_NUM_1*
		- *UART_NUM_2*
- *ticks_to_wait*
	- čekanje na događaj za takt sustava
	- ako je portMAX_DELAY onda je funkcija blokirajuća

Funkcija vraća *ESP_OK* ako je uspješno dočekan događaj. Ako je greška u parametru onda vraća *ESP_FAIL*. U slučaju da je isteklo vrijeme čekanja, a nije se pojavio događaj, vraća *ESP_ERR_TIMEOUT*.

#### Čekaj na podatke i kopiraj podatke iz RX kružnog međuspremnika u lokalni međuspremnik

```
int uart_read_bytes(uart_port_t uart_num, void *buf, uint32_t length, TickType_t ticks_to_wait)
```

Parametri ove funkcije su:

- *uart_num*
	- ciljani UART sklop, enumerator može imati vrijednosti:
		- *UART_NUM_0*
		- *UART_NUM_1*
		- *UART_NUM_2*
- *buf*
	- adresa lokalnog međuspremnika u kojeg će se kopirati podatci iz RX kružnog međuspremnika
- *length*
	- broj podataka koji je potrebno kopirati
- *ticks_to_wait*
	- čekanje na događaj za takt sustava
	- ako je portMAX_DELAY onda je funkcija blokirajuća

Funkcija vraća broj bajtova koji su kopirani iz RX kružnog međuspremnika. Funkcija također vraća -1 ako se dogodila neka greška. Tijekom dolaska podataka u RX FIFO međuspremnik, oni se kopiraju u RX kružni međuspremnik.

#### Počisti RX FIFO međuspremnik i odbaci sve podatke iz RX kružnog međuspremnika

```
esp_err_t uart_flush_input(uart_port_t uart_num
```

Parametri ove funkcije su:

- *uart_num*
	- ciljani UART sklop, enumerator može imati vrijednosti:
		- *UART_NUM_0*
		- *UART_NUM_1*
		- *UART_NUM_2*

Funkcija vraća *ESP_OK* ako je uspješno dočekan događaj. Ako je greška u parametru onda vraća *ESP_FAIL*.

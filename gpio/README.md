# GPIO

[GPIO (*eng. General Purpose Input Output*)](https://docs.espressif.com/projects/esp-idf/en/v5.3.1/esp32/api-reference/peripherals/gpio.html) su pinovi koji omogućuju fizičko povezivanje mikroupravljača i mikroračunala s drugim elektroničkim komponentama i uređajima. U slučaju ESP32 mikroupravljača, ESP32 nudi 34 GPIO pina gdje se mogu koristiti kao jednostavan ulazno-izlazni pin ili se alternativno mogu spojiti kao ulazno-izlazni pin nekog internog sklopa (primjerice UART, SPI, I2C ...).

Neki GPIO pinovi se koriste kao *strapping* pinovi gdje se prilikom *boota* ESP32-a očitavaju naponske razine i time namješta konfiguracija. Omogućuju fleksibilnost u radu jer se postavke aplikacije mogu mijenjati bez reprogramiranja čipa.

## Uobičajeni GPIO

Zaglavlje koje je potrebno uključiti za rad s osnovnim GPIO operacijama (*eng. GPIO API*) je ```driver/gpio.h```.

Pinovi imaju makro definicije oblika ```GPIO_NUM_[Broj]``` čija je vrijednost jednaka stvarnom broju pina.

### Osnovne GPIO funkcije

U nastavku su često korištene funkcije za upravljanje uobičajenim GPIO-ovima. Sve funkcije se nalaze [ovdje](https://docs.espressif.com/projects/esp-idf/en/v5.3.1/esp32/api-reference/peripherals/gpio.html#api-reference-normal-gpio).

#### Potpuna konfiguracija GPIO pina
```
esp_err_t gpio_config(const gpio_config_t *pGPIOConfig)
```

Parametar funkcije je sljedeći:

- *pGPIOConfig*
	- pokazivač na strukturu koja definira konfiguraciju

Funkcija vraća *ESP_OK* ako je konfiguracija valjana, u suprotnom vraća *ESP_ERR_INVALID_ARG*.

##### Struktura definicije konfiguracije

Struktura *gpio_config_t* sastoji se od sljedećih članova:

- *pin_bit_mask*
	- podatak tipa *uint64_t* koji definira bit masku na kojeg će se konfiguracija odnositi, svaki bit je jedan GPIO pin
- *mode*
	- podatak tipa *gpio_mode_t* koji definira smjer odabranih pinova: *GPIO_MODE_INPUT*, *GPIO_MODE_OUTPUT*, *GPIO_MODE_INPUT_OUTPUT*, ...
- *pull_up_en*
	- podatak tipa *gpio_pullup_t* koji definira je li omogućen *pull-up* otpornik: *GPIO_PULLUP_ENABLE* ili *GPIO_PULLUP_DISABLE*
- *pull_down_en*
	- podatak tipa *gpio_pulldown_t* koji definira je li omogućen *pull-down* otpornik: *GPIO_PULLDOWN_ENABLE* ili *GPIO_PULLDOWN_DISABLE*
- *intr_type*
	- podatak tipa ** koji definira na koji način će se okinuti prekid: *GPIO_INTR_DISABLE*, *GPIO_INTR_POSEDGE*, *GPIO_INTR_NEGEDGE*, *GPIO_INTR_ANYEDGE*, *GPIO_INTR_LOW_LEVEL* i *GPIO_INTR_HIGH_LEVEL*

#### Postavi GPIO pin u uobičajeno stanje
```
esp_err_t gpio_reset_pin(gpio_num_t gpio_num)
```

Parametar funkcije je sljedeći:

- *gpio_num*
	- broj pina koji se postavlja u uobičajeno stanje, najbolje proslijediti makro definiciju *GPIO_NUM_[Broj]* radi čitljivosti

Funkcija vraća *ESP_OK*.

#### Postavljanje smjera GPIO pina
```
esp_err_t gpio_set_direction(gpio_num_t gpio_num, gpio_mode_t mode)
```

Parametri funkcije su sljedeći:

- *gpio_num*
	- broj pina čiji se smjer postavlja, najbolje proslijediti makro definiciju *GPIO_NUM_[Broj]* radi čitljivosti
- *mode*
	- smjer GPIO pina, neki načini rada:
		- makro *GPIO_MODE_INPUT* - postavi pin kao ulaz
		- makro *GPIO_MODE_OUTPUT* - postavi pin kao izlaz
		- makro *GPIO_MODE_INPUT_OUTPUT* - postavi pin kao ulaz i izlaz

Funkcija vraća *ESP_OK* ako je odabran valjani pin, u suprotnom vraća *ESP_ERR_INVALID_ARG*.

#### Postavljanje stanja GPIO pina
```
esp_err_t gpio_set_level(gpio_num_t gpio_num, uint32_t level)
```

Parametri funkcije su sljedeći:

- *gpio_num*
	- broj pina čije se stanje postavlja, najbolje proslijediti makro definiciju *GPIO_NUM_[Broj]* radi čitljivosti
- *level*
	- 0 postavlja pin u logičko stanje 0 (niska naponska razina), 1 postavlja pin u logičko stanje 1 (visoka naponska razina)

Funkcija vraća *ESP_OK* ako je odabran valjani pin, u suprotnom vraća *ESP_ERR_INVALID_ARG*.

#### Čitanje stanja GPIO pina
```
int gpio_get_level(gpio_num_t gpio_num)
```

Parametar funkcije je sljedeći:

- *gpio_num*
	- broj pina čije se stanje čita, najbolje proslijediti makro definiciju *GPIO_NUM_[Broj]* radi čitljivosti

Funkcija vraća 0 ako je logičko stanje pina 0 (niska naponska razina) i 1 ako je logičko stanje pina 1 (visoka naponska razina). Ako je pin konfiguriran samo kao *GPIO_MODE_OUTPUT* povratna vrijednost ove funkcije (očitavanje stanja pina) će uvijek pokazivati 0 bez obzira ako je na izlazu pina 1.

#### Omogućivanje pull-up otpornika
```
esp_err_t gpio_pullup_en(gpio_num_t gpio_num)
```

Parametar funkcije je sljedeći:

- *gpio_num*
	- broj pina čiji se *pull-up* otpornik omogućava, najbolje proslijediti makro definiciju *GPIO_NUM_[Broj]* radi čitljivosti

Funkcija vraća *ESP_OK* ako je odabran valjani pin, u suprotnom vraća *ESP_ERR_INVALID_ARG*.

#### Onemogućivanje pull-up otpornika
```
esp_err_t gpio_pullup_dis(gpio_num_t gpio_num)
```

Parametar funkcije je sljedeći:

- *gpio_num*
	- broj pina čiji se *pull-up* otpornik onemogućava, najbolje proslijediti makro definiciju *GPIO_NUM_[Broj]* radi čitljivosti

Funkcija vraća *ESP_OK* ako je odabran valjani pin, u suprotnom vraća *ESP_ERR_INVALID_ARG*.

#### Omogućivanje pull-down otpornika
```
esp_err_t gpio_pulldown_en(gpio_num_t gpio_num)
```

Parametar funkcije je sljedeći:

- *gpio_num*
	- broj pina čiji se *pull-down* otpornik omogućava, najbolje proslijediti makro definiciju *GPIO_NUM_[Broj]* radi čitljivosti

Funkcija vraća *ESP_OK* ako je odabran valjani pin, u suprotnom vraća *ESP_ERR_INVALID_ARG*.

#### Onemogućivanje pull-down otpornika
```
esp_err_t gpio_pulldown_dis(gpio_num_t gpio_num)
```

Parametar funkcije je sljedeći:

- *gpio_num*
	- broj pina čiji se *pull-down* otpornik onemogućava, najbolje proslijediti makro definiciju *GPIO_NUM_[Broj]* radi čitljivosti

Funkcija vraća *ESP_OK* ako je odabran valjani pin, u suprotnom vraća *ESP_ERR_INVALID_ARG*.

### GPIO funkcije za upravljanje prekidima

Sljedeće funkcije su za konfiguraciju GPIO pinova kao pinove za prihvat prekida. Ovakvi prekidi se nazivaju vanjski hardverski prekidi. **Bitno je prvo registrirati prekidnu rutinu prije nego li se omoguće prekidi.**

Primjer korištenja GPIO pinova s prekidima se može naći u direktoriju [*gpio-interrupt*](gpio-interrupt).

#### Postavljanje karakteristika prekida
```
gpio_set_intr_type(gpio_num_t gpio_num, gpio_int_type_t intr_type)
```

Parametri funkcije su sljedeći:

- *gpio_num*
	- broj pina čije se karakteristike postavljaju, najbolje proslijediti makro definiciju *GPIO_NUM_[Broj]* radi čitljivosti
- *intr_type*
	- postavlja karakteristike prekida navedenog pina, neke moguće karakteristike:
		- makro *GPIO_INTR_DISABLE* - onemogući prekide
		- makro *GPIO_INTR_POSEDGE* - prekid na rastući brid
		- makro *GPIO_INTR_NEGEDGE* - prekid na padajući brid
		- makro *GPIO_INTR_ANYEDGE* - prekid na i rastući i padajući brid
		- makro *GPIO_INTR_LOW_LEVEL* - prekid na logičkoj 0
		- makro *GPIO_INTR_HIGH_LEVEL* - prekid na logičkoj 1

Funkcija vraća *ESP_OK* ako je odabran valjani pin, u suprotnom vraća *ESP_ERR_INVALID_ARG*.

#### Omogući prihvat prekida na GPIO pinu
```
esp_err_t gpio_intr_enable(gpio_num_t gpio_num)
```

Parametar funkcije je sljedeći:

- *gpio_num*
	- broj pina kojem će se prihvat prekida omogućiti, najbolje proslijediti makro definiciju *GPIO_NUM_[Broj]* radi čitljivosti

Funkcija vraća *ESP_OK* ako je odabran valjani pin, u suprotnom vraća *ESP_ERR_INVALID_ARG*.

#### Onemogući prihvat prekida na GPIO pinu
```
esp_err_t gpio_intr_disable(gpio_num_t gpio_num)
```

Parametar funkcije je sljedeći:

- *gpio_num*
	- broj pina kojem će se prihvat prekida onemogućiti, najbolje proslijediti makro definiciju *GPIO_NUM_[Broj]* radi čitljivosti

Funkcija vraća *ESP_OK* ako je odabran valjani pin, u suprotnom vraća *ESP_ERR_INVALID_ARG*.

#### Registracija prekidnih rutina

Dva su načina registracije prekidnih rutina i međusobno nisu kompatibilne:

- registracija globalne GPIO prekidne rutine za sve GPIO pinove koju je korisnik napisao, funkcija *gpio_isr_register(...)*, ...
- registracija već implementirane globalne GPIO prekidna rutine uz pomoć servisa koji nudi registraciju *handler* funkcija za svaki pin (**preferirano**)

##### Registracija servisa koji implementira globalnu GPIO prekidnu rutinu
```
esp_err_t gpio_install_isr_service(int intr_alloc_flags)
```

Parametar funkcije je sljedeći:

- *intr_alloc_flags*
	- zastavice koje opisuju karakteristike prekidne rutine
	- zastavice su definirane u ```esp_intr_alloc.h``` zaglavlju
	- ako se proslijedi 0, koriste se uobičajene postavke

Funkcija vraća *ESP_OK* ako je instalacija uspjela. Registrira već implementiranu GPIO prekidnu rutinu.

##### Odjava servisa koji implementira globalnu GPIO prekidnu rutinu
```
void gpio_uninstall_isr_service(void)
```

Funkcija ne vraća ništa. Odjavljuje već implementiranu GPIO prekidnu rutinu.

##### Registracija handler funkcije pina za globalnu GPIO prekidnu rutinu
```
esp_err_t gpio_isr_handler_add(gpio_num_t gpio_num, gpio_isr_t isr_handler, void *args)
```

Parametri funkcije su sljedeći:

- *gpio_num*
	- broj pina na kojeg se odnosi *handler* funkcija, najbolje proslijediti makro definiciju *GPIO_NUM_[Broj]* radi čitljivosti*
- *isr_handler*
	- *handler* funkcija koja će se izvršiti kad se dogodi prekid na navedenom pinu
- *args*
	- parametar funkcije

Funkcija vraća *ESP_OK* ako je registracija funkcije uspjela.

##### Odjava handler funkcije pina za globalnu GPIO prekidnu rutinu
```
esp_err_t gpio_isr_handler_remove(gpio_num_t gpio_num)
```

Parametri funkcije su sljedeći:

- *gpio_num*
	- broj pina na kojeg se odnosi *handler* funkcija, najbolje proslijediti makro definiciju *GPIO_NUM_[Broj]* radi čitljivosti*

Funkcija vraća *ESP_OK* ako je odjava funkcije uspjela.

### Općenito konfiguriranje pinova

Proces postavljanje općenite konfiguracije pinova:

- resetiranje i postavljanje konfiguracije pinova uz pomoć *gpio_config(...)* funkcije ili postojećih pojedinačnih funkcija (*gpio_reset_pin(...)*), primjerice:
	- smjer pinova
	- omogućeni ili neomogućeni *pull-up* ili *pull-down* otpornici na pinovima
	- omogućeni ili neomogućeni prekidi na pinovima

- izvršavanje operacija, primjerice:
	- postavljanje stanja na izlazu pinova
	- čitanje s pinova

Proces uklanjanja konfiguracije pinova:

- ako postoje omogućeni prekidi na pinovima onemogućiti ih

- resetiranje konfiguracije pinova uz pomoć *gpio_config(...)* funkcije ili pojedinačne funkcije (*gpio_reset_pin(...)*)

### Postavljanje i uklanjanje prekidnih pinova

Proces postavljanja prekida je sljedeći:

- resetiranje i postavljanje konfiguracije pinova uz pomoć *gpio_config(...)* funkcije ili postojećih pojedinačnih funkcija (*gpio_reset_pin(...)*, *gpio_set_direction(...)*, *gpio_set_intr_type(...)* ...):
	- pin mora biti postavljen kao ulazni
	- način okidanja prekida na pinu (rastući brid, padajući brid, ...) mora bit postavljen
	- po želji omogućiti *pull-up* ili *pull-down*

- registracija već gotove implementacije prekidne rutine za globalni GPIO prekid (*gpio_install_isr_service(...)*)

- registracija *handler* funkcije za ciljane pinove (*gpio_isr_handler_add(...)*)

- omogućivanje prekidnog signala na ciljanim pinovima (*gpio_intr_enable(...)*)

Proces uklanjanja prekida je sljedeći:

- onemogućivanje prekidnog signala na ciljanim pinovima (*gpio_intr_disable(...)*)

- odjava *handler* funkcije za ciljane pinove (*gpio_isr_handler_remove(...)*)

- odjava registrirane prekidne rutine za globalni GPIO prekid (*gpio_uninstall_isr_service()*)

- resetiranje pina uz pomoć *gpio_config(...)* ili pojedinačne funkcije (*gpio_reset_pin(...)*)

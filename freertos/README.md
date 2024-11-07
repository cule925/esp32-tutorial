# FREERTOS

[FreeRTOS](https://docs.espressif.com/projects/esp-idf/en/v5.3.1/esp32/api-reference/system/freertos.html) je operacijski sustav za rad u stvarnom vremenu. Integriran je kao dio ESP-IDF-a kao komponenta. Komponente su softverski moduli koji pružaju sučelje za izgradnju aplikacija za ESP mikroupravljače.

[Originalni MIT licencirani FreeRTOS](https://www.freertos.org/) nema potporu za simetrično multiprocesiranje (*eng. Symmetric Multiprocessing*) što postaje problem za ESP-ove čiji SoC-ovi imaju više nego jednu procesorsku jezgru. Stoga je Espressif pružio vlastitu implementaciju FreeRTOS-a koja je dostupna kao komponenta u ESP-IDF-u. ESP-IDF FreeRTOS podupire ima SMP potporu za maksimalno dvije procesorske jezgre.

U originalnoj verziji FreeRTOS-a, konfiguracija raznih dijelova jezgre FreeRTOS-a moguća je uređujući zaglavlje *FreeRTOSConfig.h* gdje se nalaze C makroi. Međutim u ESP-IDF-ovoj implementaciji FreeRTOS-a se *FreeRTOSConfig.h* ne uređuje, već se koristi ```idf.py menuconfig``` koja sprema konfiguraciju u datoteku *sdkconfig*. Kada se pokrene izgradnja projekta, izgradit će se *sdkconfig.h* koja će sadržavati navedene definicije u obliku makroa pri izlazne binarne datoteke.

Prilikom postavljanja jednojezgrenog načina rada FreeRTOS-a ("Run FreeRTOS only on first core"), generirat će se nova *sdkconfig* datoteke (dok će se zvati *sdkconfig.old* kao sigurnosna kopija i neće se uzimat u obzir). Pregledavajući datoteku *sdkconfig* može se uočiti da je postavljeno:

```
CONFIG_FREERTOS_UNICORE=y
```

Postavljanje ESP-IDF-ovog FreeRTOS-a u [jednojezgreni način rada](https://docs.espressif.com/projects/esp-idf/en/v5.3.1/esp32/api-reference/system/freertos_idf.html#freertos-idf-single-core) dovodi do sljedećih izmjena:

- sve operacije koje se izvode u jezgri FreeRTOS-a su determinističke
- standardni algoritam vremenskog raspoređivanja procesa je vraćen (npr. *Round Robin*)
- svi podatci vezani za SMP su uklonjeni u konačnoj binarnoj datoteci, iako će se SMP API funkcije i dalje moći pozivati, ali neće ništa činiti po tom pitanju

Dakle, opcije vezane za FreeRTOS se postavljaju ```idf.py meniconfig``` naredbom (ili u VSCodeu - tipka *F1* pa upisivanjem "ESP-IDF: SDK Configuration Editor (Menuconfig)").

Kod pisanja koda poželjno je pridržavati se [konvencija o imenovanju varijabli i funkcija za FreeRTOS](https://www.freertos.org/Documentation/02-Kernel/06-Coding-guidelines/02-FreeRTOS-Coding-Standard-and-Style-Guide), ali nije obavezno.

## Korištenje ESP-IDF FreeRTOS-a

Zaglavlje koje je potrebno uključiti za rad s FreeRTOS-om: ```freertos/FreeRTOS.h```.

ESP-IDF-ov FreeRTOS je zadani operacijski sustav za ESP32 mikroupravljače. Dakle, nije potrebno kao kod običnog FreeRTOS operacijskog sustava pozivati funkcije za pokretanje i zaustavljanje raspoređivača funkcijama ```vTaskStartScheduler()``` i ```vTaskStopScheduler()```.

### Već pokrenuti pozadinski zadatci

Nakon pokretanja raspoređivača, FreeRTOS jezgra automatski stvara par procesa (odnosno zadataka):

- *Idle Task (IDLEx)*
	- proces koji ništa ne radi
	- svakoj jezgri je dodijeljen, dvojezgreni način rada: *IDLE0* na jezgri 0 i *IDLE1* na jezgri 1, kod jednojezgrenog načina rada *IDLE* na jezgri 0
	- veličina stoga zadataka se može urediti u *sdkconfig* datoteci uređujući *CONFIG_FREERTOS_IDLE_TASK_STACKSIZE stavku* (*menuconfig*)
	- ovi zadatci imaju najniži prioritet, odnosno prioritet 0

- *FreeRTOS Timer Task (Tmr Svc)*
	- ako postoje Timer API pozivi u aplikaciji, ovaj zadatak se pokreće
	- veličina stoga zadatka se može urediti u *sdkconfig* datoteci uređujući CONFIG_FREERTOS_TIMER_TASK_STACK_DEPTH (*menuconfig*)
	- afinitet zadatka je jezgra 0
	- prioritet zadatka se može urediti u *sdkconfig* datoteci uređujući CONFIG_FREERTOS_TIMER_TASK_PRIORITY (*menuconfig*)

- *Main Task (main)*
	- zadatak koji poziva *app_main*
	- zadatak će se sam obrisati kada *app_main* vrati neku vrijednost
	- veličina stoga zadatka se može urediti u *sdkconfig* datoteci uređujući CONFIG_ESP_MAIN_TASK_STACK_SIZE (*menuconfig*)
	- afinitet zadatka određenoj jezgri se može urediti u *sdkconfig* datoteci uređujući CONFIG_ESP_MAIN_TASK_AFFINITY (*menuconfig*)
	- prioritet ovog zadatka je 1
	
- *IPC Tasks (ipcx)*
	- stvara se u dvojezgrenom načinu rada: *ipx0* na jezgri 0 i *ipx1*
	- *IPC* zadatak omogućuje pozive funkcija koje će se izvršavati na drugoj jezgri (jezgri na kojoj se on ne izvršava)
	- veličina stoga zadatka se može urediti u *sdkconfig* datoteci uređujući CONFIG_ESP_IPC_TASK_STACK_SIZE (*menuconfig*)
	- prioriteti ovih zadataka je 24

- *ESP Timer Task*
	- procesuira ESP Timer *callback* funkcije
	- - veličina stoga zadatka se može urediti u *sdkconfig* datoteci uređujući CONFIG_ESP_TIMER_TASK_STACK_SIZE (*menuconfig*)
	- afinitet zadatka je jezgra 0
	- prioriteti ovog zadatka je 22

Ulazna točka (*eng. entrypoint*) aplikacije mora se zvati **app_main** koja se poziva iz navedenog zadatka *main*.

### Implementacija gomile

Za razliku od originalnog FreeRTOS-a, ESP-IDF-ov FreeRTOS koristi [vlastitu implementaciju gomile](https://docs.espressif.com/projects/esp-idf/en/v5.3.1/esp32/api-reference/system/mem_alloc.html) što znači da nema biranja između implementacija koje pruža originalni FreeRTOS.

Funkcija za alokaciju memorije na gomili je:
```
void *heap_caps_malloc(size_t size, uint32_t caps)
```

Funkcija za realokaciju memorije na gomili je:
```
void *heap_caps_realloc(void *ptr, size_t size, uint32_t caps)
```

Funkcija za oslobađanje memorije na gomili je:
```
void heap_caps_free(void *ptr)
```

### Simetrično multiprocesiranje i ESP32

[Simetrično multiprocesiranje](https://docs.espressif.com/projects/esp-idf/en/v5.3.1/esp32/api-reference/system/freertos_idf.html#symmetric-multiprocessing) je računalna arhitektura gdje su dvije ili više procesorskih jezgri povezane na jednu zajedničku glavnu memoriju. Općenito, svaka jezgra ima svoje registre, prekide i prekidne rutine. Raspoređivanjem procesa odnosno zadataka na ovim jezgrama upravlja operacijski sustav.

Neki modeli ESP32 serija imaju dvojezgrene procesore (jezgra 0 i jezgra 1). Ove jezgre dijele skoro pa svu memoriju i mogu se međusobno prekidati.

Tipično u aplikacijama koje se pokreću na ESP32 mikroupravljačima s dvije jezgre, jezgra 0 ima ulogu procesiranja protokolnih zadataka (Wi-Fi, Bluetooth ...) dok jezgra 1 ima ulogu procesiranja ostatak aplikacije. U takvoj konfiguraciji jezgra 0 ima naziv *PRO_CPU*, a jezgra 2 *APP_CPU*

### Zadatci

Zaglavlje koje je potrebno uključiti za rad sa zadatcima (*eng. Task API*): ```freertos/task.h```.

[Zadatak](https://docs.espressif.com/projects/esp-idf/en/v5.3.1/esp32/api-reference/system/freertos_idf.html#tasks) je sličan procesu ili dretvi, neovisni niz instrukcija koja se može pokrenuti, pauzirati ili zaustaviti. Primjer korištenja zadatka se može naći u direktoriju [*task-blink*](task-blink).

#### Stanja zadataka

FreeRTOS zadatak može biti u jednom od [sljedećih stanja](https://www.freertos.org/Documentation/02-Kernel/02-Kernel-features/01-Tasks-and-co-routines/02-Task-states):

- *RUNNING*
	- zadatak se izvršava na procesorskoj jezgri
- *READY*
	- zadatak je spreman za izvođenje (čeka u redu zadataka), na raspoređivaču je da ga pokrene u jezgri procesora
- *BLOCKED*
	- zadatak je blokiran i čeka na neki događaj (istek timera, oslobađanje resursa)
- *SUSPENDED*
	- zadatak koji je eksplicitno zaustavljen od drugog zadatka (ili samog sebe)

Shema stanja zadataka:

```
                                      +-----------+
        +---------------------------> | SUSPENDED | <--------------------------+
        |                             +-----------+       vTaskSuspend()       |
        |                               A       |                              |
        |                vTaskSuspend() |       | vTaskResume()                |
        |                               |       V                              |
        |                             +-----------+ -------------------> +-----------+
        |        *task created* ----->|   READY   |                      |  RUNNING  |
        |                             +-----------+ <------------------- +-----------+
        |                                   A                                  |
        |                                   | *event*                          |
        |                                   |           *blocking function     |
        |        vTaskSuspend()       +-----------+           called*          |
        +---------------------------- |  BLOCKED  | <--------------------------+
                                      +-----------+
```

#### Raspoređivanje zadataka u FreeRTOS-u

Originalni FreeRTOS ima raspoređivač koji preemptivno raspoređuje zadatke s fiksnim prioritetima dodjeljujući im vremenske intervale izvršavanja.

Ovo znači da:

- fiksni prioritet
	- zadatci dobivaju fiksni prioritet prije izvršavanja

- preemptivno raspoređivanje
	- raspoređivač može promijeniti zadatak koji se izvršava na jezgri bez njegove pomoći (uz samo-suspenzije ili samo-blokade) ako se pojavi zadatak s višim prioritetom

- dodijeljeni vremenski interval
	- zadatci istog prioriteta imaju dodijeljen vremenski interval izvođenja te se izmjenjuju na jezgri *Round Robin* algoritmom

ESP-IDF-ov FreeRTOS koji koristi SMP raspoređivanje ima slične karakteristike, uz malih izmjena:

- fiksni prioritet
	- uzmimo tri zadatka; *A*, *B* i *C* s prioritetima *10*, *9* i *8* i afinitetima *0*, *0*, *1*
	- zadatak *A* će se izvršavati na jezgri *0* (*RUNNING*), dok će se zadatak *C* izvršavati na jezgri 1 (*RUNNING*)
	- zadatak *B* će biti u stanju *READY* dok se zadatak a ne blokira ili suspendira

- preemptivno raspoređivanje
	- uzmimo tri zadatka; *A*, *B* i *C* s prioritetima *8*, *9* i *10* i afinitetima *0*, *1*, *X* (*nema afiniteta*)
	- zadatak *C* je blokiran (*BLOCKED*)
	- zadatak *A* će se izvršavati na jezgri *0* (*RUNNING*), dok će se zadatak *B* izvršavati na jezgri *1* (*RUNNING*)
	- zadatak *B* odblokirava *C*, *C* ima viši prioritet od *A* i od *B*
	- raspoređivač bira jezgru zadatka koji je odblokirao blokirani zadatak, dakle jezgru *1*
	- *B* ide u stanje *READY* dok *C* počinje s izvršavanjem na jezgri *1* (*RUNNING*)

- dodijeljeni vremenski interval
	- *Round Robin* algoritam nije ovdje moguć zato što zadatak može imati afinitet prema jezgri ili se zadatak koji nema afinitet već pokreće na drugoj jezgri
	- koristi se *Best Effort Round Robin*

##### Best Effort Round Robin

*Best Effort Round Robin* je algoritam dodjeljivanja vremenskog isječka zadatcima na dvojezgrenim procesorima u ESP-IDF-ovom FreeRTOS-u. Kao primjer, može se uzmimo četiri zadatka koji imaju jednaki najviši prioritet *A*, *B*, *C*, *D* i njihove afinitete prema jezgrama *X*, *0*, *1*, *1*. U redu čekanja mogu se zapisati kao:

```
AX , B0 , C1 , D0
```

Jezgre u početku ne izvršavaju ni jedan FreeRTOS zadatak. Međutim, jezgra 0 dobiva vremenski prekid (istek vremenskog intervala) za promjenu zadatka. Bira se prvi u listi odnosno *AX*. *AX* nema afinitet pa ga raspoređivač može dodijeliti jezgri 0. Zadatak *AX* se stavlja na kraj reda:

```
              [0]
B0 , C1 , D0 , AX
```

Jezgra 1 dobiva vremenski prekid. Raspoređivač pokušava uzeti zadatak *B*, ali zadatak ima afinitet prema jezgri 0. Raspoređivač gleda sljedeći zadatak po što je zadatak *C* koji ima afinitet prema jezgri 1. Raspoređivač mu dodjeljuje jezgru 1 i stavlja ga na kraj reda:

```
         [0]  [1]
B0 , D0 , AX , C1
```

Jezgra 0 dobiva vremenski prekid. Raspoređivač uzima zadatak *B* i dodjeljuje mu jezgru 1 te ga stavlja na kraj reda:

```
         [1]  [0]
D0 , AX , C1, B0
```

Jezgra 1 dobiva vremenski prekid. Raspoređivač pokušava uzeti zadatak *D*, ali zadatak ima afinitet prema jezgri 0. Raspoređivač gleda sljedeći zadatak po što je zadatak *A* koji nema afinitet prema ni jednoj jezgri. Raspoređivač mu dodjeljuje jezgru 1 i stavlja ga na kraj reda:

```
         [0]  [1]
D0 , C1, B0 , AX

```

##### Vremenski prekid jezgre

Vremenski prekid jezgre (*eng. tick interrupt*) služi za zamjenu trenutnog zadatka koji se izvodi na jezgri i još za:

- inkrementaciju brojača takta raspoređivača
- odblokiranje blokiranih zadataka koji su čekali na prolaz nekog vremena (primjerice pauza 10 sekundi)
- provjera je li potrebna zamjena konteksta (zadatka)
- izvršenje rutine vremenskog prekida jezgre (*eng. tick hook*) odnosno pozivanje raspoređivača

Svaka jezgra dobiva svoje vremenske prekide, ali dijele iste redove zadataka u memoriji kao što se vidjelo iz primjera *Best Effort Round Robin* raspoređivanja. Međutim jezgra 1 samo provjerava je li potrebna zamjena konteksta i izvršava rutinu dok jezgra 0 radi sve gore navedeno.

##### Idle zadatak

*Idle* zadatak ima najniži prioritet i pokreće se na jezgri samo kad ni jedan drugi zadatak višeg ili jednakog prioriteta nije u stanju *READY*. Svaka jezgra dobiva svoj *Idle* zadatak i zadaća im je:

- oslobađanje memorije obrisanih zadataka
- izvršavanje *Idle* rutine (*eng. Idle hook*)

#### Stvaranje zadatka

Stvaranje zadatka u jednojezgrenim ESP32 mikroupravljačima moguće je funkcijom:

```
static inline BaseType_t xTaskCreate(TaskFunction_t pxTaskCode, const char *const pcName, const configSTACK_DEPTH_TYPE usStackDepth, void *const pvParameters, UBaseType_t uxPriority, TaskHandle_t *const pxCreatedTask)
```

Parametri funkcije su sljedeći:

- *pxTaskCode*
	- pokazivač na funkciju koju zadatak izvršava
	- funkcija mora biti beskonačna petlja, ne smije izaći iz rada
- *pcName*
	- niz znakova koji daje deskriptivno ime zadatku
- *usStackDepth*
	- veličina stoga zadatka u bajtovima
- *pvParameters*
	- pokazivač na parametar koji će se proslijediti funkciji zadatka
- *uxPriority*
	- prioritet zadatka
- *pxCreatedTask*
	- prosljeđuje se *handle* uz pomoć kojega se može referirati na zadatak

Povratna vrijednost ove funkcije u slučaju uspješnog stvaranja zadatka je *pdPASS*, u suprotnom je jedan od kodova grešaka definiranih u *projdefs.h*.

Stvaranje zadatka u dvojezgrenim ESP32 mikroupravljačima moguće je funkcijom:

```
BaseType_t xTaskCreatePinnedToCore(TaskFunction_t pxTaskCode, const char *const pcName, const uint32_t ulStackDepth, void *const pvParameters, UBaseType_t uxPriority, TaskHandle_t *const pxCreatedTask, const BaseType_t xCoreID)
```

Parametri funkcije su sljedeći:

- *pxTaskCode*
	- pokazivač na funkciju koju zadatak izvršava
	- funkcija mora biti beskonačna petlja, ne smije izaći iz rada
- *pcName*
	- niz znakova koji daje deskriptivno ime zadatku
- *usStackDepth*
	- veličina stoga zadatka u bajtovima
- *pvParameters*
	- pokazivač na parametar koji će se proslijediti funkciji zadatka
- *uxPriority*
	- prioritet zadatka
- *pxCreatedTask*
	- *handle* zadatka koji služi kao referenca na zadatak
- *xCoreID*
	- jezgra prema kojoj zadatak ima afinitet (0 ili 1)
	- u slučaju da zadatak nema afinitet prema ni jednoj jezgri, potrebno je proslijediti *tskNO_AFFINITY*

Povratna vrijednost ove funkcije u slučaju uspješnog stvaranja zadatka je *pdPASS*, u suprotnom je jedan od kodova grešaka definiranih u *projdefs.h*.

#### Brisanje zadatka

##### Stvaranje zadatka moguće je funkcijom
```
void vTaskDelete(TaskHandle_t xTaskToDelete)
```

- *xTaskToDelete*
	- *handle* zadatka koji se briše
	- ako je proslijeđen *NULL* to znači da zadatak briše sam sebe

Nema povratne vrijednosti.

#### Broj taktova

##### Broj taktova od početka rada raspoređivača
```
TickType_t xTaskGetTickCount(void)
```

Povratna vrijednost ove funkcije je broj taktova od početka rada raspoređivača.

##### Broj taktova od početka rada raspoređivača iz prekidne rutine
```
TickType_t xTaskGetTickCountFromISR(void)
```

Povratna vrijednost ove funkcije je broj taktova od početka rada raspoređivača.

#### Relativna vremenska odgoda

##### Relativna vremenska odgoda nekog zadatka
```
void vTaskDelay(const TickType_t xTicksToDelay)
```

- *xTicksToDelay*
	- odgađa zadatak za takt sustava
	- ako se želi dobit broj sekundi za kojih se želi odgoditi zadatak, potrebno je umjesto broj taktova proslijedit ```*broj taktova* / portTICK_PERIOD_MS```

Nema povratne vrijednosti. Ovakva odgoda odgađa zadatak relativno od pozivanja do broj taktova. Kada bi primjerice željeli da se neki isječak koda u zadatku ponavlja periodično 1000 taktova, taj period ne bi bio idealan već bi samo ponavljanje izvršavanja ponavljajućeg koda i ponovnog postavljanja vremena uvelo dodatne taktove:

```
        ... |  *wait for 1000 ticks* | *task ticks* | *wait for 1000 ticks* | *task ticks* | *wait for 1000 ticks* | *task ticks* | *wait for 1000 ticks* | *task ticks* | ...
```

Rješenje za ovaj problem je apsolutna vremenska odgoda.

#### Apsolutna vremenska odgoda

##### Apsolutna vremenska odgoda nekog zadatka
```
BaseType_t xTaskDelayUntil(TickType_t *const pxPreviousWakeTime, const TickType_t xTimeIncrement)
```

- *pxPreviousWakeTime*
	- pokazivač na varijablu koja pokazuje prethodni broj taktova od kada je zadatak bio odblokiran
	- kada se zadatak odblokira, vrijednost na kojeg ovaj pokazivač pokazuje inkrementira se za vrijednost varijable *xTimeIncrement*
- *xTimeIncrement*
	- broj taktova za koji treba odgoditi zadatak

Povratna vrijednost je *pdTRUE* ako je zadatak uspješno odgođen, u suprotnom je povratna vrijednost *pdFALSE*. Riješeni prethodni primjer s vremenskom odgodom od 1000 takta:

```
        ... |  *task and then wait for leftover  |  *task and then wait for leftover  |  *task and then wait for leftover  |  *task and then wait for leftover  | ...
        ... |  ticks (1000 ticks - task ticks)*  |  ticks (1000 ticks - task ticks)*  |  ticks (1000 ticks - task ticks)*  |  ticks (1000 ticks - task ticks)*  | ...
```

#### Suspendiranje

##### Suspendiranje zadatka
```
void vTaskSuspend(TaskHandle_t xTaskToSuspend)
```

- *xTaskToSuspend*
	- *handle* zadatka koji se suspendira
	- ako je proslijeđen *NULL* to znači da zadatak suspendira sam sebe

Nema povratne vrijednosti. Za pozivanje ove funkcije definicija *INCLUDE_vTaskSuspend* mora biti postavljena u 1 u *FreeRTOSConfig.h* zaglavlju. U slučaju ove funkcije više puta nad zadatkom, dovoljno je pozvati jedan ```vTaskResume()```, dakle ne "pamte" se ```vTaskSuspend()``` pozivi.

##### Suspendiranje raspoređivača
```
void vTaskSuspendAll(void)
```

Nema povratne vrijednosti. Suspendira raspoređivač, ne zadatke (**prekidi su i dalje omogućeni**). Zadatak koji se trenutno pokretao na jezgri će se izvršavati sve dok se ponovno ne pokrene raspoređivač odnosno neće biti zamjene konteksta sve dok se raspoređivač ne pokrene. Brojanje taktova i dalje radi (jer se inkrementacija brojača radi u prekidnoj rutini) tako da će se svi vremenski odgođeni zadatci biti na vrijeme odgođeni i odblokirani ako se raspoređivač pokrene prije njihovih vremenskih odgoda.

#### Pokretanje iz suspenzija

##### Pokretanje jednog zadatka iz suspenzije
```
void vTaskResume(TaskHandle_t xTaskToResume)
```

- *xTaskToResume*
	- *handle* zadatka koji se pokreće iz suspenzije

Nema povratne vrijednosti. Za pozivanje ove funkcije mora se postaviti *INCLUDE_vTaskSuspend* u *FreeFreeRTOSConfigRTOS.h* zaglavlju.

##### Pokretanje jednog zadatka iz suspenzije iz prekidne rutine
```
BaseType_t xTaskResumeFromISR(TaskHandle_t xTaskToResume)
```

- *xTaskToResume*
	- *handle* zadatka koji se pokreće iz suspenzije

Povratna vrijednost ove funkcije u slučaju uspješnog pokretanja zadatka je *pdTRUE*, inače je *pdPASS*. Za pozivanje ove funkcije mora se postaviti *INCLUDE_xTaskResumeFromISR* u *FreeRTOSConfig.h* zaglavlju.

##### Pokretanje raspoređivača
```
BaseType_t xTaskResumeAll(void)
```

Povratna vrijednost ove funkcije je *pdTRUE* ako se pri pokretanju raspoređivača odmah dogodila zamjena konteksta, inače je *pdFalse*. Pokreće raspoređivač, ne zadatke. Zamjena konteksta je omogućena te će se odblokirati zadatci čija je vremenska odgoda prošla jer je brojač takta radio u pozadini iako je raspoređivač bio neaktivan (jer se inkrementacija brojača radi u prekidnoj rutini).

#### Forsiranje promjene konteksta

##### Forsiranje promjene konteksta zadatka
```
taskYIELD(void)
```

Ovo je makro. Forsira trenutni zadatak na poziv raspoređivača.

### Korištenje realnih brojeva tijekom izmjene konteksta

Kada se na jezgri gdje se izvodi zadatak dogodi izmjena konteksta, vrijednost jezgrinih registara trenutnog zadatka se spremaju u memoriju te se učitavaju nove vrijednosti u jezgrine registre sljedećeg zadatka. Međutim, u slučaju registara jedinice za obradu realnih brojeva (*eng. Floating Point Unit - FPU*) u jezgri to se ne događa pod nekim uvjetima. FreeRTOS implementira *Lazy Context Switching* koji ne sprema stanja registara *FPU-a* odmah u memoriju sve dok raspoređivač odluči da neki drugi zadatak zamijeni trenutni na trenutnoj jezgri. Dakle, prilikom vremenskog prekida jezgre svi registri jezgre osim *FPU* registara se kopiraju u memoriju (spremanje konteksta). Ako raspoređivač odluči da se trenutni zadatak još može izvoditi, zadatak se samo nastavlja izvoditi. U slučaju da raspoređivač odluči maknuti trenutni zadatak iz jezgre, naposljetku se kopiraju i *FPU* registri te se postavlja novi kontekst u registrima jezgre uključujući i *FPU*.

ESP-IDF FreeRTOS **automatski postavlja afinitet nekog zadatka koji koristi *float* tipove nekoj jezgri** (iako mu inicijalno možda i nije bio postavljen) baš zbog *Lazy Context Switching* značajke jer kopiranje iz *FPU-a* jedne jezgre u *FPU* druge jezgre nije izvedivo. Također, ESP-IDF-ov FreeRTOS zadano ne podržava korištenje *FPU-a* u prekidnim rutinama, ali to se može promijeniti izmjenom *CONFIG_FREERTOS_FPU_IN_ISR* (*menuconfig*). Međutim, korištenje *FPU-ova* je samo dozvoljeno u prekidima razine 1 (najprioritetniji prekidi).

### Prekidi i izvođenje kritičnih sekcija

Prekidi koji koje se mogu maskirati mogu se omogućiti i onemogućiti makroima ```taskDISABLE_INTERRUPTS()``` i ```taskENABLE_INTERRUPTS()```.

Međutim, u SMP ESP32 sustavima, ovo će upravljati prekidima samo trenutne jezgre odakle su ovi makroi pozvani. Potrebno je koristiti mehanizam sinkronizacije. U ovom slučaju će se koristiti zaključavanje s radnim čekanjem.

Mehanizam s radnim čekanjem je instanca ```portMUX_TYPE``` strukture podatka. Alokacija i inicijalizacija ove strukture moguća je na sljedeći način:

```
static portMUX_TYPE my_spinlock = portMUX_INITIALIZER_UNLOCKED;
```

Moguća je i dinamična alokacija i inicijalizacija navedene strukture podataka:

```
portMUX_TYPE *my_spinlock = malloc(sizeof(portMUX_TYPE));
portMUX_INITIALIZE(my_spinlock);
```

Ulaz u kritične sekcije i onemogućavanje prekide iz običnog zadatka radi se makroom:
```
taskENTER_CRITICAL(&spinlock)
```

Ulaz u kritičnu sekciju iz prekidne rutine radi se makroom:
```
taskENTER_CRITICAL_ISR(&spinlock)
```

Jezgra će onemogućiti svoje prekide koji se mogu maskirati te ugnježđivanje prekida do prioriteta ```configMAX_SYSCALL_INTERRUPT_PRIORITY```. Zatim će jezgra raditi beskonačnu petlju i u toj petlji radit atomarnu operaciju "dohvati i usporedi" i pokušati postaviti sebe kao vlasnika navedene strukture (vlasnik strukture je identifikator jezgre - 0 ili 1). Kada jezgra postavi sebe kao vlasnika te strukture, ona izvršava svoju kritičnu sekciju te na kraju uklanja sebe kao vlasnika navedene strukture i omogućava svoje prekide koji se mogu maskirati te ugnježđivanje prekida.

Kritične sekcije u ovom slučaju moraju biti što je kraće moguće. U ovakvim kritičnim sekcijama gdje se upravlja prekidima, FreeRTOS API se ne smije zvati. Također, ne smiju se zvati nikakve blokirajuće funkcije jer će tako zablokirati cijeli sustav.

Općenito omogućivanje prekida koji se mogu maskirati radi se makroom:
```
taskENABLE_INTERRUPTS(void)
```

Općenito onemogućivanje prekida koji se mogu maskirati prekida radi se makroom:
```
taskDISABLE_INTERRUPTS(void)
```

### Komunikacijski i sinkronizacijski mehanizmi

FreeRTOS nudi razne komunikacijske i sinkronizacijske mehanizme koji omogućuju zadatcima međusobnu razmjenu podataka i koordinaciju rada. Ovi mehanizmi su ključni za pravilno upravljanje resursima i izbjegavanje konflikata okruženjima stvarnog vremena.

#### Red

[Red (*eng. queue*)](https://freertos.org/Documentation/02-Kernel/02-Kernel-features/02-Queues-mutexes-and-semaphores/01-Queues) je osnovni način komunikacije između zadataka. Red ima određenu veličinu te su čitanje i pisanje dretveno sigurne (*thread safe*) operacije. Red najčešće koristi princip FIFO (*eng. First In First Out*) što znači se elementi stavljaju (pišu) u red na kraj, a uzimaju s početka (čitaju), iako je moguće staviti elemente odmah na početak. Operacije pisanja i čitanja u redu mogu biti blokirajuće i neblokirajuće.

Zaglavlje koje je potrebno uključiti za rad s redovima (*eng. Queue API*) je ```freertos/queue.h```.

Primjer korištenja zadatka se može naći u direktoriju [*tasks-queue*](tasks-queue).

##### Stvaranje reda
```
xQueueCreate(uxQueueLength, uxItemSize)
```

Parametri makroa su sljedeći:

- *uxQueueLength*
	- maksimalni broj elemenata kojih red može držati
- *uxItemSize*
	- veličina elementa u bajtovima

Makro vraća *handle* tipa *QueueHandle_t* na novostvoreni red, a ako se red ne može stvoriti vraća 0.

##### Brisanje reda
```
void vQueueDelete(QueueHandle_t xQueue)
```

Parametar funkcije je sljedeći:

- *xQueue*
	- *handle* na red koji se briše

Nema povratne vrijednosti.

##### Slanje elementa na kraj reda
```
xQueueSendToBack(xQueue, pvItemToQueue, xTicksToWait)
```

Parametri makroa su sljedeći:

- *xQueue*
	- *handle* tipa *QueueHandle_t* na red u koji se piše (kopiranjem)
- *pvItemToQueue*
	- pokazivač na element koji će se staviti na kraj reda (kopiranjem)
- *xTicksToWait*
	- vrijeme čekanja dok se u redu ne pojavi prazno mjesto za pisanje, tipa *TickType_t*
	- ako je 0 onda je makro neblokirajući u slučaju punog reda, zadatak nastavlja se radom iako mu nije uspjelo pisati element u red
	- ako je *portMAX_DELAY* onda je blokirajuć, zadatak čeka sve dok u redu ne bude mjesta

Makro vraća *pdTRUE* (tip *BaseType_t*) ako je element uspješno umetnut na kraj reda, inače vraća *errQUEUE_FULL*

##### Slanje elementa na kraj reda iz prekidne rutine
```
xQueueSendToFrontFromISR(xQueue, pvItemToQueue, pxHigherPriorityTaskWoken)
```

Parametri makroa su sljedeći:

- *xQueue*
	- *handle* tipa *QueueHandle_t* na red u koji se piše (kopiranjem)
- *pvItemToQueue*
	- pokazivač na element koji će se staviti na kraj reda (kopiranjem)
- *pxHigherPriorityTaskWoken*
	- pokazivač na varijablu tipa *BaseType_t*
	- inicijalnu vrijednost je najbolje postaviti na *pdFALSE*
	- makro postavlja ovu vrijednost u *pdTRUE* ako je svojim slanjem elementa u red oslobodio zadatak koji ima veći prioritet od trenutnog zadatka koji se izvršava

Makro vraća *pdTRUE* (tip *BaseType_t*) ako je element uspješno umetnut na kraj reda, inače vraća *errQUEUE_FULL*. Uočiti da ovdje ne postoji blokirajuća opcija.

##### Slanje elementa na početak reda
```
xQueueSendToFront(xQueue, pvItemToQueue, xTicksToWait)
```

Parametri makroa su sljedeći:

- *xQueue*
	- *handle* tipa *QueueHandle_t* na red u koji se piše (kopiranjem)
- *pvItemToQueue*
	- pokazivač na element koji će se staviti na početak reda (kopiranjem)
- *xTicksToWait*
	- vrijeme čekanja dok se u redu ne pojavi prazno mjesto za pisanje, tipa *TickType_t*
	- ako je 0 onda je makro neblokirajući u slučaju punog reda, zadatak nastavlja se radom iako mu nije uspjelo pisati element u red
	- ako je *portMAX_DELAY* onda je blokirajuć, zadatak čeka sve dok u redu ne bude mjesta

Makro vraća *pdTRUE* (tip *BaseType_t*) ako je element uspješno umetnut na početak reda, inače vraća *errQUEUE_FULL*

##### Slanje elementa na početak reda iz prekidne rutine
```
xQueueSendToFrontFromISR(xQueue, pvItemToQueue, pxHigherPriorityTaskWoken)
```

Parametri makroa su sljedeći:

- *xQueue*
	- *handle* tipa *QueueHandle_t* na red u koji se piše (kopiranjem)
- *pvItemToQueue*
	- pokazivač na element koji će se staviti na početak reda (kopiranjem)
- *pxHigherPriorityTaskWoken*
	- pokazivač na varijablu tipa *BaseType_t*
	- inicijalnu vrijednost je najbolje postaviti na *pdFALSE*
	- makro postavlja ovu vrijednost u *pdTRUE* ako je prekidna rutina svojim pisanjem elementa u red oslobodila zadatak koji ima veći prioritet od trenutnog zadatka koji se izvršavao

Makro vraća *pdTRUE* (tip *BaseType_t*) ako je element uspješno umetnut na kraj reda, inače vraća *errQUEUE_FULL*. Uočiti da ovdje ne postoji blokirajuća opcija.

##### Uzimanje elementa
```
BaseType_t xQueueReceive(QueueHandle_t xQueue, void *const pvBuffer, TickType_t xTicksToWait)
```

Parametri funkcije su sljedeći:

- *xQueue*
	- *handle* na red iz kojeg se čita (kopiranjem)
- *pvBuffer*
	- pokazivač na element gdje će biti stavljen pročitani element (kopiranjem)
- *xTicksToWait*
	- vrijeme čekanja dok se u redu ne pojavi element za čitanje, tipa *TickType_t*
	- ako je 0 onda je makro neblokirajući u slučaju praznog reda, zadatak nastavlja se radom iako mu nije uspjelo pročitati element iz reda
	- ako je *portMAX_DELAY* onda je blokirajuć, zadatak čeka sve dok se u redu ne pojavi element

Funkcija vraća *pdTRUE* ako je element uspješno pročitan iz reda, inače vraća *pdFALSE*

##### Uzimanje elementa na početak reda iz prekidne rutine
```
BaseType_t xQueueReceiveFromISR(QueueHandle_t xQueue, void *const pvBuffer, BaseType_t *const pxHigherPriorityTaskWoken)
```

Parametri funkcije su sljedeći:

- *xQueue*
	- *handle* na red iz kojeg se čita (kopiranjem)
- *pvBuffer*
	- pokazivač na element gdje će biti stavljen pročitani element (kopiranjem)
- *pxHigherPriorityTaskWoken*
	- pokazivač na varijablu tipa *BaseType_t*
	- inicijalnu vrijednost je najbolje postaviti na *pdFALSE*
	- makro postavlja ovu vrijednost u *pdTRUE* ako je prekidna rutina svojim čitanjem elementa iz reda oslobodila zadatak koji ima veći prioritet od trenutnog zadatka koji se izvršavao

Funkcija vraća *pdTRUE* ako je element uspješno pročitan iz reda, inače vraća *pdFALSE*. Uočiti da ovdje ne postoji blokirajuća opcija.

##### Nedestruktivno uzimanje elementa
```
BaseType_t xQueuePeek(QueueHandle_t xQueue, void *const pvBuffer, TickType_t xTicksToWait)
```

Parametri funkcije su sljedeći:

- *xQueue*
	- *handle* na red iz kojeg se čita (kopiranjem)
- *pvBuffer*
	- pokazivač na element gdje će biti stavljen pročitani element (kopiranjem)
- *xTicksToWait*
	- vrijeme čekanja dok se u redu ne pojavi element za čitanje, tipa *TickType_t*
	- ako je 0 onda je makro neblokirajući u slučaju praznog reda, zadatak nastavlja se radom iako mu nije uspjelo pročitati element iz reda
	- ako je *portMAX_DELAY* onda je blokirajuć, zadatak čeka sve dok se u redu ne pojavi element

Funkcija vraća *pdTRUE* ako je element uspješno nedestruktivno pročitan iz reda, inače vraća *pdFALSE*. Ova funkcija je nedestruktivna, odnosno čitanjem elementa se ne prazni red.

##### Uzimanje elementa na početak reda iz prekidne rutine
```
BaseType_t xQueuePeekFromISR(QueueHandle_t xQueue, void *const pvBuffer)
```

Parametri funkcije su sljedeći:

- *xQueue*
	- *handle* na red iz kojeg se čita (kopiranjem)
- *pvBuffer*
	- pokazivač na element gdje će biti stavljen pročitani element (kopiranjem)

Funkcija vraća *pdTRUE* ako je element uspješno nedestruktivno pročitan iz reda, inače vraća *pdFALSE*. Uočiti da ovdje ne postoji blokirajuća opcija.

##### Provjera je li red pun
```
BaseType_t xQueueIsQueueFullFromISR(const QueueHandle_t xQueue)
```

Parametri funkcije su sljedeći:

- *xQueue*
	- *handle* na red za kojeg se provjerava je li pun

Funkcija vraća *pdTRUE* ako je red pun, inače vraća *pdFALSE*.

##### Provjera je li red prazan
```
BaseType_t xQueueIsQueueEmptyFromISR(const QueueHandle_t xQueue)
```

Parametri funkcije su sljedeći:

- *xQueue*
	- *handle* na red za kojeg se provjerava je li pun

Funkcija vraća *pdTRUE* ako je red prazan, inače vraća *pdFALSE*.

##### Provjera koliko elemenata ima u redu
```
UBaseType_t uxQueueMessagesWaiting(const QueueHandle_t xQueue)
```

Parametri funkcije su sljedeći:

- *xQueue*
	- *handle* na red za kojeg se provjerava koliko ima elemenata u redu

Funkcija vraća broj elemenata u redu.

##### Provjera koliko slobodnih prostora ima u redu
```
UBaseType_t uxQueueSpacesAvailable(const QueueHandle_t xQueue)
```

Parametri funkcije su sljedeći:

- *xQueue*
	- *handle* na red za kojeg se provjerava koliko slobodnih prostora ima u redu

Funkcija vraća broj slobodnih prostora u redu.

#### Semafori i monitori

Postoje tri vrste resursa napravljena izričito za sinkronizaciju i pristupu kritičnim sekcijama u FreeRTOS-u:

- binarni semafori
- brojeći semafori
- monitori

Nepravilno korištenje semafora i monitora može dovesti do zastoja u radu sustava.

Zaglavlje koje je potrebno uključiti za rad sa semaforima i monitorima (*eng. Semaphore API*) je ```freertos/semphr.h```.

##### Binarni semafor

[Binarni semafor (*eng. binary semaphore*)](https://freertos.org/Documentation/02-Kernel/02-Kernel-features/02-Queues-mutexes-and-semaphores/02-Binary-semaphores) je mehanizam koji se koristi za sinkronizaciju i izvođenje kritičnih sekcija.

Binarni semafor može imati dvije vrijednosti:

- *0* - označava zauzet semafor (resurs), zadatci koji pokušavaju uzeti resurs se postavljaju u red blokiranih zadataka
- *1* - slobodan semafor (resurs), zadatci mogu uzeti resurs

Iz pogleda zadatka, postoje dvije operacije:

- *P (wait)* - dekrementiraj vrijednost semafora, ako je vrijednost semafora već bila 0 (zauzet) neka se trenutni zadatak stavi u red čekanja (blokiraj se) i čekaj dok neki drugi zadatak poveća vrijednost semafora s operacijom *V*
- *V (signal)* - inkrementiraj vrijednost semafora, ako je vrijednost semafora već bila u 1 (slobodan), ova operacija neće inkrementirati vrijednost binarnog semafora na više

Primjer korištenja zadatka se može naći u direktoriju [*tasks-binary-semaphore*](tasks-binary-semaphore).

##### Brojeći semafor

[Brojeći semafor (eng. counting semaphore)](https://freertos.org/Documentation/02-Kernel/02-Kernel-features/02-Queues-mutexes-and-semaphores/03-Counting-semaphores) je mehanizam koji se koristi za sinkronizaciju i izvođenje kritičnih sekcija.

Brojeći semafor može imati više vrijednosti:

- *0* - označava zauzet semafor (resurs), zadatci koji pokušavaju uzeti resurs se postavljaju u red blokiranih zadataka
- *>0* - slobodan semafor (resurs), zadatci mogu uzeti resurs

Slično kao i kod binarnog semafora, iz pogleda zadatka, postoje dvije operacije:

- *P (wait)* - dekrementiraj vrijednost semafora, ako je vrijednost semafora već bila 0 (zauzet) neka se trenutni zadatak stavi u red čekanja (blokiraj se) i čekaj dok neki drugi zadatak poveća vrijednost semafora s operacijom *V*
- *V (signal)* - inkrementiraj vrijednost semafora, vrijednost semafora se može inkrementirati do broj **N** i nakon toga ova operacija neće inkrementirati vrijednost brojećeg semafora na više

Za razliku od binarnog semafora, brojeći semafor dozvoljava pristup kritičnoj sekciji više zadataka.

##### Monitori

[Monitori (*eng. mutex*)](https://freertos.org/Documentation/02-Kernel/02-Kernel-features/02-Queues-mutexes-and-semaphores/04-Mutexes) je mehanizam za pristup kritičnim sekcijama koji dopušta samo jednom zadatku pristup resursu. Isto tako, isti zadatak koji je pristupio resursu može ga otpustiti za razliku od semafora gdje bilo koji zadatak može inkrementirati vrijednost semafora. Također, monitor može biti rekurzivan odnosno isti zadatak može više puta zaključati monitor i onda ga isti broj otključati. Može se reći da zadatak ulaskom u monitor (zauzimajući resurs) postaje vlasnik resursa, a izlazeći iz monitora (otpuštajući resurs) odbacuje vlasništvo nad resursom.

Monitori se ne smiju koristiti u prekidnim rutinama.

Primjer korištenja zadatka se može naći u direktoriju [*tasks-mutex*](tasks-mutex).

##### Stvaranje binarnog semafora
```
xSemaphoreCreateBinary(void)
```

Makro vraća *handle* tipa *SemaphoreHandle_t* na novostvoreni binarni semafor, a ako se binarni semafor ne može stvoriti vraća NULL. Inicijalna vrijednost ovog semafora je *0* (zauzet).

##### Stvaranje brojećeg semafora
```
xSemaphoreCreateCounting(uxMaxCount, uxInitialCount)
```

Parametri makroa su sljedeći:

- *uxMaxCount*
	- postavlja maksimalnu vrijednost koju brojeći semafor može imati (**N**)
- *uxInitialCount*
	- postavlja inicijalnu vrijednost brojećeg semafora

Makro vraća *handle* tipa *SemaphoreHandle_t* na novostvoreni brojeći semafor, a ako se brojeći semafor ne može stvoriti vraća NULL.

##### Stvaranje monitora
```
xSemaphoreCreateMutex(void)
```

Makro vraća *handle* tipa *SemaphoreHandle_t* na novostvoreni monitor, a ako se monitor ne može stvoriti vraća NULL.

##### Brisanje semafora i monitora
```
vSemaphoreDelete(xSemaphore)
```

Nema povratne vrijednosti.

##### Zauzimanje semafora ili monitora
```
xSemaphoreTake(xSemaphore, xBlockTime)
```

Parametri makroa su sljedeći:

- *xSemaphore*
	- *handle* tipa *SemaphoreHandle_t* na binarni semafor, brojeći semafor ili monitor
- *xBlockTime*
	- vrijeme u taktovima tipa *TickType_t* koji označava koliko dugo će zadatak biti u redu čekanja
	- ako je 0 onda je čekanje neblokirajuće, zadatak odmah ide na resurs
	- ako je *portMAX_DELAY* onda je blokirajuć, zadatak čeka sve dok se resurs ne oslobodi

Makro vraća *pdTRUE* ako je zadatak uspio ući u semafor ili monitor (zauzeti resurs), inače vraća *pdFALSE*.

##### Rekurzivno zauzimanje monitora
```
xSemaphoreTakeRecursive(xMutex, xBlockTime)
```

Parametri makroa su sljedeći:

- *xMutex*
	- *handle* tipa *SemaphoreHandle_t* na monitor
- *xBlockTime*
	- vrijeme u taktovima tipa *TickType_t* koji označava koliko dugo će zadatak biti u redu čekanja
	- ako je 0 onda je čekanje neblokirajuće, zadatak odmah ide na resurs
	- ako je *portMAX_DELAY* onda je blokirajuć, zadatak čeka sve dok se resurs ne oslobodi

Makro vraća *pdTRUE* ako je zadatak uspio ući u monitor (zauzeti resurs), inače vraća *pdFALSE*.

##### Zauzimanje semafora u prekidnoj rutini
```
xSemaphoreTakeFromISR(xSemaphore, pxHigherPriorityTaskWoken)
```

Parametri makroa su sljedeći:

- *xSemaphore*
	- *handle* tipa *SemaphoreHandle_t* na binarni semafor ili brojeći semafor
- *pxHigherPriorityTaskWoken*
	- pokazivač na varijablu tipa *BaseType_t*
	- inicijalnu vrijednost je najbolje postaviti na *pdFALSE*
	- makro postavlja ovu vrijednost u *pdTRUE* ako je prekidna rutina svojim zauzećem oslobodila zadatak koji ima veći prioritet od trenutnog zadatka koji se izvršavao

Makro vraća *pdTRUE* (tip *BaseType_t*) ako je semafor uspješno zauzet, inače vraća *pdFALSE*.

**Ova opcija se rijetko koristi.**

##### Oslobađanje semafora ili monitora
```
xSemaphoreGive(xSemaphore)
```

Parametri makroa su sljedeći:

- *xSemaphore*
	- *handle* tipa *SemaphoreHandle_t* na binarni semafor ili brojeći semafor

Makro vraća *pdTRUE* ako je zadatak uspio izaći iz semafora ili monitora (osloboditi resurs), inače vraća *pdFALSE*.

##### Rekurzivno oslobađanje monitora
```
xSemaphoreGiveRecursive(xMutex)
```

Parametri makroa su sljedeći:

- *xMutex*
	- *handle* tipa *SemaphoreHandle_t* na monitor

Makro vraća *pdTRUE* ako je zadatak uspio izaći iz monitora (zauzeti resurs), inače vraća *pdFALSE*.

##### Oslobađanje semafora u prekidnoj rutini
```
xSemaphoreGiveFromISR(xSemaphore, pxHigherPriorityTaskWoken)
```

Parametri makroa su sljedeći:

- *xSemaphore*
	- *handle* tipa *SemaphoreHandle_t* na binarni semafor ili brojeći semafor
- *pxHigherPriorityTaskWoken*
	- pokazivač na varijablu tipa *BaseType_t*
	- inicijalnu vrijednost je najbolje postaviti na *pdFALSE*
	- makro postavlja ovu vrijednost u *pdTRUE* ako je prekidna rutina svojim oslobođenjem oslobodila zadatak koji ima veći prioritet od trenutnog zadatka koji se izvršavao

Makro vraća *pdTRUE* (tip *BaseType_t*) ako je semafor uspješno oslobođen, inače vraća *errQUEUE_FULL*.


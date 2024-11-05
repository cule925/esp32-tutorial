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

## Korištenje ESP-IDF FreeRTOS-a

ESP-IDF-ov FreeRTOS je zadani operacijski sustav za ESP32 mikroupravljače. Dakle, nije potrebno kao kod običnog FreeRTOS operacijskog sustava pozivati funkcije za pokretanje i zaustavljanje raspoređivača funkcijama ```vTaskStartScheduler()``` i ```vTaskStopScheduler()```.

### Već pokrenuti pozadinski zadatci

Nakon pokretanja raspoređivača, FreeRTOS jezgra automatski stvara par procesa (odnosno zadataka):

- *Idle Task (IDLEx)*
	- proces koji ništa ne radi
	- svakoj jezgri je dodijeljen, dvojezgreni način rada: *IDLE0* na jezgri 0 i *IDLE1* na jezgri 1, kod jednojezgrenog načina rada *IDLE* na jezgri 0
	- veličina stoga zadataka se može urediti u *sdkconfig* datoteci uređujući *CONFIG_FREERTOS_IDLE_TASK_STACKSIZE stavku*
	- ovi zadatci imaju najniži prioritet, odnosno prioritet 0

- *FreeRTOS Timer Task (Tmr Svc)*
	- ako postoje Timer API pozivi u aplikaciji, ovaj zadatak se pokreće
	- veličina stoga zadatka se može urediti u *sdkconfig* datoteci uređujući CONFIG_FREERTOS_TIMER_TASK_STACK_DEPTH
	- afinitet zadatka je jezgra 0
	- prioritet zadatka se može urediti u *sdkconfig* datoteci uređujući CONFIG_FREERTOS_TIMER_TASK_PRIORITY

- *Main Task (main)*
	- zadatak koji poziva *app_main*
	- zadatak će se sam obrisati kada *app_main* vrati neku vrijednost
	- veličina stoga zadatka se može urediti u *sdkconfig* datoteci uređujući CONFIG_ESP_MAIN_TASK_STACK_SIZE
	- afinitet zadatka određenoj jezgri se može urediti u *sdkconfig* datoteci uređujući CONFIG_ESP_MAIN_TASK_AFFINITY
	- prioritet ovog zadatka je 1
	
- *IPC Tasks (ipcx)*
	- stvara se u dvojezgrenom načinu rada: *ipx0* na jezgri 0 i *ipx1*
	- *IPC* zadatak omogućuje pozive funkcija koje će se izvršavati na drugoj jezgri (jezgri na kojoj se on ne izvršava)
	- veličina stoga zadatka se može urediti u *sdkconfig* datoteci uređujući CONFIG_ESP_IPC_TASK_STACK_SIZE
	- prioriteti ovih zadataka je 24

- *ESP Timer Task*
	- procesuira ESP Timer *callback* funkcije
	- - veličina stoga zadatka se može urediti u *sdkconfig* datoteci uređujući CONFIG_ESP_TIMER_TASK_STACK_SIZE
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

[Zadatak](https://docs.espressif.com/projects/esp-idf/en/v5.3.1/esp32/api-reference/system/freertos_idf.html#tasks) je sličan procesu ili dretvi, neovisni niz instrukcija koja se može pokrenuti, pauzirati ili zaustaviti.

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
	- prosljeđuje se *handle* uz pomoć kojega se može referirati na zadatak
- *xCoreID*
	- jezgra prema kojoj zadatak ima afinitet (0 ili 1)
	- u slučaju da zadatak nema afinitet prema ni jednoj jezgri, potrebno je proslijediti *tskNO_AFFINITY*

Povratna vrijednost ove funkcije u slučaju uspješnog stvaranja zadatka je *pdPASS*, u suprotnom je jedan od kodova grešaka definiranih u *projdefs.h*.

#### Brisanje zadatka

Stvaranje zadatka moguće je funkcijom:

```
void vTaskDelete(TaskHandle_t xTaskToDelete)
```

- *xTaskToDelete*
	- *handle* zadatka
	- ako je proslijeđen *NULL* to znači da zadatak briše sam sebe

Nema povratne vrijednosti.

#### Relativna vremenska odgoda

Relativna vremenska odgoda nekog zadatka:

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

Apsolutna vremenska odgoda nekog zadatka:

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

#### Primjer korištenja zadataka

**TODO**

##### Upravljanje prekidima i izvođenje kritičnih sekcija

Prekidi (koji se mogu onemogućiti) se mogu omogućiti i onemogućiti makroima ```taskDISABLE_INTERRUPTS()``` i ```taskENABLE_INTERRUPTS()```.

Međutim, u SMP ESP32 sustavima, ovo će upravljati prekidima samo trenutne jezgre odakle su ovi makroi pozvani.

**TODO:**

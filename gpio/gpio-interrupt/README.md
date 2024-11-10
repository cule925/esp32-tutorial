# TASKS-QUEUE

Aplikacija je napravljena za ESP32-DevKitM-1 jednojezgrenu pločicu.

Potrebno je napraviti sljedeće:
    - na GPIO pin 22 postaviti svjetleću diodu (*eng. LED*) i otpornik (1 kOhm) u seriju ih na drugi kraju povezati s GND
    - na GPIO pin 21 postaviti svjetleću diodu (*eng. LED*) i otpornik (1 kOhm) u seriju ih na drugi kraju povezati s GND
    - na GPIO pin 19 postaviti tipkalo (*eng. BUTTON*) i povezati ga s GND
    - na GPIO pin 18 postaviti tipkalo (*eng. BUTTON*) i povezati ga s GND

```
        +-----------------+
        | ESP32-DevKitM-1 |
        |                 |
        |         GPIO 18 |------------------------------------------------------------------------+
        |                 |                                                                        |
        |         GPIO 19 +-------------------------------------------------------+                |
        |                 |                                                       |                |
        |         GPIO 21 +--------------------------------------+                |                |
        |                 |                                      |                |                |
        |         GPIO 22 +---------------------+                |                |                |
        |                 |                     |                |                |                |
        |             GND +----+               _|_              _|_              -+-              -+-
        |                 |    |               \ / LED 1        \ / LED 2       | O | BUTTON 1   | o | BUTTON 2
        +-----------------+    |                |                |               -+-              -+-
                               |                |                |                |                |
                               |               +++              +++               |                |
                               |               | | R - 1kOhm    | | R - 1kOhm     |                |
                               |               | |              | |               |                |
                               |               +++              +++               |                |
                               |                |                |                |                |
                               +----------------+----------------+----------------+----------------+
```

Aplikacija radi sljedeće:

- stvara dva semafora, oba su inicijalno neprolazna (postavljeni su na 0)
- postavlja pinove GPIO 22 i GPIO 212 kao izlaz
- postavlja pinove GPIO 19 i GPIO 18 kao ulaz, inicijalizira ih u prekidnom načinu rada na padajući brid te postavlja im *pull-up* otpornike
- stvara dva zadatka: prvi zadatak bi zauzimao prvi semafor i pristupao bi GPIO 22 pinu (palio i gasio LED-icu po nekom svom uzorku), drugi zadatak bi uzimao drugi semafor i on bi pristupao GPIO 21 pinu (palio i gasio LED-icu po nekom svom uzorku), međutim oba semafora su inicijalno neprolazna pa se zadatci blokiraju
- pritiskom na prvo tipkalo događa se prekid i izvršavanje rutine koja čini prvi semafor prolaznim
- pritiskom na drugo tipkalo događa se prekid i izvršavanje rutine koja čini drugi semafor prolaznim
- kada je prvi semafor prolazan, prvi zadatak ga čini neprolaznim i izvršava svoje paljenje i gašenje LED-ice (na GPIO 22), nakon čega ponovno zahtjeva semafor koji je opet neprolazan do pritiska tipkala (ovo je beskonačna petlja)
- kada je drugi semafor prolazan, drugi zadatak ga čini neprolaznim i izvršava svoje paljenje i gašenje LED-ice (na GPIO 22), nakon čega ponovno zahtjeva semafor koji je opet neprolazan do pritiska tipkala (ovo je beskonačna petlja)
- tipkala imaju implementirane softverske *debouncere* koji osiguravaju da se prekidne rutine tipkala odnosno *handleri* ignoriraju mehanička podrhtavanja tipkala tijekom pritiska, pritiskom na tipkalo samo jednom će se učiniti semafor prolaznim 
- ispisuje logove svih zadataka na serijski monitor (kombinacija tipki *CTRL + ]* je za izlaz iz serijskog monitora)

Slikovito:

```
        initially: FIRST semaphore S1[0], SECOND semaphore S2[0]

          TASK 1                     TASK 2
            |                          |
            |                          |
        take FIRST                 take SECOND
        semaphore S1[0]            semaphore S2[0]
         BLOCKED                    BLOCKED
            .                          .
            .                          .
            .       isr_button_1       .
            .       handler S1++       .
        UNBLOCKED   <-- S1[1]          .
          S1--                         .
          S1[0]                        .
            |                          .
        execute LED                    .
        pattern                        .
            |                          .
        take FIRST                     .
        semaphore S1[0]                .
         BLOCKED                       .
            .                          .
            .                          .
            .                          .
            .                          .
            .                          .
            .                          .
            .                          .       isr_button_2
            .                          .       handler S2++
            .                      UNBLOCKED   <-- S2[1]
            .                        S2--
            .                        S2[0]
            .                          |
            .                      execute LED
            .                      pattern
            .                          |
            .                      take SECOND
            .                      semaphore S2[0]
            .                       BLOCKED
            .                          .
            .       isr_button_1       .
            .       handler S1++       .
        UNBLOCKED   <-- S1[1]          .
            .                          .
          S1--                         .
          S1[0]                        .
            |                          .
        execute LED                    .
        pattern                        .
            |                          .
        take FIRST                     .
        semaphore S1[0]                .
         BLOCKED                       .
            .                          .
            .                          .
            .                          .
            .                          .       isr_button_2
            .                          .       handler S2++
            .                      UNBLOCKED   <-- S2[1]
            .                        S2--
            .                        S2[0]
            .                          |
            .                      execute LED
            .                      pattern
            .                          |
            .                      take SECOND
            .                      semaphore S2[0]
            .                       BLOCKED
            .                          .
            .                          .
            .                          .
            .                          .
```
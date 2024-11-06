# TASKS-QUEUE

Aplikacija je napravljena za ESP32-DevKitM-1 jednojezgrenu pločicu.

Potrebno je na GPIO pin 22 postaviti svjetleću diodu (*eng. LED*) i otpornik (1 kOhm) u seriju ih na drugi kraju povezati s GND:

```
        +-----------------+
        | ESP32-DevKitM-1 |
        |                 |
        |         GPIO 22 +---------------------+
        |                 |                     |
        |             GND +----+               _|_
        |                 |    |               \ / LED
        +-----------------+    |                |
                               |                |
                               |               +++
                               |               | | R - 1kOhm
                               |               | |
                               |               +++
                               |                |
                               +----------------+
```

Aplikacija radi sljedeće:

- stvara dva semafora, prvi je inicijalno prolazan (postavljen na 1), a drugi neprolazan (postavljen na 0)
- stvara dva zadatka: prvi zadatak zauzima prvi semafor i pristupa GPIO 22 pinu (pali i gasi LED-icu po nekom svom uzorku) i zatim oslobađa drugi semafor, drugi zadatak uzima drugi semafor i sada on pristupa GPIO 22 pinu (pali i gasi LED-icu po nekom svom uzorku) i zatim oslobađa prvi semafor
- zadatci jedan drugome međusobno oslobađaju semafore, u ovom slučaju to se događa 5 puta pa se zadatci naposljetku izbrišu
- ispisuje logove svih zadataka na serijski monitor (kombinacija tipki *CTRL + ]* je za izlaz iz serijskog monitora)

Slikovito, uz 5 iteracija *žongliranja* prava nad resursom (LED pin):

```
        initially:  FIRST semaphore S1[1], SECOND semaphore S2[0]

          TASK 1                     TASK 2
            |                          |
            |                          |
        take FIRST                 take SECOND
        semaphore S1[1]            semaphore S2[0]
          S1--                      BLOCKED
          S1[0]                        .
            |                          .
        execute LED                    .
        pattern                        .
            |                          .
        give SECOND                    .
        semaphore                      .
          S2++                         .
          S2[1] -----------------> UNBLOCKED
            |                        S2--
            |                        S2[0]
            |                          |
        take FIRST                 execute LED
        semaphore S1[0]            pattern
         BLOCKED                       |
            .                      give FIRST
            .                      semaphore
            .                        S1++
        UNBLOCKED <----------------- S1[1]
          S1--                         |
          S1[0]                        | 
            |                          |
        execute LED                take SECOND
        pattern                    semaphore S2[0]
            |                       BLOCKED
        give SECOND                    .
        semaphore                      .
          S2++                         .
          S2[1] -----------------> UNBLOCKED
            |                        S2--
            |                        S2[0]
            |                          |
        take FIRST                 execute LED
        semaphore S1[0]            pattern
         BLOCKED                       |
            .                      give FIRST
            .                      semaphore
            .                        S1++
        UNBLOCKED <----------------- S1[1]
          S1--                         |
          S1[0]                        |
            |                          |
        execute LED                take SECOND
        pattern                    semaphore S2[0]
            |                       BLOCKED
        give SECOND                    .
        semaphore                      .
          S2++                         .
          S2[1] -----------------> UNBLOCKED
            |                        S2--
            |                        S2[0]
            |                          |
        take FIRST                 execute LED
        semaphore S1[0]            pattern
         BLOCKED                       |
            .                      give FIRST
            .                      semaphore
            .                        S1++
        UNBLOCKED <----------------- S1[1]
          S1--                         |
          S1[0]                        |
            |                          |
        execute LED                take SECOND
        pattern                    semaphore S2[0]
            |                       BLOCKED
        give SECOND                    .
        semaphore                      .
          S2++                         .
          S2[1] -----------------> UNBLOCKED
            |                        S2--
            |                        S2[0]
            |                          |
        take FIRST                 execute LED
        semaphore S1[0]            pattern
         BLOCKED                       |
            .                      give FIRST
            .                      semaphore
            .                        S1++
        UNBLOCKED <----------------- S1[1]
          S1--                         |
          S1[0]                        |
            |                          |
        execute LED                take SECOND
        pattern                    semaphore S2[0]
            |                       BLOCKED
        give SECOND                    .
        semaphore                      .
          S2++                         .
          S2[1] -----------------> UNBLOCKED
            |                        S2--
            |                        S2[0] 
            |                          |
          EXIT                     execute LED
                                   pattern
                                       |
                                   give FIRST
                                   semaphore
                                     S1++
                                     S1[1]
                                       |
                                       |
                                       |
                                     EXIT
```
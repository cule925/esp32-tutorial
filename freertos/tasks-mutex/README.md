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

- stvara deset zadataka
- svaki zadatak pokušava ući u jedan te isti monitor
- samo jedan zadatak može ući u monitor i pristupiti GPIO 22 pinu (pali i gasi LED-icu po nekom svom uzorku)
- svaki zadatak maksimalno 3 puta prezentira svoj uzorak, nakon toga se sam briše
- ispisuje logove svih zadataka na serijski monitor (kombinacija tipki *CTRL + ]* je za izlaz iz serijskog monitora)

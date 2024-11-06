# TASK-BLINK

Aplikacija je napravljena za ESP32-DevKitM-1 jednojezgrenu pločicu.

Potrebno je na GPIO pin 22 postaviti svjetleću diodu (*eng. LED*) i otpornik (1 kOhm) u seriju ih na drugi kraju povezati s GND:

```
        +-----------------+
        | ESP32-DevKitM-1 |
        |                 |
        |         GPIO 22 +---------------------+
        |                 |                     |
        |             GND +----+               _|_
        |                 |    |               \ /   LED
        +-----------------+    |                |
                               |                |
                               |               +++
                               |               | |   R - 1kOhm
                               |               | |
                               |               +++
                               |                |
                               +----------------+
```

Aplikacija radi sljedeće:

- postavlja GPIO pin 22 na kojem je spojen svjetleća dioda kao izlazni
- pokreće zadatak koji 5 puta upali svjetleću diodu na jednu sekundu i ugasi je na jednu sekundu
- ispisuje logove na serijski monitor (kombinacija tipki *CTRL + ]* je za izlaz iz serijskog monitora)